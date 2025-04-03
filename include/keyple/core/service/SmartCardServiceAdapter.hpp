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

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <typeinfo>
#include <vector>

#include "keyple/core/plugin/spi/PluginFactorySpi.hpp"
#include "keyple/core/plugin/spi/PoolPluginFactorySpi.hpp"
#include "keyple/core/service/AbstractPluginAdapter.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/SmartCardService.hpp"
#include "keyple/core/util/cpp/LoggerFactory.hpp"

#include "keyple/core/plugin/PluginIOException.hpp"
#include "keyple/core/plugin/spi/AutonomousObservablePluginSpi.hpp"
#include "keyple/core/plugin/spi/ObservablePluginSpi.hpp"
#include "keyple/core/plugin/spi/PluginSpi.hpp"
#include "keyple/core/plugin/spi/PoolPluginSpi.hpp"
#include "keyple/core/service/AutonomousObservableLocalPluginAdapter.hpp"
#include "keyple/core/service/KeyplePluginException.hpp"
#include "keyple/core/service/LocalPoolPluginAdapter.hpp"
#include "keyple/core/service/ObservableLocalPluginAdapter.hpp"
#include "keyple/core/service/ReaderApiFactoryAdapter.hpp"
#include "keyple/core/util/KeypleAssert.hpp"
#include "keyple/core/util/cpp/StringUtils.hpp"
#include "keyple/core/util/cpp/exception/IllegalArgumentException.hpp"
#include "keyple/core/util/cpp/exception/IllegalStateException.hpp"
#include "keyple/core/util/cpp/exception/PatternSyntaxException.hpp"
#include "keypop/card/CardApiProperties.hpp"
#include "keypop/reader/ReaderApiProperties.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::plugin::spi::PluginFactorySpi;
using keyple::core::plugin::spi::PoolPluginFactorySpi;
using keyple::core::util::cpp::Logger;
using keyple::core::util::cpp::LoggerFactory;

using keyple::core::plugin::PluginIOException;
using keyple::core::plugin::spi::AutonomousObservablePluginSpi;
using keyple::core::plugin::spi::ObservablePluginSpi;
using keyple::core::plugin::spi::PluginSpi;
using keyple::core::plugin::spi::PoolPluginSpi;
using keyple::core::service::AutonomousObservableLocalPluginAdapter;
using keyple::core::service::KeyplePluginException;
using keyple::core::service::LocalPoolPluginAdapter;
using keyple::core::service::ReaderApiFactoryAdapter;
using keyple::core::util::Assert;
using keyple::core::util::cpp::StringUtils;
using keyple::core::util::cpp::exception::IllegalArgumentException;
using keyple::core::util::cpp::exception::IllegalStateException;
using keyple::core::util::cpp::exception::PatternSyntaxException;
using keypop::card::CardApiProperties_VERSION;
using keypop::reader::ReaderApiProperties_VERSION;

/**
 * Implementation of {@link SmartCardService}.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API SmartCardServiceAdapter final
: public SmartCardService {
public:
    /**
     * Gets the single instance of SmartCardServiceAdapter.
     *
     * @return single instance of SmartCardServiceAdapter
     * @since 2.0.0
     */
    static std::shared_ptr<SmartCardServiceAdapter> getInstance();

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    std::shared_ptr<Plugin> registerPlugin(
        std::shared_ptr<KeyplePluginExtensionFactory> pluginFactory) final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void unregisterPlugin(const std::string& pluginName) final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::vector<std::string> getPluginNames() const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<Plugin>> getPlugins() const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    std::shared_ptr<Plugin>
    getPlugin(const std::string& pluginName) const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.1.0
     */
    std::shared_ptr<Plugin>
    getPlugin(const std::shared_ptr<CardReader> cardReader) const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.1.0
     */
    std::shared_ptr<CardReader>
    getReader(const std::string& readerName) const override;

    /**
     * {@inheritDoc}
     *
     * @since 3.1.0
     */
    std::shared_ptr<CardReader>
    findReader(const std::string& readerNameRegex) const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void checkCardExtension(
        const std::shared_ptr<KeypleCardExtension> cardExtension) const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    // @Override
    // public DistributedLocalService registerDistributedLocalService(
    //     KeypleDistributedLocalServiceExtensionFactory
    //     distributedLocalServiceExtensionFactory) {

    //     Assert.getInstance()
    //         .notNull(
    //             distributedLocalServiceExtensionFactory,
    //             "distributedLocalServiceExtensionFactory");

    //     DistributedLocalServiceAdapter distributedLocalService;
    //     try {
    //     if (!(distributedLocalServiceExtensionFactory instanceof
    //     LocalServiceFactorySpi)) {
    //         throw new IllegalArgumentException("The factory doesn't implement
    //         the right SPI.");
    //     }

    //     LocalServiceFactorySpi factory =
    //         (LocalServiceFactorySpi) distributedLocalServiceExtensionFactory;

    //     synchronized (distributedLocalServiceMonitor) {
    //         String localServiceName = factory.getLocalServiceName();

    //         checkDistributedLocalServiceRegistration(localServiceName);
    //         checkDistributedLocalServiceVersion(factory);

    //         LocalServiceSpi localServiceSpi = factory.getLocalService();

    //         if (!localServiceSpi.getName().equals(localServiceName)) {
    //         throw new IllegalArgumentException(
    //             String.format(
    //                 "The local service name '%s' mismatches the expected name
    //                 '%s' provided by the factory", localServiceSpi.getName(),
    //                 localServiceName));
    //         }

    //         distributedLocalService = new
    //         DistributedLocalServiceAdapter(localServiceSpi);
    //         distributedLocalService.register();

    //         distributedLocalServices.put(localServiceName,
    //         distributedLocalService);
    //     }
    //     } catch (IllegalArgumentException e) {
    //     throw new IllegalArgumentException(
    //         "The provided distributed local service factory doesn't implement
    //         the distributed local service API properly.", e);
    //     }
    //     return distributedLocalService;
    // }

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    // @Override
    // public void unregisterDistributedLocalService(String
    // distributedLocalServiceName) {
    //     logger.info(
    //         "Unregistering a distributed local service from the service :
    //         {}", distributedLocalServiceName);
    //     synchronized (distributedLocalServiceMonitor) {
    //     DistributedLocalService localService =
    //         distributedLocalServices.remove(distributedLocalServiceName);
    //     if (localService != null) {
    //         ((DistributedLocalServiceAdapter) localService).unregister();
    //     } else {
    //         logger.warn(
    //             "The distributed local service '{}' is not registered",
    //             distributedLocalServiceName);
    //     }
    //     }
    // }

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    // @Override
    // public boolean isDistributedLocalServiceRegistered(String
    // distributedLocalServiceName) {
    //     synchronized (distributedLocalServiceMonitor) {
    //     return
    //     distributedLocalServices.containsKey(distributedLocalServiceName);
    //     }
    // }

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    // @Override
    // public DistributedLocalService getDistributedLocalService(String
    // distributedLocalServiceName)
    // {
    //     synchronized (distributedLocalServiceMonitor) {
    //     return distributedLocalServices.get(distributedLocalServiceName);
    //     }
    // }

    /**
     * {@inheritDoc}
     *
     * @since 3.0.0
     */
    std::unique_ptr<ReaderApiFactory> getReaderApiFactory() final;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger
        = LoggerFactory::getLogger(typeid(SmartCardServiceAdapter));

    /**
     *
     */
    std::mutex mMutex;

    /**
     *
     */
    std::map<std::string, std::shared_ptr<Plugin>> mPlugins;

    /**
     *
     */
    static std::shared_ptr<SmartCardServiceAdapter> mInstance;

    /**
     *
     */
    static const std::string MSG_VERSION_MISMATCH_DETECTED;

    /**
     *
     */
    // private final Object pluginMonitor = new Object();

    /**
     *
     */
    // std::map<std::string, std::shared_ptr<DistributedLocalService>>
    // distributedLocalServices;

    /**
     *
     */
    // private final Object distributedLocalServiceMonitor = new Object();

    // static {
    //     // Register additional JSON adapters.
    //     JsonUtil.registerTypeAdapter(
    //         AbstractApduException.class, new
    //         ApduExceptionJsonSerializerAdapter(), true);
    // }

    /**
     * Private constructor
     */
    SmartCardServiceAdapter() = default;

    /**
     * Compare versions.
     *
     * @param providedVersion The provided version string.
     * @param localVersion The local version string.
     * @return 0 if providedVersion equals localVersion, &lt; 0 if
     * providedVersion &lt; localVersion, &gt; 0 if providedVersion &gt;
     * localVersion.
     */
    int compareVersions(
        const std::string& providedVersion,
        const std::string& localVersion) const;

    /**
     * Checks for consistency in the versions of external APIs shared by the
     * plugin and the service.
     *
     * <p>Generates warnings into the log.
     *
     * @param pluginFactorySpi The plugin factory SPI.
     */
    void checkPluginVersion(
        const std::shared_ptr<PluginFactorySpi> pluginFactorySpi);

    /**
     * Checks for consistency in the versions of external APIs shared by the
     * remote plugin and the service.
     *
     * <p>Generates warnings into the log.
     *
     * @param remotePluginFactorySpi The remote plugin factory SPI.
     */
    // private void checkRemotePluginVersion(RemotePluginFactorySpi
    // remotePluginFactorySpi) {
    //     if (compareVersions(remotePluginFactorySpi.getCommonsApiVersion(),
    //     CommonsApiProperties.VERSION)
    //         != 0) {
    //     logger.warn(
    //         "The version of Commons API used by the provided plugin ({}:{})
    //         mismatches the version used by the service ({}).",
    //         remotePluginFactorySpi.getRemotePluginName(),
    //         remotePluginFactorySpi.getCommonsApiVersion(),
    //         CommonsApiProperties.VERSION);
    //     }
    //     if (compareVersions(
    //             remotePluginFactorySpi.getDistributedRemoteApiVersion(),
    //             DistributedRemoteApiProperties.VERSION)
    //         != 0) {
    //     logger.warn(
    //         "The version of Distributed Remote Plugin API used by the
    //         provided plugin ({}:{}) mismatches the version used by the
    //         service ({}).", remotePluginFactorySpi.getRemotePluginName(),
    //         remotePluginFactorySpi.getDistributedRemoteApiVersion(),
    //         DistributedRemoteApiProperties.VERSION);
    //     }
    // }

    /**
     * Checks for consistency in the versions of external APIs shared by the
     * pool plugin and the service.
     *
     * <p>Generates warnings into the log.
     *
     * @param poolPluginFactorySpi The plugin factory SPI.
     */
    void checkPoolPluginVersion(
        const std::shared_ptr<PoolPluginFactorySpi> poolPluginFactorySpi);

    /**
     * Checks for consistency in the versions of external APIs shared by the
     * card extension and the service.
     *
     * <p>Generates warnings into the log.
     *
     * @param cardExtension The card extension.
     */
    void checkCardExtensionVersion(
        const std::shared_ptr<KeypleCardExtension> cardExtension) const;

    /**
     * Checks for consistency in the versions of external APIs shared by the
     * distributed local service and the service.
     *
     * <p>Generates warnings into the log.
     *
     * @param localServiceFactorySpi The distributed local service factory SPI.
     */
    // private void checkDistributedLocalServiceVersion(LocalServiceFactorySpi
    // localServiceFactorySpi) {
    //     if (compareVersions(localServiceFactorySpi.getCommonsApiVersion(),
    //     CommonsApiProperties.VERSION)
    //         != 0) {
    //     logger.warn(
    //         "The version of Commons API used by the provided distributed
    //         local service ({}:{}) mismatches the version used by the service
    //         ({}).", localServiceFactorySpi.getLocalServiceName(),
    //         localServiceFactorySpi.getCommonsApiVersion(),
    //         CommonsApiProperties.VERSION);
    //     }
    //     if (compareVersions(
    //             localServiceFactorySpi.getDistributedLocalApiVersion(),
    //             DistributedLocalApiProperties.VERSION)
    //         != 0) {
    //     logger.warn(
    //         "The version of Distributed Local API used by the provided
    //         distributed local service
    //         ({}:{}) mismatches the version used by the service ({}).",
    //         localServiceFactorySpi.getLocalServiceName(),
    //         localServiceFactorySpi.getDistributedLocalApiVersion(),
    //         DistributedLocalApiProperties.VERSION);
    //     }
    // }

    /**
     * Checks if the plugin is already registered.
     *
     * @param pluginName The plugin name.
     * @throw IllegalStateException if the plugin is already registered.
     */
    void checkPluginRegistration(const std::string& pluginName);

    /**
     * Checks if the distributed local service is already registered.
     *
     * @param distributedLocalServiceName The distributed local service name.
     * @throw IllegalStateException if the distributed local service is already
     * registered.
     */
    // private void checkDistributedLocalServiceRegistration(String
    // distributedLocalServiceName) {
    //     logger.info(
    //         "Registering a new distributed local service to the service :
    //         {}", distributedLocalServiceName);
    //     Assert.getInstance().notEmpty(distributedLocalServiceName,
    //     "distributedLocalServiceName"); if
    //     (isDistributedLocalServiceRegistered(distributedLocalServiceName)) {
    //     throw new IllegalStateException(
    //         String.format(
    //             "The distributed local service '%s' has already been
    //             registered to the service.", distributedLocalServiceName));
    //     }
    // }

    /**
     * Creates an instance of local plugin.
     *
     * @param pluginFactorySpi The plugin factory SPI.
     * @return A not null reference.
     */
    std::shared_ptr<AbstractPluginAdapter>
    createLocalPlugin(std::shared_ptr<PluginFactorySpi> pluginFactorySpi);

    /**
     * Creates an instance of local pool plugin.
     *
     * @param poolPluginFactorySpi The pool plugin factory SPI.
     * @return A not null reference.
     */
    std::shared_ptr<AbstractPluginAdapter> createLocalPoolPlugin(
        std::shared_ptr<PoolPluginFactorySpi> poolPluginFactorySpi);

    /**
     * Creates an instance of remote plugin.
     *
     * @param remotePluginFactorySpi The plugin factory SPI.
     * @return A not null reference.
     */
    // private AbstractPluginAdapter createRemotePlugin(RemotePluginFactorySpi
    // remotePluginFactorySpi) {

    //     checkPluginRegistration(remotePluginFactorySpi.getRemotePluginName());
    //     checkRemotePluginVersion(remotePluginFactorySpi);

    //     AbstractRemotePluginSpi remotePluginSpi =
    //     remotePluginFactorySpi.getRemotePlugin();

    //     if
    //     (!remotePluginSpi.getName().equals(remotePluginFactorySpi.getRemotePluginName()))
    //     { throw new IllegalArgumentException(
    //         String.format(
    //             "The remote plugin name '%s' mismatches the expected name
    //             '%s' provided by the factory", remotePluginSpi.getName(),
    //             remotePluginFactorySpi.getRemotePluginName()));
    //     }

    //     AbstractPluginAdapter plugin;
    //     if (remotePluginSpi instanceof RemotePoolPluginSpi) {
    //     plugin = new RemotePoolPluginAdapter((RemotePoolPluginSpi)
    //     remotePluginSpi); } else if (remotePluginSpi instanceof
    //     ObservableRemotePluginSpi) { plugin = new
    //     ObservableRemotePluginAdapter((ObservableRemotePluginSpi)
    //     remotePluginSpi); } else { plugin = new
    //     RemotePluginAdapter((RemotePluginSpi) remotePluginSpi);
    //     }
    //     return plugin;
    // }
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
