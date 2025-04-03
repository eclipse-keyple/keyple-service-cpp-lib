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
#include <string>
#include <typeinfo>
#include <vector>

#include "keyple/core/plugin/spi/ObservablePluginSpi.hpp"
#include "keyple/core/service/AbstractObservableLocalPluginAdapter.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/util/cpp/Thread.hpp"
#include "keyple/core/util/cpp/exception/Exception.hpp"
#include "keypop/reader/CardReader.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::plugin::spi::ObservablePluginSpi;
using keyple::core::util::cpp::Thread;
using keyple::core::util::cpp::exception::Exception;
using keypop::reader::CardReader;

/**
 * Implementation of a local ObservablePlugin.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API ObservableLocalPluginAdapter final
: public AbstractObservableLocalPluginAdapter {
public:
    /**
     * Constructor.
     *
     * @param observablePluginSpi The associated plugin SPI.
     * @since 2.0.0
     */
    explicit ObservableLocalPluginAdapter(
        std::shared_ptr<ObservablePluginSpi> observablePluginSpi);

    /**
     *
     */
    ~ObservableLocalPluginAdapter();

    /**
     * Indicate whether the thread is running or not.
     */
    bool isMonitoring() const;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void addObserver(std::shared_ptr<PluginObserverSpi> observer) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void
    removeObserver(const std::shared_ptr<PluginObserverSpi> observer) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void clearObservers() override;

private:
    /**
     *
     */
    class UncaughtExceptionHandler : public Thread::UncaughtExceptionHandler {
    public:
        /**
         *
         */
        explicit UncaughtExceptionHandler(ObservableLocalPluginAdapter* parent);

        /**
         *
         */
        void uncaughtException(
            std::shared_ptr<Thread> t, std::shared_ptr<Exception> e);

    private:
        /**
         *
         */
        ObservableLocalPluginAdapter* mParent;
    };

    /**
     * Thread in charge of reporting live events
     */
    class EventThread : public Thread {
    public:
        friend ObservableLocalPluginAdapter;

        /**
         *
         */
        EventThread(
            const std::string& pluginName,
            ObservableLocalPluginAdapter* parent);

        /**
         * (private)<br>
         * Indicate whether the thread is running or not
         */
        bool isMonitoring() const;

        /**
         * Marks the thread as one that should end when the last
         * threadWaitTimeout occurs
         */
        void end();

    private:
        /**
         *
         */
        const std::string mPluginName;

        /**
         *
         */
        const int64_t mMonitoringCycleDuration;

        /**
         *
         */
        bool mRunning;

        /**
         *
         */
        bool mStarted;

        /**
         *
         */
        bool mTerminated;

        /**
         *
         */
        ObservableLocalPluginAdapter* mParent;

        /**
         * Adds a reader to the list of known readers (by the plugin)
         *
         * @param readerName The name of the reader.
         * @throw PluginIOException if an error occurs while searching the
         * reader.
         */
        void addReader(const std::string& readerName);

        /**
         * Removes a reader from the list of known readers (by the plugin)
         */
        void removeReader(const std::shared_ptr<CardReader> reader);

        /**
         * Notifies observers of changes in the list of readers
         */
        void notifyChanges(
            const PluginEvent::Type type,
            const std::vector<std::string>& changedReaderNames);

        /**
         * Compares the list of current readers to the list provided by the
         * system and adds or removes readers accordingly.<br> Observers are
         * notified of changes.
         *
         * @param actualNativeReaderNames the list of readers currently known by
         * the system
         * @throw PluginIOException if an error occurs while searching readers.
         */
        void
        processChanges(const std::vector<std::string>& actualNativeReaderNames);

        /**
         * Reader monitoring loop<br>
         * Checks reader insertions and removals<br>
         * Notifies observers of any changes
         *
         * C++: this replaces run() override
         */
        void execute() override;
    };

    /**
     *
     */
    const std::unique_ptr<Logger> mLogger
        = LoggerFactory::getLogger(typeid(ObservableLocalPluginAdapter));

    /**
     *
     */
    std::shared_ptr<ObservablePluginSpi> mObservablePluginSpi;

    /**
     * Local thread to monitoring readers presence
     */
    std::shared_ptr<EventThread> mThread;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
