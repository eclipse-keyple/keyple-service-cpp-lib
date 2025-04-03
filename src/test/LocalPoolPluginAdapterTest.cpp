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

#include "keyple/core/plugin/PluginIOException.hpp"
#include "keyple/core/plugin/spi/PluginSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/ObservableReaderSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/insertion/WaitForCardInsertionBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/processing/DontWaitForCardRemovalDuringProcessingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/removal/WaitForCardRemovalBlockingSpi.hpp"
#include "keyple/core/service/KeyplePluginException.hpp"
#include "keyple/core/service/LocalPluginAdapter.hpp"
#include "keyple/core/service/LocalPoolPluginAdapter.hpp"
#include "keyple/core/service/LocalReaderAdapter.hpp"
#include "keyple/core/service/ObservableLocalReaderAdapter.hpp"
#include "keyple/core/util/cpp/exception/IllegalStateException.hpp"
#include "keypop/reader/CardReader.hpp"

/* Mock */
#include "mock/ObservableReaderSpiMock.hpp"
#include "mock/PluginSpiMock.hpp"
#include "mock/PoolPluginSpiMock.hpp"
#include "mock/ReaderSpiMock.hpp"
#include "mock/SmartCardMock.hpp"

using keyple::core::plugin::PluginIOException;
using keyple::core::plugin::spi::PluginSpi;
using keyple::core::plugin::spi::reader::observable::ObservableReaderSpi;
using keyple::core::plugin::spi::reader::observable::state::insertion::
    WaitForCardInsertionBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::processing::
    DontWaitForCardRemovalDuringProcessingSpi;
using keyple::core::plugin::spi::reader::observable::state::removal::
    WaitForCardRemovalBlockingSpi;
using keyple::core::service::KeyplePluginException;
using keyple::core::service::LocalPluginAdapter;
using keyple::core::service::LocalPoolPluginAdapter;
using keyple::core::service::LocalReaderAdapter;
using keyple::core::service::ObservableLocalReaderAdapter;
using keyple::core::util::cpp::exception::IllegalStateException;
using keypop::reader::CardReader;

using testing::_;
using testing::Return;
using testing::ReturnRef;
using testing::Throw;

static const std::string READER_NAME_1 = "reader1";
static const std::string READER_NAME_2 = "reader2";
static const std::string OBSERVABLE_READER_NAME = "observableReader";
static const std::string PLUGIN_NAME = "plugin";
static const std::string GROUP_1 = "group1";
static const std::string GROUP_2 = "group2";
static const std::string GROUP_3 = "group3";

static std::shared_ptr<ReaderSpiMock> readerSpi1;
static std::shared_ptr<ReaderSpiMock> readerSpi2;
static std::shared_ptr<ObservableReaderSpiMock> observableReader;
static std::shared_ptr<PoolPluginSpiMock> poolPluginSpi;
static std::vector<std::string> groupReference({GROUP_1, GROUP_2});
static const std::shared_ptr<SmartCardMock> smartCard;

static void
setUp()
{
    readerSpi1 = std::make_shared<ReaderSpiMock>(READER_NAME_1);
    EXPECT_CALL(*readerSpi1.get(), onUnregister()).WillRepeatedly(Return());
    EXPECT_CALL(*readerSpi1.get(), closePhysicalChannel())
        .WillRepeatedly(Return());
    EXPECT_CALL(*readerSpi1.get(), getSelectedSmartCard())
        .WillRepeatedly(Return(nullptr));

    readerSpi2 = std::make_shared<ReaderSpiMock>(READER_NAME_2);
    EXPECT_CALL(*readerSpi2.get(), onUnregister()).WillRepeatedly(Return());
    EXPECT_CALL(*readerSpi2.get(), closePhysicalChannel())
        .WillRepeatedly(Return());

    observableReader
        = std::make_shared<ObservableReaderSpiMock>(OBSERVABLE_READER_NAME);
    EXPECT_CALL(*observableReader.get(), onUnregister())
        .WillRepeatedly(Return());
    EXPECT_CALL(*observableReader.get(), closePhysicalChannel())
        .WillRepeatedly(Return());
    EXPECT_CALL(*observableReader.get(), onStopDetection())
        .WillRepeatedly(Return());

    poolPluginSpi = std::make_shared<PoolPluginSpiMock>();
    EXPECT_CALL(*poolPluginSpi.get(), getName())
        .WillRepeatedly(ReturnRef(PLUGIN_NAME));
    EXPECT_CALL(*poolPluginSpi.get(), allocateReader(GROUP_1))
        .WillRepeatedly(Return(readerSpi1));
    EXPECT_CALL(*poolPluginSpi.get(), allocateReader(GROUP_2))
        .WillRepeatedly(Return(readerSpi2));
    EXPECT_CALL(*poolPluginSpi.get(), releaseReader(_))
        .WillRepeatedly(Return());
    EXPECT_CALL(*poolPluginSpi.get(), onUnregister()).WillRepeatedly(Return());
    EXPECT_CALL(*poolPluginSpi.get(), getReaderGroupReferences())
        .WillRepeatedly(Return(groupReference));
}

static void
tearDown()
{
    observableReader.reset();
    poolPluginSpi.reset();
    readerSpi1.reset();
    readerSpi2.reset();
}

TEST(
    LocalPoolPluginAdapterTest,
    getReaderGroupReferences_whenGettingReferencesFails_shouldKPE)
{
    setUp();

    EXPECT_CALL(*poolPluginSpi.get(), getReaderGroupReferences())
        .Times(1)
        .WillOnce(Throw(PluginIOException("Plugin IO Exception")));

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    EXPECT_THROW(
        localPluginAdapter.getReaderGroupReferences(), KeyplePluginException);

    tearDown();
}

TEST(
    LocalPoolPluginAdapterTest,
    getReaderGroupReferences_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);

    EXPECT_THROW(
        localPluginAdapter.getReaderGroupReferences(), IllegalStateException);
}

TEST(
    LocalPoolPluginAdapterTest,
    getReaderGroupReferences_whenSucceeds_shouldReturnReferences)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    const std::vector<std::string>& groupReferences
        = localPluginAdapter.getReaderGroupReferences();
    ASSERT_EQ(groupReferences.size(), 2);
    ASSERT_TRUE(
        std::count(groupReferences.begin(), groupReferences.end(), GROUP_1));
    ASSERT_TRUE(
        std::count(groupReferences.begin(), groupReferences.end(), GROUP_2));

    tearDown();
}

TEST(LocalPoolPluginAdapterTest, allocateReader_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);

    EXPECT_THROW(
        localPluginAdapter.allocateReader(GROUP_1), IllegalStateException);

    tearDown();
}

TEST(
    LocalPoolPluginAdapterTest,
    allocateReader_whenAllocatingReaderFails_shouldKPE)
{
    setUp();

    EXPECT_CALL(*poolPluginSpi.get(), allocateReader(_))
        .Times(1)
        .WillOnce(Throw(PluginIOException("Plugin IO Exception")));

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    EXPECT_THROW(
        localPluginAdapter.allocateReader(GROUP_1), KeyplePluginException);

    tearDown();
}

TEST(LocalPoolPluginAdapterTest, allocateReader_whenSucceeds_shouldReturnReader)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    std::shared_ptr<CardReader> reader
        = localPluginAdapter.allocateReader(GROUP_1);
    ASSERT_EQ(reader->getName(), READER_NAME_1);
    ASSERT_NE(std::dynamic_pointer_cast<CardReader>(reader), nullptr);
    ASSERT_NE(std::dynamic_pointer_cast<LocalReaderAdapter>(reader), nullptr);

    const std::vector<std::string>& readerNames
        = localPluginAdapter.getReaderNames();
    ASSERT_EQ(readerNames.size(), 1);
    ASSERT_TRUE(
        std::count(readerNames.begin(), readerNames.end(), READER_NAME_1));

    const std::vector<std::shared_ptr<CardReader>>& readers
        = localPluginAdapter.getReaders();
    ASSERT_EQ(readers.size(), 1);
    ASSERT_TRUE(std::count(
        readers.begin(),
        readers.end(),
        localPluginAdapter.getReader(READER_NAME_1)));

    tearDown();
}

TEST(
    LocalPoolPluginAdapterTest,
    allocateReader_whenReaderIsObservable_shouldReturnObservableReader)
{
    setUp();

    EXPECT_CALL(*poolPluginSpi.get(), allocateReader(GROUP_3))
        .Times(1)
        .WillOnce(Return(observableReader));

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    std::shared_ptr<CardReader> reader
        = localPluginAdapter.allocateReader(GROUP_3);
    ASSERT_EQ(reader->getName(), OBSERVABLE_READER_NAME);
    ASSERT_NE(
        std::dynamic_pointer_cast<ObservableLocalReaderAdapter>(reader),
        nullptr);

    const std::vector<std::shared_ptr<CardReader>>& readers
        = localPluginAdapter.getReaders();
    ASSERT_EQ(readers.size(), 1);
    ASSERT_TRUE(std::count(
        readers.begin(),
        readers.end(),
        localPluginAdapter.getReader(OBSERVABLE_READER_NAME)));

    tearDown();
}

TEST(LocalPoolPluginAdapterTest, releaseReader_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    std::shared_ptr<CardReader> reader
        = localPluginAdapter.allocateReader(GROUP_1);
    localPluginAdapter.doUnregister();

    EXPECT_THROW(
        localPluginAdapter.releaseReader(reader), IllegalStateException);

    tearDown();
}

TEST(LocalPoolPluginAdapterTest, releaseReader_whenSucceeds_shouldRemoveReader)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    std::shared_ptr<CardReader> reader
        = localPluginAdapter.allocateReader(GROUP_1);
    localPluginAdapter.releaseReader(reader);

    ASSERT_EQ(localPluginAdapter.getReaderNames().size(), 0);
    ASSERT_EQ(localPluginAdapter.getReaders().size(), 0);

    tearDown();
}

TEST(
    LocalPoolPluginAdapterTest,
    releaseReader_whenReleaseReaderFails_shouldKPE_and_RemoveReader)
{
    setUp();

    EXPECT_CALL(*poolPluginSpi.get(), releaseReader(_))
        .Times(1)
        .WillOnce(Throw(PluginIOException("Plugin IO Exception")));

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    std::shared_ptr<CardReader> reader
        = localPluginAdapter.allocateReader(GROUP_1);

    EXPECT_THROW(
        localPluginAdapter.releaseReader(reader), KeyplePluginException);
    ASSERT_EQ(localPluginAdapter.getReaderNames().size(), 0);
    ASSERT_EQ(localPluginAdapter.getReaders().size(), 0);

    tearDown();
}

TEST(
    LocalPoolPluginAdapterTest,
    getSelectedSmartCard_whenSelectedSmartCardIsNull_shouldReturnNull)
{
    setUp();

    EXPECT_CALL(*readerSpi1.get(), getSelectedSmartCard())
        .WillRepeatedly(Return(nullptr));

    LocalPoolPluginAdapter plugin(poolPluginSpi);
    plugin.doRegister();

    auto reader = plugin.allocateReader(GROUP_1);
    ASSERT_EQ(plugin.getSelectedSmartCard(reader), nullptr);

    tearDown();
}

TEST(
    LocalPoolPluginAdapterTest,
    getSelectedSmartCard_whenSelectedSmartCardIsNotNull_shouldReturnSelectedSmartCard)  // NOLINT
{
    setUp();

    EXPECT_CALL(*readerSpi1.get(), getSelectedSmartCard())
        .Times(1)
        .WillOnce(Return(std::dynamic_pointer_cast<any>(smartCard)));

    LocalPoolPluginAdapter plugin(poolPluginSpi);
    plugin.doRegister();

    auto reader = plugin.allocateReader(GROUP_1);
    ASSERT_EQ(plugin.getSelectedSmartCard(reader), smartCard);

    tearDown();
}
