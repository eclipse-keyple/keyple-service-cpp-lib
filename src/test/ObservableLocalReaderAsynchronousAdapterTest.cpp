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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keyple/core/service/ObservableLocalReaderAdapter.hpp"
#include "keyple/core/service/cpp/ExecutorService.hpp"
#include "keyple/core/util/cpp/LoggerFactory.hpp"
#include "keypop/card/CardResponseApi.hpp"
#include "keypop/card/CardSelectionResponseApi.hpp"
#include "keypop/card/spi/CardSelectionRequestSpi.hpp"
#include "keypop/reader/CardReaderEvent.hpp"

/* Mock */
#include "mock/CardReaderObservationExceptionHandlerSpiMock.hpp"
#include "mock/CardResponseApiMock.hpp"
#include "mock/CardSelectionRequestSpiMock.hpp"
#include "mock/CardSelectionResponseApiMock.hpp"
#include "mock/ObservableLocalPluginSpiMock.hpp"
#include "mock/ObservableReaderAsynchronousSpiMock.hpp"
#include "mock/PluginObservationExceptionHandlerSpiMock.hpp"
#include "mock/PluginObserverSpiMock.hpp"
#include "mock/ReaderObserverSpiMock.hpp"
#include "mock/ReaderSpiMock.hpp"

/* Util */
#include "util/ObservableLocalReaderSuite.hpp"

using keyple::core::service::ObservableLocalReaderAdapter;
using keyple::core::service::cpp::ExecutorService;
using keyple::core::util::cpp::LoggerFactory;
using keypop::card::CardResponseApi;
using keypop::card::CardSelectionResponseApi;
using keypop::card::spi::CardSelectionRequestSpi;
using keypop::reader::CardReaderEvent;

using testing::_;
using testing::Return;
using testing::ReturnRef;

static const std::string PLUGIN_NAME = "plugin";

static std::shared_ptr<ObservableLocalReaderAdapter> _reader;
static std::shared_ptr<ObservableReaderAsynchronousSpiMock> readerSpi;
static std::shared_ptr<ReaderObserverSpiMock> observer;
static std::shared_ptr<CardReaderObservationExceptionHandlerSpiMock> handler;
static std::shared_ptr<ObservableLocalReaderSuite> testSuite;
static std::shared_ptr<ExecutorService> notificationExecutorService;

static std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequestSpi;
static std::shared_ptr<CardSelectionResponseApi> cardSelectionResponseApi;
static std::shared_ptr<CardResponseApi> cardResponseApi;
static std::shared_ptr<CardReaderEvent> event;

class ObservableLocalReaderAsynchronousAdapterTest { };
static const std::shared_ptr<Logger> logger = LoggerFactory::getLogger(
    typeid(ObservableLocalReaderAsynchronousAdapterTest));

static void
setUp()
{
    notificationExecutorService = std::make_shared<ExecutorService>();
    readerSpi
        = std::make_shared<ObservableReaderAsynchronousSpiMock>(READER_NAME);
    handler = std::make_shared<CardReaderObservationExceptionHandlerSpiMock>();
    EXPECT_CALL(*handler.get(), onReaderObservationError(_, _, _))
        .WillRepeatedly(Return());
    _reader = std::make_shared<ObservableLocalReaderAdapter>(
        readerSpi, PLUGIN_NAME);
    observer = std::make_shared<ReaderObserverSpiMock>(nullptr);
    testSuite = std::make_shared<ObservableLocalReaderSuite>(
        _reader, readerSpi, observer, handler, logger);
    cardSelectionRequestSpi = std::make_shared<CardSelectionRequestSpiMock>();
    cardSelectionResponseApi = std::make_shared<CardSelectionResponseApiMock>();
    cardResponseApi = std::make_shared<CardResponseApiMock>();

    /* Test with event notification executor service */
    _reader->doRegister();
}

static void
tearDown()
{
    _reader->doUnregister();

    cardResponseApi.reset();
    cardSelectionResponseApi.reset();
    cardSelectionRequestSpi.reset();
    testSuite.reset();
    observer.reset();
    _reader.reset();
    handler.reset();
    readerSpi.reset();
    notificationExecutorService.reset();
}

TEST(
    ObservableLocalReaderAsynchronousAdapterTest,
    initReader_addObserver_startDetection)
{
    setUp();

    testSuite->addFirstObserver_should_startDetection();

    tearDown();
}

TEST(ObservableLocalReaderAsynchronousAdapterTest, removeObserver)
{
    setUp();

    testSuite->removeLastObserver_should_StopDetection();

    tearDown();
}

TEST(ObservableLocalReaderAsynchronousAdapterTest, clearObservers)
{
    setUp();

    testSuite->clearObservers_shouldRemove_allObservers();

    tearDown();
}

TEST(
    ObservableLocalReaderAsynchronousAdapterTest,
    insertCard_shouldNotify_CardInsertedEvent)
{
    setUp();

    testSuite->insertCard_onWaitForCard_shouldNotify_CardInsertedEvent();

    tearDown();
}

TEST(
    ObservableLocalReaderAsynchronousAdapterTest,
    finalizeCardProcessing_afterInsert_switchState)
{
    setUp();

    testSuite->finalizeCardProcessing_afterInsert_switchState();

    tearDown();
}

TEST(
    ObservableLocalReaderAsynchronousAdapterTest,
    removeCard_afterFinalize_shouldNotify_CardRemoved)
{
    setUp();

    testSuite->removeCard_afterFinalize_shouldNotify_CardRemoved();

    tearDown();
}

TEST(
    ObservableLocalReaderAsynchronousAdapterTest,
    removeCard_beforeFinalize_shouldNotify_CardRemoved)
{
    setUp();

    testSuite->removeCard_beforeFinalize_shouldNotify_CardRemoved();

    tearDown();
}

/*
 * Method of ObservableLocalReaderAdapter
 */

TEST(
    ObservableLocalReaderAsynchronousAdapterTest,
    observerThrowsError_shouldBe_transferTo_handler)
{
    setUp();

    auto e = std::make_shared<RuntimeException>();
    auto readerObserver = std::make_shared<ReaderObserverSpiMock>(e);
    testSuite->setObserver(readerObserver);
    testSuite->insertCard_onWaitForCard_shouldNotify_CardInsertedEvent();

    EXPECT_CALL(*handler.get(), onReaderObservationError(_, READER_NAME, _))
        .WillRepeatedly(Return());

    tearDown();
}
