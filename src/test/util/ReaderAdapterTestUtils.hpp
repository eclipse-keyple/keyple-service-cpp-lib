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
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keypop/card/CardSelectionResponseApi.hpp"

#include "../mock/CardSelectionResponseApiMock.hpp"
#include "../mock/ConfigurableReaderSpiMock.hpp"
#include "../mock/ObservableControllableReaderSpiMock.hpp"

using keypop::card::CardSelectionResponseApi;

class ReaderAdapterTestUtils {
public:
    static const std::string READER_NAME;
    static const std::string CARD_PROTOCOL;
    static const std::string OTHER_CARD_PROTOCOL;
    static const std::string POWER_ON_DATA;

    std::vector<std::shared_ptr<CardSelectionResponseApi>>
        NOT_MATCHING_RESPONSES;
    std::vector<std::shared_ptr<CardSelectionResponseApi>>
        MULTI_NOT_MATCHING_RESPONSES;
    std::vector<std::shared_ptr<CardSelectionResponseApi>> MATCHING_RESPONSES;
    std::vector<std::shared_ptr<CardSelectionResponseApi>>
        MULTI_MATCHING_RESPONSES;

    ReaderAdapterTestUtils();
    ~ReaderAdapterTestUtils();

    void reset();
    std::shared_ptr<ConfigurableReaderSpiMock> getReaderSpi();
    std::shared_ptr<ConfigurableReaderSpiMock> getReaderSpiSpy();
    std::shared_ptr<ObservableControllableReaderSpiMock>
    getObservableReaderSpi();
    std::shared_ptr<CardSelectionResponseApiMock> getMatchingResponse();
    std::shared_ptr<CardSelectionResponseApiMock> getNotMatchingResponse();
};
