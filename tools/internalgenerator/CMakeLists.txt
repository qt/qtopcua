# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## qtopcua-defaultnodeidsgenerator Tool:
#####################################################################

qt_get_tool_target_name(target_name qtopcua-internalgenerator)
qt_internal_add_tool(${target_name}
    SOURCES
        main.cpp
)
qt_internal_return_unless_building_tools()
