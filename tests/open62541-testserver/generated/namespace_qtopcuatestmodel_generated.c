/* WARNING: This is a generated file.
 * Any manual changes will be overwritten. */

#include "namespace_qtopcuatestmodel_generated.h"


/* QtTestUnionType - ns=1;i=3005 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_0_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestUnionType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,
UA_NODEID_NUMERIC(ns[1], 3005LU),
UA_NODEID_NUMERIC(ns[0], 12756LU),
UA_NODEID_NUMERIC(ns[0], 45LU),
UA_QUALIFIEDNAME(ns[1], "QtTestUnionType"),
 UA_NODEID_NULL,
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_0_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 3005LU)
);
}

/* Default XML - ns=1;i=5008 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_1_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default XML");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,
UA_NODEID_NUMERIC(ns[1], 5008LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_QUALIFIEDNAME(ns[0], "Default XML"),
UA_NODEID_NUMERIC(ns[0], 76LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 5008LU), UA_NODEID_NUMERIC(ns[0], 38LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 3005LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_1_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 5008LU)
);
}

/* Default Binary - ns=1;i=5007 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_2_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default Binary");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,
UA_NODEID_NUMERIC(ns[1], 5007LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_QUALIFIEDNAME(ns[0], "Default Binary"),
UA_NODEID_NUMERIC(ns[0], 76LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 5007LU), UA_NODEID_NUMERIC(ns[0], 38LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 3005LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_2_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 5007LU)
);
}

/* QtAbstractBaseStruct - ns=1;i=3008 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_3_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.isAbstract = true;
attr.displayName = UA_LOCALIZEDTEXT("", "QtAbstractBaseStruct");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,
UA_NODEID_NUMERIC(ns[1], 3008LU),
UA_NODEID_NUMERIC(ns[0], 22LU),
UA_NODEID_NUMERIC(ns[0], 45LU),
UA_QUALIFIEDNAME(ns[1], "QtAbstractBaseStruct"),
 UA_NODEID_NULL,
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_3_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 3008LU)
);
}

/* Default XML - ns=1;i=5012 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_4_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default XML");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,
UA_NODEID_NUMERIC(ns[1], 5012LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_QUALIFIEDNAME(ns[0], "Default XML"),
UA_NODEID_NUMERIC(ns[0], 76LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 5012LU), UA_NODEID_NUMERIC(ns[0], 38LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 3008LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_4_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 5012LU)
);
}

/* Default Binary - ns=1;i=5006 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_5_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default Binary");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,
UA_NODEID_NUMERIC(ns[1], 5006LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_QUALIFIEDNAME(ns[0], "Default Binary"),
UA_NODEID_NUMERIC(ns[0], 76LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 5006LU), UA_NODEID_NUMERIC(ns[0], 38LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 3008LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_5_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 5006LU)
);
}

/* QtTestStructWithDataValue - ns=1;i=3010 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_6_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructWithDataValue");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,
UA_NODEID_NUMERIC(ns[1], 3010LU),
UA_NODEID_NUMERIC(ns[1], 3008LU),
UA_NODEID_NUMERIC(ns[0], 45LU),
UA_QUALIFIEDNAME(ns[1], "QtTestStructWithDataValue"),
 UA_NODEID_NULL,
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_6_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 3010LU)
);
}

/* Default XML - ns=1;i=5015 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_7_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default XML");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,
UA_NODEID_NUMERIC(ns[1], 5015LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_QUALIFIEDNAME(ns[0], "Default XML"),
UA_NODEID_NUMERIC(ns[0], 76LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 5015LU), UA_NODEID_NUMERIC(ns[0], 38LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 3010LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_7_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 5015LU)
);
}

/* Default Binary - ns=1;i=5014 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_8_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default Binary");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,
UA_NODEID_NUMERIC(ns[1], 5014LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_QUALIFIEDNAME(ns[0], "Default Binary"),
UA_NODEID_NUMERIC(ns[0], 76LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 5014LU), UA_NODEID_NUMERIC(ns[0], 38LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 3010LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_8_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 5014LU)
);
}

/* QtTestStructWithDiagnosticInfo - ns=1;i=3009 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_9_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructWithDiagnosticInfo");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,
UA_NODEID_NUMERIC(ns[1], 3009LU),
UA_NODEID_NUMERIC(ns[1], 3008LU),
UA_NODEID_NUMERIC(ns[0], 45LU),
UA_QUALIFIEDNAME(ns[1], "QtTestStructWithDiagnosticInfo"),
 UA_NODEID_NULL,
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_9_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 3009LU)
);
}

/* Default XML - ns=1;i=5013 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_10_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default XML");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,
UA_NODEID_NUMERIC(ns[1], 5013LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_QUALIFIEDNAME(ns[0], "Default XML"),
UA_NODEID_NUMERIC(ns[0], 76LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 5013LU), UA_NODEID_NUMERIC(ns[0], 38LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 3009LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_10_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 5013LU)
);
}

/* Default Binary - ns=1;i=5003 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_11_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default Binary");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,
UA_NODEID_NUMERIC(ns[1], 5003LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_QUALIFIEDNAME(ns[0], "Default Binary"),
UA_NODEID_NUMERIC(ns[0], 76LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 5003LU), UA_NODEID_NUMERIC(ns[0], 38LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 3009LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_11_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 5003LU)
);
}

/* QtStructWithOptionalFieldType - ns=1;i=3006 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_12_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "QtStructWithOptionalFieldType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,
UA_NODEID_NUMERIC(ns[1], 3006LU),
UA_NODEID_NUMERIC(ns[1], 3008LU),
UA_NODEID_NUMERIC(ns[0], 45LU),
UA_QUALIFIEDNAME(ns[1], "QtStructWithOptionalFieldType"),
 UA_NODEID_NULL,
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_12_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 3006LU)
);
}

/* Default XML - ns=1;i=5011 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_13_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default XML");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,
UA_NODEID_NUMERIC(ns[1], 5011LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_QUALIFIEDNAME(ns[0], "Default XML"),
UA_NODEID_NUMERIC(ns[0], 76LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 5011LU), UA_NODEID_NUMERIC(ns[0], 38LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 3006LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_13_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 5011LU)
);
}

/* Default Binary - ns=1;i=5010 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_14_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default Binary");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,
UA_NODEID_NUMERIC(ns[1], 5010LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_QUALIFIEDNAME(ns[0], "Default Binary"),
UA_NODEID_NUMERIC(ns[0], 76LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 5010LU), UA_NODEID_NUMERIC(ns[0], 38LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 3006LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_14_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 5010LU)
);
}

/* QtInnerTestStructType - ns=1;i=3004 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_15_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "QtInnerTestStructType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,
UA_NODEID_NUMERIC(ns[1], 3004LU),
UA_NODEID_NUMERIC(ns[1], 3008LU),
UA_NODEID_NUMERIC(ns[0], 45LU),
UA_QUALIFIEDNAME(ns[1], "QtInnerTestStructType"),
 UA_NODEID_NULL,
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_15_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 3004LU)
);
}

/* Default XML - ns=1;i=5005 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_16_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default XML");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,
UA_NODEID_NUMERIC(ns[1], 5005LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_QUALIFIEDNAME(ns[0], "Default XML"),
UA_NODEID_NUMERIC(ns[0], 76LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 5005LU), UA_NODEID_NUMERIC(ns[0], 38LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 3004LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_16_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 5005LU)
);
}

/* Default Binary - ns=1;i=5004 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_17_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default Binary");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,
UA_NODEID_NUMERIC(ns[1], 5004LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_QUALIFIEDNAME(ns[0], "Default Binary"),
UA_NODEID_NUMERIC(ns[0], 76LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 5004LU), UA_NODEID_NUMERIC(ns[0], 38LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 3004LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_17_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 5004LU)
);
}

/* QtTestStructType - ns=1;i=3003 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_18_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,
UA_NODEID_NUMERIC(ns[1], 3003LU),
UA_NODEID_NUMERIC(ns[1], 3008LU),
UA_NODEID_NUMERIC(ns[0], 45LU),
UA_QUALIFIEDNAME(ns[1], "QtTestStructType"),
 UA_NODEID_NULL,
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_18_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 3003LU)
);
}

/* Default XML - ns=1;i=5002 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_19_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default XML");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,
UA_NODEID_NUMERIC(ns[1], 5002LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_QUALIFIEDNAME(ns[0], "Default XML"),
UA_NODEID_NUMERIC(ns[0], 76LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 5002LU), UA_NODEID_NUMERIC(ns[0], 38LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 3003LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_19_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 5002LU)
);
}

/* Default Binary - ns=1;i=5001 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_20_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default Binary");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,
UA_NODEID_NUMERIC(ns[1], 5001LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_NODEID_NUMERIC(ns[0], 0LU),
UA_QUALIFIEDNAME(ns[0], "Default Binary"),
UA_NODEID_NUMERIC(ns[0], 76LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 5001LU), UA_NODEID_NUMERIC(ns[0], 38LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 3003LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_20_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 5001LU)
);
}

/* MyDoubleSubtype - ns=1;i=3007 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_21_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "MyDoubleSubtype");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,
UA_NODEID_NUMERIC(ns[1], 3007LU),
UA_NODEID_NUMERIC(ns[0], 11LU),
UA_NODEID_NUMERIC(ns[0], 45LU),
UA_QUALIFIEDNAME(ns[1], "MyDoubleSubtype"),
 UA_NODEID_NULL,
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_21_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 3007LU)
);
}

/* QtTestEnumeration - ns=1;i=3002 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_22_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestEnumeration");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,
UA_NODEID_NUMERIC(ns[1], 3002LU),
UA_NODEID_NUMERIC(ns[0], 29LU),
UA_NODEID_NUMERIC(ns[0], 45LU),
UA_QUALIFIEDNAME(ns[1], "QtTestEnumeration"),
 UA_NODEID_NULL,
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_22_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 3002LU)
);
}

/* EnumStrings - ns=1;i=6001 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_23_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
attr.valueRank = 1;
attr.arrayDimensionsSize = 1;
UA_UInt32 arrayDimensions[1];
arrayDimensions[0] = 4;
attr.arrayDimensions = &arrayDimensions[0];
attr.dataType = UA_NODEID_NUMERIC(ns[0], 21LU);
UA_LocalizedText variablenode_ns_1_i_6001_variant_DataContents[4];
variablenode_ns_1_i_6001_variant_DataContents[0] = UA_LOCALIZEDTEXT("", "Unknown");
variablenode_ns_1_i_6001_variant_DataContents[1] = UA_LOCALIZEDTEXT("", "FirstOption");
variablenode_ns_1_i_6001_variant_DataContents[2] = UA_LOCALIZEDTEXT("", "SecondOption");
variablenode_ns_1_i_6001_variant_DataContents[3] = UA_LOCALIZEDTEXT("", "ThirdOption");
UA_Variant_setArray(&attr.value, &variablenode_ns_1_i_6001_variant_DataContents, (UA_Int32) 4, &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
attr.displayName = UA_LOCALIZEDTEXT("", "EnumStrings");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6001LU),
UA_NODEID_NUMERIC(ns[1], 3002LU),
UA_NODEID_NUMERIC(ns[0], 46LU),
UA_QUALIFIEDNAME(ns[0], "EnumStrings"),
UA_NODEID_NUMERIC(ns[0], 68LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6001LU), UA_NODEID_NUMERIC(ns[0], 37LU), UA_EXPANDEDNODEID_NUMERIC(ns[0], 78LU), true);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_23_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6001LU)
);
}

/* DecoderTestNodes - ns=1;i=5009 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_24_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "DecoderTestNodes");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,
UA_NODEID_NUMERIC(ns[1], 5009LU),
UA_NODEID_NUMERIC(ns[0], 85LU),
UA_NODEID_NUMERIC(ns[0], 35LU),
UA_QUALIFIEDNAME(ns[1], "DecoderTestNodes"),
UA_NODEID_NUMERIC(ns[0], 61LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_24_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 5009LU)
);
}

/* StructWithDataValue - ns=1;i=6027 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_25_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 3;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[1], 3010LU);
attr.displayName = UA_LOCALIZEDTEXT("", "StructWithDataValue");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6027LU),
UA_NODEID_NUMERIC(ns[1], 5009LU),
UA_NODEID_NUMERIC(ns[0], 35LU),
UA_QUALIFIEDNAME(ns[1], "StructWithDataValue"),
UA_NODEID_NUMERIC(ns[0], 63LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_25_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6027LU)
);
}

/* UnionWithFirstMember - ns=1;i=6011 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_26_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 3;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[1], 3005LU);
attr.displayName = UA_LOCALIZEDTEXT("", "UnionWithFirstMember");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6011LU),
UA_NODEID_NUMERIC(ns[1], 5009LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "UnionWithFirstMember"),
UA_NODEID_NUMERIC(ns[0], 63LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_26_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6011LU)
);
}

/* StructWithOptionalField - ns=1;i=6010 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_27_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 3;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[1], 3006LU);
attr.displayName = UA_LOCALIZEDTEXT("", "StructWithOptionalField");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6010LU),
UA_NODEID_NUMERIC(ns[1], 5009LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "StructWithOptionalField"),
UA_NODEID_NUMERIC(ns[0], 63LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_27_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6010LU)
);
}

/* NestedStruct - ns=1;i=6009 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_28_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 3;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[1], 3003LU);
attr.displayName = UA_LOCALIZEDTEXT("", "NestedStruct");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6009LU),
UA_NODEID_NUMERIC(ns[1], 5009LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "NestedStruct"),
UA_NODEID_NUMERIC(ns[0], 63LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_28_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6009LU)
);
}

/* StructWithDiagnosticInfo - ns=1;i=6006 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_29_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 3;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[1], 3009LU);
attr.displayName = UA_LOCALIZEDTEXT("", "StructWithDiagnosticInfo");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6006LU),
UA_NODEID_NUMERIC(ns[1], 5009LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "StructWithDiagnosticInfo"),
UA_NODEID_NUMERIC(ns[0], 63LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_29_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6006LU)
);
}

/* UnionWithSecondMember - ns=1;i=6003 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_30_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 3;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[1], 3005LU);
attr.displayName = UA_LOCALIZEDTEXT("", "UnionWithSecondMember");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6003LU),
UA_NODEID_NUMERIC(ns[1], 5009LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "UnionWithSecondMember"),
UA_NODEID_NUMERIC(ns[0], 63LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_30_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6003LU)
);
}

/* StructWithoutOptionalField - ns=1;i=6002 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_31_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 3;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[1], 3006LU);
attr.displayName = UA_LOCALIZEDTEXT("", "StructWithoutOptionalField");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6002LU),
UA_NODEID_NUMERIC(ns[1], 5009LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "StructWithoutOptionalField"),
UA_NODEID_NUMERIC(ns[0], 63LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_31_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6002LU)
);
}

/* TypeDictionary - ns=1;i=6014 */
static const UA_Byte variablenode_ns_1_i_6014_variant_DataContents_byteArray[6932] = {60, 120, 115, 58, 115, 99, 104, 101, 109, 97, 32, 101, 108, 101, 109, 101, 110, 116, 70, 111, 114, 109, 68, 101, 102, 97, 117, 108, 116, 61, 34, 113, 117, 97, 108, 105, 102, 105, 101, 100, 34, 32, 116, 97, 114, 103, 101, 116, 78, 97, 109, 101, 115, 112, 97, 99, 101, 61, 34, 104, 116, 116, 112, 58, 47, 47, 113, 116, 45, 112, 114, 111, 106, 101, 99, 116, 46, 111, 114, 103, 47, 81, 116, 79, 112, 99, 85, 97, 84, 101, 115, 116, 77, 111, 100, 101, 108, 47, 84, 121, 112, 101, 115, 46, 120, 115, 100, 34, 32, 120, 109, 108, 110, 115, 58, 116, 110, 115, 61, 34, 104, 116, 116, 112, 58, 47, 47, 113, 116, 45, 112, 114, 111, 106, 101, 99, 116, 46, 111, 114, 103, 47, 81, 116, 79, 112, 99, 85, 97, 84, 101, 115, 116, 77, 111, 100, 101, 108, 47, 84, 121, 112, 101, 115, 46, 120, 115, 100, 34, 32, 120, 109, 108, 110, 115, 58, 117, 97, 61, 34, 104, 116, 116, 112, 58, 47, 47, 111, 112, 99, 102, 111, 117, 110, 100, 97, 116, 105, 111, 110, 46, 111, 114, 103, 47, 85, 65, 47, 50, 48, 48, 56, 47, 48, 50, 47, 84, 121, 112, 101, 115, 46, 120, 115, 100, 34, 32, 120, 109, 108, 110, 115, 58, 120, 115, 61, 34, 104, 116, 116, 112, 58, 47, 47, 119, 119, 119, 46, 119, 51, 46, 111, 114, 103, 47, 50, 48, 48, 49, 47, 88, 77, 76, 83, 99, 104, 101, 109, 97, 34, 62, 10, 32, 60, 120, 115, 58, 105, 109, 112, 111, 114, 116, 32, 110, 97, 109, 101, 115, 112, 97, 99, 101, 61, 34, 104, 116, 116, 112, 58, 47, 47, 111, 112, 99, 102, 111, 117, 110, 100, 97, 116, 105, 111, 110, 46, 111, 114, 103, 47, 85, 65, 47, 50, 48, 48, 56, 47, 48, 50, 47, 84, 121, 112, 101, 115, 46, 120, 115, 100, 34, 47, 62, 10, 32, 60, 120, 115, 58, 115, 105, 109, 112, 108, 101, 84, 121, 112, 101, 32, 110, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 69, 110, 117, 109, 101, 114, 97, 116, 105, 111, 110, 34, 62, 10, 32, 32, 60, 120, 115, 58, 114, 101, 115, 116, 114, 105, 99, 116, 105, 111, 110, 32, 98, 97, 115, 101, 61, 34, 120, 115, 58, 115, 116, 114, 105, 110, 103, 34, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 110, 117, 109, 101, 114, 97, 116, 105, 111, 110, 32, 118, 97, 108, 117, 101, 61, 34, 85, 110, 107, 110, 111, 119, 110, 95, 48, 34, 47, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 110, 117, 109, 101, 114, 97, 116, 105, 111, 110, 32, 118, 97, 108, 117, 101, 61, 34, 70, 105, 114, 115, 116, 79, 112, 116, 105, 111, 110, 95, 49, 34, 47, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 110, 117, 109, 101, 114, 97, 116, 105, 111, 110, 32, 118, 97, 108, 117, 101, 61, 34, 83, 101, 99, 111, 110, 100, 79, 112, 116, 105, 111, 110, 95, 50, 34, 47, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 110, 117, 109, 101, 114, 97, 116, 105, 111, 110, 32, 118, 97, 108, 117, 101, 61, 34, 84, 104, 105, 114, 100, 79, 112, 116, 105, 111, 110, 95, 51, 34, 47, 62, 10, 32, 32, 60, 47, 120, 115, 58, 114, 101, 115, 116, 114, 105, 99, 116, 105, 111, 110, 62, 10, 32, 60, 47, 120, 115, 58, 115, 105, 109, 112, 108, 101, 84, 121, 112, 101, 62, 10, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 84, 101, 115, 116, 69, 110, 117, 109, 101, 114, 97, 116, 105, 111, 110, 34, 32, 110, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 69, 110, 117, 109, 101, 114, 97, 116, 105, 111, 110, 34, 47, 62, 10, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 32, 110, 97, 109, 101, 61, 34, 76, 105, 115, 116, 79, 102, 81, 116, 84, 101, 115, 116, 69, 110, 117, 109, 101, 114, 97, 116, 105, 111, 110, 34, 62, 10, 32, 32, 60, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 117, 110, 98, 111, 117, 110, 100, 101, 100, 34, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 84, 101, 115, 116, 69, 110, 117, 109, 101, 114, 97, 116, 105, 111, 110, 34, 32, 110, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 69, 110, 117, 109, 101, 114, 97, 116, 105, 111, 110, 34, 32, 110, 105, 108, 108, 97, 98, 108, 101, 61, 34, 116, 114, 117, 101, 34, 47, 62, 10, 32, 32, 60, 47, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 62, 10, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 76, 105, 115, 116, 79, 102, 81, 116, 84, 101, 115, 116, 69, 110, 117, 109, 101, 114, 97, 116, 105, 111, 110, 34, 32, 110, 97, 109, 101, 61, 34, 76, 105, 115, 116, 79, 102, 81, 116, 84, 101, 115, 116, 69, 110, 117, 109, 101, 114, 97, 116, 105, 111, 110, 34, 32, 110, 105, 108, 108, 97, 98, 108, 101, 61, 34, 116, 114, 117, 101, 34, 47, 62, 10, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 32, 110, 97, 109, 101, 61, 34, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 62, 10, 32, 32, 60, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 47, 62, 10, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 62, 10, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 32, 110, 97, 109, 101, 61, 34, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 47, 62, 10, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 32, 110, 97, 109, 101, 61, 34, 76, 105, 115, 116, 79, 102, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 62, 10, 32, 32, 60, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 117, 110, 98, 111, 117, 110, 100, 101, 100, 34, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 32, 110, 97, 109, 101, 61, 34, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 32, 110, 105, 108, 108, 97, 98, 108, 101, 61, 34, 116, 114, 117, 101, 34, 47, 62, 10, 32, 32, 60, 47, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 62, 10, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 76, 105, 115, 116, 79, 102, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 32, 110, 97, 109, 101, 61, 34, 76, 105, 115, 116, 79, 102, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 32, 110, 105, 108, 108, 97, 98, 108, 101, 61, 34, 116, 114, 117, 101, 34, 47, 62, 10, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 32, 110, 97, 109, 101, 61, 34, 81, 116, 73, 110, 110, 101, 114, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 62, 10, 32, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 67, 111, 110, 116, 101, 110, 116, 32, 109, 105, 120, 101, 100, 61, 34, 102, 97, 108, 115, 101, 34, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 120, 116, 101, 110, 115, 105, 111, 110, 32, 98, 97, 115, 101, 61, 34, 116, 110, 115, 58, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 62, 10, 32, 32, 32, 32, 60, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 120, 115, 58, 100, 111, 117, 98, 108, 101, 34, 32, 110, 97, 109, 101, 61, 34, 68, 111, 117, 98, 108, 101, 83, 117, 98, 116, 121, 112, 101, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 32, 32, 60, 47, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 60, 47, 120, 115, 58, 101, 120, 116, 101, 110, 115, 105, 111, 110, 62, 10, 32, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 67, 111, 110, 116, 101, 110, 116, 62, 10, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 62, 10, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 73, 110, 110, 101, 114, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 32, 110, 97, 109, 101, 61, 34, 81, 116, 73, 110, 110, 101, 114, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 47, 62, 10, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 32, 110, 97, 109, 101, 61, 34, 76, 105, 115, 116, 79, 102, 81, 116, 73, 110, 110, 101, 114, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 62, 10, 32, 32, 60, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 117, 110, 98, 111, 117, 110, 100, 101, 100, 34, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 73, 110, 110, 101, 114, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 32, 110, 97, 109, 101, 61, 34, 81, 116, 73, 110, 110, 101, 114, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 32, 110, 105, 108, 108, 97, 98, 108, 101, 61, 34, 116, 114, 117, 101, 34, 47, 62, 10, 32, 32, 60, 47, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 62, 10, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 76, 105, 115, 116, 79, 102, 81, 116, 73, 110, 110, 101, 114, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 32, 110, 97, 109, 101, 61, 34, 76, 105, 115, 116, 79, 102, 81, 116, 73, 110, 110, 101, 114, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 32, 110, 105, 108, 108, 97, 98, 108, 101, 61, 34, 116, 114, 117, 101, 34, 47, 62, 10, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 32, 110, 97, 109, 101, 61, 34, 81, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 79, 112, 116, 105, 111, 110, 97, 108, 70, 105, 101, 108, 100, 84, 121, 112, 101, 34, 62, 10, 32, 32, 60, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 116, 121, 112, 101, 61, 34, 120, 115, 58, 117, 110, 115, 105, 103, 110, 101, 100, 73, 110, 116, 34, 32, 110, 97, 109, 101, 61, 34, 69, 110, 99, 111, 100, 105, 110, 103, 77, 97, 115, 107, 34, 47, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 120, 115, 58, 100, 111, 117, 98, 108, 101, 34, 32, 110, 97, 109, 101, 61, 34, 77, 97, 110, 100, 97, 116, 111, 114, 121, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 120, 115, 58, 100, 111, 117, 98, 108, 101, 34, 32, 110, 97, 109, 101, 61, 34, 79, 112, 116, 105, 111, 110, 97, 108, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 60, 47, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 62, 10, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 79, 112, 116, 105, 111, 110, 97, 108, 70, 105, 101, 108, 100, 84, 121, 112, 101, 34, 32, 110, 97, 109, 101, 61, 34, 81, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 79, 112, 116, 105, 111, 110, 97, 108, 70, 105, 101, 108, 100, 84, 121, 112, 101, 34, 47, 62, 10, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 32, 110, 97, 109, 101, 61, 34, 76, 105, 115, 116, 79, 102, 81, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 79, 112, 116, 105, 111, 110, 97, 108, 70, 105, 101, 108, 100, 84, 121, 112, 101, 34, 62, 10, 32, 32, 60, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 117, 110, 98, 111, 117, 110, 100, 101, 100, 34, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 79, 112, 116, 105, 111, 110, 97, 108, 70, 105, 101, 108, 100, 84, 121, 112, 101, 34, 32, 110, 97, 109, 101, 61, 34, 81, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 79, 112, 116, 105, 111, 110, 97, 108, 70, 105, 101, 108, 100, 84, 121, 112, 101, 34, 32, 110, 105, 108, 108, 97, 98, 108, 101, 61, 34, 116, 114, 117, 101, 34, 47, 62, 10, 32, 32, 60, 47, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 62, 10, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 76, 105, 115, 116, 79, 102, 81, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 79, 112, 116, 105, 111, 110, 97, 108, 70, 105, 101, 108, 100, 84, 121, 112, 101, 34, 32, 110, 97, 109, 101, 61, 34, 76, 105, 115, 116, 79, 102, 81, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 79, 112, 116, 105, 111, 110, 97, 108, 70, 105, 101, 108, 100, 84, 121, 112, 101, 34, 32, 110, 105, 108, 108, 97, 98, 108, 101, 61, 34, 116, 114, 117, 101, 34, 47, 62, 10, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 32, 110, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 62, 10, 32, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 67, 111, 110, 116, 101, 110, 116, 32, 109, 105, 120, 101, 100, 61, 34, 102, 97, 108, 115, 101, 34, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 120, 116, 101, 110, 115, 105, 111, 110, 32, 98, 97, 115, 101, 61, 34, 116, 110, 115, 58, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 62, 10, 32, 32, 32, 32, 60, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 120, 115, 58, 115, 116, 114, 105, 110, 103, 34, 32, 110, 97, 109, 101, 61, 34, 83, 116, 114, 105, 110, 103, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 117, 97, 58, 76, 111, 99, 97, 108, 105, 122, 101, 100, 84, 101, 120, 116, 34, 32, 110, 97, 109, 101, 61, 34, 76, 111, 99, 97, 108, 105, 122, 101, 100, 84, 101, 120, 116, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 117, 97, 58, 81, 117, 97, 108, 105, 102, 105, 101, 100, 78, 97, 109, 101, 34, 32, 110, 97, 109, 101, 61, 34, 81, 117, 97, 108, 105, 102, 105, 101, 100, 78, 97, 109, 101, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 117, 97, 58, 76, 105, 115, 116, 79, 102, 73, 110, 116, 54, 52, 34, 32, 110, 97, 109, 101, 61, 34, 73, 110, 116, 54, 52, 65, 114, 114, 97, 121, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 84, 101, 115, 116, 69, 110, 117, 109, 101, 114, 97, 116, 105, 111, 110, 34, 32, 110, 97, 109, 101, 61, 34, 69, 110, 117, 109, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 73, 110, 110, 101, 114, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 32, 110, 97, 109, 101, 61, 34, 78, 101, 115, 116, 101, 100, 83, 116, 114, 117, 99, 116, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 76, 105, 115, 116, 79, 102, 81, 116, 73, 110, 110, 101, 114, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 32, 110, 97, 109, 101, 61, 34, 78, 101, 115, 116, 101, 100, 83, 116, 114, 117, 99, 116, 65, 114, 114, 97, 121, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 32, 32, 60, 47, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 60, 47, 120, 115, 58, 101, 120, 116, 101, 110, 115, 105, 111, 110, 62, 10, 32, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 67, 111, 110, 116, 101, 110, 116, 62, 10, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 62, 10, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 32, 110, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 47, 62, 10, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 32, 110, 97, 109, 101, 61, 34, 76, 105, 115, 116, 79, 102, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 62, 10, 32, 32, 60, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 117, 110, 98, 111, 117, 110, 100, 101, 100, 34, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 32, 110, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 32, 110, 105, 108, 108, 97, 98, 108, 101, 61, 34, 116, 114, 117, 101, 34, 47, 62, 10, 32, 32, 60, 47, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 62, 10, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 76, 105, 115, 116, 79, 102, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 32, 110, 97, 109, 101, 61, 34, 76, 105, 115, 116, 79, 102, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 32, 110, 105, 108, 108, 97, 98, 108, 101, 61, 34, 116, 114, 117, 101, 34, 47, 62, 10, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 32, 110, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 97, 116, 97, 86, 97, 108, 117, 101, 34, 62, 10, 32, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 67, 111, 110, 116, 101, 110, 116, 32, 109, 105, 120, 101, 100, 61, 34, 102, 97, 108, 115, 101, 34, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 120, 116, 101, 110, 115, 105, 111, 110, 32, 98, 97, 115, 101, 61, 34, 116, 110, 115, 58, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 62, 10, 32, 32, 32, 32, 60, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 117, 97, 58, 68, 97, 116, 97, 86, 97, 108, 117, 101, 34, 32, 110, 97, 109, 101, 61, 34, 68, 97, 116, 97, 86, 97, 108, 117, 101, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 117, 97, 58, 86, 97, 114, 105, 97, 110, 116, 34, 32, 110, 97, 109, 101, 61, 34, 86, 97, 114, 105, 97, 110, 116, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 32, 32, 60, 47, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 60, 47, 120, 115, 58, 101, 120, 116, 101, 110, 115, 105, 111, 110, 62, 10, 32, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 67, 111, 110, 116, 101, 110, 116, 62, 10, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 62, 10, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 97, 116, 97, 86, 97, 108, 117, 101, 34, 32, 110, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 97, 116, 97, 86, 97, 108, 117, 101, 34, 47, 62, 10, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 32, 110, 97, 109, 101, 61, 34, 76, 105, 115, 116, 79, 102, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 97, 116, 97, 86, 97, 108, 117, 101, 34, 62, 10, 32, 32, 60, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 117, 110, 98, 111, 117, 110, 100, 101, 100, 34, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 97, 116, 97, 86, 97, 108, 117, 101, 34, 32, 110, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 97, 116, 97, 86, 97, 108, 117, 101, 34, 32, 110, 105, 108, 108, 97, 98, 108, 101, 61, 34, 116, 114, 117, 101, 34, 47, 62, 10, 32, 32, 60, 47, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 62, 10, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 76, 105, 115, 116, 79, 102, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 97, 116, 97, 86, 97, 108, 117, 101, 34, 32, 110, 97, 109, 101, 61, 34, 76, 105, 115, 116, 79, 102, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 97, 116, 97, 86, 97, 108, 117, 101, 34, 32, 110, 105, 108, 108, 97, 98, 108, 101, 61, 34, 116, 114, 117, 101, 34, 47, 62, 10, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 32, 110, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 34, 62, 10, 32, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 67, 111, 110, 116, 101, 110, 116, 32, 109, 105, 120, 101, 100, 61, 34, 102, 97, 108, 115, 101, 34, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 120, 116, 101, 110, 115, 105, 111, 110, 32, 98, 97, 115, 101, 61, 34, 116, 110, 115, 58, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 62, 10, 32, 32, 32, 32, 60, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 117, 97, 58, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 34, 32, 110, 97, 109, 101, 61, 34, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 117, 97, 58, 76, 105, 115, 116, 79, 102, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 34, 32, 110, 97, 109, 101, 61, 34, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 65, 114, 114, 97, 121, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 32, 32, 60, 47, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 60, 47, 120, 115, 58, 101, 120, 116, 101, 110, 115, 105, 111, 110, 62, 10, 32, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 67, 111, 110, 116, 101, 110, 116, 62, 10, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 62, 10, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 34, 32, 110, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 34, 47, 62, 10, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 32, 110, 97, 109, 101, 61, 34, 76, 105, 115, 116, 79, 102, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 34, 62, 10, 32, 32, 60, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 117, 110, 98, 111, 117, 110, 100, 101, 100, 34, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 34, 32, 110, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 34, 32, 110, 105, 108, 108, 97, 98, 108, 101, 61, 34, 116, 114, 117, 101, 34, 47, 62, 10, 32, 32, 60, 47, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 62, 10, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 76, 105, 115, 116, 79, 102, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 34, 32, 110, 97, 109, 101, 61, 34, 76, 105, 115, 116, 79, 102, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 34, 32, 110, 105, 108, 108, 97, 98, 108, 101, 61, 34, 116, 114, 117, 101, 34, 47, 62, 10, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 32, 110, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 85, 110, 105, 111, 110, 84, 121, 112, 101, 34, 62, 10, 32, 32, 60, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 120, 115, 58, 117, 110, 115, 105, 103, 110, 101, 100, 73, 110, 116, 34, 32, 110, 97, 109, 101, 61, 34, 83, 119, 105, 116, 99, 104, 70, 105, 101, 108, 100, 34, 47, 62, 10, 32, 32, 32, 60, 120, 115, 58, 99, 104, 111, 105, 99, 101, 62, 10, 32, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 120, 115, 58, 108, 111, 110, 103, 34, 32, 110, 97, 109, 101, 61, 34, 77, 101, 109, 98, 101, 114, 49, 34, 47, 62, 10, 32, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 49, 34, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 73, 110, 110, 101, 114, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 32, 110, 97, 109, 101, 61, 34, 77, 101, 109, 98, 101, 114, 50, 34, 47, 62, 10, 32, 32, 32, 60, 47, 120, 115, 58, 99, 104, 111, 105, 99, 101, 62, 10, 32, 32, 60, 47, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 62, 10, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 84, 101, 115, 116, 85, 110, 105, 111, 110, 84, 121, 112, 101, 34, 32, 110, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 85, 110, 105, 111, 110, 84, 121, 112, 101, 34, 47, 62, 10, 32, 60, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 32, 110, 97, 109, 101, 61, 34, 76, 105, 115, 116, 79, 102, 81, 116, 84, 101, 115, 116, 85, 110, 105, 111, 110, 84, 121, 112, 101, 34, 62, 10, 32, 32, 60, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 32, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 109, 105, 110, 79, 99, 99, 117, 114, 115, 61, 34, 48, 34, 32, 109, 97, 120, 79, 99, 99, 117, 114, 115, 61, 34, 117, 110, 98, 111, 117, 110, 100, 101, 100, 34, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 84, 101, 115, 116, 85, 110, 105, 111, 110, 84, 121, 112, 101, 34, 32, 110, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 85, 110, 105, 111, 110, 84, 121, 112, 101, 34, 32, 110, 105, 108, 108, 97, 98, 108, 101, 61, 34, 116, 114, 117, 101, 34, 47, 62, 10, 32, 32, 60, 47, 120, 115, 58, 115, 101, 113, 117, 101, 110, 99, 101, 62, 10, 32, 60, 47, 120, 115, 58, 99, 111, 109, 112, 108, 101, 120, 84, 121, 112, 101, 62, 10, 32, 60, 120, 115, 58, 101, 108, 101, 109, 101, 110, 116, 32, 116, 121, 112, 101, 61, 34, 116, 110, 115, 58, 76, 105, 115, 116, 79, 102, 81, 116, 84, 101, 115, 116, 85, 110, 105, 111, 110, 84, 121, 112, 101, 34, 32, 110, 97, 109, 101, 61, 34, 76, 105, 115, 116, 79, 102, 81, 116, 84, 101, 115, 116, 85, 110, 105, 111, 110, 84, 121, 112, 101, 34, 32, 110, 105, 108, 108, 97, 98, 108, 101, 61, 34, 116, 114, 117, 101, 34, 47, 62, 10, 60, 47, 120, 115, 58, 115, 99, 104, 101, 109, 97, 62, 10};



static UA_StatusCode function_namespace_qtopcuatestmodel_generated_32_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 15LU);
UA_ByteString *variablenode_ns_1_i_6014_variant_DataContents =  UA_ByteString_new();
if (!variablenode_ns_1_i_6014_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_ByteString_init(variablenode_ns_1_i_6014_variant_DataContents);
variablenode_ns_1_i_6014_variant_DataContents->length = 6932;
variablenode_ns_1_i_6014_variant_DataContents->data = (UA_Byte *)(void*)(uintptr_t)variablenode_ns_1_i_6014_variant_DataContents_byteArray;
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6014_variant_DataContents, &UA_TYPES[UA_TYPES_BYTESTRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "TypeDictionary");
#ifdef UA_ENABLE_NODESET_COMPILER_DESCRIPTIONS
attr.description = UA_LOCALIZEDTEXT("", "Collects the data type descriptions of http://qt-project.org/QtOpcUaTestModel/");
#endif
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6014LU),
UA_NODEID_NUMERIC(ns[0], 92LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "TypeDictionary"),
UA_NODEID_NUMERIC(ns[0], 72LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
variablenode_ns_1_i_6014_variant_DataContents->data = NULL;
variablenode_ns_1_i_6014_variant_DataContents->length = 0;
UA_ByteString_delete(variablenode_ns_1_i_6014_variant_DataContents);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_32_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6014LU)
);
}

/* QtAbstractBaseStruct - ns=1;i=6025 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_33_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 12LU);
UA_String *variablenode_ns_1_i_6025_variant_DataContents =  UA_String_new();
if (!variablenode_ns_1_i_6025_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_String_init(variablenode_ns_1_i_6025_variant_DataContents);
*variablenode_ns_1_i_6025_variant_DataContents = UA_STRING_ALLOC("//xs:element[@name='QtAbstractBaseStruct']");
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6025_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "QtAbstractBaseStruct");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6025LU),
UA_NODEID_NUMERIC(ns[1], 6014LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "QtAbstractBaseStruct"),
UA_NODEID_NUMERIC(ns[0], 69LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
UA_String_delete(variablenode_ns_1_i_6025_variant_DataContents);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6025LU), UA_NODEID_NUMERIC(ns[0], 39LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 5012LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_33_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6025LU)
);
}

/* QtTestUnionType - ns=1;i=6023 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_34_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 12LU);
UA_String *variablenode_ns_1_i_6023_variant_DataContents =  UA_String_new();
if (!variablenode_ns_1_i_6023_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_String_init(variablenode_ns_1_i_6023_variant_DataContents);
*variablenode_ns_1_i_6023_variant_DataContents = UA_STRING_ALLOC("//xs:element[@name='QtTestUnionType']");
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6023_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestUnionType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6023LU),
UA_NODEID_NUMERIC(ns[1], 6014LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "QtTestUnionType"),
UA_NODEID_NUMERIC(ns[0], 69LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
UA_String_delete(variablenode_ns_1_i_6023_variant_DataContents);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6023LU), UA_NODEID_NUMERIC(ns[0], 39LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 5008LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_34_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6023LU)
);
}

/* QtTestStructType - ns=1;i=6021 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_35_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 12LU);
UA_String *variablenode_ns_1_i_6021_variant_DataContents =  UA_String_new();
if (!variablenode_ns_1_i_6021_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_String_init(variablenode_ns_1_i_6021_variant_DataContents);
*variablenode_ns_1_i_6021_variant_DataContents = UA_STRING_ALLOC("//xs:element[@name='QtTestStructType']");
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6021_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6021LU),
UA_NODEID_NUMERIC(ns[1], 6014LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "QtTestStructType"),
UA_NODEID_NUMERIC(ns[0], 69LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
UA_String_delete(variablenode_ns_1_i_6021_variant_DataContents);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6021LU), UA_NODEID_NUMERIC(ns[0], 39LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 5002LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_35_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6021LU)
);
}

/* QtStructWithOptionalFieldType - ns=1;i=6019 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_36_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 12LU);
UA_String *variablenode_ns_1_i_6019_variant_DataContents =  UA_String_new();
if (!variablenode_ns_1_i_6019_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_String_init(variablenode_ns_1_i_6019_variant_DataContents);
*variablenode_ns_1_i_6019_variant_DataContents = UA_STRING_ALLOC("//xs:element[@name='QtStructWithOptionalFieldType']");
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6019_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "QtStructWithOptionalFieldType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6019LU),
UA_NODEID_NUMERIC(ns[1], 6014LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "QtStructWithOptionalFieldType"),
UA_NODEID_NUMERIC(ns[0], 69LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
UA_String_delete(variablenode_ns_1_i_6019_variant_DataContents);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6019LU), UA_NODEID_NUMERIC(ns[0], 39LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 5011LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_36_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6019LU)
);
}

/* QtInnerTestStructType - ns=1;i=6017 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_37_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 12LU);
UA_String *variablenode_ns_1_i_6017_variant_DataContents =  UA_String_new();
if (!variablenode_ns_1_i_6017_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_String_init(variablenode_ns_1_i_6017_variant_DataContents);
*variablenode_ns_1_i_6017_variant_DataContents = UA_STRING_ALLOC("//xs:element[@name='QtInnerTestStructType']");
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6017_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "QtInnerTestStructType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6017LU),
UA_NODEID_NUMERIC(ns[1], 6014LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "QtInnerTestStructType"),
UA_NODEID_NUMERIC(ns[0], 69LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
UA_String_delete(variablenode_ns_1_i_6017_variant_DataContents);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6017LU), UA_NODEID_NUMERIC(ns[0], 39LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 5005LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_37_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6017LU)
);
}

/* NamespaceUri - ns=1;i=6015 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_38_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 12LU);
UA_String *variablenode_ns_1_i_6015_variant_DataContents =  UA_String_new();
if (!variablenode_ns_1_i_6015_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_String_init(variablenode_ns_1_i_6015_variant_DataContents);
*variablenode_ns_1_i_6015_variant_DataContents = UA_STRING_ALLOC("http://qt-project.org/QtOpcUaTestModel/Types.xsd");
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6015_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "NamespaceUri");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6015LU),
UA_NODEID_NUMERIC(ns[1], 6014LU),
UA_NODEID_NUMERIC(ns[0], 46LU),
UA_QUALIFIEDNAME(ns[0], "NamespaceUri"),
UA_NODEID_NUMERIC(ns[0], 68LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
UA_String_delete(variablenode_ns_1_i_6015_variant_DataContents);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_38_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6015LU)
);
}

/* QtTestStructWithDataValue - ns=1;i=6008 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_39_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 12LU);
UA_String *variablenode_ns_1_i_6008_variant_DataContents =  UA_String_new();
if (!variablenode_ns_1_i_6008_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_String_init(variablenode_ns_1_i_6008_variant_DataContents);
*variablenode_ns_1_i_6008_variant_DataContents = UA_STRING_ALLOC("//xs:element[@name='QtTestStructWithDataValue']");
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6008_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructWithDataValue");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6008LU),
UA_NODEID_NUMERIC(ns[1], 6014LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "QtTestStructWithDataValue"),
UA_NODEID_NUMERIC(ns[0], 69LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
UA_String_delete(variablenode_ns_1_i_6008_variant_DataContents);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6008LU), UA_NODEID_NUMERIC(ns[0], 39LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 5015LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_39_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6008LU)
);
}

/* QtTestStructWithDiagnosticInfo - ns=1;i=6005 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_40_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 12LU);
UA_String *variablenode_ns_1_i_6005_variant_DataContents =  UA_String_new();
if (!variablenode_ns_1_i_6005_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_String_init(variablenode_ns_1_i_6005_variant_DataContents);
*variablenode_ns_1_i_6005_variant_DataContents = UA_STRING_ALLOC("//xs:element[@name='QtTestStructWithDiagnosticInfo']");
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6005_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructWithDiagnosticInfo");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6005LU),
UA_NODEID_NUMERIC(ns[1], 6014LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "QtTestStructWithDiagnosticInfo"),
UA_NODEID_NUMERIC(ns[0], 69LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
UA_String_delete(variablenode_ns_1_i_6005_variant_DataContents);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6005LU), UA_NODEID_NUMERIC(ns[0], 39LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 5013LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_40_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6005LU)
);
}

/* TypeDictionary - ns=1;i=6012 */
static const UA_Byte variablenode_ns_1_i_6012_variant_DataContents_byteArray[3114] = {60, 111, 112, 99, 58, 84, 121, 112, 101, 68, 105, 99, 116, 105, 111, 110, 97, 114, 121, 32, 120, 109, 108, 110, 115, 58, 120, 115, 105, 61, 34, 104, 116, 116, 112, 58, 47, 47, 119, 119, 119, 46, 119, 51, 46, 111, 114, 103, 47, 50, 48, 48, 49, 47, 88, 77, 76, 83, 99, 104, 101, 109, 97, 45, 105, 110, 115, 116, 97, 110, 99, 101, 34, 32, 120, 109, 108, 110, 115, 58, 116, 110, 115, 61, 34, 104, 116, 116, 112, 58, 47, 47, 113, 116, 45, 112, 114, 111, 106, 101, 99, 116, 46, 111, 114, 103, 47, 81, 116, 79, 112, 99, 85, 97, 84, 101, 115, 116, 77, 111, 100, 101, 108, 47, 34, 32, 68, 101, 102, 97, 117, 108, 116, 66, 121, 116, 101, 79, 114, 100, 101, 114, 61, 34, 76, 105, 116, 116, 108, 101, 69, 110, 100, 105, 97, 110, 34, 32, 120, 109, 108, 110, 115, 58, 111, 112, 99, 61, 34, 104, 116, 116, 112, 58, 47, 47, 111, 112, 99, 102, 111, 117, 110, 100, 97, 116, 105, 111, 110, 46, 111, 114, 103, 47, 66, 105, 110, 97, 114, 121, 83, 99, 104, 101, 109, 97, 47, 34, 32, 120, 109, 108, 110, 115, 58, 117, 97, 61, 34, 104, 116, 116, 112, 58, 47, 47, 111, 112, 99, 102, 111, 117, 110, 100, 97, 116, 105, 111, 110, 46, 111, 114, 103, 47, 85, 65, 47, 34, 32, 84, 97, 114, 103, 101, 116, 78, 97, 109, 101, 115, 112, 97, 99, 101, 61, 34, 104, 116, 116, 112, 58, 47, 47, 113, 116, 45, 112, 114, 111, 106, 101, 99, 116, 46, 111, 114, 103, 47, 81, 116, 79, 112, 99, 85, 97, 84, 101, 115, 116, 77, 111, 100, 101, 108, 47, 34, 62, 10, 32, 60, 111, 112, 99, 58, 73, 109, 112, 111, 114, 116, 32, 78, 97, 109, 101, 115, 112, 97, 99, 101, 61, 34, 104, 116, 116, 112, 58, 47, 47, 111, 112, 99, 102, 111, 117, 110, 100, 97, 116, 105, 111, 110, 46, 111, 114, 103, 47, 85, 65, 47, 34, 47, 62, 10, 32, 60, 111, 112, 99, 58, 83, 116, 114, 117, 99, 116, 117, 114, 101, 100, 84, 121, 112, 101, 32, 66, 97, 115, 101, 84, 121, 112, 101, 61, 34, 117, 97, 58, 69, 120, 116, 101, 110, 115, 105, 111, 110, 79, 98, 106, 101, 99, 116, 34, 32, 78, 97, 109, 101, 61, 34, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 47, 62, 10, 32, 60, 111, 112, 99, 58, 83, 116, 114, 117, 99, 116, 117, 114, 101, 100, 84, 121, 112, 101, 32, 66, 97, 115, 101, 84, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 32, 78, 97, 109, 101, 61, 34, 81, 116, 73, 110, 110, 101, 114, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 111, 112, 99, 58, 68, 111, 117, 98, 108, 101, 34, 32, 78, 97, 109, 101, 61, 34, 68, 111, 117, 98, 108, 101, 83, 117, 98, 116, 121, 112, 101, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 60, 47, 111, 112, 99, 58, 83, 116, 114, 117, 99, 116, 117, 114, 101, 100, 84, 121, 112, 101, 62, 10, 32, 60, 111, 112, 99, 58, 83, 116, 114, 117, 99, 116, 117, 114, 101, 100, 84, 121, 112, 101, 32, 66, 97, 115, 101, 84, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 32, 78, 97, 109, 101, 61, 34, 81, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 79, 112, 116, 105, 111, 110, 97, 108, 70, 105, 101, 108, 100, 84, 121, 112, 101, 34, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 111, 112, 99, 58, 66, 105, 116, 34, 32, 78, 97, 109, 101, 61, 34, 79, 112, 116, 105, 111, 110, 97, 108, 77, 101, 109, 98, 101, 114, 83, 112, 101, 99, 105, 102, 105, 101, 100, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 76, 101, 110, 103, 116, 104, 61, 34, 51, 49, 34, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 111, 112, 99, 58, 66, 105, 116, 34, 32, 78, 97, 109, 101, 61, 34, 82, 101, 115, 101, 114, 118, 101, 100, 49, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 111, 112, 99, 58, 68, 111, 117, 98, 108, 101, 34, 32, 78, 97, 109, 101, 61, 34, 77, 97, 110, 100, 97, 116, 111, 114, 121, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 83, 119, 105, 116, 99, 104, 70, 105, 101, 108, 100, 61, 34, 79, 112, 116, 105, 111, 110, 97, 108, 77, 101, 109, 98, 101, 114, 83, 112, 101, 99, 105, 102, 105, 101, 100, 34, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 111, 112, 99, 58, 68, 111, 117, 98, 108, 101, 34, 32, 78, 97, 109, 101, 61, 34, 79, 112, 116, 105, 111, 110, 97, 108, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 60, 47, 111, 112, 99, 58, 83, 116, 114, 117, 99, 116, 117, 114, 101, 100, 84, 121, 112, 101, 62, 10, 32, 60, 111, 112, 99, 58, 83, 116, 114, 117, 99, 116, 117, 114, 101, 100, 84, 121, 112, 101, 32, 66, 97, 115, 101, 84, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 32, 78, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 111, 112, 99, 58, 67, 104, 97, 114, 65, 114, 114, 97, 121, 34, 32, 78, 97, 109, 101, 61, 34, 83, 116, 114, 105, 110, 103, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 117, 97, 58, 76, 111, 99, 97, 108, 105, 122, 101, 100, 84, 101, 120, 116, 34, 32, 78, 97, 109, 101, 61, 34, 76, 111, 99, 97, 108, 105, 122, 101, 100, 84, 101, 120, 116, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 117, 97, 58, 81, 117, 97, 108, 105, 102, 105, 101, 100, 78, 97, 109, 101, 34, 32, 78, 97, 109, 101, 61, 34, 81, 117, 97, 108, 105, 102, 105, 101, 100, 78, 97, 109, 101, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 111, 112, 99, 58, 73, 110, 116, 51, 50, 34, 32, 78, 97, 109, 101, 61, 34, 78, 111, 79, 102, 73, 110, 116, 54, 52, 65, 114, 114, 97, 121, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 76, 101, 110, 103, 116, 104, 70, 105, 101, 108, 100, 61, 34, 78, 111, 79, 102, 73, 110, 116, 54, 52, 65, 114, 114, 97, 121, 77, 101, 109, 98, 101, 114, 34, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 111, 112, 99, 58, 73, 110, 116, 54, 52, 34, 32, 78, 97, 109, 101, 61, 34, 73, 110, 116, 54, 52, 65, 114, 114, 97, 121, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 116, 110, 115, 58, 81, 116, 84, 101, 115, 116, 69, 110, 117, 109, 101, 114, 97, 116, 105, 111, 110, 34, 32, 78, 97, 109, 101, 61, 34, 69, 110, 117, 109, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 116, 110, 115, 58, 81, 116, 73, 110, 110, 101, 114, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 32, 78, 97, 109, 101, 61, 34, 78, 101, 115, 116, 101, 100, 83, 116, 114, 117, 99, 116, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 111, 112, 99, 58, 73, 110, 116, 51, 50, 34, 32, 78, 97, 109, 101, 61, 34, 78, 111, 79, 102, 78, 101, 115, 116, 101, 100, 83, 116, 114, 117, 99, 116, 65, 114, 114, 97, 121, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 76, 101, 110, 103, 116, 104, 70, 105, 101, 108, 100, 61, 34, 78, 111, 79, 102, 78, 101, 115, 116, 101, 100, 83, 116, 114, 117, 99, 116, 65, 114, 114, 97, 121, 77, 101, 109, 98, 101, 114, 34, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 116, 110, 115, 58, 81, 116, 73, 110, 110, 101, 114, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 32, 78, 97, 109, 101, 61, 34, 78, 101, 115, 116, 101, 100, 83, 116, 114, 117, 99, 116, 65, 114, 114, 97, 121, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 60, 47, 111, 112, 99, 58, 83, 116, 114, 117, 99, 116, 117, 114, 101, 100, 84, 121, 112, 101, 62, 10, 32, 60, 111, 112, 99, 58, 83, 116, 114, 117, 99, 116, 117, 114, 101, 100, 84, 121, 112, 101, 32, 66, 97, 115, 101, 84, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 32, 78, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 97, 116, 97, 86, 97, 108, 117, 101, 34, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 117, 97, 58, 68, 97, 116, 97, 86, 97, 108, 117, 101, 34, 32, 78, 97, 109, 101, 61, 34, 68, 97, 116, 97, 86, 97, 108, 117, 101, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 117, 97, 58, 86, 97, 114, 105, 97, 110, 116, 34, 32, 78, 97, 109, 101, 61, 34, 86, 97, 114, 105, 97, 110, 116, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 60, 47, 111, 112, 99, 58, 83, 116, 114, 117, 99, 116, 117, 114, 101, 100, 84, 121, 112, 101, 62, 10, 32, 60, 111, 112, 99, 58, 83, 116, 114, 117, 99, 116, 117, 114, 101, 100, 84, 121, 112, 101, 32, 66, 97, 115, 101, 84, 121, 112, 101, 61, 34, 116, 110, 115, 58, 81, 116, 65, 98, 115, 116, 114, 97, 99, 116, 66, 97, 115, 101, 83, 116, 114, 117, 99, 116, 34, 32, 78, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 87, 105, 116, 104, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 34, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 117, 97, 58, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 34, 32, 78, 97, 109, 101, 61, 34, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 111, 112, 99, 58, 73, 110, 116, 51, 50, 34, 32, 78, 97, 109, 101, 61, 34, 78, 111, 79, 102, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 65, 114, 114, 97, 121, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 76, 101, 110, 103, 116, 104, 70, 105, 101, 108, 100, 61, 34, 78, 111, 79, 102, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 65, 114, 114, 97, 121, 77, 101, 109, 98, 101, 114, 34, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 117, 97, 58, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 34, 32, 78, 97, 109, 101, 61, 34, 68, 105, 97, 103, 110, 111, 115, 116, 105, 99, 73, 110, 102, 111, 65, 114, 114, 97, 121, 77, 101, 109, 98, 101, 114, 34, 47, 62, 10, 32, 60, 47, 111, 112, 99, 58, 83, 116, 114, 117, 99, 116, 117, 114, 101, 100, 84, 121, 112, 101, 62, 10, 32, 60, 111, 112, 99, 58, 83, 116, 114, 117, 99, 116, 117, 114, 101, 100, 84, 121, 112, 101, 32, 66, 97, 115, 101, 84, 121, 112, 101, 61, 34, 117, 97, 58, 85, 110, 105, 111, 110, 34, 32, 78, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 85, 110, 105, 111, 110, 84, 121, 112, 101, 34, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 111, 112, 99, 58, 85, 73, 110, 116, 51, 50, 34, 32, 78, 97, 109, 101, 61, 34, 83, 119, 105, 116, 99, 104, 70, 105, 101, 108, 100, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 83, 119, 105, 116, 99, 104, 70, 105, 101, 108, 100, 61, 34, 83, 119, 105, 116, 99, 104, 70, 105, 101, 108, 100, 34, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 111, 112, 99, 58, 73, 110, 116, 54, 52, 34, 32, 83, 119, 105, 116, 99, 104, 86, 97, 108, 117, 101, 61, 34, 49, 34, 32, 78, 97, 109, 101, 61, 34, 77, 101, 109, 98, 101, 114, 49, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 70, 105, 101, 108, 100, 32, 83, 119, 105, 116, 99, 104, 70, 105, 101, 108, 100, 61, 34, 83, 119, 105, 116, 99, 104, 70, 105, 101, 108, 100, 34, 32, 84, 121, 112, 101, 78, 97, 109, 101, 61, 34, 116, 110, 115, 58, 81, 116, 73, 110, 110, 101, 114, 84, 101, 115, 116, 83, 116, 114, 117, 99, 116, 84, 121, 112, 101, 34, 32, 83, 119, 105, 116, 99, 104, 86, 97, 108, 117, 101, 61, 34, 50, 34, 32, 78, 97, 109, 101, 61, 34, 77, 101, 109, 98, 101, 114, 50, 34, 47, 62, 10, 32, 60, 47, 111, 112, 99, 58, 83, 116, 114, 117, 99, 116, 117, 114, 101, 100, 84, 121, 112, 101, 62, 10, 32, 60, 111, 112, 99, 58, 69, 110, 117, 109, 101, 114, 97, 116, 101, 100, 84, 121, 112, 101, 32, 76, 101, 110, 103, 116, 104, 73, 110, 66, 105, 116, 115, 61, 34, 51, 50, 34, 32, 78, 97, 109, 101, 61, 34, 81, 116, 84, 101, 115, 116, 69, 110, 117, 109, 101, 114, 97, 116, 105, 111, 110, 34, 62, 10, 32, 32, 60, 111, 112, 99, 58, 69, 110, 117, 109, 101, 114, 97, 116, 101, 100, 86, 97, 108, 117, 101, 32, 78, 97, 109, 101, 61, 34, 85, 110, 107, 110, 111, 119, 110, 34, 32, 86, 97, 108, 117, 101, 61, 34, 48, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 69, 110, 117, 109, 101, 114, 97, 116, 101, 100, 86, 97, 108, 117, 101, 32, 78, 97, 109, 101, 61, 34, 70, 105, 114, 115, 116, 79, 112, 116, 105, 111, 110, 34, 32, 86, 97, 108, 117, 101, 61, 34, 49, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 69, 110, 117, 109, 101, 114, 97, 116, 101, 100, 86, 97, 108, 117, 101, 32, 78, 97, 109, 101, 61, 34, 83, 101, 99, 111, 110, 100, 79, 112, 116, 105, 111, 110, 34, 32, 86, 97, 108, 117, 101, 61, 34, 50, 34, 47, 62, 10, 32, 32, 60, 111, 112, 99, 58, 69, 110, 117, 109, 101, 114, 97, 116, 101, 100, 86, 97, 108, 117, 101, 32, 78, 97, 109, 101, 61, 34, 84, 104, 105, 114, 100, 79, 112, 116, 105, 111, 110, 34, 32, 86, 97, 108, 117, 101, 61, 34, 51, 34, 47, 62, 10, 32, 60, 47, 111, 112, 99, 58, 69, 110, 117, 109, 101, 114, 97, 116, 101, 100, 84, 121, 112, 101, 62, 10, 60, 47, 111, 112, 99, 58, 84, 121, 112, 101, 68, 105, 99, 116, 105, 111, 110, 97, 114, 121, 62, 10};



static UA_StatusCode function_namespace_qtopcuatestmodel_generated_41_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 15LU);
UA_ByteString *variablenode_ns_1_i_6012_variant_DataContents =  UA_ByteString_new();
if (!variablenode_ns_1_i_6012_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_ByteString_init(variablenode_ns_1_i_6012_variant_DataContents);
variablenode_ns_1_i_6012_variant_DataContents->length = 3114;
variablenode_ns_1_i_6012_variant_DataContents->data = (UA_Byte *)(void*)(uintptr_t)variablenode_ns_1_i_6012_variant_DataContents_byteArray;
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6012_variant_DataContents, &UA_TYPES[UA_TYPES_BYTESTRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "TypeDictionary");
#ifdef UA_ENABLE_NODESET_COMPILER_DESCRIPTIONS
attr.description = UA_LOCALIZEDTEXT("", "Collects the data type descriptions of http://qt-project.org/QtOpcUaTestModel/");
#endif
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6012LU),
UA_NODEID_NUMERIC(ns[0], 93LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "TypeDictionary"),
UA_NODEID_NUMERIC(ns[0], 72LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
variablenode_ns_1_i_6012_variant_DataContents->data = NULL;
variablenode_ns_1_i_6012_variant_DataContents->length = 0;
UA_ByteString_delete(variablenode_ns_1_i_6012_variant_DataContents);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_41_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6012LU)
);
}

/* QtAbstractBaseStruct - ns=1;i=6024 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_42_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 12LU);
UA_String *variablenode_ns_1_i_6024_variant_DataContents =  UA_String_new();
if (!variablenode_ns_1_i_6024_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_String_init(variablenode_ns_1_i_6024_variant_DataContents);
*variablenode_ns_1_i_6024_variant_DataContents = UA_STRING_ALLOC("QtAbstractBaseStruct");
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6024_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "QtAbstractBaseStruct");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6024LU),
UA_NODEID_NUMERIC(ns[1], 6012LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "QtAbstractBaseStruct"),
UA_NODEID_NUMERIC(ns[0], 69LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
UA_String_delete(variablenode_ns_1_i_6024_variant_DataContents);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6024LU), UA_NODEID_NUMERIC(ns[0], 39LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 5006LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_42_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6024LU)
);
}

/* QtTestUnionType - ns=1;i=6022 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_43_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 12LU);
UA_String *variablenode_ns_1_i_6022_variant_DataContents =  UA_String_new();
if (!variablenode_ns_1_i_6022_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_String_init(variablenode_ns_1_i_6022_variant_DataContents);
*variablenode_ns_1_i_6022_variant_DataContents = UA_STRING_ALLOC("QtTestUnionType");
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6022_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestUnionType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6022LU),
UA_NODEID_NUMERIC(ns[1], 6012LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "QtTestUnionType"),
UA_NODEID_NUMERIC(ns[0], 69LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
UA_String_delete(variablenode_ns_1_i_6022_variant_DataContents);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6022LU), UA_NODEID_NUMERIC(ns[0], 39LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 5007LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_43_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6022LU)
);
}

/* QtTestStructType - ns=1;i=6020 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_44_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 12LU);
UA_String *variablenode_ns_1_i_6020_variant_DataContents =  UA_String_new();
if (!variablenode_ns_1_i_6020_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_String_init(variablenode_ns_1_i_6020_variant_DataContents);
*variablenode_ns_1_i_6020_variant_DataContents = UA_STRING_ALLOC("QtTestStructType");
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6020_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6020LU),
UA_NODEID_NUMERIC(ns[1], 6012LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "QtTestStructType"),
UA_NODEID_NUMERIC(ns[0], 69LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
UA_String_delete(variablenode_ns_1_i_6020_variant_DataContents);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6020LU), UA_NODEID_NUMERIC(ns[0], 39LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 5001LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_44_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6020LU)
);
}

/* QtStructWithOptionalFieldType - ns=1;i=6018 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_45_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 12LU);
UA_String *variablenode_ns_1_i_6018_variant_DataContents =  UA_String_new();
if (!variablenode_ns_1_i_6018_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_String_init(variablenode_ns_1_i_6018_variant_DataContents);
*variablenode_ns_1_i_6018_variant_DataContents = UA_STRING_ALLOC("QtStructWithOptionalFieldType");
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6018_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "QtStructWithOptionalFieldType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6018LU),
UA_NODEID_NUMERIC(ns[1], 6012LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "QtStructWithOptionalFieldType"),
UA_NODEID_NUMERIC(ns[0], 69LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
UA_String_delete(variablenode_ns_1_i_6018_variant_DataContents);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6018LU), UA_NODEID_NUMERIC(ns[0], 39LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 5010LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_45_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6018LU)
);
}

/* QtInnerTestStructType - ns=1;i=6016 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_46_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 12LU);
UA_String *variablenode_ns_1_i_6016_variant_DataContents =  UA_String_new();
if (!variablenode_ns_1_i_6016_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_String_init(variablenode_ns_1_i_6016_variant_DataContents);
*variablenode_ns_1_i_6016_variant_DataContents = UA_STRING_ALLOC("QtInnerTestStructType");
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6016_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "QtInnerTestStructType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6016LU),
UA_NODEID_NUMERIC(ns[1], 6012LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "QtInnerTestStructType"),
UA_NODEID_NUMERIC(ns[0], 69LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
UA_String_delete(variablenode_ns_1_i_6016_variant_DataContents);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6016LU), UA_NODEID_NUMERIC(ns[0], 39LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 5004LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_46_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6016LU)
);
}

/* NamespaceUri - ns=1;i=6013 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_47_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 12LU);
UA_String *variablenode_ns_1_i_6013_variant_DataContents =  UA_String_new();
if (!variablenode_ns_1_i_6013_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_String_init(variablenode_ns_1_i_6013_variant_DataContents);
*variablenode_ns_1_i_6013_variant_DataContents = UA_STRING_ALLOC("http://qt-project.org/QtOpcUaTestModel/");
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6013_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "NamespaceUri");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6013LU),
UA_NODEID_NUMERIC(ns[1], 6012LU),
UA_NODEID_NUMERIC(ns[0], 46LU),
UA_QUALIFIEDNAME(ns[0], "NamespaceUri"),
UA_NODEID_NUMERIC(ns[0], 68LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
UA_String_delete(variablenode_ns_1_i_6013_variant_DataContents);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_47_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6013LU)
);
}

/* QtTestStructWithDataValue - ns=1;i=6007 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_48_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 12LU);
UA_String *variablenode_ns_1_i_6007_variant_DataContents =  UA_String_new();
if (!variablenode_ns_1_i_6007_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_String_init(variablenode_ns_1_i_6007_variant_DataContents);
*variablenode_ns_1_i_6007_variant_DataContents = UA_STRING_ALLOC("QtTestStructWithDataValue");
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6007_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructWithDataValue");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6007LU),
UA_NODEID_NUMERIC(ns[1], 6012LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "QtTestStructWithDataValue"),
UA_NODEID_NUMERIC(ns[0], 69LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
UA_String_delete(variablenode_ns_1_i_6007_variant_DataContents);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6007LU), UA_NODEID_NUMERIC(ns[0], 39LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 5014LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_48_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6007LU)
);
}

/* QtTestStructWithDiagnosticInfo - ns=1;i=6004 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_49_begin(UA_Server *server, UA_UInt16* ns) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(ns[0], 12LU);
UA_String *variablenode_ns_1_i_6004_variant_DataContents =  UA_String_new();
if (!variablenode_ns_1_i_6004_variant_DataContents) return UA_STATUSCODE_BADOUTOFMEMORY;
UA_String_init(variablenode_ns_1_i_6004_variant_DataContents);
*variablenode_ns_1_i_6004_variant_DataContents = UA_STRING_ALLOC("QtTestStructWithDiagnosticInfo");
UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6004_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructWithDiagnosticInfo");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,
UA_NODEID_NUMERIC(ns[1], 6004LU),
UA_NODEID_NUMERIC(ns[1], 6012LU),
UA_NODEID_NUMERIC(ns[0], 47LU),
UA_QUALIFIEDNAME(ns[1], "QtTestStructWithDiagnosticInfo"),
UA_NODEID_NUMERIC(ns[0], 69LU),
(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
UA_String_delete(variablenode_ns_1_i_6004_variant_DataContents);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6004LU), UA_NODEID_NUMERIC(ns[0], 39LU), UA_EXPANDEDNODEID_NUMERIC(ns[1], 5003LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_49_finish(UA_Server *server, UA_UInt16* ns) {
return UA_Server_addNode_finish(server, 
UA_NODEID_NUMERIC(ns[1], 6004LU)
);
}

static UA_DataTypeArray customUA_TYPES_QTOPCUATESTMODEL = {
    NULL,
    UA_TYPES_QTOPCUATESTMODEL_COUNT,
    UA_TYPES_QTOPCUATESTMODEL
};

UA_StatusCode namespace_qtopcuatestmodel_generated(UA_Server *server) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
/* Use namespace ids generated by the server */
UA_UInt16 ns[2];
ns[0] = UA_Server_addNamespace(server, "http://opcfoundation.org/UA/");
ns[1] = UA_Server_addNamespace(server, "http://qt-project.org/QtOpcUaTestModel/");

/* Load custom datatype definitions into the server */
if(UA_TYPES_QTOPCUATESTMODEL_COUNT > 0) {
customUA_TYPES_QTOPCUATESTMODEL.next = UA_Server_getConfig(server)->customDataTypes;
UA_Server_getConfig(server)->customDataTypes = &customUA_TYPES_QTOPCUATESTMODEL;

}
if((retVal = function_namespace_qtopcuatestmodel_generated_0_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_1_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_2_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_3_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_4_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_5_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_6_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_7_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_8_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_9_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_10_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_11_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_12_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_13_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_14_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_15_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_16_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_17_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_18_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_19_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_20_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_21_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_22_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_23_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_24_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_25_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_26_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_27_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_28_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_29_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_30_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_31_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_32_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_33_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_34_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_35_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_36_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_37_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_38_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_39_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_40_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_41_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_42_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_43_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_44_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_45_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_46_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_47_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_48_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_49_begin(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_49_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_48_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_47_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_46_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_45_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_44_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_43_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_42_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_41_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_40_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_39_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_38_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_37_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_36_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_35_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_34_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_33_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_32_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_31_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_30_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_29_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_28_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_27_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_26_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_25_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_24_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_23_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_22_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_21_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_20_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_19_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_18_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_17_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_16_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_15_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_14_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_13_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_12_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_11_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_10_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_9_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_8_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_7_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_6_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_5_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_4_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_3_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_2_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_1_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
if((retVal = function_namespace_qtopcuatestmodel_generated_0_finish(server, ns)) != UA_STATUSCODE_GOOD) return retVal;
return retVal;
}
