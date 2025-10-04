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

#include "keyple/core/service/SmartCardServiceProvider.hpp"

#include <memory>

namespace keyple {
namespace core {
namespace service {

SmartCardServiceProvider::SmartCardServiceProvider()
{
}

std::shared_ptr<SmartCardServiceAdapter>
SmartCardServiceProvider::getService()
{
    return SmartCardServiceAdapter::getInstance();
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
