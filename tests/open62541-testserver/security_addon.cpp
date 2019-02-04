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
// This code was mostly taken from the open62541 project from file plugins/ua_config_default.c
//

#include "security_addon.h"

UA_StatusCode createSecurityPolicyNoneEndpoint(UA_ServerConfig *conf, UA_Endpoint *endpoint, const UA_ByteString localCertificate)
{
    UA_EndpointDescription_init(&endpoint->endpointDescription);

    UA_SecurityPolicy_None(&endpoint->securityPolicy, NULL, localCertificate, conf->logger);
    endpoint->endpointDescription.securityMode = UA_MESSAGESECURITYMODE_NONE;
    endpoint->endpointDescription.securityPolicyUri =
        UA_STRING_ALLOC("http://opcfoundation.org/UA/SecurityPolicy#None");
    endpoint->endpointDescription.transportProfileUri =
        UA_STRING_ALLOC("http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary");

    /* Enable all login mechanisms from the access control plugin  */
    UA_StatusCode retval = UA_Array_copy(conf->accessControl.userTokenPolicies,
                                         conf->accessControl.userTokenPoliciesSize,
                                         (void **)&endpoint->endpointDescription.userIdentityTokens,
                                         &UA_TYPES[UA_TYPES_USERTOKENPOLICY]);
    if (retval != UA_STATUSCODE_GOOD)
        return retval;
    endpoint->endpointDescription.userIdentityTokensSize = conf->accessControl.userTokenPoliciesSize;

    UA_String_copy(&localCertificate, &endpoint->endpointDescription.serverCertificate);
    UA_ApplicationDescription_copy(&conf->applicationDescription,
                                   &endpoint->endpointDescription.server);

    return UA_STATUSCODE_GOOD;
}

UA_StatusCode
createSecurityPolicyBasic128Rsa15Endpoint(UA_ServerConfig *const conf,
                                          UA_Endpoint *endpoint,
                                          UA_MessageSecurityMode securityMode,
                                          const UA_ByteString localCertificate,
                                          const UA_ByteString localPrivateKey) {
    UA_EndpointDescription_init(&endpoint->endpointDescription);

    UA_StatusCode retval =
        UA_SecurityPolicy_Basic128Rsa15(&endpoint->securityPolicy, &conf->certificateVerification,
                                        localCertificate, localPrivateKey, conf->logger);
    if (retval != UA_STATUSCODE_GOOD) {
        endpoint->securityPolicy.deleteMembers(&endpoint->securityPolicy);
        return retval;
    }

    endpoint->endpointDescription.securityMode = securityMode;
    endpoint->endpointDescription.securityPolicyUri =
        UA_STRING_ALLOC("http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15");
    endpoint->endpointDescription.transportProfileUri =
        UA_STRING_ALLOC("http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary");

    /* Enable all login mechanisms from the access control plugin  */
    retval = UA_Array_copy(conf->accessControl.userTokenPolicies,
                           conf->accessControl.userTokenPoliciesSize,
                           (void **)&endpoint->endpointDescription.userIdentityTokens,
                           &UA_TYPES[UA_TYPES_USERTOKENPOLICY]);
    if (retval != UA_STATUSCODE_GOOD)
        return retval;
    endpoint->endpointDescription.userIdentityTokensSize =
        conf->accessControl.userTokenPoliciesSize;

    UA_String_copy(&localCertificate, &endpoint->endpointDescription.serverCertificate);
    UA_ApplicationDescription_copy(&conf->applicationDescription,
                                   &endpoint->endpointDescription.server);

    return UA_STATUSCODE_GOOD;
}

UA_StatusCode
createSecurityPolicyBasic256Sha256Endpoint(UA_ServerConfig *const conf,
                                           UA_Endpoint *endpoint,
                                           UA_MessageSecurityMode securityMode,
                                           const UA_ByteString localCertificate,
                                           const UA_ByteString localPrivateKey) {
    UA_EndpointDescription_init(&endpoint->endpointDescription);

    UA_StatusCode retval =
        UA_SecurityPolicy_Basic256Sha256(&endpoint->securityPolicy, &conf->certificateVerification, localCertificate,
                                         localPrivateKey, conf->logger);
    if (retval != UA_STATUSCODE_GOOD) {
        endpoint->securityPolicy.deleteMembers(&endpoint->securityPolicy);
        return retval;
    }

    endpoint->endpointDescription.securityMode = securityMode;
    endpoint->endpointDescription.securityPolicyUri =
        UA_STRING_ALLOC("http://opcfoundation.org/UA/SecurityPolicy#Basic256Sha256");
    endpoint->endpointDescription.transportProfileUri =
        UA_STRING_ALLOC("http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary");

    /* Enable all login mechanisms from the access control plugin  */
    retval = UA_Array_copy(conf->accessControl.userTokenPolicies,
                           conf->accessControl.userTokenPoliciesSize,
                           (void **)&endpoint->endpointDescription.userIdentityTokens,
                           &UA_TYPES[UA_TYPES_USERTOKENPOLICY]);
    if (retval != UA_STATUSCODE_GOOD)
        return retval;
    endpoint->endpointDescription.userIdentityTokensSize =
        conf->accessControl.userTokenPoliciesSize;

    UA_String_copy(&localCertificate, &endpoint->endpointDescription.serverCertificate);
    UA_ApplicationDescription_copy(&conf->applicationDescription,
                                   &endpoint->endpointDescription.server);

    return UA_STATUSCODE_GOOD;
}
