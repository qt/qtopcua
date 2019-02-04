
/* This work is licensed under a Creative Commons CCZero 1.0 Universal License.
 * See http://creativecommons.org/publicdomain/zero/1.0/ for more information.
 *
 *    Copyright 2017 (c) Julius Pfrommer, Fraunhofer IOSB
 *    Copyright 2017 (c) Julian Grothoff
 *    Copyright 2017-2018 (c) Mark Giraud, Fraunhofer IOSB
 *    Copyright 2017 (c) Stefan Profanter, fortiss GmbH
 *    Copyright 2017 (c) Thomas Stalder, Blue Time Concept SA
 *    Copyright 2018 (c) Daniel Feist, Precitec GmbH & Co. KG
 */

//
// This code was taken from the open62541 project from file plugins/ua_config_default.c
//

#include "open62541.h"

UA_StatusCode createSecurityPolicyNoneEndpoint(UA_ServerConfig *conf, UA_Endpoint *endpoint, const UA_ByteString localCertificate);
UA_StatusCode createSecurityPolicyBasic128Rsa15Endpoint(UA_ServerConfig *const conf, UA_Endpoint *endpoint, UA_MessageSecurityMode securityMode,
                                          const UA_ByteString localCertificate, const UA_ByteString localPrivateKey);
UA_StatusCode createSecurityPolicyBasic256Sha256Endpoint(UA_ServerConfig *const conf, UA_Endpoint *endpoint, UA_MessageSecurityMode securityMode,
                                           const UA_ByteString localCertificate, const UA_ByteString localPrivateKey);
