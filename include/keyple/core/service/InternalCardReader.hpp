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

#include <string>

#include "keyple/core/service/KeypleServiceExport.hpp"

namespace keyple {
namespace core {
namespace service {

/**
 * Internal interface defining the package private getters of the
 * keypop::reader::selection::CardSelector.
 *
 * @since 3.0.0
 */
class KEYPLESERVICE_API InternalCardSelector {
public:
    /**
     * Virtual destructor.
     */
    virtual ~InternalCardSelector() = default;

    /**
     * Gets the logical card protocol name.
     *
     * @return Null if no card protocol has been set.
     * @since 3.0.0
     */
    virtual const std::string& getLogicalProtocolName() const = 0;

    /**
     * Gets the regular expression to be applied to the card's power-on data.
     *
     * @return Null if no power-on data regex has been set.
     * @since 3.0.0
     */
    virtual const std::string& getPowerOnDataRegex() const = 0;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
