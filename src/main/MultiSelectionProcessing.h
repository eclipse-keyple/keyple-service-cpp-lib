/**************************************************************************************************
 * Copyright (c) 2023 Calypso Networks Association https://calypsonet.org/                        *
 *                                                                                                *
 * See the NOTICE file(s) distributed with this work for additional information regarding         *
 * copyright ownership.                                                                           *
 *                                                                                                *
 * This program and the accompanying materials are made available under the terms of the Eclipse  *
 * Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                  *
 *                                                                                                *
 * SPDX-License-Identifier: EPL-2.0                                                               *
 **************************************************************************************************/

#pragma once

#include <ostream>

namespace keyple {
namespace core {
namespace service {

/**
 * (package-private)<br>
 * Indicates whether the selection process should stop after the first matching or process all cases
 * in the selection request list.
 *
 * @since 2.0.0
 */
enum class MultiSelectionProcessing {
    /**
     * The selection process stops as soon as a selection case is successful.
     *
     * @since 2.0.0
     */
    FIRST_MATCH,

    /**
     * The selection process performs all the selection cases provided.
     *
     * @since 2.0.0
     */
    PROCESS_ALL
};

static inline std::ostream& operator<<(std::ostream& os, const MultiSelectionProcessing msp)
{
    os << "MULTI_SELECTION_PROCESSING: ";
    switch(msp) {
    case MultiSelectionProcessing::FIRST_MATCH:
        os << "FIRST_MATCH";
        break;
    case MultiSelectionProcessing::PROCESS_ALL:
        os << "PROCESS_ALL";
        break;
    default:
        os << "UNKNOWN";
        break;
    }

    return os;
}

}
}
}
