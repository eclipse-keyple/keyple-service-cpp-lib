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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keyple/core/common/KeypleReaderExtension.hpp"
#include "keyple/core/plugin/spi/reader/PoolReaderSpi.hpp"
#include "keyple/core/util/cpp/Any.hpp"

using keyple::core::common::KeypleReaderExtension;
using keyple::core::plugin::spi::reader::PoolReaderSpi;
using keyple::core::util::cpp::any;

class ReaderMock final : public KeypleReaderExtension, public PoolReaderSpi {
public:
    MOCK_METHOD((const std::string&), getName, (), (const, override, final));
    MOCK_METHOD(bool, isContactless, (), (override, final));
    MOCK_METHOD(void, openPhysicalChannel, (), (override));
    MOCK_METHOD(void, closePhysicalChannel, (), (override));
    MOCK_METHOD(bool, isPhysicalChannelOpen, (), (const, override));
    MOCK_METHOD(bool, checkCardPresence, (), (override));
    MOCK_METHOD((const std::string), getPowerOnData, (), (const, override));
    MOCK_METHOD(void, onUnregister, (), (override));
    MOCK_METHOD(
        (const std::vector<uint8_t>),
        transmitApdu,
        (const std::vector<uint8_t>& apduIn),
        (override));
    MOCK_METHOD(
        std::shared_ptr<any>, getSelectedSmartCard, (), (const, override));
};
