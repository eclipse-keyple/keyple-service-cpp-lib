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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keyple/core/service/BasicCardSelectorAdapter.hpp"
#include "keyple/core/service/CardSelectionManagerAdapter.hpp"
#include "keyple/core/service/IsoCardSelectorAdapter.hpp"
#include "keyple/core/service/ReaderApiFactoryAdapter.hpp"

using keyple::core::service::BasicCardSelectorAdapter;
using keyple::core::service::CardSelectionManagerAdapter;
using keyple::core::service::IsoCardSelectorAdapter;
using keyple::core::service::ReaderApiFactoryAdapter;

static std::shared_ptr<ReaderApiFactoryAdapter> readerApiFactory;

static void
setUp()
{
    readerApiFactory = std::make_shared<ReaderApiFactoryAdapter>();
}

static void
tearDown()
{
    readerApiFactory.reset();
}

TEST(
    ReaderApiFactoryAdapterTest,
    createCardSelectionManager_shouldReturnCardSelectionManagerInstance)
{
    setUp();

    const auto manager = readerApiFactory->createCardSelectionManager();
    const auto adapter
        = std::dynamic_pointer_cast<CardSelectionManagerAdapter>(manager);

    ASSERT_NE(adapter, nullptr);

    tearDown();
}

TEST(
    ReaderApiFactoryAdapterTest,
    createBasicCardSelector_shouldReturnBasicCardSelectorAdapterInstance)
{
    setUp();

    const auto selector = readerApiFactory->createBasicCardSelector();
    const auto adapter
        = std::dynamic_pointer_cast<BasicCardSelectorAdapter>(selector);

    ASSERT_NE(adapter, nullptr);

    tearDown();
}

TEST(
    ReaderApiFactoryAdapterTest,
    createIsoCardSelector_shouldReturnIsoCardSelectorAdapterInstance)
{
    setUp();

    const auto selector = readerApiFactory->createIsoCardSelector();
    const auto adapter
        = std::dynamic_pointer_cast<IsoCardSelectorAdapter>(selector);

    ASSERT_NE(adapter, nullptr);

    tearDown();
}
