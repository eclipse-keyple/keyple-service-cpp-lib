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

#include <atomic>
#include <memory>
#include <typeinfo>

#include "keyple/core/plugin/spi/reader/observable/state/removal/WaitForCardRemovalBlockingSpi.hpp"
#include "keyple/core/service/AbstractMonitoringJobAdapter.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/cpp/Job.hpp"
#include "keyple/core/util/cpp/LoggerFactory.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::plugin::spi::reader::observable::state::removal::
    WaitForCardRemovalBlockingSpi;
using keyple::core::util::cpp::LoggerFactory;

/**
 * Ping the card to detect removal thanks to the method {@link
 * ObservableLocalReaderAdapter#isCardPresentPing()}.
 *
 * <p>This method is invoked in another thread.
 *
 * <p>This job should be used by readers who do not have the ability to natively
 * detect the disappearance of the card at the end of the transaction.
 *
 * <p>It is based on sending a neutral APDU command as long as the card is
 * responding, an internal CARD_REMOVED event is fired when the card is no
 * longer responding.
 *
 * <p>By default a delay of 100 ms is inserted between each APDU sending .
 *
 * <p>All runtime exceptions that may occur during the monitoring process are
 * caught and notified at the application level through the appropriate
 * exception handler.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API CardRemovalActiveMonitoringJobAdapter final
: public AbstractMonitoringJobAdapter {
public:
    /**
     * Create a job monitor job that ping the card with the method
     * isCardPresentPing()
     *
     * /!\ C++: cannot use a shared_ptr for reader as this is called from constructors
     *
     * @param reader reference to the reader
     * @param sleepDurationMillis delay between between each APDU sending
     * @since 2.0.0
     */
    CardRemovalActiveMonitoringJobAdapter(
        ObservableLocalReaderAdapter* reader,
        const int64_t sleepDurationMillis);

    /**
     * Gets the monitoring process.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    std::shared_ptr<Job> getMonitoringJob(
        std::shared_ptr<AbstractObservableStateAdapter> monitoringState)
        override;

    /**
     * Terminates the monitoring process.
     *
     * @since 2.0.0
     */
    void stop() override;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger = LoggerFactory::getLogger(
        typeid(CardRemovalActiveMonitoringJobAdapter));

    /**
     *
     */
    std::shared_ptr<WaitForCardRemovalBlockingSpi> mReaderSpi;

    /**
     *
     */
    const int64_t mSleepDurationMillis;

    /**
     *
     */
    std::atomic<bool> mLoop;

    /**
     *
     */
    class CardRemovalActiveMonitoringJob final : public Job {
    public:
        /**
         *
         */
        CardRemovalActiveMonitoringJob(
            const std::shared_ptr<AbstractObservableStateAdapter>
                monitoringState,
            CardRemovalActiveMonitoringJobAdapter* parent);

        /**
         *
         */
        void execute() final;

    private:
        /**
         *
         */
        const std::shared_ptr<AbstractObservableStateAdapter> mMonitoringState;

        /**
         *
         */
        CardRemovalActiveMonitoringJobAdapter* mParent;
    };
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
