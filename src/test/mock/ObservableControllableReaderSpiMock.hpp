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

#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keyple/core/common/KeypleReaderExtension.hpp"
#include "keyple/core/plugin/CardInsertionWaiterAsynchronousApi.hpp"
#include "keyple/core/plugin/CardRemovalWaiterAsynchronousApi.hpp"
#include "keyple/core/plugin/spi/reader/ConfigurableReaderSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/ObservableReaderSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/insertion/CardInsertionWaiterAsynchronousSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/removal/CardRemovalWaiterAsynchronousSpi.hpp"

/* Mock */
#include "./ControllableReaderSpiMock.hpp"

using keyple::core::common::KeypleReaderExtension;
using keyple::core::plugin::CardInsertionWaiterAsynchronousApi;
using keyple::core::plugin::CardRemovalWaiterAsynchronousApi;
using keyple::core::plugin::spi::reader::ConfigurableReaderSpi;
using keyple::core::plugin::spi::reader::observable::ObservableReaderSpi;
using keyple::core::plugin::spi::reader::observable::state::insertion::
    CardInsertionWaiterAsynchronousSpi;
using keyple::core::plugin::spi::reader::observable::state::removal::
    CardRemovalWaiterAsynchronousSpi;

class ObservableControllableReaderSpiMock final
: public KeypleReaderExtension,
  public ConfigurableReaderSpi,
  public ObservableReaderSpi,
  public CardInsertionWaiterAsynchronousSpi,
  public CardRemovalWaiterAsynchronousSpi,
  public ControllableReaderSpiMock {
public:
    MOCK_METHOD(const std::string&, getName, (), (const, override));
    MOCK_METHOD(void, openPhysicalChannel, (), (override));
    MOCK_METHOD(void, closePhysicalChannel, (), (override));
    MOCK_METHOD(bool, isPhysicalChannelOpen, (), (const, override));
    MOCK_METHOD(bool, checkCardPresence, (), (override));
    MOCK_METHOD((const std::string), getPowerOnData, (), (const, override));
    MOCK_METHOD(bool, isContactless, (), (override));
    MOCK_METHOD(void, onUnregister, (), (override));
    MOCK_METHOD(void, onStartDetection, (), (override));
    MOCK_METHOD(void, onStopDetection, (), (override));
    MOCK_METHOD(
        (const std::vector<uint8_t>),
        transmitApdu,
        (const std::vector<uint8_t>& apduIn),
        (override));
    MOCK_METHOD(
        bool, isProtocolSupported, (const std::string&), (const, override));
    MOCK_METHOD(void, activateProtocol, (const std::string&), (override));
    MOCK_METHOD(void, deactivateProtocol, (const std::string&), (override));
    MOCK_METHOD(
        bool, isCurrentProtocol, (const std::string&), (const, override));
    MOCK_METHOD(
        void, setCallback, (CardInsertionWaiterAsynchronousApi*), (override));
    MOCK_METHOD(
        void, setCallback, (CardRemovalWaiterAsynchronousApi*), (override));
    MOCK_METHOD(void, setCardPresent, (const bool), (override));
};
