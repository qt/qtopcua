# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT INTEGRITY AND NOT VXWORKS AND (QT_FEATURE_open62541 OR QT_FEATURE_system_open62541))
    add_subdirectory(open62541)
endif()
if(QT_FEATURE_uacpp)
    add_subdirectory(uacpp)
endif()
