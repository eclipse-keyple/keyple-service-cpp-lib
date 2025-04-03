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

#include "keyple/core/common/KeyplePluginExtension.hpp"
#include "keyple/core/plugin/spi/PoolPluginFactorySpi.hpp"
#include "keyple/core/plugin/spi/PoolPluginSpi.hpp"

using keyple::core::common::KeyplePluginExtension;
using keyple::core::plugin::spi::PoolPluginFactorySpi;
using keyple::core::plugin::spi::PoolPluginSpi;

class PoolPluginFactoryMock final : public KeyplePluginExtensionFactory,
                                    public PoolPluginFactorySpi {
public:
    MOCK_METHOD(
        (const std::string), getPluginApiVersion, (), (const, override));
    MOCK_METHOD(
        (const std::string), getCommonApiVersion, (), (const, override));
    MOCK_METHOD((const std::string&), getPoolPluginName, (), (const, override));
    MOCK_METHOD(std::shared_ptr<PoolPluginSpi>, getPoolPlugin, (), (override));
};
