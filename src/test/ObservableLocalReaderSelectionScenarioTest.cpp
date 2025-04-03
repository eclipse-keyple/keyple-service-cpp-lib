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

#include <memory>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keyple/core/service/CardSelectionScenarioAdapter.hpp"
#include "keyple/core/service/ObservableLocalReaderAdapter.hpp"
#include "keyple/core/service/ScheduledCardSelectionsResponseAdapter.hpp"
#include "keyple/core/util/cpp/LoggerFactory.hpp"
#include "keypop/card/CardBrokenCommunicationException.hpp"
#include "keypop/card/ReaderBrokenCommunicationException.hpp"
#include "keypop/reader/cpp/CardSelectorBase.hpp"

/* Mock */
#include "mock/CardReaderObservationExceptionHandlerSpiMock.hpp"
#include "mock/CardResponseApiMock.hpp"
#include "mock/CardSelectionRequestSpiMock.hpp"
#include "mock/CardSelectionResponseApiMock.hpp"
#include "mock/ObservableLocalReaderAdapterMock.hpp"
#include "mock/ObservableReaderAsynchronousSpiMock.hpp"
#include "mock/ReaderObserverSpiMock.hpp"

/* Util */
#include "util/ObservableLocalReaderSuite.hpp"
#include "util/ReaderAdapterTestUtils.hpp"

using keyple::core::service::CardSelectionScenarioAdapter;
using keyple::core::service::ScheduledCardSelectionsResponseAdapter;
using keyple::core::util::cpp::LoggerFactory;
using keypop::card::CardBrokenCommunicationException;
using keypop::card::ReaderBrokenCommunicationException;
using keypop::reader::cpp::CardSelectorBase;

using testing::_;
using testing::Return;
using testing::ReturnRef;
using testing::Throw;

static const std::string PLUGIN_NAME = "plugin";

class ObservableLocalReaderSelectionScenarioTest { };
static const std::shared_ptr<Logger> logger = LoggerFactory::getLogger(
    typeid(ObservableLocalReaderSelectionScenarioTest));

static std::shared_ptr<ObservableLocalReaderAdapterMock> readerSpy;
static std::shared_ptr<ObservableReaderAsynchronousSpiMock> readerSpi;
static std::shared_ptr<ReaderObserverSpiMock> observer;
static std::shared_ptr<CardReaderObservationExceptionHandlerSpiMock> handler;
static std::shared_ptr<ObservableLocalReaderSuite> testSuite;

static std::shared_ptr<CardSelectorBase> cardSelector;
static std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequestSpi;
static std::shared_ptr<CardSelectionResponseApi> cardSelectionResponseApi;
static std::shared_ptr<CardResponseApi> cardResponseApi;
static std::shared_ptr<CardReaderEvent> event;

static void
setUp()
{
    readerSpi
        = std::make_shared<ObservableReaderAsynchronousSpiMock>(READER_NAME);
    handler = std::make_shared<CardReaderObservationExceptionHandlerSpiMock>();
    readerSpy = std::make_shared<ObservableLocalReaderAdapterMock>(
        readerSpi, PLUGIN_NAME);
    observer = std::make_shared<ReaderObserverSpiMock>(nullptr);
    testSuite = std::make_shared<ObservableLocalReaderSuite>(
        readerSpy, readerSpi, observer, handler, logger);
    cardSelectionRequestSpi = std::make_shared<CardSelectionRequestSpiMock>();
    cardSelectionResponseApi = std::make_shared<CardSelectionResponseApiMock>();
    cardResponseApi = std::make_shared<CardResponseApiMock>();

    /* Test with event notification executor service */
    readerSpy->doRegister();
    readerSpy->setReaderObservationExceptionHandler(handler);
}

static void
tearDown()
{
    cardResponseApi.reset();
    cardSelectionResponseApi.reset();
    cardSelectionRequestSpi.reset();
    testSuite.reset();
    observer.reset();
    readerSpy.reset();
    handler.reset();
    readerSpi.reset();
    event.reset();
}

/**
 * Configure card selection scenario and mock selection responses
 *
 * @param cardSelectionResponse
 * @param notificationMode
 * @throws Exception
 */
static void
mockReaderWithSelectionResponses(
    const std::vector<std::shared_ptr<CardSelectionResponseApi>>&
        cardSelectionResponse,
    const ObservableCardReader::NotificationMode notificationMode)
{
    EXPECT_CALL(*readerSpy.get(), transmitCardSelectionRequests(_, _, _, _))
        .WillRepeatedly(Return(cardSelectionResponse));

    const std::vector<std::shared_ptr<CardSelectorBase>> selector
        = {cardSelector};
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> requests
        = {cardSelectionRequestSpi};

    readerSpy->scheduleCardSelectionScenario(
        std::make_shared<CardSelectionScenarioAdapter>(
            selector,
            requests,
            MultiSelectionProcessing::PROCESS_ALL,
            ChannelControl::KEEP_OPEN),
        notificationMode);
}

/**
 * assert event is of type
 *
 * @param type
 */
static void
assertEventIs(const CardReaderEvent::Type type)
{
    ASSERT_NE(event, nullptr);
    ASSERT_EQ(event->getType(), type);
    ASSERT_EQ(event->getReaderName(), READER_NAME);
}

TEST(
    ObservableLocalReaderSelectionScenarioTest,
    process_card_with_no_scenario_return_cardInserted)
{
    setUp();

    /* Scenario : no scenario scheduled */

    event = readerSpy->processCardInserted();

    /* Verify ScheduledCardSelectionsResponseAdapter in event */
    assertEventIs(CardReaderEvent::Type::CARD_INSERTED);

    tearDown();
}

TEST(
    ObservableLocalReaderSelectionScenarioTest,
    process_card_with_matching_selections_return_cardMatched)
{
    /* Scenario : selection match */

    setUp();

    ReaderAdapterTestUtils utils;

    mockReaderWithSelectionResponses(
        utils.MATCHING_RESPONSES, ObservableCardReader::MATCHED_ONLY);

    event = readerSpy->processCardInserted();

    /* Verify ScheduledCardSelectionsResponseAdapter in event */
    assertEventIs(CardReaderEvent::Type::CARD_MATCHED);
    ASSERT_NE(event->getScheduledCardSelectionsResponse(), nullptr);

    utils.reset();

    tearDown();
}

TEST(
    ObservableLocalReaderSelectionScenarioTest,
    process_card_with_no_matching_selections_with_Always_return_CardInserted)
{
    /* Scenario : selection no match */

    setUp();

    ReaderAdapterTestUtils utils;

    mockReaderWithSelectionResponses(
        utils.NOT_MATCHING_RESPONSES, ObservableCardReader::ALWAYS);

    event = readerSpy->processCardInserted();

    assertEventIs(CardReaderEvent::Type::CARD_INSERTED);

    utils.reset();

    tearDown();
}

TEST(
    ObservableLocalReaderSelectionScenarioTest,
    process_card_with_no_matching_selections_with_MatchedOnly_return_null)
{
    /* Scenario : selection no match */

    setUp();

    ReaderAdapterTestUtils utils;

    mockReaderWithSelectionResponses(
        utils.NOT_MATCHING_RESPONSES, ObservableCardReader::MATCHED_ONLY);

    event = readerSpy->processCardInserted();

    ASSERT_EQ(event, nullptr);

    utils.reset();

    tearDown();
}

TEST(
    ObservableLocalReaderSelectionScenarioTest,
    process_card_with_multi_matching_selections_return_CardMatched)
{
    /* Scenario : first no match, second match, third match */

    setUp();

    ReaderAdapterTestUtils utils;

    mockReaderWithSelectionResponses(
        utils.MULTI_MATCHING_RESPONSES, ObservableCardReader::MATCHED_ONLY);

    event = readerSpy->processCardInserted();

    assertEventIs(CardReaderEvent::Type::CARD_MATCHED);
    ASSERT_NE(event->getScheduledCardSelectionsResponse(), nullptr);

    auto response = event->getScheduledCardSelectionsResponse();
    ASSERT_NE(
        std::dynamic_pointer_cast<ScheduledCardSelectionsResponseAdapter>(
            response),
        nullptr);

    auto adapter
        = std::dynamic_pointer_cast<ScheduledCardSelectionsResponseAdapter>(
            response);
    auto responses = adapter->getCardSelectionResponses();
    ASSERT_EQ(static_cast<int>(responses.size()), 3);

    utils.reset();

    tearDown();
}

TEST(
    ObservableLocalReaderSelectionScenarioTest,
    process_card_with_multi_not_matching_selections_with_matchedOnly_return_null)  // NOLINT
{
    /* Scenario : first no match, second no match */

    setUp();

    ReaderAdapterTestUtils utils;

    mockReaderWithSelectionResponses(
        utils.MULTI_NOT_MATCHING_RESPONSES, ObservableCardReader::MATCHED_ONLY);

    event = readerSpy->processCardInserted();

    ASSERT_EQ(event, nullptr);

    utils.reset();

    tearDown();
}

TEST(
    ObservableLocalReaderSelectionScenarioTest,
    process_card_with_throw_reader_exception_scenario_return_null)
{
    setUp();

    EXPECT_CALL(*handler.get(), onReaderObservationError(_, _, _))
        .WillRepeatedly(Return());
    EXPECT_CALL(*readerSpy.get(), transmitCardSelectionRequests(_, _, _, _))
        .WillRepeatedly(Throw(ReaderBrokenCommunicationException(
            nullptr, true, "", std::make_shared<RuntimeException>())));

    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> req
        = {cardSelectionRequestSpi};
    const std::vector<std::shared_ptr<CardSelectorBase>> selector
        = {cardSelector};
    auto adapter = std::make_shared<CardSelectionScenarioAdapter>(
        selector,
        req,
        MultiSelectionProcessing::PROCESS_ALL,
        ChannelControl::KEEP_OPEN);

    readerSpy->scheduleCardSelectionScenario(
        adapter, ObservableCardReader::MATCHED_ONLY);

    event = readerSpy->processCardInserted();

    ASSERT_EQ(event, nullptr);

    tearDown();
}

TEST(
    ObservableLocalReaderSelectionScenarioTest,
    process_card_with_throw_card_exception_scenario_return_null)
{
    setUp();

    EXPECT_CALL(*handler.get(), onReaderObservationError(_, _, _))
        .WillRepeatedly(Return());
    EXPECT_CALL(*readerSpy.get(), transmitCardSelectionRequests(_, _, _, _))
        .WillRepeatedly(Throw(CardBrokenCommunicationException(
            nullptr, true, "", std::make_shared<RuntimeException>())));

    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> req
        = {cardSelectionRequestSpi};
    const std::vector<std::shared_ptr<CardSelectorBase>> selector
        = {cardSelector};
    auto adapter = std::make_shared<CardSelectionScenarioAdapter>(
        selector,
        req,
        MultiSelectionProcessing::PROCESS_ALL,
        ChannelControl::KEEP_OPEN);

    readerSpy->scheduleCardSelectionScenario(
        adapter, ObservableCardReader::MATCHED_ONLY);

    event = readerSpy->processCardInserted();

    ASSERT_EQ(event, nullptr);

    tearDown();
}
