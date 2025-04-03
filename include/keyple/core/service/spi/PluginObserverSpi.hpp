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

#include "keyple/core/service/PluginEvent.hpp"

namespace keyple {
namespace core {
namespace service {
namespace spi {

using keyple::core::service::PluginEvent;

/**
 * Plugin observer recipient of the {@link PluginEvent} from a {@link
 * org.eclipse.keyple.core.service.ObservablePlugin}.
 *
 * @since 2.0.0
 */
class PluginObserverSpi {
public:
    /**
     * Invoked when a plugin event occurs.
     *
     * <p>The event notification is <b>synchronous</b> for <b>local</b> plugin and <b>asynchronous
     * </b> for <b>remote</b> plugin.
     *
     * <p>The same behaviour is used for notification of reader events.
     *
     * @param pluginEvent The plugin event.
     * @since 2.0.0
     */
    virtual void onPluginEvent(const std::shared_ptr<PluginEvent> pluginEvent)
        = 0;
};

} /* namespace spi */
} /* namespace service */
} /* namespace core */
} /* namespace keyple */
