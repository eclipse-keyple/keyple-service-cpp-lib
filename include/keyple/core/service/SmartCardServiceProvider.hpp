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

#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/SmartCardServiceAdapter.hpp"

namespace keyple {
namespace core {
namespace service {

/**
 * Provider of the SmartCardService.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API SmartCardServiceProvider final {
public:
    /**
     * Gets the unique instance of {@link SmartCardService}.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    static std::shared_ptr<SmartCardServiceAdapter> getService();

private:
    /**
     * Private constructor
     */
    SmartCardServiceProvider();
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
