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

#include "keyple/core/service/LocalPluginAdapter.hpp"

#include <memory>
#include <vector>

#include "keyple/core/service/LocalReaderAdapter.hpp"
#include "keyple/core/service/ObservableLocalReaderAdapter.hpp"
#include "keyple/core/util/cpp/exception/Exception.hpp"
#include "keyple/core/util/cpp/exception/IllegalArgumentException.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::util::cpp::exception::Exception;
using keyple::core::util::cpp::exception::IllegalArgumentException;

LocalPluginAdapter::LocalPluginAdapter(std::shared_ptr<PluginSpi> pluginSpi)
: AbstractPluginAdapter(
    pluginSpi ? pluginSpi->getName() : "",
    std::dynamic_pointer_cast<KeyplePluginExtension>(pluginSpi))
, mPluginSpi(pluginSpi)
{
    if (pluginSpi == nullptr) {
        throw IllegalArgumentException("Invalid pluginSpi");
    }
}

void
LocalPluginAdapter::doRegister()
{
    AbstractPluginAdapter::doRegister();

    const std::vector<std::shared_ptr<ReaderSpi>> readerSpiList
        = mPluginSpi->searchAvailableReaders();

    for (const auto& readerSpi : readerSpiList) {
        std::shared_ptr<LocalReaderAdapter> localReaderAdapter
            = buildLocalReaderAdapter(readerSpi);
        getReadersMap().insert({readerSpi->getName(), localReaderAdapter});
        localReaderAdapter->doRegister();
    }
}

void
LocalPluginAdapter::doUnregister()
{
    try {
        mPluginSpi->onUnregister();
    } catch (const Exception& e) {
        mLogger->error(
            "Error unregistering plugin extension [%]: %\n",
            getName(),
            e.getMessage());
    }

    AbstractPluginAdapter::doUnregister();
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
