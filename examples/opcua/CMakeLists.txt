# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(TARGET Qt::Widgets)
    qt_internal_add_example(opcuaviewer)
endif()
if(QT_FEATURE_gds AND QT_FEATURE_ssl AND NOT APPLE AND NOT WINRT)
    qt_internal_add_example(x509)
endif()
if(QT_FEATURE_open62541 AND TARGET Qt::Quick)
    add_subdirectory(waterpump)
endif()
