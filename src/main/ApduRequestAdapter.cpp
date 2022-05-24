/**************************************************************************************************
 * Copyright (c) 2021 Calypso Networks Association https://calypsonet.org/                        *
 *                                                                                                *
 * See the NOTICE file(s) distributed with this work for additional information regarding         *
 * copyright ownership.                                                                           *
 *                                                                                                *
 * This program and the accompanying materials are made available under the terms of the Eclipse  *
 * Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                  *
 *                                                                                                *
 * SPDX-License-Identifier: EPL-2.0                                                               *
 **************************************************************************************************/

#include "ApduRequestAdapter.h"

/* Keyple Code Util */
#include "KeypleStd.h"

namespace keyple {
namespace core {
namespace service {

const int ApduRequestAdapter::DEFAULT_SUCCESSFUL_CODE = 0x9000;

ApduRequestAdapter::ApduRequestAdapter(const std::vector<uint8_t>& apdu)
: mApdu(apdu), mSuccessfulStatusWords({DEFAULT_SUCCESSFUL_CODE}) {}

ApduRequestAdapter& ApduRequestAdapter::addSuccessfulStatusWord(const int successfulStatusWord)
{
    mSuccessfulStatusWords.push_back(successfulStatusWord);

	return *this;
}

ApduRequestAdapter& ApduRequestAdapter::setInfo(const std::string& info)
{
    mInfo = info;

	return *this;
}

const std::vector<uint8_t>& ApduRequestAdapter::getApdu() const
{
    return mApdu;
}

const std::vector<int>& ApduRequestAdapter::getSuccessfulStatusWords() const
{
    return mSuccessfulStatusWords;
}

const std::string& ApduRequestAdapter::getInfo() const
{
    return mInfo;
}

std::ostream& operator<<(std::ostream& os, const ApduRequestAdapter& ara)
{
	os << "APDU_REQUEST_ADAPTER: {"
       << "APDU = " << ara.mApdu << ", "
	   << "INFO = " << ara.mInfo
	   << "}";

	return os;
}

std::ostream& operator<<(std::ostream& os, const std::shared_ptr<ApduRequestAdapter> ara)
{
	os << *ara.get();

	return os;
}

}
}
}
