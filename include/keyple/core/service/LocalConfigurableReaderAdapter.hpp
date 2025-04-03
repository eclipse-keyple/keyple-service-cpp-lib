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

#include "keyple/core/plugin/spi/reader/ConfigurableReaderSpi.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/LocalReaderAdapter.hpp"
#include "keypop/reader/ConfigurableCardReader.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::plugin::spi::reader::ConfigurableReaderSpi;
using keypop::reader::ConfigurableCardReader;

/**
 * (package-private)<br>
 * Local configurable reader adapter.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API LocalConfigurableReaderAdapter final
: public LocalReaderAdapter,
  public ConfigurableCardReader {
public:
    /**
     * Constructor.
     *
     * @param configurableReaderSpi The configurable reader SPI.
     * @param pluginName The name of the plugin.
     * @since 2.0.0
     */
    LocalConfigurableReaderAdapter(
        std::shared_ptr<ConfigurableReaderSpi> configurableReaderSpi,
        const std::string& pluginName);

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void activateProtocol(
        const std::string& readerProtocol,
        const std::string& applicationProtocol) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void deactivateProtocol(const std::string& readerProtocol) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.1.2
     */
    const std::string& getCurrentProtocol() const override;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
