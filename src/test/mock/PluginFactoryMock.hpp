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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keyple/core/common/KeyplePluginExtensionFactory.hpp"
#include "keyple/core/plugin/spi/PluginFactorySpi.hpp"
#include "keyple/core/plugin/spi/PluginSpi.hpp"

using keyple::core::common::KeyplePluginExtensionFactory;
using keyple::core::plugin::spi::PluginFactorySpi;
using keyple::core::plugin::spi::PluginSpi;

class PluginFactoryMock final : public KeyplePluginExtensionFactory,
                                public PluginFactorySpi {
public:
    MOCK_METHOD(const std::string, getPluginApiVersion, (), (const, override));
    MOCK_METHOD(const std::string, getCommonApiVersion, (), (const, override));
    MOCK_METHOD(const std::string&, getPluginName, (), (const, override));
    MOCK_METHOD(std::shared_ptr<PluginSpi>, getPlugin, (), (override));
};
