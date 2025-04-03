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
#include <vector>

#include "keyple/core/plugin/AutonomousObservablePluginApi.hpp"
#include "keyple/core/plugin/spi/AutonomousObservablePluginSpi.hpp"
#include "keyple/core/service/AbstractObservableLocalPluginAdapter.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/util/cpp/LoggerFactory.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::plugin::AutonomousObservablePluginApi;
using keyple::core::plugin::spi::AutonomousObservablePluginSpi;

/**
 * Implementation of an autonomous local {@link ObservablePlugin}.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API AutonomousObservableLocalPluginAdapter final
: public AbstractObservableLocalPluginAdapter,
  public AutonomousObservablePluginApi,
  public std::enable_shared_from_this<AutonomousObservableLocalPluginAdapter> {
public:
    /**
     * Constructor.
     *
     * @param autonomousObservablePluginSpi The associated plugin SPI.
     * @since 2.0.0
     */
    explicit AutonomousObservableLocalPluginAdapter(
        std::shared_ptr<AutonomousObservablePluginSpi>
            autonomousObservablePluginSpi);

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void onReaderConnected(
        const std::vector<std::shared_ptr<ReaderSpi>>& readers) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void
    onReaderDisconnected(const std::vector<std::string>& readerNames) override;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger = LoggerFactory::getLogger(
        typeid(AutonomousObservableLocalPluginAdapter));

    /**
     * Create and add a reader to the reader list from a readerSpi
     *
     * @param readerSpi spi to create the reader from
     */
    void addReader(std::shared_ptr<ReaderSpi> readerSpi);
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
