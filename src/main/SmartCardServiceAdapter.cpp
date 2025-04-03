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

#include "keyple/core/service/SmartCardServiceAdapter.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "keyple/core/common/CommonApiProperties.hpp"
#include "keyple/core/plugin/PluginApiProperties.hpp"
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
#include "keyple/core/util/cpp/KeypleStd.hpp"
#include "keyple/core/util/cpp/StringUtils.hpp"
#include "keyple/core/util/cpp/exception/IllegalArgumentException.hpp"
#include "keyple/core/util/cpp/exception/IllegalStateException.hpp"
#include "keyple/core/util/cpp/exception/PatternSyntaxException.hpp"
#include "keypop/card/CardApiProperties.hpp"
#include "keypop/reader/ReaderApiProperties.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::common::CommonApiProperties_VERSION;
using keyple::core::plugin::PluginApiProperties_VERSION;
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

const std::string SmartCardServiceAdapter::MSG_VERSION_MISMATCH_DETECTED
    = "Version mismatch detected: % [%] uses '%' version '%' (expected '%'). "
      "Compatibility issues "
      "may arise\n";

std::shared_ptr<SmartCardServiceAdapter> SmartCardServiceAdapter::mInstance;

std::shared_ptr<SmartCardServiceAdapter>
SmartCardServiceAdapter::getInstance()
{
    if (mInstance == nullptr) {
        mInstance = std::shared_ptr<SmartCardServiceAdapter>(
            new SmartCardServiceAdapter());
    }

    return mInstance;
}

int
SmartCardServiceAdapter::compareVersions(
    const std::string& providedVersion, const std::string& localVersion) const
{
    const std::regex re("[.]");

    std::vector<std::string> providedVersions;
    std::split(providedVersion, re, std::back_inserter(providedVersions));

    std::vector<std::string> localVersions;
    std::split(localVersion, re, std::back_inserter(localVersions));

    if (providedVersions.size() != localVersions.size()) {
        throw IllegalStateException(
            "Inconsistent version numbers: provided = " + providedVersion
            + ", local = " + localVersion);
    }

    int provided = 0;
    int local = 0;

    try {
        for (const auto& v : providedVersions) {
            provided += std::stoi(v);
            provided *= 1000;
        }
        for (const auto& v : localVersions) {
            local += std::stoi(v);
            local *= 1000;
        }

        /* Replaced NumberFormatException by std::invalid_argument */
    } catch (const std::invalid_argument& e) {
        (void)e;
        throw IllegalStateException(
            "Bad version numbers: provided = " + providedVersion
                + ", local = " + localVersion,
            nullptr);
    }

    /* Java uses compareTo() which returns 0 when equal */
    return provided != local;
}

void
SmartCardServiceAdapter::checkPluginVersion(
    const std::shared_ptr<PluginFactorySpi> pluginFactorySpi)
{
    if (compareVersions(
            pluginFactorySpi->getCommonApiVersion(),
            CommonApiProperties_VERSION)
        != 0) {
        mLogger->warn(
            MSG_VERSION_MISMATCH_DETECTED,
            std::string("plugin"),
            pluginFactorySpi->getPluginName(),
            std::string("Common API"),
            pluginFactorySpi->getCommonApiVersion(),
            CommonApiProperties_VERSION);
    }

    if (compareVersions(
            pluginFactorySpi->getPluginApiVersion(),
            PluginApiProperties_VERSION)
        != 0) {
        mLogger->warn(
            MSG_VERSION_MISMATCH_DETECTED,
            std::string("plugin"),
            pluginFactorySpi->getPluginName(),
            std::string("Plugin API"),
            pluginFactorySpi->getPluginApiVersion(),
            PluginApiProperties_VERSION);
    }
}

std::shared_ptr<Plugin>
SmartCardServiceAdapter::registerPlugin(
    const std::shared_ptr<KeyplePluginExtensionFactory> pluginFactory)
{
    Assert::getInstance().notNull(pluginFactory, "pluginFactory");

    std::shared_ptr<AbstractPluginAdapter> plugin = nullptr;

    const std::lock_guard<std::mutex> lock(mMutex);

    try {
        const auto factorySpi
            = std::dynamic_pointer_cast<PluginFactorySpi>(pluginFactory);
        const auto poolSpi
            = std::dynamic_pointer_cast<PoolPluginFactorySpi>(pluginFactory);
        if (factorySpi) {
            plugin = createLocalPlugin(factorySpi);
        } else if (poolSpi) {
            plugin = createLocalPoolPlugin(poolSpi);
            // } else if (pluginFactory instanceof RemotePluginFactorySpi) {
            // plugin = createRemotePlugin((RemotePluginFactorySpi)
            // pluginFactory);
        } else {
            throw IllegalArgumentException(
                "The factory doesn't implement the right SPI");
        }

        mPlugins.insert({plugin->getName(), plugin});
        plugin->doRegister();
    } catch (const IllegalArgumentException& e) {
        throw IllegalArgumentException(
            "The provided plugin factory doesn't implement the plugin"
            " API properly",
            std::make_shared<IllegalArgumentException>(e));
    } catch (const PluginIOException& e) {
        throw KeyplePluginException(
            "Unable to register the plugin [" + plugin->getName()
                + "]: " + e.getMessage(),
            std::make_shared<PluginIOException>(e));
    }

    return plugin;
}

void
SmartCardServiceAdapter::unregisterPlugin(const std::string& pluginName)
{
    mLogger->info("Unregister plugin [%]\n", pluginName);

    const std::lock_guard<std::mutex> lock(mMutex);

    auto i = mPlugins.find(pluginName);
    if (i != mPlugins.end()) {
        std::shared_ptr<Plugin> removedPlugin = std::move(i->second);
        std::dynamic_pointer_cast<AbstractPluginAdapter>(removedPlugin)
            ->doUnregister();
        mPlugins.erase(i);
    } else {
        mLogger->warn("Plugin [%] not registered\n", pluginName);
    }
}

const std::vector<std::string>
SmartCardServiceAdapter::getPluginNames() const
{
    std::vector<std::string> pluginNames;
    for (const auto& pair : mPlugins) {
        pluginNames.push_back(pair.first);
    }

    return pluginNames;
}

const std::vector<std::shared_ptr<Plugin>>
SmartCardServiceAdapter::getPlugins() const
{
    std::vector<std::shared_ptr<Plugin>> plugins;
    for (const auto& pair : mPlugins) {
        plugins.push_back(pair.second);
    }

    return plugins;
}

std::shared_ptr<Plugin>
SmartCardServiceAdapter::getPlugin(const std::string& pluginName) const
{
    auto it = mPlugins.find(pluginName);
    if (it != mPlugins.end()) {
        return it->second;
    }

    return nullptr;
}

std::shared_ptr<Plugin>
SmartCardServiceAdapter::getPlugin(
    const std::shared_ptr<CardReader> cardReader) const
{
    for (const auto& plugin : mPlugins) {
        const auto& readers = plugin.second->getReaders();
        if (std::any_of(
                readers.begin(),
                readers.end(),
                [cardReader](const std::shared_ptr<CardReader>& reader) {
                    return reader == cardReader;
                })) {
            return plugin.second;
        }
    }

    return nullptr;
}

std::shared_ptr<CardReader>
SmartCardServiceAdapter::getReader(const std::string& readerName) const
{
    for (const auto& plugin : mPlugins) {
        const auto& readers = plugin.second->getReaders();
        auto it = std::find_if(
            readers.begin(),
            readers.end(),
            [readerName](const std::shared_ptr<CardReader>& reader) {
                return reader->getName() == readerName;
            });
        if (it != readers.end()) {
            return *it;
        }
    }

    return nullptr;
}

std::shared_ptr<CardReader>
SmartCardServiceAdapter::findReader(const std::string& readerNameRegex) const
{
    for (const auto& plugin : mPlugins) {
        for (const auto& reader : plugin.second->getReaders()) {
            try {
                if (StringUtils::matches(reader->getName(), readerNameRegex)) {
                    return reader;
                }
            } catch (const PatternSyntaxException& e) {
                throw IllegalArgumentException(
                    "readerNameRegex is invalid: " + e.getMessage(),
                    std::make_shared<Exception>(e));
            }
        }
    }

    return nullptr;
}

void
SmartCardServiceAdapter::checkCardExtension(
    const std::shared_ptr<KeypleCardExtension> cardExtension) const
{
    checkCardExtensionVersion(cardExtension);
}

std::unique_ptr<ReaderApiFactory>
SmartCardServiceAdapter::getReaderApiFactory()
{
    return std::unique_ptr<ReaderApiFactoryAdapter>(
        new ReaderApiFactoryAdapter());
}

void
SmartCardServiceAdapter::checkPoolPluginVersion(
    const std::shared_ptr<PoolPluginFactorySpi> poolPluginFactorySpi)
{
    if (compareVersions(
            poolPluginFactorySpi->getCommonApiVersion(),
            CommonApiProperties_VERSION)
        != 0) {
        mLogger->warn(
            MSG_VERSION_MISMATCH_DETECTED,
            std::string("pool plugin"),
            poolPluginFactorySpi->getPoolPluginName(),
            std::string("Common API"),
            poolPluginFactorySpi->getCommonApiVersion(),
            CommonApiProperties_VERSION);
    }

    if (compareVersions(
            poolPluginFactorySpi->getPluginApiVersion(),
            PluginApiProperties_VERSION)
        != 0) {
        mLogger->warn(
            MSG_VERSION_MISMATCH_DETECTED,
            std::string("pool plugin"),
            poolPluginFactorySpi->getPoolPluginName(),
            std::string("Plugin API"),
            poolPluginFactorySpi->getPluginApiVersion(),
            PluginApiProperties_VERSION);
    }
}

void
SmartCardServiceAdapter::checkCardExtensionVersion(
    const std::shared_ptr<KeypleCardExtension> cardExtension) const
{
    if (compareVersions(
            cardExtension->getCommonApiVersion(), CommonApiProperties_VERSION)
        != 0) {
        const auto& c = *cardExtension.get();
        mLogger->warn(
            MSG_VERSION_MISMATCH_DETECTED,
            std::string("card extension"),
            std::string(typeid(c).name()),
            std::string("Common API"),
            cardExtension->getCommonApiVersion(),
            CommonApiProperties_VERSION);
    }

    if (compareVersions(
            cardExtension->getCardApiVersion(), CardApiProperties_VERSION)
        != 0) {
        const auto& c = *cardExtension.get();
        mLogger->warn(
            MSG_VERSION_MISMATCH_DETECTED,
            std::string("card extension"),
            std::string(typeid(c).name()),
            std::string("Card API"),
            cardExtension->getCardApiVersion(),
            CardApiProperties_VERSION);
    }

    if (compareVersions(
            cardExtension->getReaderApiVersion(), ReaderApiProperties_VERSION)
        != 0) {
        const auto& c = *cardExtension.get();
        mLogger->warn(
            MSG_VERSION_MISMATCH_DETECTED,
            std::string("card extension"),
            std::string(typeid(c).name()),
            std::string("Reader API"),
            cardExtension->getReaderApiVersion(),
            ReaderApiProperties_VERSION);
    }
}

void
SmartCardServiceAdapter::checkPluginRegistration(const std::string& pluginName)
{
    mLogger->info("Registering a new Plugin to the service : %\n", pluginName);

    const auto it = mPlugins.find(pluginName);
    if (it != mPlugins.end()) {
        throw IllegalStateException(
            "Plugin [" + pluginName
            + "] has already been registered to the service.");
    }
}

std::shared_ptr<AbstractPluginAdapter>
SmartCardServiceAdapter::createLocalPlugin(
    std::shared_ptr<PluginFactorySpi> pluginFactorySpi)
{
    checkPluginRegistration(pluginFactorySpi->getPluginName());
    checkPluginVersion(pluginFactorySpi);

    std::shared_ptr<PluginSpi> pluginSpi = pluginFactorySpi->getPlugin();

    if (pluginSpi->getName() != pluginFactorySpi->getPluginName()) {
        throw IllegalArgumentException(
            "Plugin name [" + pluginSpi->getName()
            + "] mismatches the expected name ["
            + pluginFactorySpi->getPluginName() + "] provided by the factory");
    }

    std::shared_ptr<AbstractPluginAdapter> plugin = nullptr;

    auto observable = std::dynamic_pointer_cast<ObservablePluginSpi>(pluginSpi);
    if (observable) {
        plugin = std::make_shared<ObservableLocalPluginAdapter>(observable);
    } else {
        auto autonomous
            = std::dynamic_pointer_cast<AutonomousObservablePluginSpi>(
                pluginSpi);
        if (autonomous) {
            plugin = std::make_shared<AutonomousObservableLocalPluginAdapter>(
                autonomous);
        } else {
            plugin = std::make_shared<LocalPluginAdapter>(pluginSpi);
        }
    }

    return plugin;
}

std::shared_ptr<AbstractPluginAdapter>
SmartCardServiceAdapter::createLocalPoolPlugin(
    std::shared_ptr<PoolPluginFactorySpi> poolPluginFactorySpi)
{
    checkPluginRegistration(poolPluginFactorySpi->getPoolPluginName());
    checkPoolPluginVersion(poolPluginFactorySpi);

    std::shared_ptr<PoolPluginSpi> poolPluginSpi
        = poolPluginFactorySpi->getPoolPlugin();

    if (poolPluginSpi->getName() != poolPluginFactorySpi->getPoolPluginName()) {
        throw IllegalArgumentException(
            "Pool plugin name [" + poolPluginSpi->getName()
            + "] mismatches the expected name ["
            + poolPluginFactorySpi->getPoolPluginName()
            + "] provided by the factory");
    }

    return std::make_shared<LocalPoolPluginAdapter>(poolPluginSpi);
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
