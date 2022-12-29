/**************************************************************************************************
 * Copyright (c) 2022 Calypso Networks Association https://calypsonet.org/                        *
 *                                                                                                *
 * See the NOTICE file(s) distributed with this work for additional information regarding         *
 * copyright ownership.                                                                           *
 *                                                                                                *
 * This program and the accompanying materials are made available under the terms of the Eclipse  *
 * Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                  *
 *                                                                                                *
 * SPDX-License-Identifier: EPL-2.0                                                               *
 **************************************************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "gtest/gtest.h"

/* Keyple Core Service */
#include "KeypleReaderExtension.h"

/* Keyple Core Plugin */
#include "ReaderSpi.h"

using namespace testing;

using namespace keyple::core::plugin::spi::reader;
using namespace keyple::core::service;

class ReaderMock final : public KeypleReaderExtension, public ReaderSpi {
public:
    MOCK_METHOD((const std::string&), getName, (), (const, override, final));
    MOCK_METHOD(bool, isContactless, (), (override, final));
    MOCK_METHOD(void, openPhysicalChannel, (), (override));
    MOCK_METHOD(void, closePhysicalChannel, (), (override));
    MOCK_METHOD(bool, isPhysicalChannelOpen, (), (const, override));
    MOCK_METHOD(bool, checkCardPresence, (), (override));
    MOCK_METHOD((const std::string), getPowerOnData, (), (const, override));
    MOCK_METHOD(void, onUnregister, (), (override));
    MOCK_METHOD((const std::vector<uint8_t>), transmitApdu, (const std::vector<uint8_t>& apduIn), (override));
};
