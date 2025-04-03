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

#include "keyple/core/plugin/spi/PluginSpi.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/LocalPluginAdapter.hpp"
#include "keyple/core/service/ObservablePlugin.hpp"
#include "keyple/core/service/ObservationManagerAdapter.hpp"
#include "keyple/core/service/cpp/Job.hpp"
#include "keyple/core/service/spi/PluginObservationExceptionHandlerSpi.hpp"
#include "keyple/core/service/spi/PluginObserverSpi.hpp"
#include "keyple/core/util/cpp/LoggerFactory.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::service::cpp::Job;

/**
 * Abstract class for all observable local plugin adapters.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API AbstractObservableLocalPluginAdapter
: public LocalPluginAdapter,
  public ObservablePlugin {
public:
    /**
     * Constructor.
     *
     * @param pluginSpi The associated plugin SPI.
     * @since 2.0.0
     */
    explicit AbstractObservableLocalPluginAdapter(
        std::shared_ptr<PluginSpi> pluginSpi);

    /**
     *
     */
    virtual ~AbstractObservableLocalPluginAdapter() = default;

    /**
     * Gets the associated observation manager.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    std::shared_ptr<ObservationManagerAdapter<
        PluginObserverSpi,
        PluginObservationExceptionHandlerSpi>>
    getObservationManager() const;

    /**
     * Notifies all registered observers with the provided {@link
     * PluginEventAdapter}.
     *
     * <p>This method never throws an exception. Any errors at runtime are
     * notified to the application using the exception handler.
     *
     * @param event The plugin event.
     * @since 2.0.0
     */
    void notifyObservers(const std::shared_ptr<PluginEvent> event);

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void doUnregister() final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void
    addObserver(const std::shared_ptr<PluginObserverSpi> observer) override;

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

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    int countObservers() const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void setPluginObservationExceptionHandler(
        std::shared_ptr<PluginObservationExceptionHandlerSpi> exceptionHandler)
        final;

private:
    /**
     *
     */
    class ObservableLocalPluginAdapterJob final : public Job {
    public:
        /**
         *
         */
        ObservableLocalPluginAdapterJob(
            std::shared_ptr<PluginObserverSpi> observer,
            const std::shared_ptr<PluginEvent> event,
            AbstractObservableLocalPluginAdapter* parent);

        /**
         *
         */
        void execute() final;

    private:
        /**
         *
         */
        std::shared_ptr<PluginObserverSpi> mObserver;

        /**
         *
         */
        const std::shared_ptr<PluginEvent> mEvent;

        /**
         *
         */
        AbstractObservableLocalPluginAdapter* mParent;
    };

    /**
     *
     */
    const std::shared_ptr<Logger> mLogger = LoggerFactory::getLogger(
        typeid(AbstractObservableLocalPluginAdapter));

    /**
     *
     */
    std::shared_ptr<ObservationManagerAdapter<
        PluginObserverSpi,
        PluginObservationExceptionHandlerSpi>>
        mObservationManager;

    /**
     * Notifies a single observer of an event.
     *
     * @param observer The observer to notify.
     * @param event The event.
     */
    virtual void notifyObserver(
        std::shared_ptr<PluginObserverSpi> observer,
        const std::shared_ptr<PluginEvent> event);
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
