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

#include "keyple/core/service/CardInsertionActiveMonitoringJobAdapter.hpp"

#include <memory>
#include <string>

#include "keyple/core/util/cpp/exception/InterruptedException.hpp"
#include "keyple/core/util/cpp/exception/RuntimeException.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::util::cpp::exception::InterruptedException;
using keyple::core::util::cpp::exception::RuntimeException;

using InternalEvent = ObservableLocalReaderAdapter::InternalEvent;

const std::string CardInsertionActiveMonitoringJobAdapter::JOB_ID
    = "INSERTION_ACTIVE";

/* CARD INSERTION ACTIVE MONITORING JOB
 * --------------------------------------------------------- */

CardInsertionActiveMonitoringJobAdapter::CardInsertionActiveMonitoringJob ::
    CardInsertionActiveMonitoringJob(
        std::shared_ptr<AbstractObservableStateAdapter> monitoringState,
        CardInsertionActiveMonitoringJobAdapter* parent)
: Job("CardInsertionActiveMonitoringJobAdapter - " + parent->mReader->getName())
, mMonitoringState(monitoringState)
, mParent(parent)
{
}

void
CardInsertionActiveMonitoringJobAdapter::CardInsertionActiveMonitoringJob::
    execute()
{
    try {
        mParent->mLogger->trace(
            "[fsmJob=%, reader=%] Starting monitoring job process "
            "[mode=Polling using 'isCardPresent()']\n",
            JOB_ID,
            mParent->mReader->getName());

        /* Re-init loop value to true */
        mParent->mLoop = true;

        /*
         * C++: jobs are queued asynchronously by the ExecutorService, so
         * stop() may already have been called (and cancel() set) before this
         * method starts running, in which case the reset above would
         * otherwise silently revive a job that was already told to stop and
         * loop forever. Checking isCancelled() here and on every iteration
         * closes that race.
         */
        while (mParent->mLoop && !isCancelled()) {
            /* Polls for CARD_INSERTED */
            if (mParent->mMonitorInsertion
                && mParent->mReader->isCardPresent()) {
                mParent->mLogger->trace(
                    "[fsmJob=%, reader=%] Card present\n",
                    JOB_ID,
                    mParent->mReader->getName());
                mMonitoringState->onEvent(InternalEvent::CARD_INSERTED);
                return;
            }

            /* Polls for CARD_REMOVED */
            if (!mParent->mMonitorInsertion
                && !mParent->mReader->isCardPresent()) {
                mParent->mLogger->trace(
                    "[fsmJob=%, reader=%] Card not present\n",
                    JOB_ID,
                    mParent->mReader->getName());
                mParent->mLoop = false;
                mMonitoringState->onEvent(InternalEvent::CARD_REMOVED);
                return;
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
        mParent->mLogger->trace(
            "[fsmJob=%, reader=%] Monitoring job polling process stopped\n",
            JOB_ID,
            mParent->mReader->getName());

    } catch (const RuntimeException& e) {
        dynamic_cast<ObservableLocalReaderAdapter*>(mParent->mReader)
            ->getObservationExceptionHandler()
            ->onReaderObservationError(
                dynamic_cast<ObservableLocalReaderAdapter*>(mParent->mReader)
                    ->getPluginName(),
                mParent->mReader->getName(),
                std::make_shared<RuntimeException>(e));
    }
}

/* CARD INSERTION ACTIVE MONITORING JOB ADAPTER
 * ------------------------------------------------- */

CardInsertionActiveMonitoringJobAdapter::
    CardInsertionActiveMonitoringJobAdapter(
        ObservableLocalReaderAdapter* reader,
        const int64_t sleepDurationMillis,
        const bool monitorInsertion)
: AbstractMonitoringJobAdapter(reader)
, mSleepDurationMillis(sleepDurationMillis)
, mMonitorInsertion(monitorInsertion)
, mReader(reader)
{
}

std::shared_ptr<Job>
CardInsertionActiveMonitoringJobAdapter::getMonitoringJob(
    const std::shared_ptr<AbstractObservableStateAdapter> monitoringState)
{
    return std::make_shared<CardInsertionActiveMonitoringJob>(
        monitoringState, this);
}

void
CardInsertionActiveMonitoringJobAdapter::stop()
{
    mLoop = false;
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
