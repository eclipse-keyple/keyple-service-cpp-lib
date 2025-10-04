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

#include "keyple/core/service/BasicCardSelectorAdapter.hpp"

using keyple::core::service::BasicCardSelectorAdapter;

static std::shared_ptr<BasicCardSelectorAdapter> selectorAdapter;

static void
setUp()
{
    selectorAdapter = std::make_shared<BasicCardSelectorAdapter>();
}

static void
tearDown()
{
    selectorAdapter.reset();
}

TEST(BasicCardSelectorAdapterTest, testDefaultValues)
{
    setUp();

    ASSERT_EQ(selectorAdapter->getLogicalProtocolName(), "");
    ASSERT_EQ(selectorAdapter->getPowerOnDataRegex(), "");

    tearDown();
}

TEST(BasicCardSelectorAdapterTest, testFilterByCardProtocol)
{
    setUp();

    const std::string protocol = "TestProtocol";

    selectorAdapter->filterByCardProtocol(protocol);

    ASSERT_EQ(selectorAdapter->getLogicalProtocolName(), protocol);

    tearDown();
}

TEST(BasicCardSelectorAdapterTest, testFilterByPowerOnData)
{
    setUp();

    const std::string regex = "TestRegex";

    selectorAdapter->filterByPowerOnData(regex);

    ASSERT_EQ(selectorAdapter->getPowerOnDataRegex(), regex);

    tearDown();
}
