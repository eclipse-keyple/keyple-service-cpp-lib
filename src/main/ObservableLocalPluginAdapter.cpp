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

#include "keyple/core/service/ObservableLocalPluginAdapter.hpp"

#include <memory>
#include <string>
#include <vector>

#include "keyple/core/plugin/PluginIOException.hpp"
#include "keyple/core/service/KeyplePluginException.hpp"
#include "keyple/core/service/PluginEventAdapter.hpp"
#include "keyple/core/util/cpp/Arrays.hpp"
#include "keyple/core/util/cpp/exception/InterruptedException.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::plugin::PluginIOException;
using keyple::core::util::cpp::Arrays;
using keyple::core::util::cpp::exception::InterruptedException;

/* OBSERVABLE LOCAL PLUGIN ADAPTER JOB
 * ---------------------------------------------------------- */

/* OBSERVABLE LOCAL PLUGIN ADAPTER
 * -------------------------------------------------------------- */

ObservableLocalPluginAdapter::ObservableLocalPluginAdapter(
    std::shared_ptr<ObservablePluginSpi> observablePluginSpi)
: AbstractObservableLocalPluginAdapter(observablePluginSpi)
, mObservablePluginSpi(observablePluginSpi)
{
}

ObservableLocalPluginAdapter::~ObservableLocalPluginAdapter()
{
    if (mThread) {
        mThread->end();
        while (mThread->mRunning || !mThread->mTerminated) {
        }
    }
}

bool
ObservableLocalPluginAdapter::isMonitoring() const
{
    return mThread != nullptr && mThread->isAlive() && mThread->isMonitoring();
}

void
ObservableLocalPluginAdapter::addObserver(
    std::shared_ptr<PluginObserverSpi> observer)
{
    AbstractObservableLocalPluginAdapter::addObserver(observer);

    if (countObservers() == 1) {
        mLogger->info("Start monitoring the plugin [%]\n", getName());
        mThread = std::make_shared<EventThread>(getName(), this);
        mThread->setName("PluginEventMonitoringThread");
        mThread->setUncaughtExceptionHandler(
            std::make_shared<UncaughtExceptionHandler>(this));
        mThread->start();
        while (!mThread->mStarted) {
        }
    }
}

void
ObservableLocalPluginAdapter::removeObserver(
    const std::shared_ptr<PluginObserverSpi> observer)
{
    Assert::getInstance().notNull(observer, "observer");

    if (Arrays::contains(getObservationManager()->getObservers(), observer)) {
        AbstractObservableLocalPluginAdapter::removeObserver(observer);

        if (countObservers() == 0) {
            if (mThread != nullptr) {
                mThread->end();
                mLogger->info("Plugin monitoring stopped\n");
            }
        }
    }
}

void
ObservableLocalPluginAdapter::clearObservers()
{
    AbstractObservableLocalPluginAdapter::clearObservers();

    if (mThread != nullptr) {
        mThread->end();
        mLogger->info("Plugin monitoring stopped\n");
    }
}

/* UNCAUGHT EXCEPTION HANDLER
 * ------------------------------------------------------------------- */

ObservableLocalPluginAdapter::UncaughtExceptionHandler::
    UncaughtExceptionHandler(ObservableLocalPluginAdapter* parent)
: mParent(parent)
{
}

void
ObservableLocalPluginAdapter::UncaughtExceptionHandler::uncaughtException(
    std::shared_ptr<Thread> t, std::shared_ptr<Exception> e)
{
    (void)t;

    mParent->getObservationManager()
        ->getObservationExceptionHandler()
        ->onPluginObservationError(mParent->mThread->mPluginName, e);
}

/* EVENT THREAD
 * ---------------------------------------------------------------------------------
 */

ObservableLocalPluginAdapter::EventThread::EventThread(
    const std::string& pluginName, ObservableLocalPluginAdapter* parent)
: Thread("ObservableLocalPluginAdapter-" + pluginName)
, mPluginName(pluginName)
, mMonitoringCycleDuration(
      parent->mObservablePluginSpi->getMonitoringCycleDuration())
, mRunning(true)
, mStarted(false)
, mTerminated(false)
, mParent(parent)
{
}

void
ObservableLocalPluginAdapter::EventThread::end()
{
    mRunning = false;
    interrupt();
}

bool
ObservableLocalPluginAdapter::EventThread::isMonitoring() const
{
    return mRunning;
}

void
ObservableLocalPluginAdapter::EventThread::addReader(
    const std::string& readerName)
{
    std::shared_ptr<ReaderSpi> readerSpi
        = mParent->mObservablePluginSpi->searchReader(readerName);
    std::shared_ptr<LocalReaderAdapter> reader
        = mParent->buildLocalReaderAdapter(readerSpi);

    reader->doRegister();
    mParent->getReadersMap().insert({reader->getName(), reader});

    mParent->mLogger->info(
        "Plugin [%] adds plugged reader [%] to readers list\n",
        mPluginName,
        readerName);
}

void
ObservableLocalPluginAdapter::EventThread::removeReader(
    const std::shared_ptr<CardReader> reader)
{
    std::dynamic_pointer_cast<LocalReaderAdapter>(reader)->doUnregister();
    mParent->getReadersMap().erase(reader->getName());

    mParent->mLogger->info(
        "Plugin [%] removes unplugged reader [%] from readers list\n",
        mPluginName,
        reader->getName());
}

void
ObservableLocalPluginAdapter::EventThread::notifyChanges(
    const PluginEvent::Type type,
    const std::vector<std::string>& changedReaderNames)
{
    /* Grouped notification */
    mParent->mLogger->trace(
        "Notify reader %(s): %\n",
        type == PluginEvent::Type::READER_CONNECTED ? "connection"
                                                    : "disconnection",
        changedReaderNames);

    mParent->notifyObservers(std::make_shared<PluginEventAdapter>(
        mPluginName, changedReaderNames, type));
}

void
ObservableLocalPluginAdapter::EventThread::processChanges(
    const std::vector<std::string>& actualNativeReaderNames)
{
    std::vector<std::string> changedReaderNames;

    /* Parse the current readers list, notify for disappeared readers, update
     * readers list */
    const std::vector<std::shared_ptr<CardReader>> readers
        = mParent->getReaders();
    for (const auto& reader : readers) {
        if (!std::count(
                actualNativeReaderNames.begin(),
                actualNativeReaderNames.end(),
                reader->getName())) {
            changedReaderNames.push_back(reader->getName());
        }
    }

    /* Notify disconnections if any and update the reader list */
    if (!changedReaderNames.empty()) {
        /* List update */
        for (const auto& reader : readers) {
            if (!std::count(
                    actualNativeReaderNames.begin(),
                    actualNativeReaderNames.end(),
                    reader->getName())) {
                removeReader(reader);
            }
        }

        notifyChanges(
            PluginEvent::Type::READER_DISCONNECTED, changedReaderNames);

        /* Clean the list for a possible connection notification */
        changedReaderNames.clear();
    }

    /* Parse the new readers list, notify for readers appearance, update readers
     * list */
    for (const auto& readerName : actualNativeReaderNames) {
        const std::vector<std::string>& readerNames = mParent->getReaderNames();
        if (!std::count(readerNames.begin(), readerNames.end(), readerName)) {
            addReader(readerName);

            /* Add to the notification list */
            changedReaderNames.push_back(readerName);
        }
    }

    /* Notify connections if any */
    if (!changedReaderNames.empty()) {
        notifyChanges(PluginEvent::Type::READER_CONNECTED, changedReaderNames);
    }
}

void
ObservableLocalPluginAdapter::EventThread::execute()
{
    mStarted = true;

    try {
        while (mRunning) {
            /* Retrieves the current readers names list */
            const std::vector<std::string> actualNativeReaderNames
                = mParent->mObservablePluginSpi->searchAvailableReaderNames();

            /* Checks if it has changed this algorithm favors cases where
             * nothing change */
            const std::vector<std::string> currentlyRegisteredReaderNames
                = mParent->getReaderNames();
            if (!Arrays::containsAll(
                    currentlyRegisteredReaderNames, actualNativeReaderNames)
                || !Arrays::containsAll(
                    actualNativeReaderNames, currentlyRegisteredReaderNames)) {
                processChanges(actualNativeReaderNames);
            }

            /* Sleep for a while */
            Thread::sleep(mMonitoringCycleDuration);
        }
    } catch (const InterruptedException& e) {
        (void)e;
        mParent->mLogger->info(
            "Plugin monitoring stopped, possibly because there is no more "
            "registered observer");

        /* Restore interrupted state... */
        interrupt();
    } catch (const PluginIOException& e) {
        const auto pioe = std::make_shared<PluginIOException>(e);
        const auto kpe = std::make_shared<KeyplePluginException>(
            "An error occurred while monitoring the readers", pioe);
        mParent->getObservationManager()
            ->getObservationExceptionHandler()
            ->onPluginObservationError(mPluginName, kpe);
    }

    mTerminated = true;
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
