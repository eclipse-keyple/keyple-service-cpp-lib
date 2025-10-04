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

#include "keyple/core/plugin/spi/PoolPluginSpi.hpp"
#include "keyple/core/service/AbstractPluginAdapter.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/PoolPlugin.hpp"
#include "keypop/reader/CardReader.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::plugin::spi::PoolPluginSpi;
using keypop::reader::CardReader;

/**
 * Implementation of a local {@link PoolPlugin}.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API LocalPoolPluginAdapter final
: public AbstractPluginAdapter,
  public PoolPlugin {
public:
    /**
     * Constructor.
     *
     * @param poolPluginSpi The associated SPI.
     * @since 2.0.0
     */
    explicit LocalPoolPluginAdapter(
        std::shared_ptr<PoolPluginSpi> poolPluginSpi);

    /**     *
     * <p>Unregisters the associated SPI.
     *
     * @since 2.0.0
     */
    void doUnregister() final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::vector<std::string> getReaderGroupReferences() const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    std::shared_ptr<CardReader>
    allocateReader(const std::string& readerGroupReference) final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void releaseReader(const std::shared_ptr<CardReader> reader) final;

    /**
     * {@inheritDoc}
     *
     * @since 2.2.0
     */
    std::shared_ptr<SmartCard>
    getSelectedSmartCard(const std::shared_ptr<CardReader> reader);

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger
        = LoggerFactory::getLogger(typeid(LocalPoolPluginAdapter));

    /**
     *
     */
    std::shared_ptr<PoolPluginSpi> mPoolPluginSpi;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
