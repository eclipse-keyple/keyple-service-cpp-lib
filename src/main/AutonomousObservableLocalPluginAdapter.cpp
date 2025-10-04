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

#include "keyple/core/service/AutonomousObservableLocalPluginAdapter.hpp"

#include <memory>
#include <string>
#include <vector>

#include "keyple/core/service/PluginEventAdapter.hpp"
#include "keyple/core/util/KeypleAssert.hpp"
#include "keyple/core/util/cpp/exception/Exception.hpp"
#include "keypop/reader/CardReader.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::util::cpp::exception::Exception;
using keypop::reader::CardReader;

AutonomousObservableLocalPluginAdapter::AutonomousObservableLocalPluginAdapter(
    std::shared_ptr<AutonomousObservablePluginSpi>
        autonomousObservablePluginSpi)
: AbstractObservableLocalPluginAdapter(autonomousObservablePluginSpi)
{
    try {
        autonomousObservablePluginSpi->setCallback(this);
    } catch (const Exception& e) {
        mLogger->trace(
            "Method 'setCallback(...)' unavailable for legacy plugin: %\n",
            e.getMessage());
        autonomousObservablePluginSpi->connect(this);
    }
}

void
AutonomousObservableLocalPluginAdapter::onReaderConnected(
    const std::vector<std::shared_ptr<ReaderSpi>>& readers)
{
    Assert::getInstance().notEmpty(readers, "readers");
    std::vector<std::string> notifyReaders;

    for (const auto& readerSpi : readers) {
        /* Add reader to plugin */
        addReader(readerSpi);
        notifyReaders.push_back(readerSpi->getName());
    }

    notifyObservers(std::make_shared<PluginEventAdapter>(
        getName(), notifyReaders, PluginEvent::Type::READER_CONNECTED));
}

void
AutonomousObservableLocalPluginAdapter::onReaderDisconnected(
    const std::vector<std::string>& readerNames)
{
    Assert::getInstance().notEmpty(readerNames, "readerNames");
    std::vector<std::string> notifyReaders;

    for (const auto& readerName : readerNames) {
        const std::shared_ptr<CardReader> reader = getReader(readerName);
        if (reader == nullptr) {
            mLogger->warn(
                "Plugin [%] unable to remove unknown reader [%]\n",
                getName(),
                readerName);
        } else {
            /* Unregister and remove reader */
            std::dynamic_pointer_cast<LocalReaderAdapter>(reader)
                ->doUnregister();
            getReadersMap().erase(reader->getName());
            mLogger->info(
                "Plugin [%] removes reader [%] from readers list\n",
                getName(),
                reader->getName());

            notifyReaders.push_back(readerName);
        }
    }

    notifyObservers(std::make_shared<PluginEventAdapter>(
        getName(), notifyReaders, PluginEvent::Type::READER_DISCONNECTED));
}

void
AutonomousObservableLocalPluginAdapter::addReader(
    std::shared_ptr<ReaderSpi> readerSpi)
{
    std::shared_ptr<LocalReaderAdapter> reader
        = buildLocalReaderAdapter(readerSpi);
    reader->doRegister();
    getReadersMap().insert({reader->getName(), reader});

    mLogger->info(
        "Plugin [%] adds reader [%] to readers list\n",
        getName(),
        readerSpi->getName());
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
