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

#include "keyple/core/service/AbstractMonitoringJobAdapter.hpp"

namespace keyple {
namespace core {
namespace service {

AbstractMonitoringJobAdapter::AbstractMonitoringJobAdapter(
    ObservableLocalReaderAdapter* reader)
: mReader(reader)
{
}

ObservableLocalReaderAdapter*
AbstractMonitoringJobAdapter::getReader() const
{
    return mReader;
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
