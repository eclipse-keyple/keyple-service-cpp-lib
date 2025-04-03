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

#include "keypop/card/spi/CardRequestSpi.hpp"
#include "keypop/card/spi/CardSelectionRequestSpi.hpp"

using keypop::card::spi::CardRequestSpi;
using keypop::card::spi::CardSelectionRequestSpi;

class CardSelectionRequestSpiMock final : public CardSelectionRequestSpi {
public:
    MOCK_METHOD(
        (const std::shared_ptr<CardRequestSpi>),
        getCardRequest,
        (),
        (const, override));
    MOCK_METHOD(
        (std::vector<int>&),
        getSuccessfulSelectionStatusWords,
        (),
        (const, override));
};
