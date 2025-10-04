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

#include "keyple/core/service/ReaderApiFactoryAdapter.hpp"

#include <memory>

#include "keyple/core/service/BasicCardSelectorAdapter.hpp"
#include "keyple/core/service/CardSelectionManagerAdapter.hpp"
#include "keyple/core/service/IsoCardSelectorAdapter.hpp"

namespace keyple {
namespace core {
namespace service {

std::shared_ptr<CardSelectionManager>
ReaderApiFactoryAdapter::createCardSelectionManager()
{
    return std::make_shared<CardSelectionManagerAdapter>();
}

std::shared_ptr<BasicCardSelector>
ReaderApiFactoryAdapter::createBasicCardSelector()
{
    return std::make_shared<BasicCardSelectorAdapter>();
}

std::shared_ptr<IsoCardSelector>
ReaderApiFactoryAdapter::createIsoCardSelector()
{
    return std::make_shared<IsoCardSelectorAdapter>();
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
