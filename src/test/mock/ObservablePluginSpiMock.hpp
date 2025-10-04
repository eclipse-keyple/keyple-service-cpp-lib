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
#include "keyple/core/plugin/spi/ObservablePluginSpi.hpp"

using keyple::core::common::KeyplePluginExtension;
using keyple::core::plugin::spi::ObservablePluginSpi;

class ObservablePluginSpiMock final : public ObservablePluginSpi,
                                      public KeyplePluginExtension {
public:
    MOCK_METHOD(const std::string&, getName, (), (const, override));
    MOCK_METHOD(
        (const std::vector<std::shared_ptr<ReaderSpi>>),
        searchAvailableReaders,
        (),
        (override));
    MOCK_METHOD(
        const std::vector<std::string>,
        searchAvailableReaderNames,
        (),
        (override));
    MOCK_METHOD(
        std::shared_ptr<ReaderSpi>,
        searchReader,
        (const std::string& readerName),
        (override));
    MOCK_METHOD(int, getMonitoringCycleDuration, (), (const, override));
    MOCK_METHOD(void, onUnregister, (), (override));
};
