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

#include "keyple/core/service/AbstractObservableLocalPluginAdapter.hpp"

#include <memory>
#include <string>
#include <vector>

#include "keyple/core/service/PluginEventAdapter.hpp"
#include "keyple/core/util/cpp/exception/Exception.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::util::cpp::exception::Exception;

/* ABSTRACT OBSERVABLE LOCAL PLUGIN ADAPTER JOB
 * ------------------------------------------------- */

AbstractObservableLocalPluginAdapter::ObservableLocalPluginAdapterJob ::
    ObservableLocalPluginAdapterJob(
        std::shared_ptr<PluginObserverSpi> observer,
        const std::shared_ptr<PluginEvent> event,
        AbstractObservableLocalPluginAdapter* parent)
: Job("AbstractObservableLocalPluginAdapter")
, mObserver(observer)
, mEvent(event)
, mParent(parent)
{
}

void
AbstractObservableLocalPluginAdapter::ObservableLocalPluginAdapterJob::execute()
{
    mParent->notifyObserver(mObserver, mEvent);
}

/* ABSTRACT OBSERVABLE LOCAL PLUGIN ADAPTER
 * ----------------------------------------------------- */

AbstractObservableLocalPluginAdapter::AbstractObservableLocalPluginAdapter(
    std::shared_ptr<PluginSpi> pluginSpi)
: LocalPluginAdapter(pluginSpi)
, mObservationManager(std::make_shared<ObservationManagerAdapter<
                          PluginObserverSpi,
                          PluginObservationExceptionHandlerSpi>>("", ""))
{
}

std::shared_ptr<ObservationManagerAdapter<
    PluginObserverSpi,
    PluginObservationExceptionHandlerSpi>>
AbstractObservableLocalPluginAdapter::getObservationManager() const
{
    return mObservationManager;
}

void
AbstractObservableLocalPluginAdapter::notifyObservers(
    const std::shared_ptr<PluginEvent> event)
{
    mLogger->debug(
        "Plugin [%] notifies event [%] to % observer(s)\n",
        getName(),
        event->getType(),
        countObservers());

    for (const auto& observer : mObservationManager->getObservers()) {
        notifyObserver(observer, event);
    }
}

void
AbstractObservableLocalPluginAdapter::notifyObserver(
    std::shared_ptr<PluginObserverSpi> observer,
    const std::shared_ptr<PluginEvent> event)
{
    try {
        observer->onPluginEvent(event);
    } catch (const Exception& e) {
        try {
            mObservationManager->getObservationExceptionHandler()
                ->onPluginObservationError(
                    getName(), std::make_shared<Exception>(e));
        } catch (const Exception& e2) {
            mLogger->error(
                "Event notification error: % %\n", e2.getMessage(), e2);
            mLogger->error("Original cause: % %\n", e.getMessage(), e);
        }
    }
}

void
AbstractObservableLocalPluginAdapter::doUnregister()
{
    const std::vector<std::string>& unregisteredReaderNames = getReaderNames();

    notifyObservers(std::make_shared<PluginEventAdapter>(
        getName(), unregisteredReaderNames, PluginEvent::Type::UNAVAILABLE));

    clearObservers();
    LocalPluginAdapter::doUnregister();
}

void
AbstractObservableLocalPluginAdapter::addObserver(
    const std::shared_ptr<PluginObserverSpi> observer)
{
    checkStatus();
    mObservationManager->addObserver(observer);
}

void
AbstractObservableLocalPluginAdapter::removeObserver(
    const std::shared_ptr<PluginObserverSpi> observer)
{
    Assert::getInstance().notNull(observer, "observer");

    const auto& observers = mObservationManager->getObservers();
    const auto it = std::find(observers.begin(), observers.end(), observer);

    if (it != observers.end()) {
        mObservationManager->removeObserver(observer);
    }
}

void
AbstractObservableLocalPluginAdapter::clearObservers()
{
    mObservationManager->clearObservers();
}

int
AbstractObservableLocalPluginAdapter::countObservers() const
{
    return mObservationManager->countObservers();
}

void
AbstractObservableLocalPluginAdapter::setPluginObservationExceptionHandler(
    std::shared_ptr<PluginObservationExceptionHandlerSpi> exceptionHandler)
{
    checkStatus();
    mObservationManager->setObservationExceptionHandler(exceptionHandler);
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
