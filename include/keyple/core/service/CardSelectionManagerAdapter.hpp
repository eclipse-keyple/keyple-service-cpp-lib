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

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "keyple/core/service/AbstractReaderAdapter.hpp"
#include "keyple/core/service/CardSelectionResultAdapter.hpp"
#include "keyple/core/service/CardSelectionScenarioAdapter.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/MultiSelectionProcessing.hpp"
#include "keyple/core/service/ObservableLocalReaderAdapter.hpp"
#include "keyple/core/service/ScheduledCardSelectionsResponseAdapter.hpp"
#include "keyple/core/util/KeypleAssert.hpp"
#include "keyple/core/util/cpp/Logger.hpp"
#include "keyple/core/util/cpp/LoggerFactory.hpp"
#include "keyple/core/util/cpp/exception/IllegalArgumentException.hpp"
#include "keyple/core/util/cpp/exception/IllegalStateException.hpp"
#include "keyple/core/util/cpp/exception/UnsupportedOperationException.hpp"
#include "keypop/card/CardBrokenCommunicationException.hpp"
#include "keypop/card/CardSelectionResponseApi.hpp"
#include "keypop/card/ChannelControl.hpp"
#include "keypop/card/ParseException.hpp"
#include "keypop/card/ReaderBrokenCommunicationException.hpp"
#include "keypop/card/spi/CardSelectionExtensionSpi.hpp"
#include "keypop/card/spi/CardSelectionRequestSpi.hpp"
#include "keypop/reader/CardCommunicationException.hpp"
#include "keypop/reader/CardReader.hpp"
#include "keypop/reader/ObservableCardReader.hpp"
#include "keypop/reader/ReaderCommunicationException.hpp"
#include "keypop/reader/selection/CardSelectionManager.hpp"
#include "keypop/reader/selection/CardSelectionResult.hpp"
#include "keypop/reader/selection/InvalidCardResponseException.hpp"
#include "keypop/reader/selection/ScheduledCardSelectionsResponse.hpp"
#include "keypop/reader/selection/spi/CardSelectionExtension.hpp"
#include "keypop/reader/selection/spi/SmartCard.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::util::Assert;
using keyple::core::util::cpp::Logger;
using keyple::core::util::cpp::LoggerFactory;
using keyple::core::util::cpp::exception::IllegalArgumentException;
using keyple::core::util::cpp::exception::IllegalStateException;
using keyple::core::util::cpp::exception::UnsupportedOperationException;
using keypop::card::CardBrokenCommunicationException;
using keypop::card::CardSelectionResponseApi;
using keypop::card::ChannelControl;
using keypop::card::ParseException;
using keypop::card::ReaderBrokenCommunicationException;
using keypop::card::spi::CardSelectionExtensionSpi;
using keypop::card::spi::CardSelectionRequestSpi;
using keypop::reader::CardCommunicationException;
using keypop::reader::CardReader;
using keypop::reader::ObservableCardReader;
using keypop::reader::ReaderCommunicationException;
using keypop::reader::selection::CardSelectionManager;
using keypop::reader::selection::CardSelectionResult;
using keypop::reader::selection::InvalidCardResponseException;
using keypop::reader::selection::ScheduledCardSelectionsResponse;
using keypop::reader::selection::spi::CardSelectionExtension;
using keypop::reader::selection::spi::SmartCard;

using DetectionMode = ObservableCardReader::DetectionMode;
using NotificationMode = ObservableCardReader::NotificationMode;

/**
 * Implementation of the CardSelectionManager.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API CardSelectionManagerAdapter final
: public CardSelectionManager {
public:
    /**
     * Creates an instance of the service with which the selection stops as soon
     * as a card matches a selection case.
     *
     * @since 2.0.0
     */
    CardSelectionManagerAdapter();

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void setMultipleSelectionMode() override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    int prepareSelection(
        const std::shared_ptr<CardSelectorBase> cardSelector,
        const std::shared_ptr<CardSelectionExtension> cardSelectionExtension)
        override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void prepareReleaseChannel() override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::shared_ptr<CardSelectionResult>
    processCardSelectionScenario(std::shared_ptr<CardReader> reader) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void scheduleCardSelectionScenario(
        std::shared_ptr<ObservableCardReader> observableCardReader,
        const NotificationMode notificationMode) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::shared_ptr<CardSelectionResult>
    parseScheduledCardSelectionsResponse(
        const std::shared_ptr<ScheduledCardSelectionsResponse>
            scheduledCardSelectionsResponse) final;

    /**
     * {@inheritDoc}
     *
     * @since 2.3.0
     */
    const std::string exportProcessedCardSelectionScenario() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.3.0
     */
    const std::shared_ptr<CardSelectionResult>
    importProcessedCardSelectionScenario(
        const std::string& processedCardSelectionScenario) const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.1.1
     */
    const std::string exportCardSelectionScenario() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.1.1
     */
    int importCardSelectionScenario(
        const std::string& cardSelectionScenario) override;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger
        = LoggerFactory::getLogger(typeid(CardSelectionManagerAdapter));

    /**
     *
     */
    static const std::string MULTI_SELECTION_PROCESSING;
    static const std::string CHANNEL_CONTROL;
    static const std::string CARD_SELECTORS_TYPES;
    static const std::string CARD_SELECTORS;
    static const std::string CARD_SELECTIONS_TYPES;
    static const std::string CARD_SELECTIONS;
    static const std::string DEFAULT_CARD_SELECTIONS;

    /**
     *
     */
    std::vector<std::shared_ptr<CardSelectorBase>> mCardSelectors;

    /**
     *
     */
    std::vector<std::shared_ptr<CardSelectionExtensionSpi>> mCardSelections;

    /**
     *
     */
    std::vector<std::shared_ptr<CardSelectionRequestSpi>>
        mCardSelectionRequests;

    /**
     *
     */
    std::vector<std::shared_ptr<CardSelectionResponseApi>>
        mCardSelectionResponses;

    /**
     *
     */
    MultiSelectionProcessing mMultiSelectionProcessing;

    /**
     *
     */
    ChannelControl mChannelControl = ChannelControl::KEEP_OPEN;

    /**
     * Analyzes the responses received in return of the execution of a card
     * selection scenario and returns the CardSelectionResult.
     *
     * @param cardSelectionResponses The card selection responses.
     * @return A not null reference.
     * @throw IllegalArgumentException If the list is null or empty.
     */
    const std::shared_ptr<CardSelectionResult> processCardSelectionResponses(
        const std::vector<std::shared_ptr<CardSelectionResponseApi>>&
            cardSelectionResponses);
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
