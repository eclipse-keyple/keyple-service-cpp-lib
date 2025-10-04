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

#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keypop/card/ApduResponseApi.hpp"

using keypop::card::ApduResponseApi;

class ApduResponseApiMock final : public ApduResponseApi {
public:
    MOCK_METHOD((const std::vector<uint8_t>&), getApdu, (), (const, override));
    MOCK_METHOD(
        (const std::vector<uint8_t>), getDataOut, (), (const, override));
    MOCK_METHOD(int, getStatusWord, (), (const, override));
};
