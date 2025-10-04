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

#include "keyple/core/service/CardSelectionManagerAdapter.hpp"

#include <memory>
#include <string>
#include <vector>

#include "keyple/core/service/AbstractReaderAdapter.hpp"
#include "keyple/core/service/CardSelectionResultAdapter.hpp"
#include "keyple/core/service/CardSelectionScenarioAdapter.hpp"
#include "keyple/core/service/ObservableLocalReaderAdapter.hpp"
#include "keyple/core/service/ScheduledCardSelectionsResponseAdapter.hpp"
#include "keyple/core/util/KeypleAssert.hpp"
#include "keyple/core/util/cpp/exception/IllegalArgumentException.hpp"
#include "keyple/core/util/cpp/exception/IllegalStateException.hpp"
#include "keyple/core/util/cpp/exception/UnsupportedOperationException.hpp"
#include "keypop/card/CardBrokenCommunicationException.hpp"
#include "keypop/card/CardSelectionResponseApi.hpp"
#include "keypop/card/ParseException.hpp"
#include "keypop/card/ReaderBrokenCommunicationException.hpp"
#include "keypop/card/spi/CardSelectionExtensionSpi.hpp"
#include "keypop/reader/CardCommunicationException.hpp"
#include "keypop/reader/ReaderCommunicationException.hpp"
#include "keypop/reader/cpp/CardSelectorBase.hpp"
#include "keypop/reader/selection/InvalidCardResponseException.hpp"
#include "keypop/reader/selection/spi/SmartCard.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::util::Assert;
using keyple::core::util::cpp::exception::IllegalArgumentException;
using keyple::core::util::cpp::exception::IllegalStateException;
using keyple::core::util::cpp::exception::UnsupportedOperationException;
using keypop::card::CardBrokenCommunicationException;
using keypop::card::CardSelectionResponseApi;
using keypop::card::ParseException;
using keypop::card::ReaderBrokenCommunicationException;
using keypop::card::spi::CardSelectionExtensionSpi;
using keypop::reader::CardCommunicationException;
using keypop::reader::ReaderCommunicationException;
using keypop::reader::cpp::CardSelectorBase;
using keypop::reader::selection::InvalidCardResponseException;
using keypop::reader::selection::spi::SmartCard;

const std::string CardSelectionManagerAdapter::MULTI_SELECTION_PROCESSING
    = "multiSelectionProcessing";
const std::string CardSelectionManagerAdapter::CHANNEL_CONTROL
    = "channelControl";
const std::string CardSelectionManagerAdapter::CARD_SELECTORS_TYPES
    = "cardSelectorsTypes";
const std::string CardSelectionManagerAdapter::CARD_SELECTORS = "cardSelectors";
const std::string CardSelectionManagerAdapter::CARD_SELECTIONS_TYPES
    = "cardSelectionsTypes";
const std::string CardSelectionManagerAdapter::CARD_SELECTIONS
    = "cardSelections";
const std::string CardSelectionManagerAdapter::DEFAULT_CARD_SELECTIONS
    = "defaultCardSelections";

CardSelectionManagerAdapter::CardSelectionManagerAdapter()
: mMultiSelectionProcessing(MultiSelectionProcessing::FIRST_MATCH)
, mChannelControl(ChannelControl::KEEP_OPEN)
{
}

void
CardSelectionManagerAdapter::setMultipleSelectionMode()
{
    mMultiSelectionProcessing = MultiSelectionProcessing::PROCESS_ALL;
}

int
CardSelectionManagerAdapter::prepareSelection(
    const std::shared_ptr<CardSelectorBase> cardSelector,
    const std::shared_ptr<CardSelectionExtension> cardSelectionExtension)
{
    Assert::getInstance()
        .notNull(cardSelector, "cardSelector")
        .notNull(cardSelectionExtension, "cardSelectionExtension");

    auto cardSelectionExtensionSpi
        = std::dynamic_pointer_cast<CardSelectionExtensionSpi>(
            cardSelectionExtension);
    if (!cardSelectionExtensionSpi) {
        throw IllegalArgumentException(
            "The provided 'cardSelectionExtension' must be an instance"
            " of 'CardSelectionExtensionSpi");
    }

    /* Keep the selection request */
    mCardSelectors.push_back(cardSelector);
    mCardSelections.push_back(
        std::dynamic_pointer_cast<CardSelectionExtensionSpi>(
            cardSelectionExtension));
    mCardSelectionRequests.push_back(
        std::dynamic_pointer_cast<CardSelectionExtensionSpi>(
            cardSelectionExtension)
            ->getCardSelectionRequest());

    /* Return the selection index (starting at 0) */
    return static_cast<int>(mCardSelections.size()) - 1;
}

void
CardSelectionManagerAdapter::prepareReleaseChannel()
{
    mChannelControl = ChannelControl::CLOSE_AFTER;
}

const std::shared_ptr<CardSelectionResult>
CardSelectionManagerAdapter::processCardSelectionScenario(
    std::shared_ptr<CardReader> reader)
{
    Assert::getInstance().notNull(reader, "reader");

    /* Communicate with the card to make the actual selection */
    std::vector<std::shared_ptr<CardSelectionResponseApi>>
        cardSelectionResponses;

    try {
        cardSelectionResponses
            = std::dynamic_pointer_cast<AbstractReaderAdapter>(reader)
                  ->transmitCardSelectionRequests(
                      mCardSelectors,
                      mCardSelectionRequests,
                      mMultiSelectionProcessing,
                      mChannelControl);
    } catch (const ReaderBrokenCommunicationException& e) {
        throw ReaderCommunicationException(
            e.getMessage(),
            std::make_shared<ReaderBrokenCommunicationException>(e));
    } catch (const CardBrokenCommunicationException& e) {
        throw CardCommunicationException(
            e.getMessage(),
            std::make_shared<CardBrokenCommunicationException>(e));
    }

    /* Analyze the received responses */
    return processCardSelectionResponses(cardSelectionResponses);
}

void
CardSelectionManagerAdapter::scheduleCardSelectionScenario(
    std::shared_ptr<ObservableCardReader> observableCardReader,
    const NotificationMode notificationMode)
{
    Assert::getInstance().notNull(observableCardReader, "observableCardReader");

    auto cardSelectionScenario = std::make_shared<CardSelectionScenarioAdapter>(
        mCardSelectors,
        mCardSelectionRequests,
        mMultiSelectionProcessing,
        mChannelControl);

    auto local = std::dynamic_pointer_cast<ObservableLocalReaderAdapter>(
        observableCardReader);
    //        auto remote
    //        =std::dynamic_pointer_cast<ObservableRemoteReaderAdapter>(observableCardReader);
    if (local) {
        local->scheduleCardSelectionScenario(
            cardSelectionScenario, notificationMode);
        //        } else if (remote) {
        //            remote->scheduleCardSelectionScenario(cardSelectionScenario,
        //            notificationMode);
    } else {
        throw IllegalArgumentException("Not a Keyple reader implementation");
    }
}

const std::shared_ptr<CardSelectionResult>
CardSelectionManagerAdapter::parseScheduledCardSelectionsResponse(
    const std::shared_ptr<ScheduledCardSelectionsResponse>
        scheduledCardSelectionsResponse)
{
    Assert::getInstance().notNull(
        scheduledCardSelectionsResponse, "scheduledCardSelectionsResponse");

    return processCardSelectionResponses(
        std::static_pointer_cast<ScheduledCardSelectionsResponseAdapter>(
            scheduledCardSelectionsResponse)
            ->getCardSelectionResponses());
}

const std::string
CardSelectionManagerAdapter::exportProcessedCardSelectionScenario() const
{
    /* TODO: necessary when implementing distributed mode */

    throw IllegalStateException("not implemented");
}

const std::shared_ptr<CardSelectionResult>
CardSelectionManagerAdapter::importProcessedCardSelectionScenario(
    const std::string& /*processedCardSelectionScenario*/) const
{
    /* TODO: necessary when implementing distributed mode */

    throw IllegalStateException("not implemented");
}

const std::shared_ptr<CardSelectionResult>
CardSelectionManagerAdapter::processCardSelectionResponses(
    const std::vector<std::shared_ptr<CardSelectionResponseApi>>&
        cardSelectionResponses)
{
    Assert::getInstance().isInRange(
        cardSelectionResponses.size(),
        1,
        mCardSelections.size(),
        "cardSelectionResponses");

    auto cardSelectionsResult = std::make_shared<CardSelectionResultAdapter>();
    int index = 0;

    for (const auto& cardSelectionResponse : cardSelectionResponses) {
        if (cardSelectionResponse->hasMatched()) {
            /* Invoke the parse method defined by the card extension to retrieve
             * the smart card */
            std::shared_ptr<SmartCard> smartCard = nullptr;
            try {
                smartCard = std::dynamic_pointer_cast<SmartCard>(
                    mCardSelections[index]->parse(cardSelectionResponse));

            } catch (const ParseException& e) {
                throw InvalidCardResponseException(
                    "Error occurred while parsing the card response: "
                        + e.getMessage(),
                    std::make_shared<ParseException>(e));

            } catch (const UnsupportedOperationException&) {
                mLogger->warn("Unable to parse card selection responses due to "
                              "missing card "
                              "extensions in runtime environment");
                cardSelectionsResult
                    = std::make_shared<CardSelectionResultAdapter>();  // Empty
                                                                       // result
                break;
            }

            cardSelectionsResult->addSmartCard(index, smartCard);
        }

        index++;
    }

    mCardSelectionResponses = cardSelectionResponses;

    return cardSelectionsResult;
}

const std::string
CardSelectionManagerAdapter::exportCardSelectionScenario() const
{
    /* TODO: necessary when implementing distributed mode */

    return "";
}

int
CardSelectionManagerAdapter::importCardSelectionScenario(
    const std::string& cardSelectionScenario)
{
    /* TODO: necessary when implementing distributed mode */

    (void)cardSelectionScenario;

    return 0;
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
