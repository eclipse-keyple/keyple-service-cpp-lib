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
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keyple/core/common/KeypleReaderExtension.hpp"
#include "keyple/core/plugin/CardInsertionWaiterAsynchronousApi.hpp"
#include "keyple/core/plugin/CardRemovalWaiterAsynchronousApi.hpp"
#include "keyple/core/plugin/spi/reader/ConfigurableReaderSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/ObservableReaderSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/insertion/CardInsertionWaiterAsynchronousSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/removal/CardRemovalWaiterAsynchronousSpi.hpp"

/* Mock */
#include "mock/ControllableReaderSpiMock.hpp"

using keyple::core::common::KeypleReaderExtension;
using keyple::core::plugin::CardInsertionWaiterAsynchronousApi;
using keyple::core::plugin::CardRemovalWaiterAsynchronousApi;
using keyple::core::plugin::spi::reader::ConfigurableReaderSpi;
using keyple::core::plugin::spi::reader::observable::ObservableReaderSpi;
using keyple::core::plugin::spi::reader::observable::state::insertion::
    CardInsertionWaiterAsynchronousSpi;
using keyple::core::plugin::spi::reader::observable::state::removal::
    CardRemovalWaiterAsynchronousSpi;

class ObservableReaderAsynchronousSpiMock final
: public KeypleReaderExtension,
  public ObservableReaderSpi,
  public CardInsertionWaiterAsynchronousSpi,
  public CardRemovalWaiterAsynchronousSpi,
  public ControllableReaderSpiMock {
public:
    explicit ObservableReaderAsynchronousSpiMock(const std::string& name)
    : mCardInsertionWaiterAsynchronousApi(nullptr)
    , mCardRemovalWaiterAsynchronousApi(nullptr)
    , mDetectionStarted(false)
    , mPhysicalChannelOpen(false)
    , mCardPresent(false)
    , mName(name)
    {
    }

    void
    onStartDetection() override
    {
        mDetectionStarted = true;
    }

    void
    onStopDetection() override
    {
        mDetectionStarted = false;
    }

    const std::string&
    getName() const override
    {
        return mName;
    }

    bool
    isProtocolSupported(const std::string& readerProtocol) const
    {
        (void)readerProtocol;

        return true;
    }

    void
    activateProtocol(const std::string& readerProtocol)
    {
        (void)readerProtocol;
    }

    void
    deactivateProtocol(const std::string& readerProtocol)
    {
        (void)readerProtocol;
    }

    bool
    isCurrentProtocol(const std::string& readerProtocol) const
    {
        (void)readerProtocol;

        return false;
    }

    void
    openPhysicalChannel() override
    {
        mPhysicalChannelOpen = true;
    }

    void
    closePhysicalChannel() override
    {
        mPhysicalChannelOpen = false;
    }

    bool
    isPhysicalChannelOpen() const override
    {
        return mPhysicalChannelOpen;
    }

    bool
    checkCardPresence() override
    {
        return mCardPresent;
    }

    const std::string
    getPowerOnData() const override
    {
        return "";
    }

    const std::vector<uint8_t>
    transmitApdu(const std::vector<uint8_t>& apduIn) override
    {
        (void)apduIn;

        return std::vector<uint8_t>();
    }

    bool
    isContactless() override
    {
        return false;
    }

    void
    onUnregister() override
    {
        /* Nothing to do */
    }

    void
    setCallback(CardInsertionWaiterAsynchronousApi* callback) override
    {
        mCardInsertionWaiterAsynchronousApi = callback;
    }

    void
    setCallback(CardRemovalWaiterAsynchronousApi* callback) override
    {
        mCardRemovalWaiterAsynchronousApi = callback;
    }

    void
    setCardPresent(const bool cardPresent) override
    {
        mCardPresent = cardPresent;
        if (cardPresent) {
            mCardInsertionWaiterAsynchronousApi->onCardInserted();
        } else {
            mCardRemovalWaiterAsynchronousApi->onCardRemoved();
        }
    }

private:
    CardInsertionWaiterAsynchronousApi* mCardInsertionWaiterAsynchronousApi;
    CardRemovalWaiterAsynchronousApi* mCardRemovalWaiterAsynchronousApi;
    bool mDetectionStarted;
    bool mPhysicalChannelOpen;
    std::atomic<bool> mCardPresent;
    std::string mName;
};
