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
#include <string>
#include <typeinfo>
#include <vector>

#include "keyple/core/common/KeyplePluginExtension.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/LocalReaderAdapter.hpp"
#include "keyple/core/service/Plugin.hpp"
#include "keyple/core/util/cpp/LoggerFactory.hpp"
#include "keypop/reader/CardReader.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::util::cpp::Logger;
using keyple::core::util::cpp::LoggerFactory;
using keypop::reader::CardReader;

/**
 * Abstract class for all plugins.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API AbstractPluginAdapter : virtual public Plugin {
public:
    /**
     * Constructor.
     *
     * @param pluginName The name of the plugin.
     * @param pluginExtension The associated plugin extension SPI.
     * @since 2.0.0
     */
    AbstractPluginAdapter(
        const std::string& pluginName,
        std::shared_ptr<KeyplePluginExtension> pluginExtension);

    /**
     *
     */
    virtual ~AbstractPluginAdapter() = default;

    /**
     * Builds a local reader adapter using the provided SPI.
     *
     * @param readerSpi The associated reader SPI.
     * @return A new instance.
     */
    std::shared_ptr<LocalReaderAdapter>
    buildLocalReaderAdapter(std::shared_ptr<ReaderSpi> readerSpi);

    /**
     * Check if the plugin is registered.
     *
     * @throw IllegalStateException is thrown when plugin is not or no longer
     * registered.
     * @since 2.0.0
     */
    void checkStatus() const;

    /**
     * Changes the plugin status to registered.
     *
     * @throw PluginIOException If registration failed.
     * @since 2.0.0
     */
    virtual void doRegister();

    /**
     * Unregisters the plugin and the readers present in its list.
     *
     * @since 2.0.0
     */
    virtual void doUnregister();

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::string& getName() const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    std::shared_ptr<KeyplePluginExtension>
    getExtension(const std::type_info& pluginExtensionClass) const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.1.0
     */
    std::shared_ptr<KeypleReaderExtension> getReaderExtension(
        const std::type_info& readerExtensionClass,
        const std::string& readerName) const final;

    /**
     * Gets the Map of all connected readers.
     *
     * @since 2.0.0
     */
    std::map<const std::string, std::shared_ptr<CardReader>>& getReadersMap();

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::vector<std::string> getReaderNames() const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<CardReader>> getReaders() const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    std::shared_ptr<CardReader> getReader(const std::string& name) const final;

    /**
     * {@inheritDoc}
     *
     * @since 3.1.0
     */
    std::shared_ptr<CardReader>
    findReader(const std::string& readerNameRegex) const final;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger
        = LoggerFactory::getLogger(typeid(AbstractPluginAdapter));

    /**
     *
     */
    const std::string mPluginName;

    /**
     *
     */
    std::shared_ptr<KeyplePluginExtension> mPluginExtension;

    /**
     *
     */
    bool mIsRegistered;

    /**
     *
     */
    std::map<const std::string, std::shared_ptr<CardReader>> mReaders;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
