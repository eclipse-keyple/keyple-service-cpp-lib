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

#include "keyple/core/service/CardRemovalPassiveMonitoringJobAdapter.hpp"

#include <memory>

#include "keyple/core/plugin/ReaderIOException.hpp"
#include "keyple/core/plugin/TaskCanceledException.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/processing/CardPresenceMonitorBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/processing/WaitForCardRemovalDuringProcessingBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/removal/CardRemovalWaiterBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/removal/WaitForCardRemovalBlockingSpi.hpp"
#include "keyple/core/service/ObservableLocalReaderAdapter.hpp"
#include "keyple/core/util/cpp/exception/RuntimeException.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::plugin::ReaderIOException;
using keyple::core::plugin::TaskCanceledException;
using keyple::core::plugin::spi::reader::observable::state::processing::
    CardPresenceMonitorBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::processing::
    WaitForCardRemovalDuringProcessingBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::removal::
    CardRemovalWaiterBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::removal::
    WaitForCardRemovalBlockingSpi;
using keyple::core::util::cpp::exception::RuntimeException;

using InternalEvent = ObservableLocalReaderAdapter::InternalEvent;

/* CARD REMOVAL PASSIVE MONITORING JOB
 * ---------------------------------------------------------- */

CardRemovalPassiveMonitoringJobAdapter::CardRemovalPassiveMonitoringJob ::
    CardRemovalPassiveMonitoringJob(
        std::shared_ptr<AbstractObservableStateAdapter> monitoringState,
        CardRemovalPassiveMonitoringJobAdapter* parent)
: Job("CardRemovalPassiveMonitoringJobAdapter")
, mMonitoringState(monitoringState)
, mParent(parent)
{
}

void
CardRemovalPassiveMonitoringJobAdapter::CardRemovalPassiveMonitoringJob::
    execute()
{
    auto cardRemovalWaiterBlockingSpi
        = std::dynamic_pointer_cast<CardRemovalWaiterBlockingSpi>(
            mParent->mReaderSpi);
    auto waitForCardRemovalBlockingSpi
        = std::dynamic_pointer_cast<WaitForCardRemovalBlockingSpi>(
            mParent->mReaderSpi);
    auto cardPresenceMonitorBlockingSpi
        = std::dynamic_pointer_cast<CardPresenceMonitorBlockingSpi>(
            mParent->mReaderSpi);
    auto waitForCardRemovalDuringProcessingBlockingSpi
        = std::dynamic_pointer_cast<
            WaitForCardRemovalDuringProcessingBlockingSpi>(mParent->mReaderSpi);

    try {
        if (cardRemovalWaiterBlockingSpi) {
            cardRemovalWaiterBlockingSpi->waitForCardRemoval();
        } else if (waitForCardRemovalBlockingSpi) {
            waitForCardRemovalBlockingSpi->waitForCardRemoval();
        } else if (cardPresenceMonitorBlockingSpi) {
            cardPresenceMonitorBlockingSpi
                ->monitorCardPresenceDuringProcessing();
        } else if (waitForCardRemovalDuringProcessingBlockingSpi) {
            waitForCardRemovalDuringProcessingBlockingSpi
                ->waitForCardRemovalDuringProcessing();
        }
    } catch (const ReaderIOException& e) {
        /* Just warn as it can be a disconnection of the reader. */
        mParent->mLogger->warn(
            "Monitoring job error while processing card removal event on "
            "reader [%]: %\n",
            mParent->getReader()->getName(),
            e.getMessage());
    } catch (const TaskCanceledException& e) {
        mParent->mLogger->warn(
            "Monitoring job process cancelled: %\n", e.getMessage());
    } catch (const RuntimeException& e) {
        mParent->getReader()
            ->getObservationExceptionHandler()
            ->onReaderObservationError(
                mParent->getReader()->getPluginName(),
                mParent->getReader()->getName(),
                std::make_shared<RuntimeException>(e));
    }

    /* Finally */
    mMonitoringState->onEvent(InternalEvent::CARD_REMOVED);
}

/* CARD REMOVAL PASSIVE MONITORING JOB ADAPTER
 * -------------------------------------------------- */

CardRemovalPassiveMonitoringJobAdapter::CardRemovalPassiveMonitoringJobAdapter(
    ObservableLocalReaderAdapter* reader)
: AbstractMonitoringJobAdapter(reader)
{
    mReaderSpi = reader->getObservableReaderSpi();
}

std::shared_ptr<Job>
CardRemovalPassiveMonitoringJobAdapter::getMonitoringJob(
    std::shared_ptr<AbstractObservableStateAdapter> monitoringState)
{
    return std::make_shared<CardRemovalPassiveMonitoringJob>(
        monitoringState, this);
}

void
CardRemovalPassiveMonitoringJobAdapter::stop()
{
    mLogger->trace("Stop monitoring job process\n");

    auto cardRemovalWaiterBlockingSpi
        = std::dynamic_pointer_cast<CardRemovalWaiterBlockingSpi>(mReaderSpi);
    auto waitForCardRemovalBlockingSpi
        = std::dynamic_pointer_cast<WaitForCardRemovalBlockingSpi>(mReaderSpi);
    auto cardPresenceMonitorBlockingSpi
        = std::dynamic_pointer_cast<CardPresenceMonitorBlockingSpi>(mReaderSpi);
    auto waitForCardRemovalDuringProcessingBlockingSpi
        = std::dynamic_pointer_cast<
            WaitForCardRemovalDuringProcessingBlockingSpi>(mReaderSpi);

    if (cardRemovalWaiterBlockingSpi) {
        cardRemovalWaiterBlockingSpi->stopWaitForCardRemoval();
    } else if (waitForCardRemovalBlockingSpi) {
        waitForCardRemovalBlockingSpi->stopWaitForCardRemoval();
    } else if (cardPresenceMonitorBlockingSpi) {
        cardPresenceMonitorBlockingSpi
            ->stopCardPresenceMonitoringDuringProcessing();
    } else if (waitForCardRemovalDuringProcessingBlockingSpi) {
        waitForCardRemovalDuringProcessingBlockingSpi
            ->stopWaitForCardRemovalDuringProcessing();
    }

    mLogger->trace("Monitoring job process stopped\n");
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
