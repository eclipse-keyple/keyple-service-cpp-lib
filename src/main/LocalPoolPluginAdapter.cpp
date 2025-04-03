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

#include "keyple/core/service/LocalPoolPluginAdapter.hpp"

#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

#include "keyple/core/common/KeypleReaderExtension.hpp"
#include "keyple/core/plugin/PluginIOException.hpp"
#include "keyple/core/plugin/spi/reader/PoolReaderSpi.hpp"
#include "keyple/core/service/KeyplePluginException.hpp"
#include "keyple/core/util/KeypleAssert.hpp"
#include "keyple/core/util/cpp/exception/Exception.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::common::KeypleReaderExtension;
using keyple::core::plugin::PluginIOException;
using keyple::core::plugin::spi::reader::PoolReaderSpi;
using keyple::core::util::Assert;
using keyple::core::util::cpp::exception::Exception;

LocalPoolPluginAdapter::LocalPoolPluginAdapter(
    std::shared_ptr<PoolPluginSpi> poolPluginSpi)
: AbstractPluginAdapter(
    poolPluginSpi->getName(),
    std::dynamic_pointer_cast<KeyplePluginExtension>(poolPluginSpi))
, mPoolPluginSpi(poolPluginSpi)
{
}

void
LocalPoolPluginAdapter::doUnregister()
{
    try {
        mPoolPluginSpi->onUnregister();
    } catch (const Exception& e) {
        mLogger->error(
            "Error unregistering plugin extension [%]: %\n",
            getName(),
            e.getMessage());
    }

    AbstractPluginAdapter::doUnregister();
}

const std::vector<std::string>
LocalPoolPluginAdapter::getReaderGroupReferences() const
{
    checkStatus();

    try {
        return mPoolPluginSpi->getReaderGroupReferences();
    } catch (const PluginIOException& e) {
        throw KeyplePluginException(
            "Pool plugin [" + getName()
                + "] is unable to get reader group references: "
                + e.getMessage(),
            std::make_shared<PluginIOException>(e));
    }
}

std::shared_ptr<CardReader>
LocalPoolPluginAdapter::allocateReader(const std::string& readerGroupReference)
{
    checkStatus();

    mLogger->debug(
        "Pool plugin [%] allocates reader of group reference [%]\n",
        getName(),
        readerGroupReference);

    Assert::getInstance().notEmpty(
        readerGroupReference, "readerGroupReference");

    std::shared_ptr<ReaderSpi> readerSpi = nullptr;

    try {
        readerSpi = mPoolPluginSpi->allocateReader(readerGroupReference);
    } catch (const PluginIOException& e) {
        throw KeyplePluginException(
            "Pool plugin [" + getName()
                + "] unable to allocate reader of reader group reference ["
                + readerGroupReference + "]: " + e.getMessage(),
            std::make_shared<PluginIOException>(e));
    }

    std::shared_ptr<LocalReaderAdapter> localReaderAdapter
        = buildLocalReaderAdapter(readerSpi);
    getReadersMap().insert({localReaderAdapter->getName(), localReaderAdapter});
    localReaderAdapter->doRegister();

    return localReaderAdapter;
}

void
LocalPoolPluginAdapter::releaseReader(std::shared_ptr<CardReader> reader)
{
    checkStatus();

    mLogger->debug(
        "Pool plugin [%] releases reader [%]\n",
        getName(),
        reader != nullptr ? reader->getName() : "null");

    Assert::getInstance().notNull(reader, "reader");

    try {
        mPoolPluginSpi->releaseReader(
            std::dynamic_pointer_cast<LocalReaderAdapter>(reader)
                ->getReaderSpi());

        /* Java 'finally' code moved here */
        getReadersMap().erase(reader->getName());
        std::dynamic_pointer_cast<LocalReaderAdapter>(reader)->doUnregister();
    } catch (const PluginIOException& e) {
        /* Java 'finally' code moved here */
        getReadersMap().erase(reader->getName());
        std::dynamic_pointer_cast<LocalReaderAdapter>(reader)->doUnregister();

        throw KeyplePluginException(
            "Pool plugin [" + getName() + "] unable to release reader ["
                + reader->getName() + "]: " + e.getMessage(),
            std::make_shared<PluginIOException>(e));
    }
}

std::shared_ptr<SmartCard>
LocalPoolPluginAdapter::getSelectedSmartCard(
    const std::shared_ptr<CardReader> reader)
{
    Assert::getInstance().notNull(reader, "reader");

    auto poolReaderSpi = std::dynamic_pointer_cast<PoolReaderSpi>(
        getReaderExtension(typeid(KeypleReaderExtension), reader->getName()));

    return std::dynamic_pointer_cast<SmartCard>(
        poolReaderSpi->getSelectedSmartCard());
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
