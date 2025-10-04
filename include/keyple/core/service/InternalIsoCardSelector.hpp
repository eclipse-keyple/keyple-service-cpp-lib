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

#include <vector>

#include "keyple/core/service/InternalCardSelector.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keypop/reader/selection/CommonIsoCardSelector.hpp"
#include "keypop/reader/selection/FileControlInformation.hpp"
#include "keypop/reader/selection/FileOccurrence.hpp"

namespace keyple {
namespace core {
namespace service {

using keypop::reader::selection::FileControlInformation;
using keypop::reader::selection::FileOccurrence;

/**
 * Internal interface defining the package private getters of the
 * keypop::reader::selection::IsoCardSelector.
 *
 * @since 3.0.0
 */
class KEYPLESERVICE_API InternalIsoCardSelector : public InternalCardSelector {
public:
    /**
     * Virtual destructor.
     */
    virtual ~InternalIsoCardSelector() = default;

    /**
     * Gets the ISO7816-4 Application Identifier (AID).
     *
     * @return Null if no AID has been set.
     * @since 3.0.0
     */
    virtual const std::vector<uint8_t> getAid() const = 0;

    /**
     * Gets the CommonIsoCardSelector::FileOccurrence parameter defining the
     * navigation within the card applications list.
     *
     * @return A not null reference.
     * @since 3.0.0
     */
    virtual FileOccurrence getFileOccurrence() const = 0;

    /**
     * Gets the CommonIsoCardSelector::FileControlInformation parameter defining
     * the output type of the select command.
     *
     * @return A not null reference.
     * @since 3.0.0
     */
    virtual FileControlInformation getFileControlInformation() const = 0;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
