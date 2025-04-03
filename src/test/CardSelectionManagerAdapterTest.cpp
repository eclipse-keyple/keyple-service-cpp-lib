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

#include "keyple/core/service/CardSelectionManagerAdapter.hpp"
#include "keyple/core/service/SmartCardServiceProvider.hpp"
#include "keyple/core/util/cpp/exception/IllegalStateException.hpp"

using keyple::core::service::CardSelectionManagerAdapter;
using keyple::core::service::SmartCardServiceProvider;
using keyple::core::util::cpp::exception::IllegalStateException;

static std::shared_ptr<CardSelectionManagerAdapter> manager;

static void
setUp()
{
    manager = std::dynamic_pointer_cast<CardSelectionManagerAdapter>(
        SmartCardServiceProvider::getService()
            ->getReaderApiFactory()
            ->createCardSelectionManager());
}

static void
tearDown()
{
    manager.reset();
}

// TEST(CardSelectionManagerAdapterTest,
// exportProcessedCardSelectionScenario_whenScenarioIsNotProcessed_shouldThrowISE)
// {
//     setUp();

//     EXPECT_THROW(manager->exportProcessedCardSelectionScenario(),
//     IllegalStateException);

//     tearDown();
// }

// TEST(CardSelectionManagerAdapterTest,
// importProcessedCardSelectionScenario_whenArgIsNull_shouldThrowIAE)
// {
//     setUp();

//     EXPECT_THROW(manager->importProcessedCardSelectionScenario(nullptr),
//     IllegalStateException);
// }

TEST(
    CardSelectionManagerAdapterTest,
    importProcessedCardSelectionScenario_whenArgIsNull2_shouldThrowIAE)
{
    setUp();

    EXPECT_THROW(
        manager->importProcessedCardSelectionScenario("null"),
        IllegalStateException);

    tearDown();
}

TEST(
    CardSelectionManagerAdapterTest,
    importProcessedCardSelectionScenario_whenArgIsEmpty_shouldThrowIAE)
{
    setUp();

    EXPECT_THROW(
        manager->importProcessedCardSelectionScenario(""),
        IllegalStateException);

    tearDown();
}

TEST(
    CardSelectionManagerAdapterTest,
    importProcessedCardSelectionScenario_whenArgIsMalformed_shouldThrowIAE)
{
    setUp();

    EXPECT_THROW(
        manager->importProcessedCardSelectionScenario("test"),
        IllegalStateException);

    tearDown();
}
