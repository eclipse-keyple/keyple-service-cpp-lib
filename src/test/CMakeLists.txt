# *************************************************************************************************
# Copyright (c) 2021 Calypso Networks Association                                                 *
# https://www.calypsonet-asso.org/                                                                *
#                                                                                                 *
# See the NOTICE file(s) distributed with this work for additional information regarding          *
# copyright ownership.                                                                            *
#                                                                                                 *
# This program and the accompanying materials are made available under the terms of the Eclipse   *
# Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                   *
#                                                                                                 *
# SPDX-License-Identifier: EPL-2.0                                                                *
# *************************************************************************************************/

SET(EXECTUABLE_NAME keypleservicecpplib_ut)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../main
    ${CMAKE_CURRENT_SOURCE_DIR}/../test/mock
    ${CMAKE_CURRENT_SOURCE_DIR}/../test/util
)

ADD_EXECUTABLE(
    ${EXECTUABLE_NAME}

    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractReaderAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AutonomousObservableLocalPluginAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardSelectionResultAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalPluginAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalPoolPluginAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalReaderAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalPluginAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalReaderAutonomousAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalReaderAutonomousAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalReaderBlockingAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalReaderNonBlockingAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalReaderSelectionScenarioTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SmartCardServiceAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MainTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/util/ReaderAdapterTestUtils.cpp
)

# Add Google Test
SET(GOOGLETEST_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
INCLUDE(CMakeLists.txt.googletest)

TARGET_LINK_LIBRARIES(${EXECTUABLE_NAME} gtest gmock Keyple::Service)
