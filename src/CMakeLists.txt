# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

add_subdirectory(opcua)
add_subdirectory(plugins)
if(TARGET Qt::Quick)
    add_subdirectory(declarative_opcua)
endif()
