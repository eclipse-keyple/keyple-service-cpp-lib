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
#include "keyple/core/plugin/spi/reader/observable/state/insertion/CardInsertionWaiterBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/processing/CardPresenceMonitorBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/removal/CardRemovalWaiterBlockingSpi.hpp"
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
    CardInsertionWaiterBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::processing::
    CardPresenceMonitorBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::removal::
    CardRemovalWaiterBlockingSpi;
using keyple::core::util::cpp::Thread;
using keyple::core::util::cpp::exception::InterruptedException;

class ObservableReaderBlockingSpiMock final
: public KeypleReaderExtension,
  public ConfigurableReaderSpi,
  public ObservableReaderSpi,
  public CardInsertionWaiterBlockingSpi,
  public CardPresenceMonitorBlockingSpi,
  public CardRemovalWaiterBlockingSpi,
  public ControllableReaderSpiMock {
public:
    ObservableReaderBlockingSpiMock(
        const std::string& name,
        const int64_t waitInsertion,
        const int64_t waitRemoval)
    : mDetectionStarted(false)
    , mPhysicalChannelOpen(false)
    , mCardPresent(false)
    , mInsertions(0)
    , mRemovals(0)
    , mWaitInsertion(waitInsertion)
    , mWaitRemoval(waitRemoval)
    , mName(name)
    {
    }

    void
    setCardPresent(const bool cardPresent)
    {
        mCardPresent = cardPresent;
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

    /**
     * Wait for a one time card insertion
     *
     * @throws ReaderIOException
     * @throws TaskCanceledException
     */
    void
    waitForCardInsertion()
    {
        try {
            /* Card is detected after a timeout */
            Thread::sleep(mWaitInsertion);
            mInsertions++;
        } catch (const InterruptedException& e) {
            (void)e;
        }

        /* If card already inserted, throw ex */
        if (mInsertions > 1) {
            throw ReaderIOException("no card present");
        }
    }

    void
    stopWaitForCardInsertion()
    {
        // FIXME: Thread.currentThread().interrupt();
    }

    void
    waitForCardRemoval()
    {
        try {
            Thread::sleep(mWaitRemoval);
            /* Card removal is detected after a timeout */
            mRemovals++;
        } catch (const InterruptedException& e) {
            (void)e;
        }

        if (mRemovals > 1) {
            throw ReaderIOException("card not removed ?!");
        }
    }

    void
    stopWaitForCardRemoval()
    {
        // FIXME: Thread.currentThread().interrupt();
    }

    void
    monitorCardPresenceDuringProcessing()
    {
        waitForCardRemoval();
    }

    void
    stopCardPresenceMonitoringDuringProcessing()
    {
        stopWaitForCardRemoval();
    }

private:
    bool mDetectionStarted;
    bool mPhysicalChannelOpen;
    std::atomic<bool> mCardPresent;
    std::atomic<int> mInsertions;
    std::atomic<int> mRemovals;
    int64_t mWaitInsertion;
    int64_t mWaitRemoval;
    std::string mName;
};
