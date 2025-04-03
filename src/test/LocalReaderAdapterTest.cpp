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

#include "keyple/core/common/KeypleReaderExtension.hpp"
#include "keyple/core/plugin/CardIOException.hpp"
#include "keyple/core/plugin/ReaderIOException.hpp"
#include "keyple/core/plugin/spi/reader/ReaderSpi.hpp"
#include "keyple/core/service/BasicCardSelectorAdapter.hpp"
#include "keyple/core/service/LocalConfigurableReaderAdapter.hpp"
#include "keyple/core/service/LocalReaderAdapter.hpp"
#include "keyple/core/service/MultiSelectionProcessing.hpp"
#include "keyple/core/service/SmartCardServiceProvider.hpp"
#include "keyple/core/util/HexUtil.hpp"
#include "keypop/card/CardBrokenCommunicationException.hpp"
#include "keypop/card/ChannelControl.hpp"
#include "keypop/card/ReaderBrokenCommunicationException.hpp"
#include "keypop/card/UnexpectedStatusWordException.hpp"
#include "keypop/reader/ReaderCommunicationException.hpp"
#include "keypop/reader/ReaderProtocolNotSupportedException.hpp"
#include "keypop/reader/cpp/CardSelectorBase.hpp"
#include "keypop/reader/selection/BasicCardSelector.hpp"
#include "keypop/reader/selection/CardSelector.hpp"
#include "keypop/reader/selection/IsoCardSelector.hpp"

/* Mock */
#include "mock/ApduRequestSpiMock.hpp"
#include "mock/CardRequestSpiMock.hpp"
#include "mock/CardSelectionRequestSpiMock.hpp"
#include "mock/ConfigurableReaderSpiMock.hpp"

/* Utils */
#include "util/ReaderAdapterTestUtils.hpp"

using keyple::core::common::KeypleReaderExtension;
using keyple::core::plugin::CardIOException;
using keyple::core::plugin::ReaderIOException;
using keyple::core::plugin::spi::reader::ReaderSpi;
using keyple::core::service::BasicCardSelectorAdapter;
using keyple::core::service::LocalConfigurableReaderAdapter;
using keyple::core::service::LocalReaderAdapter;
using keyple::core::service::MultiSelectionProcessing;
using keyple::core::service::SmartCardServiceProvider;
using keyple::core::util::HexUtil;
using keypop::card::CardBrokenCommunicationException;
using keypop::card::ChannelControl;
using keypop::card::ReaderBrokenCommunicationException;
using keypop::card::UnexpectedStatusWordException;
using keypop::reader::ReaderCommunicationException;
using keypop::reader::ReaderProtocolNotSupportedException;
using keypop::reader::cpp::CardSelectorBase;
using keypop::reader::selection::BasicCardSelector;
using keypop::reader::selection::CardSelector;
using keypop::reader::selection::IsoCardSelector;

using testing::_;
using testing::Return;
using testing::ReturnRef;
using testing::Throw;

static const std::string PLUGIN_NAME = "plugin";
static const std::string READER_NAME = "reader";
static const std::string CARD_PROTOCOL = "cardProtocol";
static const std::string OTHER_CARD_PROTOCOL = "otherCardProtocol";
static const std::string POWER_ON_DATA = "12345678";

static std::shared_ptr<ConfigurableReaderSpiMock> readerSpi;
static std::shared_ptr<CardSelectorBase> cardSelector;
static std::shared_ptr<CardSelectionRequestSpiMock> cardSelectionRequestSpi;
static std::shared_ptr<ApduRequestSpiMock> apduRequestSpi;
static std::shared_ptr<CardRequestSpiMock> cardRequestSpi;

static const std::vector<int> successfulStatusWords({0x9000});
static const std::vector<int> statusWords({0x9000, 0x6283});

/* ? */
std::vector<int> ok_sw = {0x9000};
const std::vector<uint8_t> ok_rapdu = {0x90, 0x00};
const std::vector<uint8_t> uint8_t_dummy;
const std::string info = "dummy info";
std::vector<std::shared_ptr<ApduRequestSpi>> apduRequests;

static bool mPhysicalChannelOpen;

static void
setUp()
{
    mPhysicalChannelOpen = false;

    readerSpi = std::make_shared<ConfigurableReaderSpiMock>();
    EXPECT_CALL(*readerSpi.get(), getName())
        .WillRepeatedly(ReturnRef(READER_NAME));
    EXPECT_CALL(*readerSpi.get(), checkCardPresence())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*readerSpi.get(), getPowerOnData())
        .WillRepeatedly(Return(POWER_ON_DATA));
    EXPECT_CALL(*readerSpi.get(), closePhysicalChannel()).WillRepeatedly([]() {
        mPhysicalChannelOpen = false;
    });
    EXPECT_CALL(*readerSpi.get(), openPhysicalChannel()).WillRepeatedly([]() {
        mPhysicalChannelOpen = true;
    });
    EXPECT_CALL(*readerSpi.get(), isPhysicalChannelOpen())
        .WillRepeatedly(Return(mPhysicalChannelOpen));
    EXPECT_CALL(*readerSpi.get(), isContactless()).WillRepeatedly(Return(true));
    EXPECT_CALL(*readerSpi.get(), transmitApdu(_))
        .WillRepeatedly(Return(HexUtil::toByteArray("6D00")));
    EXPECT_CALL(*readerSpi.get(), isProtocolSupported(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*readerSpi.get(), isCurrentProtocol(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*readerSpi.get(), activateProtocol(_)).WillRepeatedly(Return());

    cardSelector = SmartCardServiceProvider::getService()
                       ->getReaderApiFactory()
                       ->createBasicCardSelector();

    cardSelectionRequestSpi = std::make_shared<CardSelectionRequestSpiMock>();
    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardRequest())
        .WillRepeatedly(Return(nullptr));
    EXPECT_CALL(
        *cardSelectionRequestSpi.get(), getSuccessfulSelectionStatusWords())
        .WillRepeatedly(ReturnRef(ok_sw));

    apduRequestSpi = std::make_shared<ApduRequestSpiMock>();
    EXPECT_CALL(*apduRequestSpi.get(), getSuccessfulStatusWords())
        .WillRepeatedly(ReturnRef(ok_sw));
    EXPECT_CALL(*apduRequestSpi.get(), getInfo())
        .WillRepeatedly(ReturnRef(info));
    EXPECT_CALL(*apduRequestSpi.get(), getApdu())
        .WillRepeatedly(Return(ok_rapdu));
    apduRequests.push_back(apduRequestSpi);

    cardRequestSpi = std::make_shared<CardRequestSpiMock>();
    EXPECT_CALL(*cardRequestSpi.get(), getApduRequests())
        .WillRepeatedly(ReturnRef(apduRequests));
    EXPECT_CALL(*cardRequestSpi.get(), stopOnUnsuccessfulStatusWord())
        .WillRepeatedly(Return(false));
}

static void
tearDown()
{
    cardSelector.reset();
    readerSpi.reset();
    cardSelectionRequestSpi.reset();
    apduRequestSpi.reset();
    cardRequestSpi.reset();
    apduRequests.clear();
}

TEST(LocalReaderAdapterTest, getReaderSpi_shouldReturnReaderSpi)
{
    setUp();

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);

    ASSERT_EQ(localReaderAdapter.getReaderSpi(), readerSpi);

    tearDown();
}

/*
 * Transmit card selections operations
 */

TEST(
    LocalReaderAdapterTest,
    transmitCardSelectionRequests_withPermissiveCardSelector_shouldReturnMatchingResponseAndOpenChannel)  // NOLINT
{
    setUp();

    // Commented out in Java code
    // EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector())
    //     .WillRepeatedly(Return(cardSelector));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const std::vector<std::shared_ptr<CardSelectorBase>> selector
        = {cardSelector};
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> requests
        = {cardSelectionRequestSpi};
    const auto& cardSelectionResponses
        = localReaderAdapter.transmitCardSelectionRequests(
            selector,
            requests,
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_EQ(cardSelectionResponses[0]->getPowerOnData(), POWER_ON_DATA);
    ASSERT_TRUE(cardSelectionResponses[0]->hasMatched());
    ASSERT_TRUE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    transmitCardSelectionRequests_withPermissiveCardSelectorAndProcessALL_shouldReturnMatchingResponseAndNotOpenChannel)  // NOLINT
{
    setUp();

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const std::vector<std::shared_ptr<CardSelectorBase>> selector
        = {cardSelector};
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> requests
        = {cardSelectionRequestSpi};
    const auto& cardSelectionResponses
        = localReaderAdapter.transmitCardSelectionRequests(
            selector,
            requests,
            MultiSelectionProcessing::PROCESS_ALL,
            ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_EQ(cardSelectionResponses[0]->getPowerOnData(), POWER_ON_DATA);
    ASSERT_TRUE(cardSelectionResponses[0]->hasMatched());
    ASSERT_FALSE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    transmitCardSelectionRequests_withNonMatchingPowerOnDataFilteringCardSelector_shouldReturnNotMatchingResponseAndNotOpenChannel)  // NOLINT
{
    setUp();

    std::dynamic_pointer_cast<BasicCardSelector>(cardSelector)
        ->filterByPowerOnData("FAILINGREGEX");

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const std::vector<std::shared_ptr<CardSelectorBase>> selector
        = {cardSelector};
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> requests
        = {cardSelectionRequestSpi};
    const auto& cardSelectionResponses
        = localReaderAdapter.transmitCardSelectionRequests(
            selector,
            requests,
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_EQ(cardSelectionResponses[0]->getPowerOnData(), POWER_ON_DATA);
    ASSERT_FALSE(cardSelectionResponses[0]->hasMatched());
    ASSERT_FALSE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    transmitCardSelectionRequests_withNonMatchingDFNameFilteringCardSelector_shouldReturnNotMatchingResponseAndNotOpenChannel)  // NOLINT
{
    setUp();

    cardSelector = SmartCardServiceProvider::getService()
                       ->getReaderApiFactory()
                       ->createIsoCardSelector();
    std::dynamic_pointer_cast<IsoCardSelector>(cardSelector)
        ->filterByDfName("1122334455");

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const std::vector<std::shared_ptr<CardSelectorBase>> selector
        = {cardSelector};
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> requests
        = {cardSelectionRequestSpi};
    const auto& cardSelectionResponses
        = localReaderAdapter.transmitCardSelectionRequests(
            selector,
            requests,
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_EQ(cardSelectionResponses[0]->getPowerOnData(), POWER_ON_DATA);
    ASSERT_FALSE(cardSelectionResponses[0]->hasMatched());
    ASSERT_FALSE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    transmitCardSelectionRequests_withMatchingDFNameFilteringCardSelector_shouldReturnMatchingResponseAndOpenChannel)  // NOLINT
{
    setUp();

    auto selectResponseApdu = HexUtil::toByteArray("123456789000");
    EXPECT_CALL(*readerSpi.get(), transmitApdu(_))
        .WillRepeatedly(Return(selectResponseApdu));

    cardSelector = SmartCardServiceProvider::getService()
                       ->getReaderApiFactory()
                       ->createIsoCardSelector();
    std::dynamic_pointer_cast<IsoCardSelector>(cardSelector)
        ->filterByDfName("1122334455");

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const std::vector<std::shared_ptr<CardSelectorBase>> selector
        = {cardSelector};
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> requests
        = {cardSelectionRequestSpi};
    const auto& cardSelectionResponses
        = localReaderAdapter.transmitCardSelectionRequests(
            selector,
            requests,
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_EQ(cardSelectionResponses[0]->getPowerOnData(), POWER_ON_DATA);
    ASSERT_EQ(
        cardSelectionResponses[0]->getSelectApplicationResponse()->getApdu(),
        selectResponseApdu);
    ASSERT_TRUE(cardSelectionResponses[0]->hasMatched());
    ASSERT_TRUE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    transmitCardSelectionRequests_withMatchingDFNameFilteringCardSelectorInvalidatedRejected_shouldReturnNotMatchingResponseAndNotOpenChannel)  // NOLINT
{
    setUp();

    auto selectResponseApdu = HexUtil::toByteArray("123456786283");
    EXPECT_CALL(*readerSpi.get(), transmitApdu(_))
        .WillRepeatedly(Return(selectResponseApdu));

    cardSelector = SmartCardServiceProvider::getService()
                       ->getReaderApiFactory()
                       ->createIsoCardSelector();
    std::dynamic_pointer_cast<IsoCardSelector>(cardSelector)
        ->filterByDfName("1122334455");

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const std::vector<std::shared_ptr<CardSelectorBase>> selector
        = {cardSelector};
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> requests
        = {cardSelectionRequestSpi};
    const auto& cardSelectionResponses
        = localReaderAdapter.transmitCardSelectionRequests(
            selector,
            requests,
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_EQ(cardSelectionResponses[0]->getPowerOnData(), POWER_ON_DATA);
    ASSERT_EQ(
        cardSelectionResponses[0]->getSelectApplicationResponse()->getApdu(),
        selectResponseApdu);
    ASSERT_FALSE(cardSelectionResponses[0]->hasMatched());
    ASSERT_FALSE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    transmitCardSelectionRequests_withMatchingDFNameFilteringCardSelectorInvalidatedAccepted_shouldReturnMatchingResponseAndOpenChannel)  // NOLINT
{
    setUp();

    auto selectResponseApdu = HexUtil::toByteArray("123456786283");
    EXPECT_CALL(*readerSpi.get(), transmitApdu(_))
        .WillRepeatedly(Return(selectResponseApdu));

    cardSelector = SmartCardServiceProvider::getService()
                       ->getReaderApiFactory()
                       ->createIsoCardSelector();
    std::dynamic_pointer_cast<IsoCardSelector>(cardSelector)
        ->filterByDfName("1122334455");

    std::vector<int> sw = {0x9000, 0x6283};
    EXPECT_CALL(
        *cardSelectionRequestSpi.get(), getSuccessfulSelectionStatusWords())
        .WillRepeatedly(ReturnRef(sw));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const std::vector<std::shared_ptr<CardSelectorBase>> selector
        = {cardSelector};
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> requests
        = {cardSelectionRequestSpi};
    const auto& cardSelectionResponses
        = localReaderAdapter.transmitCardSelectionRequests(
            selector,
            requests,
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_EQ(cardSelectionResponses[0]->getPowerOnData(), POWER_ON_DATA);
    ASSERT_EQ(
        cardSelectionResponses[0]->getSelectApplicationResponse()->getApdu(),
        selectResponseApdu);
    ASSERT_TRUE(cardSelectionResponses[0]->hasMatched());
    ASSERT_TRUE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    transmitCardSelectionRequests_withNonMatchingCardProtocolFilteringCardSelector_shouldReturnNotMatchingResponseAndNotOpenChannel)  // NOLINT
{
    setUp();

    std::dynamic_pointer_cast<BasicCardSelector>(cardSelector)
        ->filterByCardProtocol(OTHER_CARD_PROTOCOL);

    LocalConfigurableReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();
    localReaderAdapter.activateProtocol(CARD_PROTOCOL, CARD_PROTOCOL);

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const std::vector<std::shared_ptr<CardSelectorBase>> selector
        = {cardSelector};
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> requests
        = {cardSelectionRequestSpi};
    const auto& cardSelectionResponses
        = localReaderAdapter.transmitCardSelectionRequests(
            selector,
            requests,
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_FALSE(cardSelectionResponses[0]->hasMatched());
    ASSERT_FALSE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    transmitCardSelectionRequests_whenOpenPhysicalThrowsReaderIOException_shouldRCE)  // NOLINT
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), openPhysicalChannel())
        .Times(1)
        .WillOnce(Throw(ReaderIOException("Reader IO Exception")));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const std::vector<std::shared_ptr<CardSelectorBase>> selector
        = {cardSelector};
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> requests
        = {cardSelectionRequestSpi};
    EXPECT_THROW(
        localReaderAdapter.transmitCardSelectionRequests(
            selector,
            requests,
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER),
        ReaderBrokenCommunicationException);

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    transmitCardSelectionRequests_whenOpenPhysicalThrowsCArdIOException_shouldCCE)  // NOLINT
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), openPhysicalChannel())
        .Times(1)
        .WillOnce(Throw(CardIOException("Card IO Exception")));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const std::vector<std::shared_ptr<CardSelectorBase>> selector
        = {cardSelector};
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> requests
        = {cardSelectionRequestSpi};
    EXPECT_THROW(
        localReaderAdapter.transmitCardSelectionRequests(
            selector,
            requests,
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER),
        CardBrokenCommunicationException);

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    transmitCardSelectionRequests_whenTransmitApduThrowsCardIOException_shouldCCE)  // NOLINT
{
    setUp();

    cardSelector = SmartCardServiceProvider::getService()
                       ->getReaderApiFactory()
                       ->createIsoCardSelector();
    std::dynamic_pointer_cast<IsoCardSelector>(cardSelector)
        ->filterByDfName("12341234");

    EXPECT_CALL(*readerSpi.get(), openPhysicalChannel())
        .Times(1)
        .WillOnce(Throw(CardIOException("Card IO Exception")));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const std::vector<std::shared_ptr<CardSelectorBase>> selector
        = {cardSelector};
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> requests
        = {cardSelectionRequestSpi};
    EXPECT_THROW(
        localReaderAdapter.transmitCardSelectionRequests(
            selector,
            requests,
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER),
        CardBrokenCommunicationException);

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    transmitCardSelectionRequests_whenTransmitApduThrowsReaderIOException_shouldRCE)  // NOLINT
{
    setUp();

    cardSelector = SmartCardServiceProvider::getService()
                       ->getReaderApiFactory()
                       ->createIsoCardSelector();
    std::dynamic_pointer_cast<IsoCardSelector>(cardSelector)
        ->filterByDfName("12341234");

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_))
        .Times(1)
        .WillOnce(Throw(ReaderIOException("Reader IO Exception")));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const std::vector<std::shared_ptr<CardSelectorBase>> selector
        = {cardSelector};
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> requests
        = {cardSelectionRequestSpi};
    EXPECT_THROW(
        localReaderAdapter.transmitCardSelectionRequests(
            selector,
            requests,
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER),
        ReaderBrokenCommunicationException);

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    transmitCardSelectionRequests_whenFirstMatchAndSecondSelectionFails_shouldNotMatch)  // NOLINT
{
    setUp();

    cardSelector = SmartCardServiceProvider::getService()
                       ->getReaderApiFactory()
                       ->createIsoCardSelector();
    std::dynamic_pointer_cast<IsoCardSelector>(cardSelector)
        ->filterByDfName("12341234");

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    /* First successful selection */
    EXPECT_CALL(*readerSpi.get(), transmitApdu(_))
        .WillRepeatedly(Return(HexUtil::toByteArray("AABBCCDDEE9000")));

    const std::vector<std::shared_ptr<CardSelectorBase>> selector
        = {cardSelector};
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> requests
        = {cardSelectionRequestSpi};

    std::vector<std::shared_ptr<CardSelectionResponseApi>>
        cardSelectionResponses
        = localReaderAdapter.transmitCardSelectionRequests(
            selector,
            requests,
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::KEEP_OPEN);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_TRUE(cardSelectionResponses[0]->hasMatched());
    ASSERT_TRUE(localReaderAdapter.isLogicalChannelOpen());

    /* Second not matching selection */
    EXPECT_CALL(*readerSpi.get(), transmitApdu(_))
        .WillRepeatedly(Return(HexUtil::toByteArray("6B00")));

    cardSelectionResponses = localReaderAdapter.transmitCardSelectionRequests(
        selector,
        requests,
        MultiSelectionProcessing::FIRST_MATCH,
        ChannelControl::KEEP_OPEN);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_FALSE(cardSelectionResponses[0]->hasMatched());
    ASSERT_FALSE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

/*
 * Transmit card request
 */

TEST(LocalReaderAdapterTest, transmitCardRequest_shouldReturnResponse)
{
    setUp();

    const std::vector<uint8_t> responseApdu
        = HexUtil::toByteArray("123456786283");
    std::vector<uint8_t> requestApdu = HexUtil::toByteArray("0000");

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_))
        .WillRepeatedly(Return(responseApdu));
    EXPECT_CALL(*apduRequestSpi.get(), getApdu())
        .WillRepeatedly(Return(requestApdu));

    LocalConfigurableReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();
    localReaderAdapter.activateProtocol(CARD_PROTOCOL, CARD_PROTOCOL);

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    auto cardResponse = localReaderAdapter.transmitCardRequest(
        cardRequestSpi, ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardResponse->getApduResponses()[0]->getApdu(), responseApdu);
    ASSERT_FALSE(cardResponse->isLogicalChannelOpen());

    tearDown();
}

TEST(LocalReaderAdapterTest, transmitCardRequest_isCase4)
{
    setUp();

    std::vector<uint8_t> requestApdu
        = HexUtil::toByteArray("11223344041234567802");
    const std::vector<uint8_t> responseApdu = HexUtil::toByteArray("9000");
    const std::vector<uint8_t> getResponseRApdu
        = HexUtil::toByteArray("00C0000002");
    const std::vector<uint8_t> getResponseCApdu
        = HexUtil::toByteArray("00009000");

    EXPECT_CALL(*apduRequestSpi.get(), getApdu())
        .WillRepeatedly(Return(requestApdu));
    EXPECT_CALL(*readerSpi.get(), transmitApdu(requestApdu))
        .WillRepeatedly(Return(responseApdu));
    EXPECT_CALL(*readerSpi.get(), transmitApdu(getResponseRApdu))
        .WillRepeatedly(Return(getResponseCApdu));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();
    auto response = localReaderAdapter.transmitCardRequest(
        cardRequestSpi, ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(response->getApduResponses()[0]->getApdu(), getResponseCApdu);

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    transmitCardRequest_withUnsuccessfulStatusWord_shouldThrow_USW)
{
    setUp();

    const std::vector<uint8_t> responseApdu
        = HexUtil::toByteArray("123456789000");
    std::vector<uint8_t> requestApdu = HexUtil::toByteArray("0000");
    const std::vector<int> resp = {0x9001};

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_))
        .WillRepeatedly(Return(responseApdu));
    EXPECT_CALL(*apduRequestSpi.get(), getApdu())
        .WillRepeatedly(Return(requestApdu));
    EXPECT_CALL(*apduRequestSpi.get(), getSuccessfulStatusWords())
        .WillRepeatedly(ReturnRef(resp));
    EXPECT_CALL(*cardRequestSpi.get(), stopOnUnsuccessfulStatusWord())
        .WillRepeatedly(Return(true));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    EXPECT_THROW(
        localReaderAdapter.transmitCardRequest(
            cardRequestSpi, ChannelControl::CLOSE_AFTER),
        UnexpectedStatusWordException);

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    transmitCardRequest_withCardExceptionOnTransmit_shouldThrow_CBCE)
{
    setUp();

    std::vector<uint8_t> requestApdu = HexUtil::toByteArray("0000");

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_))
        .WillRepeatedly(Throw(CardIOException("")));
    EXPECT_CALL(*apduRequestSpi.get(), getApdu())
        .WillRepeatedly(Return(requestApdu));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    EXPECT_THROW(
        localReaderAdapter.transmitCardRequest(
            cardRequestSpi, ChannelControl::CLOSE_AFTER),
        CardBrokenCommunicationException);

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    transmitCardRequest_withCardExceptionOnTransmit_shouldThrow_RBCE)
{
    setUp();

    std::vector<uint8_t> requestApdu = HexUtil::toByteArray("0000");

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_))
        .WillRepeatedly(Throw(ReaderIOException("")));
    EXPECT_CALL(*apduRequestSpi.get(), getApdu())
        .WillRepeatedly(Return(requestApdu));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    EXPECT_THROW(
        localReaderAdapter.transmitCardRequest(
            cardRequestSpi, ChannelControl::CLOSE_AFTER),
        ReaderBrokenCommunicationException);

    tearDown();
}

/*
 * active protocol operations
 */

TEST(
    LocalReaderAdapterTest,
    deActivateProtocol_shouldInvoke_deActivateProcotol_OnReaderSpi)
{
    setUp();

    ReaderAdapterTestUtils utils;
    std::shared_ptr<ConfigurableReaderSpiMock> spy = utils.getReaderSpiSpy();
    EXPECT_CALL(*spy.get(), deactivateProtocol(CARD_PROTOCOL)).Times(1);
    EXPECT_CALL(*spy.get(), activateProtocol(_)).WillRepeatedly(Return());

    LocalConfigurableReaderAdapter localReaderAdapter(spy, PLUGIN_NAME);

    localReaderAdapter.doRegister();
    localReaderAdapter.activateProtocol(CARD_PROTOCOL, CARD_PROTOCOL);
    localReaderAdapter.deactivateProtocol(CARD_PROTOCOL);

    tearDown();
}

TEST(LocalReaderAdapterTest, activateProtocol_whileNotSupported_should_RPNS)
{
    ReaderAdapterTestUtils utils;
    std::shared_ptr<ConfigurableReaderSpiMock> spy = utils.getReaderSpiSpy();
    EXPECT_CALL(*spy.get(), isProtocolSupported(_))
        .WillRepeatedly(Return(false));
    EXPECT_CALL(*spy.get(), activateProtocol(_)).WillRepeatedly(Return());

    LocalConfigurableReaderAdapter localReaderAdapter(spy, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    EXPECT_THROW(
        localReaderAdapter.activateProtocol(CARD_PROTOCOL, CARD_PROTOCOL),
        ReaderProtocolNotSupportedException);
}

TEST(LocalReaderAdapterTest, deActivateProtocol_whileNotSupported_should_RPNS)
{
    ReaderAdapterTestUtils utils;
    std::shared_ptr<ConfigurableReaderSpiMock> spy = utils.getReaderSpiSpy();
    EXPECT_CALL(*spy.get(), isProtocolSupported(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*spy.get(), activateProtocol(_)).WillRepeatedly(Return());
    EXPECT_CALL(*spy.get(), deactivateProtocol(_)).WillRepeatedly(Return());

    LocalConfigurableReaderAdapter localReaderAdapter(spy, PLUGIN_NAME);
    localReaderAdapter.doRegister();
    localReaderAdapter.activateProtocol(CARD_PROTOCOL, CARD_PROTOCOL);

    EXPECT_CALL(*spy.get(), isProtocolSupported(_))
        .WillRepeatedly(Return(false));

    EXPECT_THROW(
        localReaderAdapter.deactivateProtocol(CARD_PROTOCOL),
        ReaderProtocolNotSupportedException);
}

/*
 * Misc operations
 */

TEST(
    LocalReaderAdapterTest, isContactless_whenSpiIsContactless_shouldReturnTrue)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), isContactless())
        .Times(1)
        .WillOnce(Return(true));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isContactless());

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    isContactless_whenSpiIsNotContactless_shouldReturnFalse)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), isContactless())
        .Times(1)
        .WillOnce(Return(false));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_FALSE(localReaderAdapter.isContactless());

    tearDown();
}

TEST(
    LocalReaderAdapterTest,
    closeLogicalAndPhysicalChannelsSilently_withException_does_not_propagate)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), closePhysicalChannel())
        .WillRepeatedly(Throw(ReaderIOException("")));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.closeLogicalAndPhysicalChannelsSilently();

    /* No exception is propagated */

    tearDown();
}

TEST(LocalReaderAdapterTest, releaseChannel_withException_throwRBCE)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), closePhysicalChannel())
        .WillRepeatedly(Throw(ReaderIOException("")));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    /* Exception is thrown */
    EXPECT_THROW(
        localReaderAdapter.releaseChannel(),
        ReaderBrokenCommunicationException);

    tearDown();
}

TEST(LocalReaderAdapterTest, isCardPresent_whenReaderSpiFails_shouldKRCE)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), checkCardPresence())
        .WillRepeatedly(Throw(ReaderIOException("Reader IO Exception")));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    EXPECT_THROW(
        localReaderAdapter.isCardPresent(), ReaderCommunicationException);

    tearDown();
}
