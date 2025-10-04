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

#include "keyple/core/service/WaitForCardInsertionStateAdapter.hpp"

#include <memory>

namespace keyple {
namespace core {
namespace service {

WaitForCardInsertionStateAdapter::WaitForCardInsertionStateAdapter(
    ObservableLocalReaderAdapter* reader,
    std::shared_ptr<AbstractMonitoringJobAdapter> monitoringJob,
    std::shared_ptr<ExecutorService> executorService)
: AbstractObservableStateAdapter(
      MonitoringState::WAIT_FOR_CARD_INSERTION,
      reader,
      monitoringJob,
      executorService)
{
}

WaitForCardInsertionStateAdapter::WaitForCardInsertionStateAdapter(
    ObservableLocalReaderAdapter* reader)
: WaitForCardInsertionStateAdapter(reader, nullptr, nullptr)
{
}

void
WaitForCardInsertionStateAdapter::onEvent(const InternalEvent event)
{
    mLogger->trace(
        "Internal event [%] received for reader [%] in current state [%]\n",
        getReader()->getName(),
        event,
        getMonitoringState());

    /* Process InternalEvent */
    switch (event) {
    case InternalEvent::CARD_INSERTED: {
        /* Process default selection if any, return an event, can be null */
        const std::shared_ptr<CardReaderEvent> cardEvent
            = getReader()->processCardInserted();
        if (cardEvent != nullptr) {
            /* Switch internal state */
            switchState(MonitoringState::WAIT_FOR_CARD_PROCESSING);
            /* Notify the external observer of the event */
            getReader()->notifyObservers(cardEvent);
        } else {
            /*
             * If none event was sent to the application, back to card detection
             * stay in the same state, however switch to WAIT_FOR_CARD_INSERTION
             * to relaunch the monitoring job
             */
            mLogger->trace("Inserted card hasn't matched\n");

            switchState(MonitoringState::WAIT_FOR_CARD_REMOVAL);
        }
        break;
    }
    case InternalEvent::STOP_DETECT:
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
