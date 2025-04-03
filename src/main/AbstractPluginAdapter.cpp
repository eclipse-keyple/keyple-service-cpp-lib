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

#include "keyple/core/service/AbstractPluginAdapter.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "keyple/core/service/AbstractReaderAdapter.hpp"
#include "keyple/core/service/LocalConfigurableReaderAdapter.hpp"
#include "keyple/core/service/ObservableLocalConfigurableReaderAdapter.hpp"
#include "keyple/core/util/cpp/StringUtils.hpp"
#include "keyple/core/util/cpp/exception/Exception.hpp"
#include "keyple/core/util/cpp/exception/IllegalArgumentException.hpp"
#include "keyple/core/util/cpp/exception/IllegalStateException.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::util::cpp::StringUtils;
using keyple::core::util::cpp::exception::Exception;
using keyple::core::util::cpp::exception::IllegalArgumentException;
using keyple::core::util::cpp::exception::IllegalStateException;

AbstractPluginAdapter::AbstractPluginAdapter(
    const std::string& pluginName,
    std::shared_ptr<KeyplePluginExtension> pluginExtension)
: mPluginName(pluginName)
, mPluginExtension(pluginExtension)
, mIsRegistered(false)
{
}

std::shared_ptr<LocalReaderAdapter>
AbstractPluginAdapter::buildLocalReaderAdapter(
    std::shared_ptr<ReaderSpi> readerSpi)
{
    std::shared_ptr<LocalReaderAdapter> adapter = nullptr;

    const auto& observable
        = std::dynamic_pointer_cast<ObservableReaderSpi>(readerSpi);
    const auto& configurable
        = std::dynamic_pointer_cast<ConfigurableReaderSpi>(readerSpi);

    if (observable) {
        if (configurable) {
            adapter
                = std::make_shared<ObservableLocalConfigurableReaderAdapter>(
                    configurable, getName());
        } else {
            adapter = std::make_shared<ObservableLocalReaderAdapter>(
                observable, getName());
        }
    } else if (configurable) {
        adapter = std::make_shared<LocalConfigurableReaderAdapter>(
            configurable, getName());
    } else {
        adapter = std::make_shared<LocalReaderAdapter>(readerSpi, getName());
    }

    return adapter;
}

void
AbstractPluginAdapter::checkStatus() const
{
    if (!mIsRegistered) {
        throw IllegalStateException(
            "Plugin [" + mPluginName + "] is not or no longer registered");
    }
}

void
AbstractPluginAdapter::doRegister()
{
    mIsRegistered = true;
}

void
AbstractPluginAdapter::doUnregister()
{
    mIsRegistered = false;

    for (const auto& pair : mReaders) {
        try {
            std::dynamic_pointer_cast<AbstractReaderAdapter>(pair.second)
                ->doUnregister();
        } catch (const Exception& e) {
            mLogger->error(
                "Error unregistering reader [%] %'\n",
                pair.second->getName(),
                e.getMessage());
        }
    }

    mReaders.clear();
    mIsRegistered = false;
}

const std::string&
AbstractPluginAdapter::getName() const
{
    return mPluginName;
}

std::shared_ptr<KeyplePluginExtension>
AbstractPluginAdapter::getExtension(
    const std::type_info& pluginExtensionClass) const
{
    (void)pluginExtensionClass;

    checkStatus();

    return mPluginExtension;
}

std::shared_ptr<KeypleReaderExtension>
AbstractPluginAdapter::getReaderExtension(
    const std::type_info& readerExtensionClass,
    const std::string& readerName) const
{
    checkStatus();

    const auto reader = std::dynamic_pointer_cast<AbstractReaderAdapter>(
        getReader(readerName));
    if (reader == nullptr) {
        throw IllegalArgumentException(
            "Reader [" + readerName + "] not found!");
    }

    return reader->getExtension(readerExtensionClass);
}

std::map<const std::string, std::shared_ptr<CardReader>>&
AbstractPluginAdapter::getReadersMap()
{
    return mReaders;
}

const std::vector<std::string>
AbstractPluginAdapter::getReaderNames() const
{
    checkStatus();

    std::vector<std::string> readerNames;
    for (const auto& pair : mReaders) {
        readerNames.push_back(pair.first);
    }

    return readerNames;
}

const std::vector<std::shared_ptr<CardReader>>
AbstractPluginAdapter::getReaders() const
{
    checkStatus();

    std::vector<std::shared_ptr<CardReader>> readers;
    for (const auto& pair : mReaders) {
        readers.push_back(pair.second);
    }

    return readers;
}

std::shared_ptr<CardReader>
AbstractPluginAdapter::getReader(const std::string& name) const
{
    checkStatus();

    if (mReaders.find(name) != mReaders.end()) {
        return mReaders.at(name);
    }

    return nullptr;
}

std::shared_ptr<CardReader>
AbstractPluginAdapter::findReader(const std::string& readerNameRegex) const
{
    for (const auto& reader : mReaders) {
        try {
            if (StringUtils::matches(
                    reader.second->getName(), readerNameRegex)) {
                return reader.second;
            }

        } catch (const std::regex_error& e) {
            throw IllegalArgumentException(
                "readerNameRegex is invalid: " + std::string(e.what()),
                std::make_shared<Exception>(e.what()));
        }
    }

    return nullptr;
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
