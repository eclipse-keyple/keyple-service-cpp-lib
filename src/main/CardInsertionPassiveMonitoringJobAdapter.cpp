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

#include "keyple/core/service/CardInsertionPassiveMonitoringJobAdapter.hpp"

#include <memory>

#include "keyple/core/plugin/ReaderIOException.hpp"
#include "keyple/core/plugin/TaskCanceledException.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/insertion/CardInsertionWaiterBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/insertion/WaitForCardInsertionBlockingSpi.hpp"
#include "keyple/core/util/cpp/exception/RuntimeException.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::plugin::ReaderIOException;
using keyple::core::plugin::TaskCanceledException;
using keyple::core::plugin::spi::reader::observable::state::insertion::
    CardInsertionWaiterBlockingSpi;
using keyple::core::plugin::spi::reader::observable::state::insertion::
    WaitForCardInsertionBlockingSpi;
using keyple::core::util::cpp::exception::RuntimeException;

using InternalEvent = ObservableLocalReaderAdapter::InternalEvent;

/* CARD INSERTION PASSIVE MONITORING JOB
 * -------------------------------------------------------- */

CardInsertionPassiveMonitoringJobAdapter::CardInsertionPassiveMonitoringJob ::
    CardInsertionPassiveMonitoringJob(
        std::shared_ptr<AbstractObservableStateAdapter> monitoringState,
        CardInsertionPassiveMonitoringJobAdapter* parent)
: Job("CardInsertionPassiveMonitoringJob")
, mMonitoringState(monitoringState)
, mParent(parent)
{
}

void
CardInsertionPassiveMonitoringJobAdapter::CardInsertionPassiveMonitoringJob::
    execute()
{
    try {
        mParent->mLogger->trace(
            "Start monitoring job process on reader [%]\n",
            mParent->getReader()->getName());

        const auto& cardInsertion
            = std::dynamic_pointer_cast<CardInsertionWaiterBlockingSpi>(
                mParent->mReaderSpi);
        const auto& waitForCard
            = std::dynamic_pointer_cast<WaitForCardInsertionBlockingSpi>(
                mParent->mReaderSpi);
        if (cardInsertion != nullptr) {
            cardInsertion->waitForCardInsertion();
        } else if (waitForCard != nullptr) {
            waitForCard->waitForCardInsertion();
        }
        mMonitoringState->onEvent(InternalEvent::CARD_INSERTED);

    } catch (const ReaderIOException& e) {
        /* Just warn as it can be a disconnection of the reader. */
        mParent->mLogger->warn(
            "Monitoring job error while processing card insertion event on "
            "reader [%]: %\n",
            mParent->getReader()->getName(),
            e.getMessage());

    } catch (const TaskCanceledException& e) {
        mParent->mLogger->warn(
            "Monitoring job process cancelled: %\n", e.getMessage());

    } catch (const RuntimeException& e) {
        mParent->getReader()
            ->getObservationExceptionHandler()
            ->onReaderObservationError(
                mParent->getReader()->getPluginName(),
                mParent->getReader()->getName(),
                std::make_shared<RuntimeException>(e));
    }
}

/* CARD INSERTION PASSIVE MONITORING JOB ADAPTER
 * ------------------------------------------------ */

CardInsertionPassiveMonitoringJobAdapter::
    CardInsertionPassiveMonitoringJobAdapter(
        ObservableLocalReaderAdapter* reader)
: AbstractMonitoringJobAdapter(reader)
, mReaderSpi(reader->getObservableReaderSpi())
{
}

std::shared_ptr<Job>
CardInsertionPassiveMonitoringJobAdapter::getMonitoringJob(
    std::shared_ptr<AbstractObservableStateAdapter> monitoringState)
{
    return std::make_shared<CardInsertionPassiveMonitoringJob>(
        monitoringState, this);
}

void
CardInsertionPassiveMonitoringJobAdapter::stop()
{
    mLogger->trace("Stop monitoring job process\n");

    const auto& cardInsertion
        = std::dynamic_pointer_cast<CardInsertionWaiterBlockingSpi>(mReaderSpi);
    const auto& waitForCard
        = std::dynamic_pointer_cast<WaitForCardInsertionBlockingSpi>(
            mReaderSpi);
    if (cardInsertion != nullptr) {
        cardInsertion->stopWaitForCardInsertion();
    } else if (waitForCard != nullptr) {
        waitForCard->stopWaitForCardInsertion();
    }

    mLogger->trace("Monitoring job process stopped\n");
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
