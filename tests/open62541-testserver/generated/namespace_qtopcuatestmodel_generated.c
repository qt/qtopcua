/* WARNING: This is a generated file.
 * Any manual changes will be overwritten. */

#include "namespace_qtopcuatestmodel_generated.h"


/* QtTestUnionType - ns=1;i=3005 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_0_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestUnionType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3005LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12756LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 45LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtTestUnionType"), UA_NODEID_NULL,(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_0_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3005LU));
}

/* Default XML - ns=1;i=5008 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_1_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default XML");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5008LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "Default XML"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 76LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5008LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 38LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3005LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_1_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5008LU));
}

/* Default Binary - ns=1;i=5007 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_2_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default Binary");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5007LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "Default Binary"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 76LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5007LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 38LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3005LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_2_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5007LU));
}

/* QtAbstractBaseStruct - ns=1;i=3008 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_3_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.isAbstract = true;
attr.displayName = UA_LOCALIZEDTEXT("", "QtAbstractBaseStruct");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3008LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 22LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 45LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtAbstractBaseStruct"), UA_NODEID_NULL,(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_3_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3008LU));
}

/* Default XML - ns=1;i=5012 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_4_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default XML");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5012LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "Default XML"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 76LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5012LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 38LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3008LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_4_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5012LU));
}

/* Default Binary - ns=1;i=5006 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_5_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default Binary");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5006LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "Default Binary"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 76LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5006LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 38LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3008LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_5_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5006LU));
}

/* QtRecursiveTestStruct - ns=1;i=3012 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_6_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "QtRecursiveTestStruct");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3012LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3008LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 45LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtRecursiveTestStruct"), UA_NODEID_NULL,(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_6_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3012LU));
}

/* Default XML - ns=1;i=5017 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_7_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default XML");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5017LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "Default XML"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 76LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5017LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 38LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3012LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_7_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5017LU));
}

/* Default Binary - ns=1;i=5016 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_8_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default Binary");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5016LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "Default Binary"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 76LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5016LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 38LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3012LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_8_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5016LU));
}

/* QtTestStructWithDataValue - ns=1;i=3010 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_9_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructWithDataValue");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3010LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3008LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 45LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtTestStructWithDataValue"), UA_NODEID_NULL,(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_9_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3010LU));
}

/* Default XML - ns=1;i=5015 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_10_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default XML");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5015LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "Default XML"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 76LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5015LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 38LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3010LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_10_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5015LU));
}

/* Default Binary - ns=1;i=5014 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_11_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default Binary");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5014LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "Default Binary"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 76LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5014LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 38LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3010LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_11_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5014LU));
}

/* QtTestStructWithDiagnosticInfo - ns=1;i=3009 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_12_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructWithDiagnosticInfo");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3009LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3008LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 45LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtTestStructWithDiagnosticInfo"), UA_NODEID_NULL,(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_12_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3009LU));
}

/* Default XML - ns=1;i=5013 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_13_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default XML");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5013LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "Default XML"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 76LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5013LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 38LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3009LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_13_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5013LU));
}

/* Default Binary - ns=1;i=5003 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_14_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default Binary");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5003LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "Default Binary"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 76LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5003LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 38LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3009LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_14_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5003LU));
}

/* QtStructWithOptionalFieldType - ns=1;i=3006 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_15_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "QtStructWithOptionalFieldType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3006LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3008LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 45LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtStructWithOptionalFieldType"), UA_NODEID_NULL,(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_15_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3006LU));
}

/* Default XML - ns=1;i=5011 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_16_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default XML");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5011LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "Default XML"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 76LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5011LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 38LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3006LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_16_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5011LU));
}

/* Default Binary - ns=1;i=5010 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_17_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default Binary");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5010LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "Default Binary"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 76LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5010LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 38LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3006LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_17_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5010LU));
}

/* QtInnerTestStructType - ns=1;i=3004 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_18_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "QtInnerTestStructType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3004LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3008LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 45LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtInnerTestStructType"), UA_NODEID_NULL,(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_18_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3004LU));
}

/* Default XML - ns=1;i=5005 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_19_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default XML");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5005LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "Default XML"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 76LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5005LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 38LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3004LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_19_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5005LU));
}

/* Default Binary - ns=1;i=5004 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_20_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default Binary");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5004LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "Default Binary"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 76LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5004LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 38LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3004LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_20_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5004LU));
}

/* QtTestStructType - ns=1;i=3003 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_21_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3003LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3008LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 45LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtTestStructType"), UA_NODEID_NULL,(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_21_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3003LU));
}

/* Default XML - ns=1;i=5002 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_22_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default XML");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5002LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "Default XML"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 76LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5002LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 38LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3003LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_22_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5002LU));
}

/* Default Binary - ns=1;i=5001 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_23_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "Default Binary");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5001LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 0LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "Default Binary"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 76LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5001LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 38LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3003LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_23_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5001LU));
}

/* MyDoubleSubtype - ns=1;i=3007 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_24_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "MyDoubleSubtype");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3007LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 11LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 45LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "MyDoubleSubtype"), UA_NODEID_NULL,(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_24_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3007LU));
}

/* QtTestEnumeration - ns=1;i=3002 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_25_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_DataTypeAttributes attr = UA_DataTypeAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestEnumeration");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_DATATYPE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3002LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 29LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 45LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtTestEnumeration"), UA_NODEID_NULL,(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_DATATYPEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_25_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3002LU));
}

/* EnumStrings - ns=1;i=6001 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_26_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
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
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 21LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<ListOfLocalizedText>"
                "<LocalizedText>"
                    "<Text>Unknown</Text>"
                "</LocalizedText>"
                "<LocalizedText>"
                    "<Text>FirstOption</Text>"
                "</LocalizedText>"
                "<LocalizedText>"
                    "<Text>SecondOption</Text>"
                "</LocalizedText>"
                "<LocalizedText>"
                    "<Text>ThirdOption</Text>"
                "</LocalizedText>"
            "</ListOfLocalizedText>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
if(UA_Variant_isScalar(&attr.value) && attr.value.data != NULL) {
    UA_LOG_WARNING(UA_Server_getConfig(server)->logging,
                  UA_LOGCATEGORY_USERLAND,
                  "Node ns=1;i=6001: ValueRank=1 but the XML value is scalar. Auto-wrapping into a one-element array.");
    attr.value.arrayLength = 1;
}
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "EnumStrings");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6001LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3002LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 46LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "EnumStrings"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 68LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_26_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6001LU));
}

/* DecoderTestNodes - ns=1;i=5009 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_27_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_ObjectAttributes attr = UA_ObjectAttributes_default;
attr.displayName = UA_LOCALIZEDTEXT("", "DecoderTestNodes");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECT,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5009LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 85LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 35LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "DecoderTestNodes"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 61LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_OBJECTATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_27_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5009LU));
}

/* RecursiveStruct - ns=1;i=6029 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_28_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 3;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3012LU);
attr.displayName = UA_LOCALIZEDTEXT("", "RecursiveStruct");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6029LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5009LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "RecursiveStruct"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 63LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_28_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6029LU));
}

/* StructWithDataValue - ns=1;i=6027 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_29_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 3;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3010LU);
attr.displayName = UA_LOCALIZEDTEXT("", "StructWithDataValue");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6027LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5009LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 35LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "StructWithDataValue"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 63LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_29_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6027LU));
}

/* UnionWithFirstMember - ns=1;i=6011 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_30_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 3;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3005LU);
attr.displayName = UA_LOCALIZEDTEXT("", "UnionWithFirstMember");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6011LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5009LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "UnionWithFirstMember"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 63LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_30_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6011LU));
}

/* StructWithOptionalField - ns=1;i=6010 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_31_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 3;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3006LU);
attr.displayName = UA_LOCALIZEDTEXT("", "StructWithOptionalField");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6010LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5009LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "StructWithOptionalField"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 63LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_31_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6010LU));
}

/* NestedStruct - ns=1;i=6009 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_32_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 3;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3003LU);
attr.displayName = UA_LOCALIZEDTEXT("", "NestedStruct");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6009LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5009LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "NestedStruct"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 63LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_32_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6009LU));
}

/* StructWithDiagnosticInfo - ns=1;i=6006 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_33_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 3;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3009LU);
attr.displayName = UA_LOCALIZEDTEXT("", "StructWithDiagnosticInfo");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6006LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5009LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "StructWithDiagnosticInfo"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 63LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_33_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6006LU));
}

/* UnionWithSecondMember - ns=1;i=6003 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_34_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 3;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3005LU);
attr.displayName = UA_LOCALIZEDTEXT("", "UnionWithSecondMember");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6003LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5009LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "UnionWithSecondMember"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 63LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_34_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6003LU));
}

/* StructWithoutOptionalField - ns=1;i=6002 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_35_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 3;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 3006LU);
attr.displayName = UA_LOCALIZEDTEXT("", "StructWithoutOptionalField");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6002LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5009LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "StructWithoutOptionalField"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 63LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_35_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6002LU));
}

/* TypeDictionary - ns=1;i=6014 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_36_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 15LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<ByteString>PHhzOnNjaGVtYSBlbGVtZW50Rm9ybURlZmF1bHQ9InF1YWxpZmllZCIgdGFyZ2V0TmFtZXNwYWNlPSJod"
        "HRwOi8vcXQtcHJvamVjdC5vcmcvUXRPcGNVYVRlc3RNb2RlbC9UeXBlcy54c2QiIHhtbG5zO"
        "nRucz0iaHR0cDovL3F0LXByb2plY3Qub3JnL1F0T3BjVWFUZXN0TW9kZWwvVHlwZXMueHNkI"
        "iB4bWxuczp1YT0iaHR0cDovL29wY2ZvdW5kYXRpb24ub3JnL1VBLzIwMDgvMDIvVHlwZXMue"
        "HNkIiB4bWxuczp4cz0iaHR0cDovL3d3dy53My5vcmcvMjAwMS9YTUxTY2hlbWEiPgogPHhzO"
        "mltcG9ydCBuYW1lc3BhY2U9Imh0dHA6Ly9vcGNmb3VuZGF0aW9uLm9yZy9VQS8yMDA4LzAyL"
        "1R5cGVzLnhzZCIvPgogPHhzOnNpbXBsZVR5cGUgbmFtZT0iUXRUZXN0RW51bWVyYXRpb24iP"
        "gogIDx4czpyZXN0cmljdGlvbiBiYXNlPSJ4czpzdHJpbmciPgogICA8eHM6ZW51bWVyYXRpb"
        "24gdmFsdWU9IlVua25vd25fMCIvPgogICA8eHM6ZW51bWVyYXRpb24gdmFsdWU9IkZpcnN0T"
        "3B0aW9uXzEiLz4KICAgPHhzOmVudW1lcmF0aW9uIHZhbHVlPSJTZWNvbmRPcHRpb25fMiIvP"
        "gogICA8eHM6ZW51bWVyYXRpb24gdmFsdWU9IlRoaXJkT3B0aW9uXzMiLz4KICA8L3hzOnJlc"
        "3RyaWN0aW9uPgogPC94czpzaW1wbGVUeXBlPgogPHhzOmVsZW1lbnQgdHlwZT0idG5zOlF0V"
        "GVzdEVudW1lcmF0aW9uIiBuYW1lPSJRdFRlc3RFbnVtZXJhdGlvbiIvPgogPHhzOmNvbXBsZ"
        "XhUeXBlIG5hbWU9Ikxpc3RPZlF0VGVzdEVudW1lcmF0aW9uIj4KICA8eHM6c2VxdWVuY2U+C"
        "iAgIDx4czplbGVtZW50IG1pbk9jY3Vycz0iMCIgbWF4T2NjdXJzPSJ1bmJvdW5kZWQiIHR5c"
        "GU9InRuczpRdFRlc3RFbnVtZXJhdGlvbiIgbmFtZT0iUXRUZXN0RW51bWVyYXRpb24iIG5pb"
        "GxhYmxlPSJ0cnVlIi8+CiAgPC94czpzZXF1ZW5jZT4KIDwveHM6Y29tcGxleFR5cGU+CiA8e"
        "HM6ZWxlbWVudCB0eXBlPSJ0bnM6TGlzdE9mUXRUZXN0RW51bWVyYXRpb24iIG5hbWU9Ikxpc"
        "3RPZlF0VGVzdEVudW1lcmF0aW9uIiBuaWxsYWJsZT0idHJ1ZSIvPgogPHhzOmNvbXBsZXhUe"
        "XBlIG5hbWU9IlF0QWJzdHJhY3RCYXNlU3RydWN0Ij4KICA8eHM6c2VxdWVuY2UvPgogPC94c"
        "zpjb21wbGV4VHlwZT4KIDx4czplbGVtZW50IHR5cGU9InRuczpRdEFic3RyYWN0QmFzZVN0c"
        "nVjdCIgbmFtZT0iUXRBYnN0cmFjdEJhc2VTdHJ1Y3QiLz4KIDx4czpjb21wbGV4VHlwZSBuY"
        "W1lPSJMaXN0T2ZRdEFic3RyYWN0QmFzZVN0cnVjdCI+CiAgPHhzOnNlcXVlbmNlPgogICA8e"
        "HM6ZWxlbWVudCBtaW5PY2N1cnM9IjAiIG1heE9jY3Vycz0idW5ib3VuZGVkIiB0eXBlPSJ0b"
        "nM6UXRBYnN0cmFjdEJhc2VTdHJ1Y3QiIG5hbWU9IlF0QWJzdHJhY3RCYXNlU3RydWN0IiBua"
        "WxsYWJsZT0idHJ1ZSIvPgogIDwveHM6c2VxdWVuY2U+CiA8L3hzOmNvbXBsZXhUeXBlPgogP"
        "HhzOmVsZW1lbnQgdHlwZT0idG5zOkxpc3RPZlF0QWJzdHJhY3RCYXNlU3RydWN0IiBuYW1lP"
        "SJMaXN0T2ZRdEFic3RyYWN0QmFzZVN0cnVjdCIgbmlsbGFibGU9InRydWUiLz4KIDx4czpjb"
        "21wbGV4VHlwZSBuYW1lPSJRdElubmVyVGVzdFN0cnVjdFR5cGUiPgogIDx4czpjb21wbGV4Q"
        "29udGVudCBtaXhlZD0iZmFsc2UiPgogICA8eHM6ZXh0ZW5zaW9uIGJhc2U9InVhOkV4dGVuc"
        "2lvbk9iamVjdCI+CiAgICA8eHM6c2VxdWVuY2U+CiAgICAgPHhzOmVsZW1lbnQgbWluT2Njd"
        "XJzPSIwIiBtYXhPY2N1cnM9IjEiIHR5cGU9InhzOmRvdWJsZSIgbmFtZT0iRG91YmxlU3Vid"
        "HlwZU1lbWJlciIvPgogICAgPC94czpzZXF1ZW5jZT4KICAgPC94czpleHRlbnNpb24+CiAgP"
        "C94czpjb21wbGV4Q29udGVudD4KIDwveHM6Y29tcGxleFR5cGU+CiA8eHM6ZWxlbWVudCB0e"
        "XBlPSJ0bnM6UXRJbm5lclRlc3RTdHJ1Y3RUeXBlIiBuYW1lPSJRdElubmVyVGVzdFN0cnVjd"
        "FR5cGUiLz4KIDx4czpjb21wbGV4VHlwZSBuYW1lPSJMaXN0T2ZRdElubmVyVGVzdFN0cnVjd"
        "FR5cGUiPgogIDx4czpzZXF1ZW5jZT4KICAgPHhzOmVsZW1lbnQgbWluT2NjdXJzPSIwIiBtY"
        "XhPY2N1cnM9InVuYm91bmRlZCIgdHlwZT0idG5zOlF0SW5uZXJUZXN0U3RydWN0VHlwZSIgb"
        "mFtZT0iUXRJbm5lclRlc3RTdHJ1Y3RUeXBlIiBuaWxsYWJsZT0idHJ1ZSIvPgogIDwveHM6c"
        "2VxdWVuY2U+CiA8L3hzOmNvbXBsZXhUeXBlPgogPHhzOmVsZW1lbnQgdHlwZT0idG5zOkxpc"
        "3RPZlF0SW5uZXJUZXN0U3RydWN0VHlwZSIgbmFtZT0iTGlzdE9mUXRJbm5lclRlc3RTdHJ1Y"
        "3RUeXBlIiBuaWxsYWJsZT0idHJ1ZSIvPgogPHhzOmNvbXBsZXhUeXBlIG5hbWU9IlF0UmVjd"
        "XJzaXZlVGVzdFN0cnVjdCI+CiAgPHhzOmNvbXBsZXhDb250ZW50IG1peGVkPSJmYWxzZSI+C"
        "iAgIDx4czpleHRlbnNpb24gYmFzZT0idWE6RXh0ZW5zaW9uT2JqZWN0Ij4KICAgIDx4czpzZ"
        "XF1ZW5jZT4KICAgICA8eHM6ZWxlbWVudCBtaW5PY2N1cnM9IjAiIG1heE9jY3Vycz0iMSIgd"
        "HlwZT0ieHM6c3RyaW5nIiBuYW1lPSJTdHJpbmdNZW1iZXIiLz4KICAgICA8eHM6ZWxlbWVud"
        "CBtaW5PY2N1cnM9IjAiIG1heE9jY3Vycz0iMSIgdHlwZT0idG5zOkxpc3RPZlF0UmVjdXJza"
        "XZlVGVzdFN0cnVjdCIgbmFtZT0iUmVjdXJzaXZlQXJyYXlNZW1iZXIiLz4KICAgIDwveHM6c"
        "2VxdWVuY2U+CiAgIDwveHM6ZXh0ZW5zaW9uPgogIDwveHM6Y29tcGxleENvbnRlbnQ+CiA8L"
        "3hzOmNvbXBsZXhUeXBlPgogPHhzOmVsZW1lbnQgdHlwZT0idG5zOlF0UmVjdXJzaXZlVGVzd"
        "FN0cnVjdCIgbmFtZT0iUXRSZWN1cnNpdmVUZXN0U3RydWN0Ii8+CiA8eHM6Y29tcGxleFR5c"
        "GUgbmFtZT0iTGlzdE9mUXRSZWN1cnNpdmVUZXN0U3RydWN0Ij4KICA8eHM6c2VxdWVuY2U+C"
        "iAgIDx4czplbGVtZW50IG1pbk9jY3Vycz0iMCIgbWF4T2NjdXJzPSJ1bmJvdW5kZWQiIHR5c"
        "GU9InRuczpRdFJlY3Vyc2l2ZVRlc3RTdHJ1Y3QiIG5hbWU9IlF0UmVjdXJzaXZlVGVzdFN0c"
        "nVjdCIgbmlsbGFibGU9InRydWUiLz4KICA8L3hzOnNlcXVlbmNlPgogPC94czpjb21wbGV4V"
        "HlwZT4KIDx4czplbGVtZW50IHR5cGU9InRuczpMaXN0T2ZRdFJlY3Vyc2l2ZVRlc3RTdHJ1Y"
        "3QiIG5hbWU9Ikxpc3RPZlF0UmVjdXJzaXZlVGVzdFN0cnVjdCIgbmlsbGFibGU9InRydWUiL"
        "z4KIDx4czpjb21wbGV4VHlwZSBuYW1lPSJRdFN0cnVjdFdpdGhPcHRpb25hbEZpZWxkVHlwZ"
        "SI+CiAgPHhzOnNlcXVlbmNlPgogICA8eHM6ZWxlbWVudCBtaW5PY2N1cnM9IjAiIHR5cGU9I"
        "nhzOnVuc2lnbmVkSW50IiBuYW1lPSJFbmNvZGluZ01hc2siLz4KICAgPHhzOmVsZW1lbnQgb"
        "WluT2NjdXJzPSIwIiBtYXhPY2N1cnM9IjEiIHR5cGU9InhzOmRvdWJsZSIgbmFtZT0iTWFuZ"
        "GF0b3J5TWVtYmVyIi8+CiAgIDx4czplbGVtZW50IG1pbk9jY3Vycz0iMCIgbWF4T2NjdXJzP"
        "SIxIiB0eXBlPSJ4czpkb3VibGUiIG5hbWU9Ik9wdGlvbmFsTWVtYmVyIi8+CiAgPC94czpzZ"
        "XF1ZW5jZT4KIDwveHM6Y29tcGxleFR5cGU+CiA8eHM6ZWxlbWVudCB0eXBlPSJ0bnM6UXRTd"
        "HJ1Y3RXaXRoT3B0aW9uYWxGaWVsZFR5cGUiIG5hbWU9IlF0U3RydWN0V2l0aE9wdGlvbmFsR"
        "mllbGRUeXBlIi8+CiA8eHM6Y29tcGxleFR5cGUgbmFtZT0iTGlzdE9mUXRTdHJ1Y3RXaXRoT"
        "3B0aW9uYWxGaWVsZFR5cGUiPgogIDx4czpzZXF1ZW5jZT4KICAgPHhzOmVsZW1lbnQgbWluT"
        "2NjdXJzPSIwIiBtYXhPY2N1cnM9InVuYm91bmRlZCIgdHlwZT0idG5zOlF0U3RydWN0V2l0a"
        "E9wdGlvbmFsRmllbGRUeXBlIiBuYW1lPSJRdFN0cnVjdFdpdGhPcHRpb25hbEZpZWxkVHlwZ"
        "SIgbmlsbGFibGU9InRydWUiLz4KICA8L3hzOnNlcXVlbmNlPgogPC94czpjb21wbGV4VHlwZ"
        "T4KIDx4czplbGVtZW50IHR5cGU9InRuczpMaXN0T2ZRdFN0cnVjdFdpdGhPcHRpb25hbEZpZ"
        "WxkVHlwZSIgbmFtZT0iTGlzdE9mUXRTdHJ1Y3RXaXRoT3B0aW9uYWxGaWVsZFR5cGUiIG5pb"
        "GxhYmxlPSJ0cnVlIi8+CiA8eHM6Y29tcGxleFR5cGUgbmFtZT0iUXRUZXN0U3RydWN0VHlwZ"
        "SI+CiAgPHhzOmNvbXBsZXhDb250ZW50IG1peGVkPSJmYWxzZSI+CiAgIDx4czpleHRlbnNpb"
        "24gYmFzZT0idWE6RXh0ZW5zaW9uT2JqZWN0Ij4KICAgIDx4czpzZXF1ZW5jZT4KICAgICA8e"
        "HM6ZWxlbWVudCBtaW5PY2N1cnM9IjAiIG1heE9jY3Vycz0iMSIgdHlwZT0ieHM6c3RyaW5nI"
        "iBuYW1lPSJTdHJpbmdNZW1iZXIiLz4KICAgICA8eHM6ZWxlbWVudCBtaW5PY2N1cnM9IjAiI"
        "G1heE9jY3Vycz0iMSIgdHlwZT0idWE6TG9jYWxpemVkVGV4dCIgbmFtZT0iTG9jYWxpemVkV"
        "GV4dE1lbWJlciIvPgogICAgIDx4czplbGVtZW50IG1pbk9jY3Vycz0iMCIgbWF4T2NjdXJzP"
        "SIxIiB0eXBlPSJ1YTpRdWFsaWZpZWROYW1lIiBuYW1lPSJRdWFsaWZpZWROYW1lTWVtYmVyI"
        "i8+CiAgICAgPHhzOmVsZW1lbnQgbWluT2NjdXJzPSIwIiBtYXhPY2N1cnM9IjEiIHR5cGU9I"
        "nVhOkxpc3RPZkludDY0IiBuYW1lPSJJbnQ2NEFycmF5TWVtYmVyIi8+CiAgICAgPHhzOmVsZ"
        "W1lbnQgbWluT2NjdXJzPSIwIiBtYXhPY2N1cnM9IjEiIHR5cGU9InRuczpRdFRlc3RFbnVtZ"
        "XJhdGlvbiIgbmFtZT0iRW51bU1lbWJlciIvPgogICAgIDx4czplbGVtZW50IG1pbk9jY3Vyc"
        "z0iMCIgbWF4T2NjdXJzPSIxIiB0eXBlPSJ0bnM6UXRJbm5lclRlc3RTdHJ1Y3RUeXBlIiBuY"
        "W1lPSJOZXN0ZWRTdHJ1Y3RNZW1iZXIiLz4KICAgICA8eHM6ZWxlbWVudCBtaW5PY2N1cnM9I"
        "jAiIG1heE9jY3Vycz0iMSIgdHlwZT0idG5zOkxpc3RPZlF0SW5uZXJUZXN0U3RydWN0VHlwZ"
        "SIgbmFtZT0iTmVzdGVkU3RydWN0QXJyYXlNZW1iZXIiLz4KICAgIDwveHM6c2VxdWVuY2U+C"
        "iAgIDwveHM6ZXh0ZW5zaW9uPgogIDwveHM6Y29tcGxleENvbnRlbnQ+CiA8L3hzOmNvbXBsZ"
        "XhUeXBlPgogPHhzOmVsZW1lbnQgdHlwZT0idG5zOlF0VGVzdFN0cnVjdFR5cGUiIG5hbWU9I"
        "lF0VGVzdFN0cnVjdFR5cGUiLz4KIDx4czpjb21wbGV4VHlwZSBuYW1lPSJMaXN0T2ZRdFRlc"
        "3RTdHJ1Y3RUeXBlIj4KICA8eHM6c2VxdWVuY2U+CiAgIDx4czplbGVtZW50IG1pbk9jY3Vyc"
        "z0iMCIgbWF4T2NjdXJzPSJ1bmJvdW5kZWQiIHR5cGU9InRuczpRdFRlc3RTdHJ1Y3RUeXBlI"
        "iBuYW1lPSJRdFRlc3RTdHJ1Y3RUeXBlIiBuaWxsYWJsZT0idHJ1ZSIvPgogIDwveHM6c2Vxd"
        "WVuY2U+CiA8L3hzOmNvbXBsZXhUeXBlPgogPHhzOmVsZW1lbnQgdHlwZT0idG5zOkxpc3RPZ"
        "lF0VGVzdFN0cnVjdFR5cGUiIG5hbWU9Ikxpc3RPZlF0VGVzdFN0cnVjdFR5cGUiIG5pbGxhY"
        "mxlPSJ0cnVlIi8+CiA8eHM6Y29tcGxleFR5cGUgbmFtZT0iUXRUZXN0U3RydWN0V2l0aERhd"
        "GFWYWx1ZSI+CiAgPHhzOmNvbXBsZXhDb250ZW50IG1peGVkPSJmYWxzZSI+CiAgIDx4czple"
        "HRlbnNpb24gYmFzZT0idWE6RXh0ZW5zaW9uT2JqZWN0Ij4KICAgIDx4czpzZXF1ZW5jZT4KI"
        "CAgICA8eHM6ZWxlbWVudCBtaW5PY2N1cnM9IjAiIG1heE9jY3Vycz0iMSIgdHlwZT0idWE6R"
        "GF0YVZhbHVlIiBuYW1lPSJEYXRhVmFsdWVNZW1iZXIiLz4KICAgICA8eHM6ZWxlbWVudCBta"
        "W5PY2N1cnM9IjAiIG1heE9jY3Vycz0iMSIgdHlwZT0idWE6VmFyaWFudCIgbmFtZT0iVmFya"
        "WFudE1lbWJlciIvPgogICAgPC94czpzZXF1ZW5jZT4KICAgPC94czpleHRlbnNpb24+CiAgP"
        "C94czpjb21wbGV4Q29udGVudD4KIDwveHM6Y29tcGxleFR5cGU+CiA8eHM6ZWxlbWVudCB0e"
        "XBlPSJ0bnM6UXRUZXN0U3RydWN0V2l0aERhdGFWYWx1ZSIgbmFtZT0iUXRUZXN0U3RydWN0V"
        "2l0aERhdGFWYWx1ZSIvPgogPHhzOmNvbXBsZXhUeXBlIG5hbWU9Ikxpc3RPZlF0VGVzdFN0c"
        "nVjdFdpdGhEYXRhVmFsdWUiPgogIDx4czpzZXF1ZW5jZT4KICAgPHhzOmVsZW1lbnQgbWluT"
        "2NjdXJzPSIwIiBtYXhPY2N1cnM9InVuYm91bmRlZCIgdHlwZT0idG5zOlF0VGVzdFN0cnVjd"
        "FdpdGhEYXRhVmFsdWUiIG5hbWU9IlF0VGVzdFN0cnVjdFdpdGhEYXRhVmFsdWUiIG5pbGxhY"
        "mxlPSJ0cnVlIi8+CiAgPC94czpzZXF1ZW5jZT4KIDwveHM6Y29tcGxleFR5cGU+CiA8eHM6Z"
        "WxlbWVudCB0eXBlPSJ0bnM6TGlzdE9mUXRUZXN0U3RydWN0V2l0aERhdGFWYWx1ZSIgbmFtZ"
        "T0iTGlzdE9mUXRUZXN0U3RydWN0V2l0aERhdGFWYWx1ZSIgbmlsbGFibGU9InRydWUiLz4KI"
        "Dx4czpjb21wbGV4VHlwZSBuYW1lPSJRdFRlc3RTdHJ1Y3RXaXRoRGlhZ25vc3RpY0luZm8iP"
        "gogIDx4czpjb21wbGV4Q29udGVudCBtaXhlZD0iZmFsc2UiPgogICA8eHM6ZXh0ZW5zaW9uI"
        "GJhc2U9InVhOkV4dGVuc2lvbk9iamVjdCI+CiAgICA8eHM6c2VxdWVuY2U+CiAgICAgPHhzO"
        "mVsZW1lbnQgbWluT2NjdXJzPSIwIiBtYXhPY2N1cnM9IjEiIHR5cGU9InVhOkRpYWdub3N0a"
        "WNJbmZvIiBuYW1lPSJEaWFnbm9zdGljSW5mb01lbWJlciIvPgogICAgIDx4czplbGVtZW50I"
        "G1pbk9jY3Vycz0iMCIgbWF4T2NjdXJzPSIxIiB0eXBlPSJ1YTpMaXN0T2ZEaWFnbm9zdGljS"
        "W5mbyIgbmFtZT0iRGlhZ25vc3RpY0luZm9BcnJheU1lbWJlciIvPgogICAgPC94czpzZXF1Z"
        "W5jZT4KICAgPC94czpleHRlbnNpb24+CiAgPC94czpjb21wbGV4Q29udGVudD4KIDwveHM6Y"
        "29tcGxleFR5cGU+CiA8eHM6ZWxlbWVudCB0eXBlPSJ0bnM6UXRUZXN0U3RydWN0V2l0aERpY"
        "Wdub3N0aWNJbmZvIiBuYW1lPSJRdFRlc3RTdHJ1Y3RXaXRoRGlhZ25vc3RpY0luZm8iLz4KI"
        "Dx4czpjb21wbGV4VHlwZSBuYW1lPSJMaXN0T2ZRdFRlc3RTdHJ1Y3RXaXRoRGlhZ25vc3RpY"
        "0luZm8iPgogIDx4czpzZXF1ZW5jZT4KICAgPHhzOmVsZW1lbnQgbWluT2NjdXJzPSIwIiBtY"
        "XhPY2N1cnM9InVuYm91bmRlZCIgdHlwZT0idG5zOlF0VGVzdFN0cnVjdFdpdGhEaWFnbm9zd"
        "GljSW5mbyIgbmFtZT0iUXRUZXN0U3RydWN0V2l0aERpYWdub3N0aWNJbmZvIiBuaWxsYWJsZ"
        "T0idHJ1ZSIvPgogIDwveHM6c2VxdWVuY2U+CiA8L3hzOmNvbXBsZXhUeXBlPgogPHhzOmVsZ"
        "W1lbnQgdHlwZT0idG5zOkxpc3RPZlF0VGVzdFN0cnVjdFdpdGhEaWFnbm9zdGljSW5mbyIgb"
        "mFtZT0iTGlzdE9mUXRUZXN0U3RydWN0V2l0aERpYWdub3N0aWNJbmZvIiBuaWxsYWJsZT0id"
        "HJ1ZSIvPgogPHhzOmNvbXBsZXhUeXBlIG5hbWU9IlF0VGVzdFVuaW9uVHlwZSI+CiAgPHhzO"
        "nNlcXVlbmNlPgogICA8eHM6ZWxlbWVudCBtaW5PY2N1cnM9IjAiIG1heE9jY3Vycz0iMSIgd"
        "HlwZT0ieHM6dW5zaWduZWRJbnQiIG5hbWU9IlN3aXRjaEZpZWxkIi8+CiAgIDx4czpjaG9pY"
        "2U+CiAgICA8eHM6ZWxlbWVudCBtaW5PY2N1cnM9IjAiIG1heE9jY3Vycz0iMSIgdHlwZT0ie"
        "HM6bG9uZyIgbmFtZT0iTWVtYmVyMSIvPgogICAgPHhzOmVsZW1lbnQgbWluT2NjdXJzPSIwI"
        "iBtYXhPY2N1cnM9IjEiIHR5cGU9InRuczpRdElubmVyVGVzdFN0cnVjdFR5cGUiIG5hbWU9I"
        "k1lbWJlcjIiLz4KICAgPC94czpjaG9pY2U+CiAgPC94czpzZXF1ZW5jZT4KIDwveHM6Y29tc"
        "GxleFR5cGU+CiA8eHM6ZWxlbWVudCB0eXBlPSJ0bnM6UXRUZXN0VW5pb25UeXBlIiBuYW1lP"
        "SJRdFRlc3RVbmlvblR5cGUiLz4KIDx4czpjb21wbGV4VHlwZSBuYW1lPSJMaXN0T2ZRdFRlc"
        "3RVbmlvblR5cGUiPgogIDx4czpzZXF1ZW5jZT4KICAgPHhzOmVsZW1lbnQgbWluT2NjdXJzP"
        "SIwIiBtYXhPY2N1cnM9InVuYm91bmRlZCIgdHlwZT0idG5zOlF0VGVzdFVuaW9uVHlwZSIgb"
        "mFtZT0iUXRUZXN0VW5pb25UeXBlIiBuaWxsYWJsZT0idHJ1ZSIvPgogIDwveHM6c2VxdWVuY"
        "2U+CiA8L3hzOmNvbXBsZXhUeXBlPgogPHhzOmVsZW1lbnQgdHlwZT0idG5zOkxpc3RPZlF0V"
        "GVzdFVuaW9uVHlwZSIgbmFtZT0iTGlzdE9mUXRUZXN0VW5pb25UeXBlIiBuaWxsYWJsZT0id"
        "HJ1ZSIvPgo8L3hzOnNjaGVtYT4K</ByteString>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "TypeDictionary");

#ifdef UA_ENABLE_NODESET_COMPILER_DESCRIPTIONS

attr.description = UA_LOCALIZEDTEXT("", "Collects the data type descriptions of http://qt-project.org/QtOpcUaTestModel/");

#endif

retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6014LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 92LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "TypeDictionary"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 72LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_36_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6014LU));
}

/* QtRecursiveTestStruct - ns=1;i=6028 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_37_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>//xs:element[@name='QtRecursiveTestStruct']</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "QtRecursiveTestStruct");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6028LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6014LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtRecursiveTestStruct"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 69LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6028LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 39LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5017LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_37_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6028LU));
}

/* QtAbstractBaseStruct - ns=1;i=6025 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_38_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>//xs:element[@name='QtAbstractBaseStruct']</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "QtAbstractBaseStruct");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6025LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6014LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtAbstractBaseStruct"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 69LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6025LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 39LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5012LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_38_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6025LU));
}

/* QtTestUnionType - ns=1;i=6023 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_39_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>//xs:element[@name='QtTestUnionType']</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestUnionType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6023LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6014LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtTestUnionType"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 69LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6023LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 39LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5008LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_39_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6023LU));
}

/* QtTestStructType - ns=1;i=6021 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_40_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>//xs:element[@name='QtTestStructType']</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6021LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6014LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtTestStructType"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 69LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6021LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 39LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5002LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_40_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6021LU));
}

/* QtStructWithOptionalFieldType - ns=1;i=6019 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_41_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>//xs:element[@name='QtStructWithOptionalFieldType']</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "QtStructWithOptionalFieldType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6019LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6014LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtStructWithOptionalFieldType"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 69LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6019LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 39LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5011LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_41_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6019LU));
}

/* QtInnerTestStructType - ns=1;i=6017 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_42_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>//xs:element[@name='QtInnerTestStructType']</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "QtInnerTestStructType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6017LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6014LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtInnerTestStructType"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 69LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6017LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 39LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5005LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_42_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6017LU));
}

/* NamespaceUri - ns=1;i=6015 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_43_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>http://qt-project.org/QtOpcUaTestModel/Types.xsd</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "NamespaceUri");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6015LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6014LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 46LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "NamespaceUri"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 68LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_43_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6015LU));
}

/* QtTestStructWithDataValue - ns=1;i=6008 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_44_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>//xs:element[@name='QtTestStructWithDataValue']</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructWithDataValue");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6008LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6014LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtTestStructWithDataValue"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 69LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6008LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 39LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5015LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_44_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6008LU));
}

/* QtTestStructWithDiagnosticInfo - ns=1;i=6005 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_45_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>//xs:element[@name='QtTestStructWithDiagnosticInfo']</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructWithDiagnosticInfo");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6005LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6014LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtTestStructWithDiagnosticInfo"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 69LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6005LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 39LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5013LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_45_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6005LU));
}

/* TypeDictionary - ns=1;i=6012 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_46_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 15LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<ByteString>PG9wYzpUeXBlRGljdGlvbmFyeSB4bWxuczp4c2k9Imh0dHA6Ly93d3cudzMub3JnLzIwMDEvWE1MU2NoZ"
        "W1hLWluc3RhbmNlIiB4bWxuczp0bnM9Imh0dHA6Ly9xdC1wcm9qZWN0Lm9yZy9RdE9wY1VhV"
        "GVzdE1vZGVsLyIgRGVmYXVsdEJ5dGVPcmRlcj0iTGl0dGxlRW5kaWFuIiB4bWxuczpvcGM9I"
        "mh0dHA6Ly9vcGNmb3VuZGF0aW9uLm9yZy9CaW5hcnlTY2hlbWEvIiB4bWxuczp1YT0iaHR0c"
        "DovL29wY2ZvdW5kYXRpb24ub3JnL1VBLyIgVGFyZ2V0TmFtZXNwYWNlPSJodHRwOi8vcXQtc"
        "HJvamVjdC5vcmcvUXRPcGNVYVRlc3RNb2RlbC8iPgogPG9wYzpJbXBvcnQgTmFtZXNwYWNlP"
        "SJodHRwOi8vb3BjZm91bmRhdGlvbi5vcmcvVUEvIi8+CiA8b3BjOlN0cnVjdHVyZWRUeXBlI"
        "EJhc2VUeXBlPSJ1YTpFeHRlbnNpb25PYmplY3QiIE5hbWU9IlF0QWJzdHJhY3RCYXNlU3Ryd"
        "WN0Ii8+CiA8b3BjOlN0cnVjdHVyZWRUeXBlIEJhc2VUeXBlPSJ0bnM6UXRBYnN0cmFjdEJhc"
        "2VTdHJ1Y3QiIE5hbWU9IlF0SW5uZXJUZXN0U3RydWN0VHlwZSI+CiAgPG9wYzpGaWVsZCBUe"
        "XBlTmFtZT0ib3BjOkRvdWJsZSIgTmFtZT0iRG91YmxlU3VidHlwZU1lbWJlciIvPgogPC9vc"
        "GM6U3RydWN0dXJlZFR5cGU+CiA8b3BjOlN0cnVjdHVyZWRUeXBlIEJhc2VUeXBlPSJ0bnM6U"
        "XRBYnN0cmFjdEJhc2VTdHJ1Y3QiIE5hbWU9IlF0UmVjdXJzaXZlVGVzdFN0cnVjdCI+CiAgP"
        "G9wYzpGaWVsZCBUeXBlTmFtZT0ib3BjOkNoYXJBcnJheSIgTmFtZT0iU3RyaW5nTWVtYmVyI"
        "i8+CiAgPG9wYzpGaWVsZCBUeXBlTmFtZT0ib3BjOkludDMyIiBOYW1lPSJOb09mUmVjdXJza"
        "XZlQXJyYXlNZW1iZXIiLz4KICA8b3BjOkZpZWxkIExlbmd0aEZpZWxkPSJOb09mUmVjdXJza"
        "XZlQXJyYXlNZW1iZXIiIFR5cGVOYW1lPSJ0bnM6UXRSZWN1cnNpdmVUZXN0U3RydWN0IiBOY"
        "W1lPSJSZWN1cnNpdmVBcnJheU1lbWJlciIvPgogPC9vcGM6U3RydWN0dXJlZFR5cGU+CiA8b"
        "3BjOlN0cnVjdHVyZWRUeXBlIEJhc2VUeXBlPSJ0bnM6UXRBYnN0cmFjdEJhc2VTdHJ1Y3QiI"
        "E5hbWU9IlF0U3RydWN0V2l0aE9wdGlvbmFsRmllbGRUeXBlIj4KICA8b3BjOkZpZWxkIFR5c"
        "GVOYW1lPSJvcGM6Qml0IiBOYW1lPSJPcHRpb25hbE1lbWJlclNwZWNpZmllZCIvPgogIDxvc"
        "GM6RmllbGQgTGVuZ3RoPSIzMSIgVHlwZU5hbWU9Im9wYzpCaXQiIE5hbWU9IlJlc2VydmVkM"
        "SIvPgogIDxvcGM6RmllbGQgVHlwZU5hbWU9Im9wYzpEb3VibGUiIE5hbWU9Ik1hbmRhdG9ye"
        "U1lbWJlciIvPgogIDxvcGM6RmllbGQgU3dpdGNoRmllbGQ9Ik9wdGlvbmFsTWVtYmVyU3BlY"
        "2lmaWVkIiBUeXBlTmFtZT0ib3BjOkRvdWJsZSIgTmFtZT0iT3B0aW9uYWxNZW1iZXIiLz4KI"
        "Dwvb3BjOlN0cnVjdHVyZWRUeXBlPgogPG9wYzpTdHJ1Y3R1cmVkVHlwZSBCYXNlVHlwZT0id"
        "G5zOlF0QWJzdHJhY3RCYXNlU3RydWN0IiBOYW1lPSJRdFRlc3RTdHJ1Y3RUeXBlIj4KICA8b"
        "3BjOkZpZWxkIFR5cGVOYW1lPSJvcGM6Q2hhckFycmF5IiBOYW1lPSJTdHJpbmdNZW1iZXIiL"
        "z4KICA8b3BjOkZpZWxkIFR5cGVOYW1lPSJ1YTpMb2NhbGl6ZWRUZXh0IiBOYW1lPSJMb2Nhb"
        "Gl6ZWRUZXh0TWVtYmVyIi8+CiAgPG9wYzpGaWVsZCBUeXBlTmFtZT0idWE6UXVhbGlmaWVkT"
        "mFtZSIgTmFtZT0iUXVhbGlmaWVkTmFtZU1lbWJlciIvPgogIDxvcGM6RmllbGQgVHlwZU5hb"
        "WU9Im9wYzpJbnQzMiIgTmFtZT0iTm9PZkludDY0QXJyYXlNZW1iZXIiLz4KICA8b3BjOkZpZ"
        "WxkIExlbmd0aEZpZWxkPSJOb09mSW50NjRBcnJheU1lbWJlciIgVHlwZU5hbWU9Im9wYzpJb"
        "nQ2NCIgTmFtZT0iSW50NjRBcnJheU1lbWJlciIvPgogIDxvcGM6RmllbGQgVHlwZU5hbWU9I"
        "nRuczpRdFRlc3RFbnVtZXJhdGlvbiIgTmFtZT0iRW51bU1lbWJlciIvPgogIDxvcGM6Rmllb"
        "GQgVHlwZU5hbWU9InRuczpRdElubmVyVGVzdFN0cnVjdFR5cGUiIE5hbWU9Ik5lc3RlZFN0c"
        "nVjdE1lbWJlciIvPgogIDxvcGM6RmllbGQgVHlwZU5hbWU9Im9wYzpJbnQzMiIgTmFtZT0iT"
        "m9PZk5lc3RlZFN0cnVjdEFycmF5TWVtYmVyIi8+CiAgPG9wYzpGaWVsZCBMZW5ndGhGaWVsZ"
        "D0iTm9PZk5lc3RlZFN0cnVjdEFycmF5TWVtYmVyIiBUeXBlTmFtZT0idG5zOlF0SW5uZXJUZ"
        "XN0U3RydWN0VHlwZSIgTmFtZT0iTmVzdGVkU3RydWN0QXJyYXlNZW1iZXIiLz4KIDwvb3BjO"
        "lN0cnVjdHVyZWRUeXBlPgogPG9wYzpTdHJ1Y3R1cmVkVHlwZSBCYXNlVHlwZT0idG5zOlF0Q"
        "WJzdHJhY3RCYXNlU3RydWN0IiBOYW1lPSJRdFRlc3RTdHJ1Y3RXaXRoRGF0YVZhbHVlIj4KI"
        "CA8b3BjOkZpZWxkIFR5cGVOYW1lPSJ1YTpEYXRhVmFsdWUiIE5hbWU9IkRhdGFWYWx1ZU1lb"
        "WJlciIvPgogIDxvcGM6RmllbGQgVHlwZU5hbWU9InVhOlZhcmlhbnQiIE5hbWU9IlZhcmlhb"
        "nRNZW1iZXIiLz4KIDwvb3BjOlN0cnVjdHVyZWRUeXBlPgogPG9wYzpTdHJ1Y3R1cmVkVHlwZ"
        "SBCYXNlVHlwZT0idG5zOlF0QWJzdHJhY3RCYXNlU3RydWN0IiBOYW1lPSJRdFRlc3RTdHJ1Y"
        "3RXaXRoRGlhZ25vc3RpY0luZm8iPgogIDxvcGM6RmllbGQgVHlwZU5hbWU9InVhOkRpYWdub"
        "3N0aWNJbmZvIiBOYW1lPSJEaWFnbm9zdGljSW5mb01lbWJlciIvPgogIDxvcGM6RmllbGQgV"
        "HlwZU5hbWU9Im9wYzpJbnQzMiIgTmFtZT0iTm9PZkRpYWdub3N0aWNJbmZvQXJyYXlNZW1iZ"
        "XIiLz4KICA8b3BjOkZpZWxkIExlbmd0aEZpZWxkPSJOb09mRGlhZ25vc3RpY0luZm9BcnJhe"
        "U1lbWJlciIgVHlwZU5hbWU9InVhOkRpYWdub3N0aWNJbmZvIiBOYW1lPSJEaWFnbm9zdGljS"
        "W5mb0FycmF5TWVtYmVyIi8+CiA8L29wYzpTdHJ1Y3R1cmVkVHlwZT4KIDxvcGM6U3RydWN0d"
        "XJlZFR5cGUgQmFzZVR5cGU9InVhOlVuaW9uIiBOYW1lPSJRdFRlc3RVbmlvblR5cGUiPgogI"
        "DxvcGM6RmllbGQgVHlwZU5hbWU9Im9wYzpVSW50MzIiIE5hbWU9IlN3aXRjaEZpZWxkIi8+C"
        "iAgPG9wYzpGaWVsZCBTd2l0Y2hGaWVsZD0iU3dpdGNoRmllbGQiIFR5cGVOYW1lPSJvcGM6S"
        "W50NjQiIFN3aXRjaFZhbHVlPSIxIiBOYW1lPSJNZW1iZXIxIi8+CiAgPG9wYzpGaWVsZCBTd"
        "2l0Y2hGaWVsZD0iU3dpdGNoRmllbGQiIFR5cGVOYW1lPSJ0bnM6UXRJbm5lclRlc3RTdHJ1Y"
        "3RUeXBlIiBTd2l0Y2hWYWx1ZT0iMiIgTmFtZT0iTWVtYmVyMiIvPgogPC9vcGM6U3RydWN0d"
        "XJlZFR5cGU+CiA8b3BjOkVudW1lcmF0ZWRUeXBlIExlbmd0aEluQml0cz0iMzIiIE5hbWU9I"
        "lF0VGVzdEVudW1lcmF0aW9uIj4KICA8b3BjOkVudW1lcmF0ZWRWYWx1ZSBOYW1lPSJVbmtub"
        "3duIiBWYWx1ZT0iMCIvPgogIDxvcGM6RW51bWVyYXRlZFZhbHVlIE5hbWU9IkZpcnN0T3B0a"
        "W9uIiBWYWx1ZT0iMSIvPgogIDxvcGM6RW51bWVyYXRlZFZhbHVlIE5hbWU9IlNlY29uZE9wd"
        "GlvbiIgVmFsdWU9IjIiLz4KICA8b3BjOkVudW1lcmF0ZWRWYWx1ZSBOYW1lPSJUaGlyZE9wd"
        "GlvbiIgVmFsdWU9IjMiLz4KIDwvb3BjOkVudW1lcmF0ZWRUeXBlPgo8L29wYzpUeXBlRGljd"
        "GlvbmFyeT4K</ByteString>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "TypeDictionary");

#ifdef UA_ENABLE_NODESET_COMPILER_DESCRIPTIONS

attr.description = UA_LOCALIZEDTEXT("", "Collects the data type descriptions of http://qt-project.org/QtOpcUaTestModel/");

#endif

retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6012LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 93LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "TypeDictionary"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 72LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_46_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6012LU));
}

/* QtRecursiveTestStruct - ns=1;i=6026 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_47_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>QtRecursiveTestStruct</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "QtRecursiveTestStruct");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6026LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6012LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtRecursiveTestStruct"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 69LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6026LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 39LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5016LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_47_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6026LU));
}

/* QtAbstractBaseStruct - ns=1;i=6024 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_48_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>QtAbstractBaseStruct</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "QtAbstractBaseStruct");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6024LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6012LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtAbstractBaseStruct"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 69LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6024LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 39LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5006LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_48_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6024LU));
}

/* QtTestUnionType - ns=1;i=6022 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_49_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>QtTestUnionType</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestUnionType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6022LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6012LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtTestUnionType"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 69LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6022LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 39LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5007LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_49_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6022LU));
}

/* QtTestStructType - ns=1;i=6020 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_50_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>QtTestStructType</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6020LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6012LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtTestStructType"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 69LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6020LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 39LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5001LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_50_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6020LU));
}

/* QtStructWithOptionalFieldType - ns=1;i=6018 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_51_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>QtStructWithOptionalFieldType</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "QtStructWithOptionalFieldType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6018LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6012LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtStructWithOptionalFieldType"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 69LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6018LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 39LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5010LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_51_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6018LU));
}

/* QtInnerTestStructType - ns=1;i=6016 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_52_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>QtInnerTestStructType</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "QtInnerTestStructType");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6016LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6012LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtInnerTestStructType"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 69LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6016LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 39LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5004LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_52_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6016LU));
}

/* NamespaceUri - ns=1;i=6013 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_53_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>http://qt-project.org/QtOpcUaTestModel/</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "NamespaceUri");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6013LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6012LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 46LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 0), "NamespaceUri"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 68LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_53_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6013LU));
}

/* QtTestStructWithDataValue - ns=1;i=6007 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_54_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>QtTestStructWithDataValue</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructWithDataValue");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6007LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6012LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtTestStructWithDataValue"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 69LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6007LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 39LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5014LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_54_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6007LU));
}

/* QtTestStructWithDiagnosticInfo - ns=1;i=6004 */

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_55_begin(UA_Server *server, UA_NamespaceMapping *nsMapping) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
UA_VariableAttributes attr = UA_VariableAttributes_default;
attr.minimumSamplingInterval = 0.000000;
attr.userAccessLevel = 1;
attr.accessLevel = 1;
/* Value rank inherited */
attr.valueRank = -2;
attr.dataType = UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 12LU);
#ifdef UA_ENABLE_XML_ENCODING
UA_String xmlValue = UA_STRING("<Value>"
            "<String>QtTestStructWithDiagnosticInfo</String>"
        "</Value>");
UA_DecodeXmlOptions opts;
memset(&opts, 0, sizeof(UA_DecodeXmlOptions));
opts.unwrapped = true;
opts.namespaceMapping = nsMapping;
opts.customTypes = UA_Server_getConfig(server)->customDataTypes;
retVal |= UA_decodeXml(&xmlValue, &attr.value, &UA_TYPES[UA_TYPES_VARIANT], &opts);
#endif /* UA_ENABLE_XML_ENCODING */
attr.displayName = UA_LOCALIZEDTEXT("", "QtTestStructWithDiagnosticInfo");
retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE,UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6004LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6012LU),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 47LU),UA_QUALIFIEDNAME(UA_NamespaceMapping_local2Remote(nsMapping, 1), "QtTestStructWithDiagnosticInfo"),UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 69LU),(const UA_NodeAttributes*)&attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],NULL, NULL);
#ifdef UA_ENABLE_XML_ENCODING
UA_Variant_clear(&attr.value);
#endif /* UA_ENABLE_XML_ENCODING */
retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6004LU), UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 0), 39LU), UA_EXPANDEDNODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 5003LU), false);
return retVal;
}

static UA_StatusCode function_namespace_qtopcuatestmodel_generated_55_finish(UA_Server *server, UA_NamespaceMapping *nsMapping) {
return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(UA_NamespaceMapping_local2Remote(nsMapping, 1), 6004LU));
}

static UA_DataTypeArray customUA_TYPES_QTOPCUATESTMODEL = {
    NULL,
    UA_TYPES_QTOPCUATESTMODEL_COUNT,
    UA_TYPES_QTOPCUATESTMODEL,
    UA_FALSE
};

UA_StatusCode namespace_qtopcuatestmodel_generated(UA_Server *server) {
UA_StatusCode retVal = UA_STATUSCODE_GOOD;
/* Use namespace ids generated by the server */
UA_UInt16 ns[3];
ns[0] = UA_Server_addNamespace(server, "http://opcfoundation.org/UA/");
ns[1] = UA_Server_addNamespace(server, "http://qt-project.org/QtOpcUaTestModel/");
UA_UInt16 nsMappingTable[2] = {ns[0], ns[1]};
UA_NamespaceMapping nsMapping;
memset(&nsMapping, 0, sizeof(UA_NamespaceMapping));
nsMapping.local2remote = ns;
nsMapping.local2remoteSize = 2;
nsMapping.remote2local = nsMappingTable;
nsMapping.remote2localSize = 2;
/* Change namespaceIndex from current namespace */
#if UA_TYPES_QTOPCUATESTMODEL_COUNT > 0
for(int i = 0; i < UA_TYPES_QTOPCUATESTMODEL_COUNT; i++) {
UA_TYPES_QTOPCUATESTMODEL[i].typeId.namespaceIndex = ns[1];
UA_TYPES_QTOPCUATESTMODEL[i].binaryEncodingId.namespaceIndex = ns[1];
}
#endif

/* Load custom datatype definitions into the server */
if(UA_TYPES_QTOPCUATESTMODEL_COUNT > 0) {
customUA_TYPES_QTOPCUATESTMODEL.next = UA_Server_getConfig(server)->customDataTypes;
UA_Server_getConfig(server)->customDataTypes = &customUA_TYPES_QTOPCUATESTMODEL;

}
retVal |= function_namespace_qtopcuatestmodel_generated_0_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_1_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_2_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_3_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_4_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_5_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_6_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_7_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_8_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_9_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_10_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_11_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_12_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_13_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_14_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_15_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_16_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_17_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_18_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_19_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_20_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_21_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_22_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_23_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_24_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_25_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_26_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_27_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_28_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_29_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_30_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_31_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_32_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_33_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_34_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_35_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_36_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_37_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_38_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_39_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_40_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_41_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_42_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_43_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_44_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_45_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_46_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_47_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_48_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_49_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_50_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_51_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_52_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_53_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_54_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_55_begin(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_55_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_54_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_53_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_52_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_51_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_50_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_49_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_48_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_47_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_46_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_45_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_44_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_43_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_42_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_41_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_40_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_39_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_38_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_37_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_36_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_35_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_34_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_33_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_32_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_31_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_30_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_29_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_28_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_27_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_26_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_25_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_24_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_23_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_22_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_21_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_20_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_19_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_18_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_17_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_16_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_15_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_14_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_13_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_12_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_11_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_10_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_9_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_8_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_7_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_6_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_5_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_4_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_3_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_2_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_1_finish(server, &nsMapping);
retVal |= function_namespace_qtopcuatestmodel_generated_0_finish(server, &nsMapping);
return retVal;
}
