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

#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keyple/core/service/ApduResponseAdapter.hpp"
#include "keyple/core/util/HexUtil.hpp"

using keyple::core::service::ApduResponseAdapter;
using keyple::core::util::HexUtil;

static const std::string HEX_REQUEST = "123456789000";
static const std::string HEX_REQUEST_DATA = "12345678";

TEST(ApduResponseAdapterTest, buildApduResponseAdapter)
{
    ApduResponseAdapter apduResponseAdapter(HexUtil::toByteArray(HEX_REQUEST));

    ASSERT_EQ(apduResponseAdapter.getApdu(), HexUtil::toByteArray(HEX_REQUEST));
    ASSERT_EQ(apduResponseAdapter.getStatusWord(), 0x9000);
    ASSERT_EQ(
        apduResponseAdapter.getDataOut(),
        HexUtil::toByteArray(HEX_REQUEST_DATA));
}
