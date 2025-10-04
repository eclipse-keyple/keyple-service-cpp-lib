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
#include "keyple/core/plugin/AutonomousObservablePluginApi.hpp"
#include "keyple/core/plugin/spi/AutonomousObservablePluginSpi.hpp"
#include "keyple/core/plugin/spi/reader/ReaderSpi.hpp"

using keyple::core::common::KeyplePluginExtension;
using keyple::core::plugin::AutonomousObservablePluginApi;
using keyple::core::plugin::spi::AutonomousObservablePluginSpi;
using keyple::core::plugin::spi::reader::ReaderSpi;

class AutonomousObservablePluginSpiMock final
: public AutonomousObservablePluginSpi,
  public KeyplePluginExtension {
public:
    MOCK_METHOD(void, onUnregister, (), (override));
    MOCK_METHOD((const std::string&), getName, (), (const, override));
    MOCK_METHOD(void, connect, (AutonomousObservablePluginApi*), (override));
    MOCK_METHOD(
        (const std::vector<std::shared_ptr<ReaderSpi>>),
        searchAvailableReaders,
        (),
        (override));
    MOCK_METHOD(
        void, setCallback, (AutonomousObservablePluginApi*), (override));
};
