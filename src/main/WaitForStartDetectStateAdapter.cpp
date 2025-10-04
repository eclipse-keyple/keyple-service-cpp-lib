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

#include "keyple/core/service/WaitForStartDetectStateAdapter.hpp"

#include <memory>

namespace keyple {
namespace core {
namespace service {

WaitForStartDetectStateAdapter::WaitForStartDetectStateAdapter(
    ObservableLocalReaderAdapter* reader,
    std::shared_ptr<AbstractMonitoringJobAdapter> monitoringJob,
    std::shared_ptr<ExecutorService> executorService)
: AbstractObservableStateAdapter(
    MonitoringState::WAIT_FOR_START_DETECTION,
    reader,
    monitoringJob,
    executorService)
{
}

WaitForStartDetectStateAdapter::WaitForStartDetectStateAdapter(
    ObservableLocalReaderAdapter* reader)
: WaitForStartDetectStateAdapter(reader, nullptr, nullptr)
{
}

void
WaitForStartDetectStateAdapter::onEvent(const InternalEvent event)
{
    mLogger->trace(
        "Internal event [%] received for reader [%] in current state [%]\n",
        getReader()->getName(),
        event,
        getMonitoringState());

    /* Process InternalEvent */
    switch (event) {
    case InternalEvent::START_DETECT:
        switchState(MonitoringState::WAIT_FOR_CARD_INSERTION);
        break;

    default:
        mLogger->trace("Event ignored\n");
        break;
    }
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
