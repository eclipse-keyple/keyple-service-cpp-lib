/******************************************************************************
 * Copyright (c) 2025 Calypso Networks Association https://calypsonet.org/    *
 *                                                                            *
 * See the NOTICE file(s) distributed with this work for additional           *
 * information regarding copyright ownership.                                 *
 *                                                                            *
 * This program and the accompanying materials are made available under the   *
 * terms of the Eclipse Public License 2.0 which is available at              *
 * http://www.eclipse.org/legal/epl-2.0                                       *
 *                                                                            *
 * SPDX-License-Identifier: EPL-2.0                                           *
 ******************************************************************************/

#include "keyple/core/service/ObservableLocalReaderAdapter.hpp"

#include <memory>
#include <string>
#include <vector>

#include "keyple/core/plugin/CardIOException.hpp"
#include "keyple/core/plugin/ReaderIOException.hpp"
#include "keyple/core/service/ObservableReaderStateServiceAdapter.hpp"
#include "keyple/core/service/ReaderEventAdapter.hpp"
#include "keyple/core/service/ScheduledCardSelectionsResponseAdapter.hpp"
#include "keyple/core/util/cpp/Arrays.hpp"
#include "keyple/core/util/cpp/exception/Exception.hpp"
#include "keypop/card/CardBrokenCommunicationException.hpp"
#include "keypop/card/ReaderBrokenCommunicationException.hpp"
#include "keypop/reader/ReaderCommunicationException.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::plugin::CardIOException;
using keyple::core::plugin::ReaderIOException;
using keyple::core::util::cpp::Arrays;
using keyple::core::util::cpp::exception::Exception;
using keypop::card::CardBrokenCommunicationException;
using keypop::card::ReaderBrokenCommunicationException;
using keypop::reader::ReaderCommunicationException;

/* OBSERVABLE LOCAL READER ADAPTER JOB
 * ---------------------------------------------------------- */

ObservableLocalReaderAdapter::ObservableLocalReaderAdapterJob::
    ObservableLocalReaderAdapterJob(
        std::shared_ptr<CardReaderObserverSpi> observer,
        const std::shared_ptr<CardReaderEvent> event,
        ObservableLocalReaderAdapter* parent)
: Job("ObservableLocalReaderAdapter")
, mObserver(observer)
, mEvent(event)
, mParent(parent)
{
}

void
ObservableLocalReaderAdapter::ObservableLocalReaderAdapterJob::execute()
{
    mParent->notifyObserver(mObserver, mEvent);
}

/* OBSERVABLE LOCAL READER ADAPTER
 * -------------------------------------------------------------- */

const std::vector<uint8_t> ObservableLocalReaderAdapter::APDU_PING_CARD_PRESENCE
    = {0x00, 0xC0, 0x00, 0x00, 0x00};
const std::string ObservableLocalReaderAdapter::READER_MONITORING_ERROR
    = "An error occurred while monitoring the reader.";

ObservableLocalReaderAdapter::ObservableLocalReaderAdapter(
    std::shared_ptr<ObservableReaderSpi> observableReaderSpi,
    const std::string& pluginName)
: LocalReaderAdapter(observableReaderSpi, pluginName)
, mObservableReaderSpi(observableReaderSpi)
, mStateService(std::make_shared<ObservableReaderStateServiceAdapter>(this))
, mObservationManager(
      std::make_shared<ObservationManagerAdapter<
          CardReaderObserverSpi,
          CardReaderObservationExceptionHandlerSpi>>(pluginName, getName()))
{
    auto asynchronousInsertion
        = std::dynamic_pointer_cast<CardInsertionWaiterAsynchronousSpi>(
            observableReaderSpi);
    auto autonomousInsertion
        = std::dynamic_pointer_cast<WaitForCardInsertionAutonomousSpi>(
            observableReaderSpi);
    if (asynchronousInsertion) {
        asynchronousInsertion->setCallback(this);
    } else if (autonomousInsertion) {
        autonomousInsertion->connect(this);
    }

    auto asynchronousRemoval
        = std::dynamic_pointer_cast<CardRemovalWaiterAsynchronousSpi>(
            observableReaderSpi);
    auto autonomousRemoval
        = std::dynamic_pointer_cast<WaitForCardRemovalAutonomousSpi>(
            observableReaderSpi);
    if (asynchronousRemoval) {
        asynchronousRemoval->setCallback(this);
    } else if (autonomousRemoval) {
        autonomousRemoval->connect(this);
    }
}

std::shared_ptr<ObservableReaderSpi>
ObservableLocalReaderAdapter::getObservableReaderSpi() const
{
    return mObservableReaderSpi;
}

std::shared_ptr<CardReaderObservationExceptionHandlerSpi>
ObservableLocalReaderAdapter::getObservationExceptionHandler() const
{
    return mObservationManager->getObservationExceptionHandler();
}

DetectionMode
ObservableLocalReaderAdapter::getDetectionMode() const
{
    return mDetectionMode;
}

MonitoringState
ObservableLocalReaderAdapter::getCurrentMonitoringState() const
{
    return mStateService->getCurrentMonitoringState();
}

bool
ObservableLocalReaderAdapter::isCardPresentPing()
{
    /* Transmits the APDU and checks for the IO exception */
    try {
        mObservableReaderSpi->transmitApdu(APDU_PING_CARD_PRESENCE);
    } catch (const ReaderIOException& e) {
        /* Notify the reader communication failure with the exception handler */
        const auto rce = std::make_shared<ReaderCommunicationException>(
            READER_MONITORING_ERROR, e);
        getObservationExceptionHandler()->onReaderObservationError(
            getPluginName(), getName(), rce);
        return false;

    } catch (const CardIOException&) {
        return false;
    }

    return true;
}

std::shared_ptr<CardReaderEvent>
ObservableLocalReaderAdapter::processCardInserted()
{
    /* RL-DET-INSNOTIF.1 */
    mLogger->trace("[reader=%] Processing inserted card\n", getName());

    mIsCardRemovedEventNotificationEnabled = true;

    if (mCardSelectionScenario == nullptr) {
        mLogger->trace(
            "[reader=%] No card selection scenario defined. Notifying card "
            "reader event [eventType=CARD_INSERTED]\n",
            getName());

        /* No default request is defined, just notify the card insertion */
        return std::make_shared<ReaderEventAdapter>(
            getPluginName(),
            getName(),
            CardReaderEvent::Type::CARD_INSERTED,
            nullptr);
    }

    /*
     * A card selection scenario is defined, send it and notify according to the
     * notification mode and the selection status
     */
    try {
        const std::vector<std::shared_ptr<CardSelectionResponseApi>>
            cardSelectionResponses = transmitCardSelectionRequests(
                mCardSelectionScenario->getCardSelectors(),
                mCardSelectionScenario->getCardSelectionRequests(),
                mCardSelectionScenario->getMultiSelectionProcessing(),
                mCardSelectionScenario->getChannelControl());

        if (hasACardMatched(cardSelectionResponses)) {
            return std::make_shared<ReaderEventAdapter>(
                getPluginName(),
                getName(),
                CardReaderEvent::Type::CARD_MATCHED,
                std::make_shared<ScheduledCardSelectionsResponseAdapter>(
                    cardSelectionResponses));
        }

        if (mNotificationMode == NotificationMode::MATCHED_ONLY) {
            /*
             * Notify only if a card matched the selection, just ignore if not.
             */
            mLogger->trace(
                "[reader=%] Selection hasn't matched. Event not notified "
                "because the notification mode is MATCHED_ONLY\n",
                getName());

            return nullptr;
        }

        /*
         * The card didn't match, notify an CARD_INSERTED event with the
         * received response.
         */
        mLogger->debug(
            "[reader=%] No selection cases matched [selectionCaseCount=%]\n",
            getName(),
            cardSelectionResponses.size());

        return std::make_shared<ReaderEventAdapter>(
            getPluginName(),
            getName(),
            CardReaderEvent::Type::CARD_INSERTED,
            std::make_shared<ScheduledCardSelectionsResponseAdapter>(
                cardSelectionResponses));

    } catch (const ReaderBrokenCommunicationException& e) {
        /* Notify the reader communication failure with the exception handler */
        const auto rce = std::make_shared<ReaderCommunicationException>(
            READER_MONITORING_ERROR, e);
        getObservationExceptionHandler()->onReaderObservationError(
            getPluginName(), getName(), rce);

    } catch (const CardBrokenCommunicationException& e) {
        /*
         * The last transmission failed, close the logical and physical channels
         */
        closeLogicalAndPhysicalChannelsSilently();

        /*
         * The card was removed or not read correctly, no exception raising or
         * event notification, just log.
         */
        mLogger->warn(
            "[reader=%] Failed to process card selection scenario [reason=%]\n",
            getName(),
            e.getMessage());
    }

    /*
     * Here we close the physical channel in case it was opened for a card
     * excluded by the selection scenario.
     */
    try {
        mObservableReaderSpi->closePhysicalChannel();

    } catch (const ReaderIOException& e) {
        /* Notify the reader communication failure with the exception handler */
        const auto rce = std::make_shared<ReaderCommunicationException>(
            READER_MONITORING_ERROR, e);
        getObservationExceptionHandler()->onReaderObservationError(
            getPluginName(), getName(), rce);
    }

    /* No event returned */
    return nullptr;
}

bool
ObservableLocalReaderAdapter::hasACardMatched(
    const std::vector<std::shared_ptr<CardSelectionResponseApi>>&
        cardSelectionResponses)
{
    if (std::any_of(
            cardSelectionResponses.begin(),
            cardSelectionResponses.end(),
            [](const std::shared_ptr<CardSelectionResponseApi>&
                   cardSelectionResponse) {
                return cardSelectionResponse != nullptr
                       && cardSelectionResponse->hasMatched();
            })) {
        mLogger->trace(
            "[reader=%] A default selection case matched\n", getName());
        return true;
    }

    return false;
}

void
ObservableLocalReaderAdapter::processCardRemoved()
{
    /* RL-DET-REMNOTIF.1 */
    closeLogicalAndPhysicalChannelsSilently();
    if (mIsCardRemovedEventNotificationEnabled) {
        notifyObservers(
            std::make_shared<ReaderEventAdapter>(
                getPluginName(),
                getName(),
                CardReaderEvent::Type::CARD_REMOVED,
                nullptr));
    }
}

void
ObservableLocalReaderAdapter::switchState(const MonitoringState stateId)
{
    mStateService->switchState(stateId);
}

void
ObservableLocalReaderAdapter::notifyObservers(
    const std::shared_ptr<CardReaderEvent> event)
{
    mLogger->debug(
        "[reader=%] Notifying observers [eventType=%, observerCount=%]\n",
        getName(),
        event->getType(),
        countObservers());

    for (const auto& observer : mObservationManager->getObservers()) {
        notifyObserver(observer, event);
    }

    mLogger->debug("[reader=%] Observers notified\n", getName());
}

void
ObservableLocalReaderAdapter::notifyObserver(
    std::shared_ptr<CardReaderObserverSpi> observer,
    const std::shared_ptr<CardReaderEvent> event)
{
    try {
        observer->onReaderEvent(event);

    } catch (const Exception& e) {
        try {
            mObservationManager->getObservationExceptionHandler()
                ->onReaderObservationError(
                    getPluginName(), getName(), std::make_shared<Exception>(e));

        } catch (const Exception& e2) {
            mLogger->error(
                "[reader=%] Failed to notify observer [reason=%]\n",
                getName(),
                e.getMessage());
            mLogger->error(
                "[reader=%] Failed to notify observation exception handler "
                "[reason=%]\n",
                getName(),
                e2.getMessage());
        }

    } catch (const std::exception& e) {
        try {
            mObservationManager->getObservationExceptionHandler()
                ->onReaderObservationError(
                    getPluginName(),
                    getName(),
                    std::make_shared<std::runtime_error>(e.what()));
        } catch (const std::exception& e2) {
            mLogger->error(
                "Reader [%] failed to notify observer: %\n",
                getName(),
                e.what());
            mLogger->error(
                "Reader [%] failed to notify observation exception handler: "
                "%\n",
                getName(),
                e2.what());
        }
    }
}

void
ObservableLocalReaderAdapter::scheduleCardSelectionScenario(
    std::shared_ptr<CardSelectionScenarioAdapter> cardSelectionScenario,
    const NotificationMode notificationMode)
{
    mCardSelectionScenario = cardSelectionScenario;
    mNotificationMode = notificationMode;
}

void
ObservableLocalReaderAdapter::doUnregister()
{
    try {
        stopCardDetection();
    } catch (const Exception& e) {
        mLogger->warn(
            "[reader=%] Failed to stop card monitoring [reason=%]\n",
            getName(),
            e.getMessage());
    }

    /* Finally */
    mStateService->shutdown();

    notifyObservers(
        std::make_shared<ReaderEventAdapter>(
            getPluginName(),
            getName(),
            CardReaderEvent::Type::UNAVAILABLE,
            nullptr));
    clearObservers();
    LocalReaderAdapter::doUnregister();
}

bool
ObservableLocalReaderAdapter::isCardPresent()
{
    checkStatus();

    if (LocalReaderAdapter::isCardPresent()) {
        return true;
    } else {
        /*
         * If the card is no longer present but one of the channels is still
         * open, then the card removal sequence is initiated.
         */
        if (isLogicalChannelOpen()
            || mObservableReaderSpi->isPhysicalChannelOpen()) {
            processCardRemoved();
        }

        return false;
    }
}

void
ObservableLocalReaderAdapter::addObserver(
    std::shared_ptr<CardReaderObserverSpi> observer)
{
    checkStatus();
    mObservationManager->addObserver(observer);
}

void
ObservableLocalReaderAdapter::removeObserver(
    std::shared_ptr<CardReaderObserverSpi> observer)
{
    Assert::getInstance().notNull(observer, "observer");

    if (Arrays::contains(mObservationManager->getObservers(), observer)) {
        mObservationManager->removeObserver(observer);
    }
}

int
ObservableLocalReaderAdapter::countObservers() const
{
    return mObservationManager->countObservers();
}

void
ObservableLocalReaderAdapter::clearObservers()
{
    mObservationManager->clearObservers();
}

void
ObservableLocalReaderAdapter::startCardDetection(
    const DetectionMode detectionMode)
{
    /* RL-DET-REMCTRL.1 */
    checkStatus();

    mLogger->info(
        "[reader=%] Starting card monitoring [detectionMode=%]\n",
        getName(),
        detectionMode);

    mDetectionMode = detectionMode;
    mStateService->onEvent(InternalEvent::START_DETECT);
}

void
ObservableLocalReaderAdapter::stopCardDetection()
{
    /* RL-DET-REMCTRL.1 */
    mLogger->info("[reader=%] Stopping card monitoring\n", getName());

    mStateService->onEvent(InternalEvent::STOP_DETECT);
}

void
ObservableLocalReaderAdapter::finalizeCardProcessing()
{
    mLogger->info("[reader=%] Starting card removal sequence\n", getName());

    mStateService->onEvent(InternalEvent::CARD_PROCESSED);
}

void
ObservableLocalReaderAdapter::setReaderObservationExceptionHandler(
    std::shared_ptr<CardReaderObservationExceptionHandlerSpi> exceptionHandler)
{
    checkStatus();
    mObservationManager->setObservationExceptionHandler(exceptionHandler);
}

void
ObservableLocalReaderAdapter::onCardInserted()
{
    mStateService->onEvent(InternalEvent::CARD_INSERTED);
}

void
ObservableLocalReaderAdapter::onCardRemoved()
{
    mStateService->onEvent(InternalEvent::CARD_REMOVED);
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
