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
#include "keyple/core/plugin/CardIOException.hpp"
#include "keyple/core/plugin/ReaderIOException.hpp"
#include "keyple/core/plugin/spi/reader/ConfigurableReaderSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/ObservableReaderSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/insertion/CardInsertionWaiterNonBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/removal/CardRemovalWaiterNonBlockingSpi.hpp"
#include "keyple/core/util/cpp/Thread.hpp"
#include "keyple/core/util/cpp/exception/InterruptedException.hpp"

/* Mock */
#include "mock/ControllableReaderSpiMock.hpp"

using keyple::core::common::KeypleReaderExtension;
using keyple::core::plugin::CardIOException;
using keyple::core::plugin::ReaderIOException;
using keyple::core::plugin::spi::reader::ConfigurableReaderSpi;
using keyple::core::plugin::spi::reader::observable::ObservableReaderSpi;
using keyple::core::plugin::spi::reader::observable::state::insertion::
    CardInsertionWaiterNonBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::removal::
    CardRemovalWaiterNonBlockingSpi;
using keyple::core::util::cpp::Thread;
using keyple::core::util::cpp::exception::InterruptedException;

class ObservableReaderNonBlockingSpiMock final
: public KeypleReaderExtension,
  public ConfigurableReaderSpi,
  public ObservableReaderSpi,
  public CardInsertionWaiterNonBlockingSpi,
  public CardRemovalWaiterNonBlockingSpi,
  public ControllableReaderSpiMock {
public:
    explicit ObservableReaderNonBlockingSpiMock(const std::string& name)
    : mName(name)
    {
    }

    void
    onStartDetection()
    {
        mDetectionStarted = true;
    }

    void
    onStopDetection()
    {
        mDetectionStarted = false;
    }

    const std::string&
    getName() const
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
    openPhysicalChannel()
    {
        mPhysicalChannelOpen = true;
    }

    void
    closePhysicalChannel()
    {
        mPhysicalChannelOpen = false;
    }

    bool
    isPhysicalChannelOpen() const
    {
        return mPhysicalChannelOpen;
    }

    bool
    checkCardPresence()
    {
        return mCardPresent;
    }

    const std::string
    getPowerOnData() const
    {
        return "";
    }

    const std::vector<uint8_t>
    transmitApdu(const std::vector<uint8_t>& apduIn)
    {
        (void)apduIn;

        if (mCardPresent) {
            return std::vector<uint8_t>();
        } else {
            throw CardIOException("card is not present");
        }
    }

    bool
    isContactless()
    {
        return false;
    }

    void
    onUnregister()
    {
        /* Nothing to do */
    }

    void
    setCardPresent(const bool cardPresent)
    {
        mCardPresent = cardPresent;
    }

    int
    getCardInsertionMonitoringSleepDuration() const
    {
        return 10;
    }

    int
    getCardRemovalMonitoringSleepDuration() const
    {
        return 10;
    }

private:
    bool mDetectionStarted = false;
    bool mPhysicalChannelOpen = false;
    std::atomic<bool> mCardPresent {false};
    std::string mName;
};
