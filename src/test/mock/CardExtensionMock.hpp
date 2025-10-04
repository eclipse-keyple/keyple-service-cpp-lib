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

#include "keyple/core/common/KeypleCardExtension.hpp"

using keyple::core::common::KeypleCardExtension;

class CardExtensionMock final : public KeypleCardExtension {
public:
    MOCK_METHOD(const std::string, getReaderApiVersion, (), (const, override));
    MOCK_METHOD(const std::string, getCardApiVersion, (), (const, override));
    MOCK_METHOD(const std::string, getCommonApiVersion, (), (const, override));
};
