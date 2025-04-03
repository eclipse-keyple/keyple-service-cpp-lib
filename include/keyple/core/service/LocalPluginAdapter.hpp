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

#include "keyple/core/plugin/spi/PluginSpi.hpp"
#include "keyple/core/service/AbstractPluginAdapter.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/util/cpp/LoggerFactory.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::plugin::spi::PluginSpi;
using keyple::core::util::cpp::Logger;
using keyple::core::util::cpp::LoggerFactory;

/**
 * Implementation of a local {@link Plugin}.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API LocalPluginAdapter : public AbstractPluginAdapter {
public:
    /**
     * Constructor.
     *
     * @param pluginSpi The associated SPI.
     * @since 2.0.0
     */
    explicit LocalPluginAdapter(std::shared_ptr<PluginSpi> pluginSpi);

    /**
     *
     */
    virtual ~LocalPluginAdapter() = default;

    /**
     * {@inheritDoc}
     *
     * <p>Populates its list of available readers and registers each of them.
     *
     * @since 2.0.0
     */
    void doRegister() final;

    /**
     * {@inheritDoc}
     *
     * <p>Unregisters the associated SPI.
     *
     * @since 2.0.0
     */
    void doUnregister() override;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger
        = LoggerFactory::getLogger(typeid(LocalPluginAdapter));

    /**
     *
     */
    std::shared_ptr<PluginSpi> mPluginSpi;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
