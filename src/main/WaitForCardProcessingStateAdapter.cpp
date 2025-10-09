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

#include "keyple/core/service/WaitForCardProcessingStateAdapter.hpp"

#include <memory>

namespace keyple {
namespace core {
namespace service {

using DetectionMode = ObservableCardReader::DetectionMode;

WaitForCardProcessingStateAdapter::WaitForCardProcessingStateAdapter(
    ObservableLocalReaderAdapter* reader,
    std::shared_ptr<AbstractMonitoringJobAdapter> monitoringJob,
    std::shared_ptr<ExecutorService> executorService)
: AbstractObservableStateAdapter(
    MonitoringState::WAIT_FOR_CARD_PROCESSING,
    reader,
    monitoringJob,
    executorService)
{
}

WaitForCardProcessingStateAdapter::WaitForCardProcessingStateAdapter(
    ObservableLocalReaderAdapter* reader)
: WaitForCardProcessingStateAdapter(reader, nullptr, nullptr)
{
}

void
WaitForCardProcessingStateAdapter::onEvent(const InternalEvent event)
{
    mLogger->trace(
        "Internal event [%] received for reader [%] in current state [%]\n",
        event,
        getReader()->getName(),
        getMonitoringState());

    /* Process InternalEvent */
    switch (event) {
    case InternalEvent::CARD_PROCESSED:
        if (getReader()->getDetectionMode() == DetectionMode::REPEATING) {
            switchState(MonitoringState::WAIT_FOR_CARD_REMOVAL);
        } else {
            /* We close the channels now and notify the application of the
             * CARD_REMOVED event */
            getReader()->processCardRemoved();
            switchState(MonitoringState::WAIT_FOR_START_DETECTION);
        }
        break;

    case InternalEvent::CARD_REMOVED:
        /*
         * The card has been removed, we close all channels and return to the
         * currentState of waiting for insertion We notify the application of
         * the CARD_REMOVED event.
         */
        getReader()->processCardRemoved();
        if (getReader()->getDetectionMode() == DetectionMode::REPEATING) {
            switchState(MonitoringState::WAIT_FOR_CARD_INSERTION);
        } else {
            switchState(MonitoringState::WAIT_FOR_START_DETECTION);
        }
        break;

    case InternalEvent::STOP_DETECT:
        getReader()->processCardRemoved();
        switchState(MonitoringState::WAIT_FOR_START_DETECTION);
        break;

    default:
        mLogger->trace("Event ignored\n");
        break;
    }
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
