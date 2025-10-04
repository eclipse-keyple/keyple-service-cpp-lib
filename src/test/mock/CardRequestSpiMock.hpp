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
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keypop/card/spi/ApduRequestSpi.hpp"
#include "keypop/card/spi/CardRequestSpi.hpp"

using keypop::card::spi::ApduRequestSpi;
using keypop::card::spi::CardRequestSpi;

class CardRequestSpiMock final : public CardRequestSpi {
public:
    MOCK_METHOD(bool, stopOnUnsuccessfulStatusWord, (), (const, override));
    MOCK_METHOD(
        (const std::vector<std::shared_ptr<ApduRequestSpi>>&),
        getApduRequests,
        (),
        (const, override));
};
