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

#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keypop/card/spi/SmartCardSpi.hpp"
#include "keypop/reader/selection/spi/SmartCard.hpp"

using keypop::card::spi::SmartCardSpi;
using keypop::reader::selection::spi::SmartCard;

class SmartCardMock final : public SmartCard, public SmartCardSpi {
public:
    MOCK_METHOD((const std::string&), getPowerOnData, (), (const, override));
};
