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

#include "keyple/core/common/KeypleReaderExtension.hpp"
#include "keyple/core/service/AbstractReaderAdapter.hpp"
#include "keyple/core/service/MultiSelectionProcessing.hpp"
#include "keypop/card/CardResponseApi.hpp"
#include "keypop/card/CardSelectionResponseApi.hpp"
#include "keypop/card/ChannelControl.hpp"
#include "keypop/card/spi/CardRequestSpi.hpp"
#include "keypop/card/spi/CardSelectionRequestSpi.hpp"
#include "keypop/reader/cpp/CardSelectorBase.hpp"

using keyple::core::common::KeypleReaderExtension;
using keyple::core::service::AbstractReaderAdapter;
using keyple::core::service::MultiSelectionProcessing;
using keypop::card::CardResponseApi;
using keypop::card::CardSelectionResponseApi;
using keypop::card::ChannelControl;
using keypop::card::spi::CardRequestSpi;
using keypop::card::spi::CardSelectionRequestSpi;
using keypop::reader::cpp::CardSelectorBase;

class AbstractReaderAdapterMock final : public AbstractReaderAdapter {
public:
    AbstractReaderAdapterMock(
        const std::string& readerName,
        const std::shared_ptr<KeypleReaderExtension> readerExtension,
        const std::string& pluginName)
    : AbstractReaderAdapter(readerName, readerExtension, pluginName)
    {
    }

    MOCK_METHOD(void, releaseChannel, (), (override));
    MOCK_METHOD(bool, isContactless, (), (override));
    MOCK_METHOD(bool, isCardPresent, (), (override));
    MOCK_METHOD(
        (std::vector<std::shared_ptr<CardSelectionResponseApi>>),
        processCardSelectionRequests,
        (const std::vector<std::shared_ptr<CardSelectorBase>>& cardSelectors,
         const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
             cardSelectionRequests,
         const MultiSelectionProcessing multiSelectionProcessing,
         const ChannelControl channelControl),
        (override));
    MOCK_METHOD(
        (std::shared_ptr<CardResponseApi>),
        processCardRequest,
        (const std::shared_ptr<CardRequestSpi> cardRequest,
         const ChannelControl channelControl),
        (override));
};
