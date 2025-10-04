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

#include "keyple/core/plugin/spi/reader/observable/ObservableReaderSpi.hpp"
#include "keyple/core/service/MultiSelectionProcessing.hpp"
#include "keyple/core/service/ObservableLocalReaderAdapter.hpp"
#include "keypop/card/ChannelControl.hpp"
#include "keypop/reader/cpp/CardSelectorBase.hpp"

using keyple::core::plugin::spi::reader::observable::ObservableReaderSpi;
using keyple::core::service::MultiSelectionProcessing;
using keyple::core::service::ObservableLocalReaderAdapter;
using keypop::card::ChannelControl;
using keypop::reader::cpp::CardSelectorBase;

class ObservableLocalReaderAdapterMock final
: public ObservableLocalReaderAdapter {
public:
    ObservableLocalReaderAdapterMock(
        std::shared_ptr<ObservableReaderSpi> observableReaderSpi,
        const std::string& pluginName)
    : ObservableLocalReaderAdapter(observableReaderSpi, pluginName)
    {
    }

    MOCK_METHOD(
        const std::vector<std::shared_ptr<CardSelectionResponseApi>>,
        transmitCardSelectionRequests,
        (const std::vector<std::shared_ptr<CardSelectorBase>>& cardSelector,
         const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
             cardSelectionRequests,
         const MultiSelectionProcessing multiSelectionProcessing,
         const ChannelControl channelControl),
        (override, final));
};
