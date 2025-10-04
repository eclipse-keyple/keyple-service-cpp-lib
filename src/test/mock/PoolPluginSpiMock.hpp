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

#include "keyple/core/common/KeyplePluginExtension.hpp"
#include "keyple/core/plugin/spi/PoolPluginSpi.hpp"
#include "keyple/core/plugin/spi/reader/PoolReaderSpi.hpp"
#include "keyple/core/plugin/spi/reader/ReaderSpi.hpp"

using keyple::core::common::KeyplePluginExtension;
using keyple::core::plugin::spi::PoolPluginSpi;
using keyple::core::plugin::spi::reader::PoolReaderSpi;
using keyple::core::plugin::spi::reader::ReaderSpi;

class PoolPluginSpiMock final : public KeyplePluginExtension,
                                public PoolPluginSpi {
public:
    MOCK_METHOD((const std::string&), getName, (), (const, override));
    MOCK_METHOD(
        (const std::vector<std::string>),
        getReaderGroupReferences,
        (),
        (const, override));
    MOCK_METHOD(
        void,
        releaseReader,
        (std::shared_ptr<ReaderSpi> readerSpi),
        (override));
    MOCK_METHOD(void, onUnregister, (), (override));
    MOCK_METHOD(
        (std::shared_ptr<PoolReaderSpi>),
        allocateReader,
        (const std::string& readerGroupReference),
        (override));
};
