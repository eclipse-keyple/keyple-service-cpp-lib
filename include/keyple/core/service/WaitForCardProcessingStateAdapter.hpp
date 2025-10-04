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

#include <memory>
#include <typeinfo>

#include "keyple/core/service/AbstractObservableStateAdapter.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/util/cpp/LoggerFactory.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::util::cpp::LoggerFactory;

using InternalEvent = ObservableLocalReaderAdapter::InternalEvent;

/**
 * Wait for card processing state implementation.
 *
 * <p>The state during which the card is being processed by the application.
 *
 * <ul>
 *   <li>Upon CARD_PROCESSED event, the machine changes state for
 * WAIT_FOR_CARD_REMOVAL or WAIT_FOR_CARD_DETECTION according to the
 * ObservableCardReader::DetectionMode setting. <li>Upon STOP_DETECT event, the
 * machine changes state for WAIT_FOR_CARD_DETECTION.
 * </ul>
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API WaitForCardProcessingStateAdapter final
: public AbstractObservableStateAdapter {
public:
    /**
     * Creates an instance.
     *
     * @param reader The observable local reader adapter.
     * @param monitoringJob The monitoring job.
     * @param executorService The executor service to use.
     * @since 2.0.0
     */
    WaitForCardProcessingStateAdapter(
        ObservableLocalReaderAdapter* reader,
        std::shared_ptr<AbstractMonitoringJobAdapter> monitoringJob,
        std::shared_ptr<ExecutorService> executorService);

    /**
     * Creates an instance.
     *
     * @param reader The observable local reader adapter.
     * @since 2.0.0
     */
    explicit WaitForCardProcessingStateAdapter(
        ObservableLocalReaderAdapter* reader);

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void onEvent(const InternalEvent event) override;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger
        = LoggerFactory::getLogger(typeid(WaitForCardProcessingStateAdapter));
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
