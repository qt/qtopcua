# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(QT_FEATURE_ssl AND NOT APPLE AND NOT WINRT)
    add_subdirectory(gds)
endif()
