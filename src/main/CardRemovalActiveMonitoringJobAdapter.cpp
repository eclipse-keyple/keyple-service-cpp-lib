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

#include "keyple/core/service/CardRemovalActiveMonitoringJobAdapter.hpp"

#include <memory>

#include "keyple/core/plugin/ReaderIOException.hpp"
#include "keyple/core/plugin/TaskCanceledException.hpp"
#include "keyple/core/service/ObservableLocalReaderAdapter.hpp"
#include "keyple/core/util/cpp/Thread.hpp"
#include "keyple/core/util/cpp/exception/InterruptedException.hpp"
#include "keyple/core/util/cpp/exception/RuntimeException.hpp"

namespace keyple {
namespace core {
namespace service {

using InternalEvent = ObservableLocalReaderAdapter::InternalEvent;
using keyple::core::util::cpp::exception::InterruptedException;
using keyple::core::util::cpp::exception::RuntimeException;

/* CARD REMOVAL ACTIVE MONITORING JOB
 * ----------------------------------------------------------- */

CardRemovalActiveMonitoringJobAdapter::CardRemovalActiveMonitoringJob ::
    CardRemovalActiveMonitoringJob(
        std::shared_ptr<AbstractObservableStateAdapter> monitoringState,
        CardRemovalActiveMonitoringJobAdapter* parent)
: Job("CardRemovalActiveMonitoringJobAdapter")
, mMonitoringState(monitoringState)
, mParent(parent)
{
}

void
CardRemovalActiveMonitoringJobAdapter::CardRemovalActiveMonitoringJob::execute()
{
    try {
        mParent->mLogger->trace(
            "Start monitoring job polling process using 'isCardPresentPing()'"
            "method on reader [%]\n",
            mParent->getReader()->getName());

        /* Re-init loop value to true */
        mParent->mLoop = true;

        while (mParent->mLoop) {
            if (!mParent->getReader()->isCardPresentPing()) {
                mParent->mLogger->trace("Card stop responding\n");
                break;
            }

            /* Wait a bit */
            try {
                Thread::sleep(mParent->mSleepDurationMillis);
            } catch (const InterruptedException& ignored) {
                (void)ignored;
                /* Restore interrupted state... */
                interrupt();
                mParent->mLoop = false;
            }
        }

        mParent->mLogger->trace("Monitoring job polling process stopped\n");
    } catch (const RuntimeException& e) {
        mParent->getReader()
            ->getObservationExceptionHandler()
            ->onReaderObservationError(
                mParent->getReader()->getPluginName(),
                mParent->getReader()->getName(),
                std::make_shared<RuntimeException>(e));
    }

    /* Finally block */
    mMonitoringState->onEvent(InternalEvent::CARD_REMOVED);
}

/* CARD REMOVAL ACTIVE MONITORING JOB ADAPTER
 * --------------------------------------------------- */

CardRemovalActiveMonitoringJobAdapter::CardRemovalActiveMonitoringJobAdapter(
    ObservableLocalReaderAdapter* reader, const int64_t sleepDurationMillis)
: AbstractMonitoringJobAdapter(reader)
, mReaderSpi(std::dynamic_pointer_cast<WaitForCardRemovalBlockingSpi>(
      reader->getObservableReaderSpi()))
, mSleepDurationMillis(sleepDurationMillis)
{
}

std::shared_ptr<Job>
CardRemovalActiveMonitoringJobAdapter::getMonitoringJob(
    std::shared_ptr<AbstractObservableStateAdapter> monitoringState)
{
    return std::make_shared<CardRemovalActiveMonitoringJob>(
        monitoringState, this);
}

void
CardRemovalActiveMonitoringJobAdapter::stop()
{
    mLoop = false;
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
