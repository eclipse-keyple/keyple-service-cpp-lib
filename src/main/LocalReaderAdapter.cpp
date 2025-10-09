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

#include "keyple/core/service/LocalReaderAdapter.hpp"

#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "keyple/core/plugin/CardIOException.hpp"
#include "keyple/core/plugin/ReaderIOException.hpp"
#include "keyple/core/plugin/spi/reader/AutonomousSelectionReaderSpi.hpp"
#include "keyple/core/plugin/spi/reader/ConfigurableReaderSpi.hpp"
#include "keyple/core/service/CardSelectionResponseAdapter.hpp"
#include "keyple/core/util/ApduUtil.hpp"
#include "keyple/core/util/HexUtil.hpp"
#include "keyple/core/util/KeypleAssert.hpp"
#include "keyple/core/util/cpp/Arrays.hpp"
#include "keyple/core/util/cpp/KeypleStd.hpp"
#include "keyple/core/util/cpp/System.hpp"
#include "keyple/core/util/cpp/exception/Exception.hpp"
#include "keyple/core/util/cpp/exception/IllegalStateException.hpp"
#include "keyple/core/util/cpp/exception/RuntimeException.hpp"
#include "keypop/card/CardBrokenCommunicationException.hpp"
#include "keypop/card/ReaderBrokenCommunicationException.hpp"
#include "keypop/card/UnexpectedStatusWordException.hpp"
#include "keypop/reader/ReaderCommunicationException.hpp"
#include "keypop/reader/ReaderProtocolNotSupportedException.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::plugin::CardIOException;
using keyple::core::plugin::ReaderIOException;
using keyple::core::plugin::spi::reader::AutonomousSelectionReaderSpi;
using keyple::core::plugin::spi::reader::ConfigurableReaderSpi;
using keyple::core::util::ApduUtil;
using keyple::core::util::Assert;
using keyple::core::util::HexUtil;
using keyple::core::util::cpp::Arrays;
using keyple::core::util::cpp::System;
using keyple::core::util::cpp::exception::Exception;
using keyple::core::util::cpp::exception::IllegalStateException;
using keyple::core::util::cpp::exception::RuntimeException;
using keypop::card::CardBrokenCommunicationException;
using keypop::card::ReaderBrokenCommunicationException;
using keypop::card::UnexpectedStatusWordException;
using keypop::reader::ReaderCommunicationException;
using keypop::reader::ReaderProtocolNotSupportedException;

/* LOCAL READER ADAPTER
 * ------------------------------------------------------------------------- */

const int LocalReaderAdapter::SW_6100 = 0x6100;

const int LocalReaderAdapter::SW_6C00 = 0x6C00;

const int LocalReaderAdapter::SW1_MASK = 0xFF00;
const int LocalReaderAdapter::SW2_MASK = 0x00FF;

LocalReaderAdapter::LocalReaderAdapter(
    std::shared_ptr<ReaderSpi> readerSpi, const std::string& pluginName)
: AbstractReaderAdapter(
      readerSpi->getName(),
      std::dynamic_pointer_cast<KeypleReaderExtension>(readerSpi),
      pluginName)
, mReaderSpi(readerSpi)
, mBefore(0)
, mIsLogicalChannelOpen(false)
, mUseDefaultProtocol(false)
, mCurrentLogicalProtocolName("")
, mCurrentPhysicalProtocolName("")
, mProtocolAssociations({})
{
}

void
LocalReaderAdapter::computeCurrentProtocol()
{
    mCurrentLogicalProtocolName = "";
    mCurrentPhysicalProtocolName = "";

    if (mProtocolAssociations.empty()) {
        mUseDefaultProtocol = true;
    } else {
        mUseDefaultProtocol = false;

        const auto configurable
            = std::dynamic_pointer_cast<ConfigurableReaderSpi>(mReaderSpi);
        for (const auto& entry : mProtocolAssociations) {
            if (configurable->isCurrentProtocol(entry.first)) {
                mCurrentLogicalProtocolName = entry.second;
                mCurrentPhysicalProtocolName = entry.first;
            }
        }
    }
}

const std::string&
LocalReaderAdapter::getCurrentPhysicalProtocolName() const
{
    return mCurrentPhysicalProtocolName;
}

void
LocalReaderAdapter::closeLogicalChannel()
{
    mLogger->trace("Reader [%] closes logical channel\n", getName());

    auto reader
        = std::dynamic_pointer_cast<AutonomousSelectionReaderSpi>(mReaderSpi);
    if (reader) {
        /* AutonomousSelectionReader have an explicit method for closing
         * channels */
        reader->closeLogicalChannel();
    }

    mIsLogicalChannelOpen = false;
    mLogger->trace("Logical channel closed\n");
}

uint8_t
LocalReaderAdapter::computeSelectApplicationP2(
    const FileOccurrence fileOccurrence,
    const FileControlInformation fileControlInformation)
{
    uint8_t p2;

    switch (fileOccurrence) {
    case FileOccurrence::FIRST:
        p2 = 0x00;
        break;
    case FileOccurrence::LAST:
        p2 = 0x01;
        break;
    case FileOccurrence::NEXT:
        p2 = 0x02;
        break;
    case FileOccurrence::PREVIOUS:
        p2 = 0x03;
        break;
    default:
        std::stringstream ss;
        ss << fileOccurrence;
        throw IllegalStateException("Unexpected value: " + ss.str());
    }

    switch (fileControlInformation) {
    case FileControlInformation::FCI:
        p2 |= 0x00;
        break;
    case FileControlInformation::FCP:
        p2 |= 0x04;
        break;
    case FileControlInformation::FMD:
        p2 |= 0x08;
        break;
    case FileControlInformation::NO_RESPONSE:
        p2 |= 0x0C;
        break;
    default:
        std::stringstream ss;
        ss << fileControlInformation;
        throw IllegalStateException("Unexpected value: " + ss.str());
    }

    return p2;
}

std::shared_ptr<ApduResponseAdapter>
LocalReaderAdapter::processExplicitAidSelection(
    std::shared_ptr<InternalIsoCardSelector> cardSelector)
{
    const std::vector<uint8_t>& aid = cardSelector->getAid();

    mLogger->debug(
        "Reader [%] selects application with AID [%]\n",
        getName(),
        HexUtil::toHex(aid));

    /*
     * Build a get response command the actual length expected by the card in
     * the get response command is handled in transmitApdu
     *
     * RL-SEL-CLA.1
     * RL-SEL-P2LC.1
     */
    std::vector<uint8_t> selectApplicationCommand(6 + aid.size());
    selectApplicationCommand[0] = 0x00; /* CLA */
    selectApplicationCommand[1] = 0xA4; /* INS */
    selectApplicationCommand[2] = 0x04; /* P1: select by name */
    /*
     * P2: b0,b1 define the File occurrence, b2,b3 define the File control
     * information we use the bitmask defined in the respective enums
     */
    selectApplicationCommand[3] = computeSelectApplicationP2(
        cardSelector->getFileOccurrence(),
        cardSelector->getFileControlInformation());
    selectApplicationCommand[4] = static_cast<uint8_t>(aid.size()); /* Lc */
    System::arraycopy(
        aid, 0, selectApplicationCommand, 5, static_cast<int>(aid.size()));
    selectApplicationCommand[5 + aid.size()] = 0x00; /* Le */

    auto apduRequest = std::shared_ptr<ApduRequest>(
        new ApduRequest(selectApplicationCommand));
    apduRequest->setInfo("Internal Select Application");

    return processApduRequest(apduRequest);
}

std::shared_ptr<ApduResponseAdapter>
LocalReaderAdapter::selectByAid(
    std::shared_ptr<InternalIsoCardSelector> cardSelector)
{
    std::shared_ptr<ApduResponseAdapter> fciResponse = nullptr;

    /*
     * RL-SEL-P2LC.1
     * RL-SEL-DFNAME.1
     */
    Assert::getInstance().isInRange(
        cardSelector->getAid().size(), 0, 16, "aid");

    auto reader
        = std::dynamic_pointer_cast<AutonomousSelectionReaderSpi>(mReaderSpi);
    if (reader) {
        const std::vector<uint8_t>& aid = cardSelector->getAid();
        const uint8_t p2 = computeSelectApplicationP2(
            cardSelector->getFileOccurrence(),
            cardSelector->getFileControlInformation());
        const std::vector<uint8_t> selectionDataBytes
            = reader->openChannelForAid(aid, p2);
        fciResponse = std::make_shared<ApduResponseAdapter>(selectionDataBytes);
    } else {
        fciResponse = processExplicitAidSelection(cardSelector);
    }

    return fciResponse;
}

bool
LocalReaderAdapter::checkPowerOnData(
    const std::string& powerOnData,
    std::shared_ptr<InternalCardSelector> cardSelector)
{
    const std::string& powerOnDataRegex = cardSelector->getPowerOnDataRegex();

    /* Check the power-on data */
    if (powerOnData != "" && powerOnDataRegex != ""
        && !std::regex_match(powerOnData, std::regex(powerOnDataRegex))) {
        mLogger->trace(
            "Power-on data didn't match (powerOnData: %, powerOnDataRegex: %\n",
            getName(),
            powerOnDataRegex);

        /* The power-on data have been rejected */
        return false;
    } else {
        /* The power-on data have been accepted */
        return true;
    }
}

std::shared_ptr<LocalReaderAdapter::SelectionStatus>
LocalReaderAdapter::processSelection(
    std::shared_ptr<CardSelectorBase> cardSelector,
    std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequest)
{
    try {
        /* RL-CLA-CHAAUTO.1 */
        std::string powerOnData = "";
        std::shared_ptr<ApduResponseAdapter> fciResponse = nullptr;
        bool hasMatched = true;

        auto internalSelector
            = std::dynamic_pointer_cast<InternalCardSelector>(cardSelector);
        if (!internalSelector) {
            throw RuntimeException(
                "cardSelector is not of type InternalCardSelector.");
        }

        const std::string& logicalProtocolName
            = internalSelector->getLogicalProtocolName();
        if (logicalProtocolName != "" && mUseDefaultProtocol) {
            throw IllegalStateException(
                "Protocol " + logicalProtocolName
                + " not associated to a reader protocol.");
        }

        /* Check protocol if enabled */
        if (logicalProtocolName == ""
            || logicalProtocolName == mCurrentLogicalProtocolName) {
            /*
             * Protocol check succeeded, check power-on data if enabled
             * RL-ATR-FILTER
             * RL-SEL-USAGE.1
             */
            powerOnData = mReaderSpi->getPowerOnData();
            if (checkPowerOnData(powerOnData, internalSelector)) {
                /* No power-on data filter or power-on data check succeeded,
                 * select by AID if enabled */
                const auto internalIsoCardSelector
                    = std::dynamic_pointer_cast<InternalIsoCardSelector>(
                        cardSelector);
                if (internalIsoCardSelector
                    && internalIsoCardSelector->getAid().size() != 0) {
                    fciResponse = selectByAid(internalIsoCardSelector);
                    const std::vector<int>& statusWords
                        = cardSelectionRequest
                              ->getSuccessfulSelectionStatusWords();
                    hasMatched = std::find(
                                     statusWords.begin(),
                                     statusWords.end(),
                                     fciResponse->getStatusWord())
                                 != statusWords.end();
                } else {
                    fciResponse = nullptr;
                }
            } else {
                /* Check failed */
                hasMatched = false;
                fciResponse = nullptr;
            }
        } else {
            /* Protocol failed */
            powerOnData = "";
            fciResponse = nullptr;
            hasMatched = false;
        }

        return std::make_shared<SelectionStatus>(
            powerOnData, fciResponse, hasMatched);

    } catch (const ReaderIOException& e) {
        throw ReaderBrokenCommunicationException(
            std::make_shared<CardResponseAdapter>(
                std::vector<std::shared_ptr<ApduResponseApi>>({}), false),
            false,
            e.getMessage(),
            std::make_shared<ReaderIOException>(e));

    } catch (const CardIOException& e) {
        throw CardBrokenCommunicationException(
            std::make_shared<CardResponseAdapter>(
                std::vector<std::shared_ptr<ApduResponseApi>>({}), false),
            false,
            e.getMessage(),
            std::make_shared<CardIOException>(e));
    }
}

std::shared_ptr<CardSelectionResponseApi>
LocalReaderAdapter::processCardSelectionRequest(
    std::shared_ptr<CardSelectorBase> cardSelector,
    std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequest,
    const ChannelControl channelControl)
{
    mIsLogicalChannelOpen = false;

    std::shared_ptr<SelectionStatus> selectionStatus(
        processSelection(cardSelector, cardSelectionRequest));
    if (!selectionStatus->mHasMatched) {
        /* The selection failed, return an empty response having the selection
         * status */
        return std::make_shared<CardSelectionResponseAdapter>(
            selectionStatus->mPowerOnData,
            selectionStatus->mSelectApplicationResponse,
            false,
            std::make_shared<CardResponseAdapter>(
                std::vector<std::shared_ptr<ApduResponseApi>>({}), false));
    }

    mIsLogicalChannelOpen = true;

    std::shared_ptr<CardResponseAdapter> cardResponse = nullptr;

    if (cardSelectionRequest->getCardRequest() != nullptr) {
        cardResponse
            = std::dynamic_pointer_cast<CardResponseAdapter>(processCardRequest(
                cardSelectionRequest->getCardRequest(), channelControl));
    } else {
        cardResponse = nullptr;
    }

    return std::make_shared<CardSelectionResponseAdapter>(
        selectionStatus->mPowerOnData,
        selectionStatus->mSelectApplicationResponse,
        true,
        cardResponse);
}

std::shared_ptr<ApduResponseAdapter>
LocalReaderAdapter::processApduRequest(
    const std::shared_ptr<ApduRequestSpi> apduRequest)
{
    std::shared_ptr<ApduResponseAdapter> apduResponse = nullptr;

    uint64_t timeStamp = System::nanoTime();
    uint64_t elapsed10ms = (timeStamp - mBefore) / 100000;
    mBefore = timeStamp;

    mLogger->debug(
        "Reader [%] --> apduRequest: %, elapsed % ms\n",
        getName(),
        apduRequest,
        elapsed10ms / 10.0);

    apduResponse = std::make_shared<ApduResponseAdapter>(
        mReaderSpi->transmitApdu(apduRequest->getApdu()));

    timeStamp = System::nanoTime();
    elapsed10ms = (timeStamp - mBefore) / 100000;
    mBefore = timeStamp;

    mLogger->debug(
        "Reader [%] <-- apduResponse: %, elapsed % ms\n",
        getName(),
        apduResponse,
        elapsed10ms / 10.0);

    if (apduResponse->getDataOut().size() == 0) {
        if ((apduResponse->getStatusWord() & SW1_MASK) == SW_6100) {
            /*
             * RL-SW-61XX.1
             * Build a GetResponse APDU command with the provided "le"
             */
            const uint8_t le = apduResponse->getStatusWord() & SW2_MASK;
            const std::vector<uint8_t> getResponseApdu
                = {0x00, 0xC0, 0x00, 0x00, le};

            /* Execute APDU */
            auto adapter = std::shared_ptr<ApduRequest>(
                new ApduRequest(getResponseApdu));
            adapter->setInfo("Internal Get Response");
            apduResponse = processApduRequest(adapter);

        } else if ((apduResponse->getStatusWord() & SW1_MASK) == SW_6C00) {
            /*
             * RL-SW-6CXX.1
             * Update the last command with the provided "le"
             */
            apduRequest->getApdu()[apduRequest->getApdu().size() - 1]
                = (apduResponse->getStatusWord() & SW2_MASK);

            /* Replay the last command APDU */
            apduResponse = processApduRequest(apduRequest);

        } else if (
            ApduUtil::isCase4(apduRequest->getApdu())
            && Arrays::contains(
                apduRequest->getSuccessfulStatusWords(),
                apduResponse->getStatusWord())) {
            /*
             * RL-SW-ANALYSIS.1
             * RL-SW-CASE4.1 (SW=6200 not taken into account here)
             * Build a GetResponse APDU command with the original "le"
             */
            const uint8_t le
                = apduRequest->getApdu()[apduRequest->getApdu().size() - 1];
            const std::vector<uint8_t> getResponseApdu
                = {0x00, 0xC0, 0x00, 0x00, le};

            /* Execute GetResponse APDU */
            auto adapter = std::shared_ptr<ApduRequest>(
                new ApduRequest(getResponseApdu));
            adapter->setInfo("Internal Get Response");
            apduResponse = processApduRequest(adapter);
        }
    }

    return apduResponse;
}

void
LocalReaderAdapter::releaseChannel()
{
    checkStatus();

    try {
        mReaderSpi->closePhysicalChannel();
    } catch (const ReaderIOException& e) {
        throw ReaderBrokenCommunicationException(
            nullptr,
            false,
            "Failed to release the physical channel",
            std::make_shared<ReaderIOException>(e));
    }
}

void
LocalReaderAdapter::deactivateReaderProtocol(const std::string& readerProtocol)
{
    /* RL-CL-PROTOCOL.1 */
    checkStatus();
    Assert::getInstance().notEmpty(readerProtocol, "readerProtocol");

    mProtocolAssociations.erase(readerProtocol);

    const auto configurable
        = std::dynamic_pointer_cast<ConfigurableReaderSpi>(mReaderSpi);
    if (!configurable || !configurable->isProtocolSupported(readerProtocol)) {
        throw ReaderProtocolNotSupportedException(readerProtocol);
    }

    configurable->deactivateProtocol(readerProtocol);
}

void
LocalReaderAdapter::activateReaderProtocol(
    const std::string& readerProtocol, const std::string& applicationProtocol)
{
    /* RL-CL-PROTOCOL.1 */
    checkStatus();
    Assert::getInstance()
        .notEmpty(readerProtocol, "readerProtocol")
        .notEmpty(applicationProtocol, "applicationProtocol");

    const auto configurable
        = std::dynamic_pointer_cast<ConfigurableReaderSpi>(mReaderSpi);
    if (!configurable || !configurable->isProtocolSupported(readerProtocol)) {
        throw ReaderProtocolNotSupportedException(readerProtocol);
    }

    configurable->activateProtocol(readerProtocol);

    mProtocolAssociations.insert({readerProtocol, applicationProtocol});
}

bool
LocalReaderAdapter::isCardPresent()
{
    /*
     * RL-DET-PCRQ.1
     * RL-DET-PCAPDU.1
     */
    checkStatus();

    try {
        return mReaderSpi->checkCardPresence();
    } catch (const ReaderIOException& e) {
        throw ReaderCommunicationException(
            "An exception occurred while checking the card presence",
            std::make_shared<ReaderIOException>(e));
    }
}

bool
LocalReaderAdapter::isContactless()
{
    return mReaderSpi->isContactless();
}

std::shared_ptr<CardResponseApi>
LocalReaderAdapter::processCardRequest(
    const std::shared_ptr<CardRequestSpi> cardRequest,
    const ChannelControl channelControl)
{
    checkStatus();

    /* Proceeds with the APDU requests present in the CardRequest */
    std::vector<std::shared_ptr<ApduResponseApi>> apduResponses;

    for (const auto& apduRequest : cardRequest->getApduRequests()) {
        try {
            const std::shared_ptr<ApduResponseAdapter> apduResponse(
                processApduRequest(apduRequest));
            apduResponses.push_back(apduResponse);
            if (cardRequest->stopOnUnsuccessfulStatusWord()
                && !Arrays::contains(
                    apduRequest->getSuccessfulStatusWords(),
                    apduResponse->getStatusWord())) {
                if (channelControl == ChannelControl::CLOSE_AFTER) {
                    closeLogicalAndPhysicalChannelsSilently();
                }
                throw UnexpectedStatusWordException(
                    std::make_shared<CardResponseAdapter>(apduResponses, false),
                    cardRequest->getApduRequests().size()
                        == apduResponses.size(),
                    "Unexpected status word");
            }

        } catch (const ReaderIOException& e) {
            closeLogicalAndPhysicalChannelsSilently();
            throw ReaderBrokenCommunicationException(
                std::make_shared<CardResponseAdapter>(apduResponses, false),
                false,
                "Reader communication failure while transmitting a card "
                "request",
                std::make_shared<ReaderIOException>(e));

        } catch (const CardIOException& e) {
            closeLogicalAndPhysicalChannelsSilently();
            throw CardBrokenCommunicationException(
                std::make_shared<CardResponseAdapter>(apduResponses, false),
                false,
                "Card communication failure while transmitting a card request",
                std::make_shared<CardIOException>(e));
        }
    }

    /* Close the channel if requested */
    if (channelControl == ChannelControl::CLOSE_AFTER) {
        releaseChannel();
    }

    return std::make_shared<CardResponseAdapter>(
        apduResponses, mIsLogicalChannelOpen);
}

std::vector<std::shared_ptr<CardSelectionResponseApi>>
LocalReaderAdapter::processCardSelectionRequests(
    const std::vector<std::shared_ptr<CardSelectorBase>>& cardSelectors,
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
        cardSelectionRequests,
    const MultiSelectionProcessing multiSelectionProcessing,
    const ChannelControl channelControl)
{
    checkStatus();

    /* Open the physical channel, determine the current protocol */
    if (!mReaderSpi->isPhysicalChannelOpen()) {
        try {
            mReaderSpi->openPhysicalChannel();
            computeCurrentProtocol();
        } catch (const ReaderIOException& e) {
            throw ReaderBrokenCommunicationException(
                nullptr,
                false,
                "Reader communication failure while opening physical channel",
                std::make_shared<ReaderIOException>(e));
        } catch (const CardIOException& e) {
            throw CardBrokenCommunicationException(
                nullptr,
                false,
                "Card communication failure while opening physical channel",
                std::make_shared<CardIOException>(e));
        }
    }

    std::vector<std::shared_ptr<CardSelectionResponseApi>>
        cardSelectionResponses;

    /* Loop over all CardRequest provided in the list */
    for (auto p
         = std::make_pair(cardSelectors.begin(), cardSelectionRequests.begin());
         p.second != cardSelectionRequests.end()
         && p.first != cardSelectors.end();
         ++p.first, ++p.second) {
        /* Process the CardRequest and append the CardResponse list */
        const auto cardSelectionResponse
            = processCardSelectionRequest(*p.first, *p.second, channelControl);
        cardSelectionResponses.push_back(cardSelectionResponse);

        if (multiSelectionProcessing == MultiSelectionProcessing::PROCESS_ALL) {
            /*
             * Multi CardRequest case: just close the logical channel and go on
             * with the next selection.
             */
            closeLogicalChannel();
        } else {
            if (mIsLogicalChannelOpen) {
                /* The logical channel being open, we stop here */
                break; /* Exit for loop */
            }
        }
    }

    /* Close the channel if requested */
    if (channelControl == ChannelControl::CLOSE_AFTER) {
        releaseChannel();
    }

    return cardSelectionResponses;
}

void
LocalReaderAdapter::doUnregister()
{
    try {
        mReaderSpi->closePhysicalChannel();
    } catch (const Exception& e) {
        mLogger->error(
            "Error closing physical channel on reader [%] - %\n", getName(), e);
    }

    try {
        mReaderSpi->onUnregister();
    } catch (const Exception& e) {
        mLogger->error(
            "Error unregistering reader extension of reader [%]: % - %\n",
            getName(),
            e.getMessage(),
            e);
    }

    AbstractReaderAdapter::doUnregister();
}

void
LocalReaderAdapter::closeLogicalAndPhysicalChannelsSilently()
{
    closeLogicalChannel();

    /* Closes the physical channel and resets the current protocol info */
    mCurrentLogicalProtocolName = "";
    mUseDefaultProtocol = false;

    try {
        mReaderSpi->closePhysicalChannel();
    } catch (const ReaderIOException& e) {
        mLogger->error(
            "Error closing physical channel on reader [%]: % - %\n",
            getName(),
            e.getMessage(),
            e);
    }
}

bool
LocalReaderAdapter::isLogicalChannelOpen() const
{
    return mIsLogicalChannelOpen;
}

std::shared_ptr<ReaderSpi>
LocalReaderAdapter::getReaderSpi() const
{
    return mReaderSpi;
}

/* SELECTION STATUS
 * -----------------------------------------------------------------------------
 */

LocalReaderAdapter::SelectionStatus::SelectionStatus(
    const std::string& powerOnData,
    const std::shared_ptr<ApduResponseAdapter> selectApplicationResponse,
    const bool hasMatched)
: mPowerOnData(powerOnData)
, mSelectApplicationResponse(selectApplicationResponse)
, mHasMatched(hasMatched)
{
}

/* APDU REQUEST
 * ---------------------------------------------------------------------------------
 */

const int LocalReaderAdapter::ApduRequest::DEFAULT_SUCCESSFUL_CODE = 0x9000;

LocalReaderAdapter::ApduRequest::ApduRequest(const std::vector<uint8_t>& apdu)
: mApdu(apdu)
, mSuccessfulStatusWords({DEFAULT_SUCCESSFUL_CODE})
{
}

LocalReaderAdapter::ApduRequest&
LocalReaderAdapter::ApduRequest::setInfo(const std::string& info)
{
    mInfo = info;

    return *this;
}

std::vector<uint8_t>
LocalReaderAdapter::ApduRequest::getApdu() const
{
    return mApdu;
}

const std::vector<int>&
LocalReaderAdapter::ApduRequest::getSuccessfulStatusWords() const
{
    return mSuccessfulStatusWords;
}

const std::string&
LocalReaderAdapter::ApduRequest::getInfo() const
{
    return mInfo;
}

std::ostream&
operator<<(std::ostream& os, LocalReaderAdapter::ApduRequest& ar)
{
    os << "APDU_REQUEST: {"
       << "APDU: " << ar.getApdu() << ", "
       << "SUCCESSFUL_STATUS_WORDS: " << ar.getSuccessfulStatusWords() << ", "
       << "INFO: " << ar.getInfo() << "}";

    return os;
}

std::ostream&
operator<<(
    std::ostream& os, const std::shared_ptr<LocalReaderAdapter::ApduRequest> ar)
{
    if (ar == nullptr) {
        os << "APDU_REQUEST: null";
    } else {
        os << *ar;
    }

    return os;
}

std::ostream&
operator<<(
    std::ostream& os,
    const std::vector<std::shared_ptr<LocalReaderAdapter::ApduRequest>>& ars)
{
    os << "APDU_REQUESTS: {";

    for (auto it = ars.begin(); it != ars.end(); it++) {
        if (it != ars.begin()) {
            os << ", ";
        }
        os << *it;
    }

    os << "}";

    return os;
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
