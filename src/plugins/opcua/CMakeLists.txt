# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

# Generated from opcua.pro.

if(NOT INTEGRITY AND NOT VXWORKS AND (QT_FEATURE_open62541 OR QT_FEATURE_system_open62541))
    add_subdirectory(open62541)
endif()
if(QT_FEATURE_uacpp)
    add_subdirectory(uacpp)
endif()
