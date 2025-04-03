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
#include "keyple/core/plugin/spi/reader/ConfigurableReaderSpi.hpp"
#include "keyple/core/plugin/spi/reader/ReaderSpi.hpp"

using keyple::core::common::KeypleReaderExtension;
using keyple::core::plugin::spi::reader::ConfigurableReaderSpi;
using keyple::core::plugin::spi::reader::ReaderSpi;

class ConfigurableReaderSpiMock final : public KeypleReaderExtension,
                                        public ConfigurableReaderSpi {
public:
    MOCK_METHOD(const std::string&, getName, (), (const, override));
    MOCK_METHOD(void, openPhysicalChannel, (), (override));
    MOCK_METHOD(void, closePhysicalChannel, (), (override, final));
    MOCK_METHOD(bool, isPhysicalChannelOpen, (), (const, override));
    MOCK_METHOD(bool, checkCardPresence, (), (override, final));
    MOCK_METHOD((const std::string), getPowerOnData, (), (const, override));
    MOCK_METHOD(bool, isContactless, (), (override, final));
    MOCK_METHOD(void, onUnregister, (), (override, final));
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
};
