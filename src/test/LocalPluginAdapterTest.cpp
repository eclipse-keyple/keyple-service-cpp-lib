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

#include "keyple/core/common/KeyplePluginExtension.hpp"
#include "keyple/core/plugin/PluginIOException.hpp"
#include "keyple/core/plugin/spi/PluginSpi.hpp"
#include "keyple/core/plugin/spi/reader/ReaderSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/ObservableReaderSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/insertion/WaitForCardInsertionBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/processing/DontWaitForCardRemovalDuringProcessingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/removal/WaitForCardRemovalBlockingSpi.hpp"
#include "keyple/core/service/LocalPluginAdapter.hpp"
#include "keyple/core/service/LocalReaderAdapter.hpp"
#include "keyple/core/service/ObservableLocalReaderAdapter.hpp"
#include "keyple/core/util/cpp/exception/IllegalArgumentException.hpp"
#include "keyple/core/util/cpp/exception/IllegalStateException.hpp"
#include "keypop/reader/CardReader.hpp"
#include "keypop/reader/ObservableCardReader.hpp"

/* Mock */
#include "mock/ObservableReaderSpiMock.hpp"
#include "mock/PluginSpiMock.hpp"
#include "mock/ReaderSpiMock.hpp"

using keyple::core::common::KeyplePluginExtension;
using keyple::core::plugin::PluginIOException;
using keyple::core::plugin::spi::PluginSpi;
using keyple::core::plugin::spi::reader::ReaderSpi;
using keyple::core::plugin::spi::reader::observable::ObservableReaderSpi;
using keyple::core::plugin::spi::reader::observable::state::insertion::
    WaitForCardInsertionBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::processing::
    DontWaitForCardRemovalDuringProcessingSpi;
using keyple::core::plugin::spi::reader::observable::state::removal::
    WaitForCardRemovalBlockingSpi;
using keyple::core::service::LocalPluginAdapter;
using keyple::core::service::LocalReaderAdapter;
using keyple::core::service::ObservableLocalReaderAdapter;
using keyple::core::util::cpp::exception::IllegalArgumentException;
using keyple::core::util::cpp::exception::IllegalStateException;
using keypop::reader::CardReader;
using keypop::reader::ObservableCardReader;

using testing::Return;
using testing::ReturnRef;
using testing::Throw;

static const std::string PLUGIN_NAME = "plugin";
static const std::string READER_NAME_1 = "reader1";
static const std::string READER_NAME_2 = "reader2";
static const std::string OBSERVABLE_READER_NAME = "observableReader";

static std::shared_ptr<PluginSpiMock> pluginSpi;
static std::shared_ptr<ReaderSpiMock> readerSpi1;
static std::shared_ptr<ReaderSpiMock> readerSpi2;
static std::shared_ptr<ObservableReaderSpiMock> observableReader;

static void
setUp()
{
    pluginSpi = std::make_shared<PluginSpiMock>();
    EXPECT_CALL(*pluginSpi.get(), getName())
        .WillRepeatedly(ReturnRef(PLUGIN_NAME));
    EXPECT_CALL(*pluginSpi.get(), onUnregister()).WillRepeatedly(Return());

    readerSpi1 = std::make_shared<ReaderSpiMock>(READER_NAME_1);
    EXPECT_CALL(*readerSpi1.get(), onUnregister()).WillRepeatedly(Return());
    EXPECT_CALL(*readerSpi1.get(), closePhysicalChannel())
        .WillRepeatedly(Return());

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
}

static void
tearDown()
{
    pluginSpi.reset();
    readerSpi1.reset();
    readerSpi2.reset();
    observableReader.reset();
}

TEST(LocalPluginAdapterTest, register_whenSearchReaderFails_shouldPIO)
{
    setUp();

    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders())
        .Times(1)
        .WillOnce(Throw(PluginIOException("Plugin IO Exception")));

    LocalPluginAdapter localPluginAdapter(pluginSpi);

    EXPECT_THROW(localPluginAdapter.doRegister(), PluginIOException);

    tearDown();
}

TEST(
    LocalPluginAdapterTest,
    register_whenSearchReaderReturnsReader_shouldRegisterReader)
{
    setUp();

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    readerSpis.push_back(readerSpi1);
    readerSpis.push_back(readerSpi2);

    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders())
        .WillRepeatedly(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    ASSERT_EQ(localPluginAdapter.getName(), PLUGIN_NAME);

    localPluginAdapter.doRegister();
    localPluginAdapter.checkStatus();

    const std::vector<std::string> readerNames
        = localPluginAdapter.getReaderNames();
    auto it = std::find(readerNames.begin(), readerNames.end(), READER_NAME_1);
    ASSERT_NE(it, readerNames.end());
    it = std::find(readerNames.begin(), readerNames.end(), READER_NAME_2);
    ASSERT_NE(it, readerNames.end());

    const std::vector<std::shared_ptr<CardReader>> readers
        = localPluginAdapter.getReaders();
    ASSERT_EQ(readers.size(), 2);
    auto itt = std::find(
        readers.begin(),
        readers.end(),
        localPluginAdapter.getReader(READER_NAME_1));
    ASSERT_NE(itt, readers.end());
    itt = std::find(
        readers.begin(),
        readers.end(),
        localPluginAdapter.getReader(READER_NAME_2));
    ASSERT_NE(itt, readers.end());

    const auto& reader1 = localPluginAdapter.getReader(READER_NAME_1);
    const auto reader1Class = std::dynamic_pointer_cast<CardReader>(reader1);
    ASSERT_NE(reader1Class, nullptr);
    const auto localReader1Class
        = std::dynamic_pointer_cast<LocalReaderAdapter>(reader1);
    ASSERT_NE(localReader1Class, nullptr);

    const auto& reader2 = localPluginAdapter.getReader(READER_NAME_2);
    const auto reader2Class = std::dynamic_pointer_cast<CardReader>(reader2);
    ASSERT_NE(reader2Class, nullptr);
    const auto localReader2Class
        = std::dynamic_pointer_cast<LocalReaderAdapter>(reader2);
    ASSERT_NE(localReader2Class, nullptr);

    ASSERT_NE(reader1, reader2);

    tearDown();
}

TEST(
    LocalPluginAdapterTest,
    register_whenSearchReaderReturnsObservableReader_shouldRegisterObservableReader)  // NOLINT
{
    setUp();

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    readerSpis.push_back(
        std::dynamic_pointer_cast<ReaderSpi>(observableReader));

    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders())
        .WillRepeatedly(Return(readerSpis));
    EXPECT_CALL(*observableReader.get(), onStopDetection())
        .WillRepeatedly(Return());

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    localPluginAdapter.doRegister();
    localPluginAdapter.checkStatus();

    const std::vector<std::string> readerNames
        = localPluginAdapter.getReaderNames();
    auto it = std::find(
        readerNames.begin(), readerNames.end(), OBSERVABLE_READER_NAME);
    ASSERT_NE(it, readerNames.end());

    const std::vector<std::shared_ptr<CardReader>> readers
        = localPluginAdapter.getReaders();
    ASSERT_EQ(readers.size(), 1);

    const auto& reader = localPluginAdapter.getReader(OBSERVABLE_READER_NAME);
    const auto readerClass
        = std::dynamic_pointer_cast<ObservableCardReader>(reader);
    ASSERT_NE(readerClass, nullptr);
    const auto localReaderClass
        = std::dynamic_pointer_cast<ObservableLocalReaderAdapter>(reader);
    ASSERT_NE(localReaderClass, nullptr);

    tearDown();
}

TEST(LocalPluginAdapterTest, getReaders_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPluginAdapter localPluginAdapter(pluginSpi);

    EXPECT_THROW(localPluginAdapter.getReaders(), IllegalStateException);

    tearDown();
}

TEST(LocalPluginAdapterTest, getReader_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPluginAdapter localPluginAdapter(pluginSpi);

    EXPECT_THROW(
        localPluginAdapter.getReader(READER_NAME_1), IllegalStateException);

    tearDown();
}

TEST(
    LocalPluginAdapterTest,
    findReader_whenReaderNameRegexMatches_returnsExistingReader)
{
    setUp();

    const std::string readerNameRegex = ".*1";

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    readerSpis.push_back(readerSpi1);
    readerSpis.push_back(readerSpi2);

    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders())
        .Times(1)
        .WillOnce(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    localPluginAdapter.doRegister();

    auto foundedReader = localPluginAdapter.findReader(readerNameRegex);

    ASSERT_NE(foundedReader, nullptr);
    ASSERT_EQ(foundedReader->getName(), READER_NAME_1);

    tearDown();
}

TEST(LocalPluginAdapterTest, findReader_whenNoReaderNameMatches_returnsNull)
{
    setUp();

    const std::string readerNameRegex = ".*3";

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    readerSpis.push_back(readerSpi1);
    readerSpis.push_back(readerSpi2);

    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders())
        .Times(1)
        .WillOnce(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    localPluginAdapter.doRegister();

    auto foundedReader = localPluginAdapter.findReader(readerNameRegex);

    ASSERT_EQ(foundedReader, nullptr);

    tearDown();
}

TEST(
    LocalPluginAdapterTest,
    findReader_whenReaderNameRegexIsNotAValidPattern_throwsIllegalArgumentsException)  // NOLINT
{
    setUp();

    const std::string invalidReaderNameRegex = "[";

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    readerSpis.push_back(readerSpi1);
    readerSpis.push_back(readerSpi2);

    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders())
        .Times(1)
        .WillOnce(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    localPluginAdapter.doRegister();

    EXPECT_THROW(
        localPluginAdapter.findReader(invalidReaderNameRegex),
        IllegalArgumentException);

    tearDown();
}

TEST(LocalPluginAdapterTest, getReaderNames_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPluginAdapter localPluginAdapter(pluginSpi);

    EXPECT_THROW(localPluginAdapter.getReaderNames(), IllegalStateException);

    tearDown();
}

TEST(LocalPluginAdapterTest, unregister_shouldDisableMethodsWithISE)
{
    setUp();

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    readerSpis.push_back(readerSpi1);

    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders())
        .WillRepeatedly(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    localPluginAdapter.doRegister();
    localPluginAdapter.doUnregister();

    EXPECT_THROW(localPluginAdapter.getReaders(), IllegalStateException);

    tearDown();
}

TEST(LocalPluginAdapterTest, getExtension_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPluginAdapter localPluginAdapter(pluginSpi);

    EXPECT_THROW(
        localPluginAdapter.getExtension(typeid(PluginSpiMock)),
        IllegalStateException);

    tearDown();
}

TEST(LocalPluginAdapterTest, getExtension_whenRegistered_shouldReturnExtension)
{
    setUp();

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders())
        .WillRepeatedly(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    localPluginAdapter.doRegister();

    const auto extension
        = localPluginAdapter.getExtension(typeid(PluginSpiMock));
    const auto pluginMock = std::static_pointer_cast<PluginSpiMock>(extension);
    ASSERT_NE(pluginMock, nullptr);

    tearDown();
}

TEST(
    LocalPluginAdapterTest,
    getReaderExtension_whenReaderIsRegistered_shouldReturnExtension)
{
    setUp();

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    readerSpis.push_back(readerSpi1);
    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders())
        .WillRepeatedly(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    localPluginAdapter.doRegister();
    const auto extension = localPluginAdapter.getReaderExtension(
        typeid(PluginSpiMock), READER_NAME_1);

    ASSERT_EQ(extension, readerSpi1);

    tearDown();
}

TEST(
    LocalPluginAdapterTest,
    getReaderExtension_whenReaderNameIsUnknown_shouldReturnIAE)
{
    setUp();

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    readerSpis.push_back(readerSpi1);
    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders())
        .WillRepeatedly(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    localPluginAdapter.doRegister();

    EXPECT_THROW(
        localPluginAdapter.getReaderExtension(typeid(PluginSpiMock), "UNKNOWN"),
        IllegalArgumentException);

    tearDown();
}

TEST(
    LocalPluginAdapterTest,
    getReaderExtension_whenPluginIsNotRegistered_shouldISE)
{
    setUp();

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    readerSpis.push_back(readerSpi1);
    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders())
        .WillRepeatedly(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);

    EXPECT_THROW(
        localPluginAdapter.getReaderExtension(
            typeid(PluginSpiMock), READER_NAME_1),
        IllegalStateException);

    tearDown();
}
