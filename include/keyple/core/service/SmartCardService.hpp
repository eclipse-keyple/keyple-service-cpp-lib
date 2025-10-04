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
#include <vector>

#include "keyple/core/common/KeypleCardExtension.hpp"
#include "keyple/core/common/KeyplePluginExtensionFactory.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keypop/reader/ReaderApiFactory.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::common::KeypleCardExtension;
using keyple::core::common::KeyplePluginExtensionFactory;
using keypop::reader::ReaderApiFactory;

/**
 * Keyple main service.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API SmartCardService {
public:
    /**
     *
     */
    virtual ~SmartCardService() = default;

    /**
     * Registers a new plugin to the service.
     *
     * @param pluginFactory The plugin factory.
     * @return A not null reference to the registered {@link Plugin}.
     * @throws KeyplePluginException If instantiation failed.
     * @throws IllegalStateException If the plugin has already been registered.
     * @since 2.0.0
     */
    virtual std::shared_ptr<Plugin>
    registerPlugin(std::shared_ptr<KeyplePluginExtensionFactory> pluginFactory)
        = 0;

    /**
     * Attempts to unregister the plugin having the provided name from the
     * service.
     *
     * @param pluginName The plugin name.
     * @since 2.0.0
     */
    virtual void unregisterPlugin(const std::string& pluginName) = 0;

    /**
     * Returns the names of all registered plugins.
     *
     * @return A not null Set String.
     * @since 2.0.0
     */
    virtual const std::vector<std::string> getPluginNames() const = 0;

    /**
     * Returns all registered plugins.
     *
     * @return A not null Set of Plugin.
     * @since 2.0.0
     */
    virtual const std::vector<std::shared_ptr<Plugin>> getPlugins() const = 0;

    /**
     * Returns the plugin whose name is provided as an argument.
     *
     * @param pluginName The plugin name.
     * @return Null if the plugin is not found or no longer registered.
     * @since 2.0.0
     */
    virtual std::shared_ptr<Plugin>
    getPlugin(const std::string& pluginName) const = 0;

    /**
     * Returns the plugin associated to the provided CardReader.
     *
     * @param reader The card reader.
     * @return Null if the plugin is not found or no longer registered.
     * @since 2.1.0
     */
    virtual std::shared_ptr<Plugin>
    getPlugin(const std::shared_ptr<CardReader> cardReader) const = 0;

    /**
     * Returns the reader associated to the provided unique name.
     *
     * @param readerName The name of the card reader.
     * @return Null if the reader is not found or no longer registered.
     * @since 2.1.0
     */
    virtual std::shared_ptr<CardReader>
    getReader(const std::string& readerName) const = 0;

    /**
     * Returns the first reader whose name matches the provided regular
     * expression.
     *
     * @param readerNameRegex The name of the card reader as a regular
     * expression string.
     * @return Null if the reader is not found or no longer registered.
     * @throws IllegalArgumentException If the provided regex is invalid.
     * @since 3.1.0
     */
    virtual std::shared_ptr<CardReader>
    findReader(const std::string& readerNameRegex) const = 0;

    /**
     * Verifies the compatibility with the service of the provided card
     * extension.
     *
     * <p>The verification is based on the comparison of the respective API
     * versions.
     *
     * @param cardExtension A not null KeypleCardExtension reference object
     * @since 2.0.0
     */
    virtual void checkCardExtension(
        const std::shared_ptr<KeypleCardExtension> cardExtension) const = 0;

    /**
     * Registers a new distributed local service to the service.
     *
     * @param distributedLocalServiceExtensionFactory Factory to use to
     * instantiate a Distributed Local Service extension
     * @return A not null reference to the registered DistributedLocalService.
     * @throw IllegalStateException If the distributed local service has already
     * been registered.
     * @since 2.0.0
     */
    // DistributedLocalService registerDistributedLocalService(
    //     KeypleDistributedLocalServiceExtensionFactory
    //     distributedLocalServiceExtensionFactory);

    /**
     * Attempts to unregister the distributed local service having the provided
     * name from the service.
     *
     * @param distributedLocalServiceName The distributed local service name.
     * @since 2.0.0
     */
    // void unregisterDistributedLocalService(String
    // distributedLocalServiceName);

    /**
     * Returns the distributed local service having the provided name.
     *
     * @param distributedLocalServiceName The name of the distributed local
     * service.
     * @return Null if the distributed local service is not found or no longer
     * registered.
     * @since 2.0.0
     */
    // DistributedLocalService getDistributedLocalService(String
    // distributedLocalServiceName);

    /**
     * Returns the factory to create the Reader API objects.
     *
     * @return A not null reference.
     * @since 3.0.0
     */
    virtual std::unique_ptr<ReaderApiFactory> getReaderApiFactory() = 0;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
