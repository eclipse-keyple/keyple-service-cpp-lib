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
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keyple/core/service/IsoCardSelectorAdapter.hpp"
#include "keyple/core/util/HexUtil.hpp"
#include "keypop/reader/selection/CommonIsoCardSelector.hpp"
#include "keypop/reader/selection/FileControlInformation.hpp"
#include "keypop/reader/selection/FileOccurrence.hpp"
#include "keypop/reader/selection/IsoCardSelector.hpp"

using keyple::core::service::IsoCardSelectorAdapter;
using keyple::core::util::HexUtil;
using keypop::reader::selection::CommonIsoCardSelector;
using keypop::reader::selection::FileControlInformation;
using keypop::reader::selection::FileOccurrence;
using keypop::reader::selection::IsoCardSelector;

static std::shared_ptr<IsoCardSelectorAdapter> selectorAdapter;

static void
setUp()
{
    selectorAdapter = std::make_shared<IsoCardSelectorAdapter>();
}

static void
tearDown()
{
    selectorAdapter.reset();
}

TEST(IsoCardSelectorAdapterTest, testDefaultValues)
{
    setUp();

    ASSERT_TRUE(selectorAdapter->getAid().empty());
    ASSERT_EQ(selectorAdapter->getLogicalProtocolName(), "");
    ASSERT_EQ(selectorAdapter->getPowerOnDataRegex(), "");
    ASSERT_EQ(selectorAdapter->getFileOccurrence(), FileOccurrence::FIRST);
    ASSERT_EQ(
        selectorAdapter->getFileControlInformation(),
        FileControlInformation::FCI);

    tearDown();
}

TEST(IsoCardSelectorAdapterTest, testFilterByDfNameByte)
{
    setUp();

    const std::vector<uint8_t> aid = {1, 2, 3};

    selectorAdapter->filterByDfName(aid);

    ASSERT_EQ(selectorAdapter->getAid(), aid);

    tearDown();
}

TEST(IsoCardSelectorAdapterTest, testFilterByDfNameString)
{
    setUp();

    const std::string aid = "010203";

    selectorAdapter->filterByDfName(aid);

    ASSERT_EQ(selectorAdapter->getAid(), HexUtil::toByteArray(aid));

    tearDown();
}

TEST(IsoCardSelectorAdapterTest, testSetFileOccurrence)
{
    setUp();

    FileOccurrence fo = FileOccurrence::LAST;

    selectorAdapter->setFileOccurrence(fo);

    ASSERT_EQ(selectorAdapter->getFileOccurrence(), fo);

    tearDown();
}

TEST(IsoCardSelectorAdapterTest, testSetFileControlInformation)
{
    setUp();

    FileControlInformation fci = FileControlInformation::FCP;

    selectorAdapter->setFileControlInformation(fci);

    ASSERT_EQ(selectorAdapter->getFileControlInformation(), fci);

    tearDown();
}

TEST(IsoCardSelectorAdapterTest, testFilterByCardProtocol)
{
    setUp();

    const std::string protocol = "TestProtocol";

    selectorAdapter->filterByCardProtocol(protocol);

    ASSERT_EQ(selectorAdapter->getLogicalProtocolName(), protocol);

    tearDown();
}

TEST(IsoCardSelectorAdapterTest, testFilterByPowerOnData)
{
    setUp();

    const std::string regex = "TestRegex";

    selectorAdapter->filterByPowerOnData(regex);

    ASSERT_EQ(selectorAdapter->getPowerOnDataRegex(), regex);

    tearDown();
}
