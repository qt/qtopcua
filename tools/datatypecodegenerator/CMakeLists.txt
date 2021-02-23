# Copyright (C) 2023 basysKom GmbH
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## qopcuaxmldatatypes2cpp Tool:
#####################################################################

qt_get_tool_target_name(target_name qopcuaxmldatatypes2cpp)
qt_internal_add_tool(${target_name}
    TOOLS_TARGET
        QtOpcUa
    SOURCES
        main.cpp
        util.cpp util.h
        datatypefilewriter.cpp datatypefilewriter.h
        dependencydatatypevalidator.cpp dependencydatatypevalidator.h
        enumeratedtype.cpp enumeratedtype.h
        enumeratedvalue.cpp enumeratedvalue.h
        field.cpp field.h
        import.cpp import.h
        mappingfilegenerator.cpp mappingfilegenerator.h
        recursivedescentparser.cpp recursivedescentparser.h
        stringidentifier.cpp stringidentifier.h
        structuredtype.cpp structuredtype.h
        typedictionary.cpp typedictionary.h
        visitor.h
        xmlelement.cpp xmlelement.h
)
qt_internal_return_unless_building_tools()
