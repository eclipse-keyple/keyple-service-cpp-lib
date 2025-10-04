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

#include "keyple/core/service/ObservableReaderStateServiceAdapter.hpp"

#include <memory>

#include "keyple/core/plugin/spi/reader/observable/state/insertion/CardInsertionWaiterAsynchronousSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/insertion/CardInsertionWaiterBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/insertion/CardInsertionWaiterNonBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/insertion/WaitForCardInsertionBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/insertion/WaitForCardInsertionNonBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/processing/CardPresenceMonitorBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/processing/WaitForCardRemovalDuringProcessingBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/removal/CardRemovalWaiterBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/removal/CardRemovalWaiterNonBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/removal/WaitForCardRemovalNonBlockingSpi.hpp"
#include "keyple/core/service/CardInsertionActiveMonitoringJobAdapter.hpp"
#include "keyple/core/service/CardInsertionPassiveMonitoringJobAdapter.hpp"
#include "keyple/core/service/CardRemovalActiveMonitoringJobAdapter.hpp"
#include "keyple/core/service/CardRemovalPassiveMonitoringJobAdapter.hpp"
#include "keyple/core/service/WaitForCardInsertionStateAdapter.hpp"
#include "keyple/core/service/WaitForCardProcessingStateAdapter.hpp"
#include "keyple/core/service/WaitForCardRemovalStateAdapter.hpp"
#include "keyple/core/service/WaitForStartDetectStateAdapter.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::plugin::spi::reader::observable::state::insertion::
    CardInsertionWaiterAsynchronousSpi;
using keyple::core::plugin::spi::reader::observable::state::insertion::
    CardInsertionWaiterBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::insertion::
    CardInsertionWaiterNonBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::insertion::
    WaitForCardInsertionBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::insertion::
    WaitForCardInsertionNonBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::processing::
    CardPresenceMonitorBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::processing::
    WaitForCardRemovalDuringProcessingBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::removal::
    CardRemovalWaiterBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::removal::
    CardRemovalWaiterNonBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::removal::
    WaitForCardRemovalNonBlockingSpi;

ObservableReaderStateServiceAdapter::ObservableReaderStateServiceAdapter(
    ObservableLocalReaderAdapter* reader)
: mReader(reader)
, mReaderSpi(reader->getObservableReaderSpi())
, mExecutorService(std::make_shared<ExecutorService>())
{
    /* Wait for start */
    mStates.insert(
        {MonitoringState::WAIT_FOR_START_DETECTION,
         std::make_shared<WaitForStartDetectStateAdapter>(mReader)});

    /* Insertion */
    if (std::dynamic_pointer_cast<CardInsertionWaiterAsynchronousSpi>(
            mReaderSpi)
        || std::dynamic_pointer_cast<WaitForCardInsertionAutonomousSpi>(
            mReaderSpi)) {
        mStates.insert(
            {MonitoringState::WAIT_FOR_CARD_INSERTION,
             std::make_shared<WaitForCardInsertionStateAdapter>(mReader)});

    } else if (
        std::dynamic_pointer_cast<CardInsertionWaiterNonBlockingSpi>(mReaderSpi)
        || std::dynamic_pointer_cast<WaitForCardInsertionNonBlockingSpi>(
            mReaderSpi)) {
        const auto readerSpi
            = std::dynamic_pointer_cast<CardInsertionWaiterNonBlockingSpi>(
                mReaderSpi);
        int sleepDurationMillis
            = readerSpi ? readerSpi->getCardInsertionMonitoringSleepDuration()
                        : 100;

        auto cardInsertionActiveMonitoringJobAdapter
            = std::make_shared<CardInsertionActiveMonitoringJobAdapter>(
                mReader, sleepDurationMillis, true);

        mStates.insert(
            {MonitoringState::WAIT_FOR_CARD_INSERTION,
             std::make_shared<WaitForCardInsertionStateAdapter>(
                 mReader,
                 cardInsertionActiveMonitoringJobAdapter,
                 mExecutorService)});

    } else if (
        std::dynamic_pointer_cast<CardInsertionWaiterBlockingSpi>(mReaderSpi)
        || std::dynamic_pointer_cast<WaitForCardInsertionBlockingSpi>(
            mReaderSpi)) {
        auto cardInsertionPassiveMonitoringJobAdapter
            = std::make_shared<CardInsertionPassiveMonitoringJobAdapter>(
                mReader);
        mStates.insert(
            {MonitoringState::WAIT_FOR_CARD_INSERTION,
             std::make_shared<WaitForCardInsertionStateAdapter>(
                 mReader,
                 cardInsertionPassiveMonitoringJobAdapter,
                 mExecutorService)});

    } else {
        throw IllegalStateException(
            "Reader should implement implement a WaitForCardInsertion "
            "interface");
    }

    /* Processing */
    if (std::dynamic_pointer_cast<CardPresenceMonitorBlockingSpi>(mReaderSpi)
        || std::dynamic_pointer_cast<
            WaitForCardRemovalDuringProcessingBlockingSpi>(mReaderSpi)) {
        auto cardRemovalPassiveMonitoringJobAdapter
            = std::make_shared<CardRemovalPassiveMonitoringJobAdapter>(mReader);
        mStates.insert(
            {MonitoringState::WAIT_FOR_CARD_PROCESSING,
             std::make_shared<WaitForCardProcessingStateAdapter>(
                 mReader,
                 cardRemovalPassiveMonitoringJobAdapter,
                 mExecutorService)});

    } else {
        mStates.insert(
            {MonitoringState::WAIT_FOR_CARD_PROCESSING,
             std::make_shared<WaitForCardProcessingStateAdapter>(mReader)});
    }

    /* Removal */
    if (std::dynamic_pointer_cast<CardRemovalWaiterAsynchronousSpi>(mReaderSpi)
        || std::dynamic_pointer_cast<WaitForCardRemovalAutonomousSpi>(
            mReaderSpi)) {
        mStates.insert(
            {MonitoringState::WAIT_FOR_CARD_REMOVAL,
             std::make_shared<WaitForCardRemovalStateAdapter>(mReader)});

    } else if (
        std::dynamic_pointer_cast<CardRemovalWaiterNonBlockingSpi>(mReaderSpi)
        || std::dynamic_pointer_cast<WaitForCardRemovalNonBlockingSpi>(
            mReaderSpi)) {
        const auto readerSpi
            = std::dynamic_pointer_cast<CardRemovalWaiterNonBlockingSpi>(
                mReaderSpi);
        int sleepDurationMillis
            = readerSpi ? readerSpi->getCardRemovalMonitoringSleepDuration()
                        : 100;

        auto cardRemovalActiveMonitoringJobAdapter
            = std::make_shared<CardRemovalActiveMonitoringJobAdapter>(
                mReader, sleepDurationMillis);

        mStates.insert(
            {MonitoringState::WAIT_FOR_CARD_REMOVAL,
             std::make_shared<WaitForCardRemovalStateAdapter>(
                 mReader,
                 cardRemovalActiveMonitoringJobAdapter,
                 mExecutorService)});

    } else if (
        std::dynamic_pointer_cast<CardRemovalWaiterBlockingSpi>(mReaderSpi)
        || std::dynamic_pointer_cast<WaitForCardRemovalBlockingSpi>(
            mReaderSpi)) {
        auto cardRemovalPassiveMonitoringJobAdapter
            = std::make_shared<CardRemovalPassiveMonitoringJobAdapter>(mReader);
        mStates.insert(
            {MonitoringState::WAIT_FOR_CARD_REMOVAL,
             std::make_shared<WaitForCardRemovalStateAdapter>(
                 mReader,
                 cardRemovalPassiveMonitoringJobAdapter,
                 mExecutorService)});

    } else {
        throw IllegalStateException(
            "Reader should implement implement a WaitForCardRemoval "
            "interface");
    }

    switchState(MonitoringState::WAIT_FOR_START_DETECTION);
}

void
ObservableReaderStateServiceAdapter::onEvent(const InternalEvent event)
{
    /*
     * C++: cannot use std::lock_guard as mutex needs to be unlocked before
     * calling onEvent.
     */
    mMutex.lock();

    switch (event) {
    case InternalEvent::CARD_INSERTED:
    case InternalEvent::CARD_REMOVED:
    case InternalEvent::CARD_PROCESSED:
    case InternalEvent::STOP_DETECT:
    case InternalEvent::TIME_OUT:
        break;
    case InternalEvent::START_DETECT:
        mReaderSpi->onStartDetection();
        break;
    }

    mMutex.unlock();
    mCurrentState->onEvent(event);
}

void
ObservableReaderStateServiceAdapter::switchState(const MonitoringState stateId)
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (mCurrentState != nullptr) {
        mLogger->trace(
            "Switch state of reader [%] from % to %\n",
            mReader->getName(),
            mCurrentState->getMonitoringState(),
            stateId);
        mCurrentState->onDeactivate();
    } else {
        mLogger->trace(
            "Switch state of reader [%] to %\n", mReader->getName(), stateId);
    }

    /* Switch currentState */
    mCurrentState = mStates.at(stateId);

    /*
     * As soon as the state machine returns to the WAIT_FOR_START_DETECTION
     * state, we deactivate card detection in the plugin.
     */
    if (stateId == MonitoringState::WAIT_FOR_START_DETECTION) {
        mReaderSpi->onStopDetection();
    }

    /* onActivate the new current state */
    mCurrentState->onActivate();
}

std::shared_ptr<AbstractObservableStateAdapter>
ObservableReaderStateServiceAdapter::getCurrentState()
{
    std::lock_guard<std::mutex> lock(mMutex);

    return mCurrentState;
}

MonitoringState
ObservableReaderStateServiceAdapter::getCurrentMonitoringState()
{
    std::lock_guard<std::mutex> lock(mMutex);

    return mCurrentState->getMonitoringState();
}

void
ObservableReaderStateServiceAdapter::shutdown()
{
    mExecutorService->shutdown();
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
