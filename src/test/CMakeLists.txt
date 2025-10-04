#******************************************************************************
#* Copyright (c) 2025 Calypso Networks Association https://calypsonet.org/    *
#*                                                                            *
#* See the NOTICE file(s) distributed with this work for additional           *
#* information regarding copyright ownership.                                 *
#*                                                                            *
#* This program and the accompanying materials are made available under the   *
#* terms of the Eclipse Public License 2.0 which is available at              *
#* http://www.eclipse.org/legal/epl-2.0                                       *
#*                                                                            *
#* SPDX-License-Identifier: EPL-2.0                                           *
#******************************************************************************/

SET(EXECTUABLE_NAME keypleservicecpplib_ut)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_EXECUTABLE(
    ${EXECTUABLE_NAME}

    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractReaderAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ApduResponseAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AutonomousObservableLocalPluginAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BasicCardSelectorAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardSelectionManagerAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardSelectionResultAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IsoCardSelectorAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalPluginAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalPoolPluginAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalReaderAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalPluginAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalReaderAsynchronousAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalReaderBlockingAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalReaderNonBlockingAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalReaderSelectionScenarioTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SmartCardServiceAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ReaderApiFactoryAdapterTest.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/MainTest.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/util/ReaderAdapterTestUtils.cpp
)

TARGET_INCLUDE_DIRECTORIES(
    ${EXECTUABLE_NAME}

    PUBLIC

    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Include deps
SET(GOOGLETEST_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
INCLUDE("CMakeLists.txt.googletest")

TARGET_LINK_LIBRARIES(
    ${EXECTUABLE_NAME}

    PUBLIC

    gtest
    gmock
    Keyple::Service
    Keyple::Util
)
