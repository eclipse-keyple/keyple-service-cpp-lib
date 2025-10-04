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

#include "keyple/core/service/CardSelectionResultAdapter.hpp"

#include <map>
#include <memory>

#include "keyple/core/util/cpp/exception/IllegalStateException.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::util::cpp::exception::IllegalStateException;

CardSelectionResultAdapter::CardSelectionResultAdapter()
: mActiveSelectionIndex(-1)
{
}

void
CardSelectionResultAdapter::addSmartCard(
    const int selectionIndex, std::shared_ptr<SmartCard> smartCard)
{
    mSmartCardMap.insert({selectionIndex, smartCard});

    /* Keep the current selection index */
    mActiveSelectionIndex = selectionIndex;
}

const std::map<int, std::shared_ptr<SmartCard>>&
CardSelectionResultAdapter::getSmartCards() const
{
    return mSmartCardMap;
}

std::shared_ptr<SmartCard>
CardSelectionResultAdapter::getActiveSmartCard() const
{
    const auto& it = mSmartCardMap.find(mActiveSelectionIndex);

    return (it != mSmartCardMap.end()) ? it->second : nullptr;
}

int
CardSelectionResultAdapter::getActiveSelectionIndex() const
{
    return mActiveSelectionIndex;
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
