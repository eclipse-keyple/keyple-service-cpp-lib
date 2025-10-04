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

#include "keyple/core/common/KeyplePluginExtension.hpp"
#include "keyple/core/common/KeypleReaderExtension.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keypop/reader/CardReader.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::common::KeyplePluginExtension;
using keyple::core::common::KeypleReaderExtension;
using keypop::reader::CardReader;

/**
 * Manager for one or more CardReader from the same family.
 *
 * <p>Provides the means to get the plugin name, enumerate and retrieve the
 * readers.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API Plugin {
public:
    /**
     *
     */
    virtual ~Plugin() = default;

    /**
     * Gets the name of the plugin.
     *
     * @return A not empty string.
     * @since 2.0.0
     */
    virtual const std::string& getName() const = 0;

    /**
     * Returns the KeyplePluginExtension that is plugin-specific.
     *
     * <p>Note: the provided argument is used at compile time to check the type
     * consistency.
     *
     * @param pluginExtensionClass The specific class of the plugin.
     * @param <T> The type of the plugin extension.
     * @return A not null reference.
     * @since 2.0.0
     */
    virtual std::shared_ptr<KeyplePluginExtension>
    getExtension(const std::type_info& pluginExtensionClass) const = 0;

    /**
     * Returns the {@link KeypleReaderExtension} that is reader-specific.
     *
     * <p>Note: the provided argument is used at compile time to check the type
     * consistency.
     *
     * @param readerExtensionClass The specific class of the reader.
     * @param readerName The name of the reader.
     * @param <T> The type of the reader extension.
     * @return A {@link KeypleReaderExtension}.
     * @throw IllegalStateException If plugin or reader is no longer registered.
     * @throw IllegalArgumentException If the reader name is unknown.
     * @since 2.1.0
     */
    virtual std::shared_ptr<KeypleReaderExtension> getReaderExtension(
        const std::type_info& readerExtensionClass,
        const std::string& readerName) const = 0;

    /**
     * Gets the names of all connected readers.
     *
     * @return An empty set if there's no reader connected.
     * @throw IllegalStateException if plugin is no longer registered.
     * @since 2.0.0
     */
    virtual const std::vector<std::string> getReaderNames() const = 0;

    /**
     * Gets all connected readers.
     *
     * @return An empty Set if there's no reader connected.
     * @throw IllegalStateException if the plugin is no longer registered.
     * @since 2.0.0
     */
    virtual const std::vector<std::shared_ptr<CardReader>>
    getReaders() const = 0;

    /**
     * Gets the CardReader whose name is provided.
     *
     * @param name The name of the reader.
     * @return Null if the reader has not been found.
     * @throw IllegalStateException if the plugin is no longer registered.
     * @since 2.0.0
     */
    virtual std::shared_ptr<CardReader>
    getReader(const std::string& name) const = 0;

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
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
