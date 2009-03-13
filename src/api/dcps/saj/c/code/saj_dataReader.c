#include "saj_dataReader.h"
#include "saj_dataReaderListener.h"
#include "saj_utilities.h"
#include "saj_qosUtils.h"
#include "saj_status.h"

#define SAJ_FUNCTION(name) Java_org_opensplice_dds_dcps_DataReaderImpl_##name

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniCreateReadcondition
 * Signature: (III)LDDS/ReadCondition;
 */
JNIEXPORT jobject JNICALL
SAJ_FUNCTION(jniCreateReadcondition)(
    JNIEnv *env,
    jobject jdataReader,
    jint jsampleStates,
    jint jviewStates,
    jint jinstanceStates)
{
    jobject jcondition;
    gapi_dataReader dataReader;
    gapi_readCondition condition;
    saj_returnCode rc;

    jcondition = NULL;
    condition = GAPI_OBJECT_NIL;

    dataReader = (gapi_dataReader) saj_read_gapi_address(env, jdataReader);
    condition = gapi_dataReader_create_readcondition(
                                dataReader,
                                (const gapi_sampleStateMask)jsampleStates,
                                (const gapi_viewStateMask)jviewStates,
                                (const gapi_instanceStateMask)jinstanceStates);

    if (condition != GAPI_OBJECT_NIL){
        rc = saj_construct_java_object(env,  PACKAGENAME "ReadConditionImpl",
                                        (PA_ADDRCAST)condition, &jcondition);
    }
    return jcondition;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniCreateQuerycondition
 * Signature: (IIILjava/lang/String;[Ljava/lang/String;)LDDS/QueryCondition;
 */
JNIEXPORT jobject JNICALL
SAJ_FUNCTION(jniCreateQuerycondition)(
    JNIEnv *env,
    jobject jdataReader,
    jint jsampleStates,
    jint jviewStates,
    jint jinstanceStates,
    jstring jexpression,
    jobjectArray jparams)
{
    jobject jcondition;
    gapi_dataReader dataReader;
    gapi_queryCondition condition;
    gapi_stringSeq* params;
    saj_returnCode rc;
    gapi_char* expression;

    jcondition = NULL;
    condition = GAPI_OBJECT_NIL;
    expression = NULL;

    dataReader = (gapi_dataReader) saj_read_gapi_address(env, jdataReader);
    rc = SAJ_RETCODE_OK;
    params = gapi_stringSeq__alloc();

    if(jparams != NULL){
        rc = saj_stringSequenceCopyIn(env, jparams, params);
    }

    if(rc == SAJ_RETCODE_OK){
        if(jexpression != NULL){
            expression = (gapi_char*)(*env)->GetStringUTFChars(env, jexpression, 0);
        }
        condition = gapi_dataReader_create_querycondition(
                                dataReader,
                                (const gapi_sampleStateMask)jsampleStates,
                                (const gapi_viewStateMask)jviewStates,
                                (const gapi_instanceStateMask)jinstanceStates,
                                expression,
                                params);

        if(jexpression != NULL){
            (*env)->ReleaseStringUTFChars(env, jexpression, expression);
        }
        if (condition != GAPI_OBJECT_NIL){
            rc = saj_construct_java_object(env,
                                        PACKAGENAME "QueryConditionImpl",
                                        (PA_ADDRCAST)condition, &jcondition);
        }
    }
    gapi_free(params);

    return jcondition;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniDeleteReadcondition
 * Signature: (LDDS/ReadCondition;)I
 */
JNIEXPORT jint JNICALL
SAJ_FUNCTION(jniDeleteReadcondition)(
    JNIEnv *env,
    jobject jdataReader,
    jobject jcondition)
{
    gapi_readCondition condition;
    gapi_dataReader dataReader;
    gapi_returnCode_t grc;
    saj_userData ud;

    condition = (gapi_readCondition) saj_read_gapi_address(env, jcondition);
    dataReader = (gapi_dataReader) saj_read_gapi_address(env, jdataReader);

    ud = saj_userData(gapi_object_get_user_data(condition));
    grc = gapi_dataReader_delete_readcondition(dataReader, condition);

    if(grc == GAPI_RETCODE_OK){
        saj_destroy_user_data(env, ud);
    }
    return (jint)grc;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniDeleteContainedEntities
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
SAJ_FUNCTION(jniDeleteContainedEntities)(
    JNIEnv *env,
    jobject jdataReader)
{
    gapi_dataReader dataReader;

    dataReader = (gapi_dataReader)saj_read_gapi_address(env, jdataReader);

    return (jint)gapi_dataReader_delete_contained_entities(dataReader,
                                        saj_destroy_user_data_callback, (void *)env);
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniSetQos
 * Signature: (LDDS/DataReaderQos;)I
 */
JNIEXPORT jint JNICALL
SAJ_FUNCTION(jniSetQos)(
    JNIEnv *env,
    jobject jdataReader,
    jobject jqos)
{
    gapi_dataReaderQos* qos;
    gapi_dataReader dataReader;
    saj_returnCode rc;
    jint result;

    dataReader = (gapi_dataReader)saj_read_gapi_address(env, jdataReader);

    qos = gapi_dataReaderQos__alloc();
    rc = saj_DataReaderQosCopyIn(env, jqos, qos);
    result = (jint)GAPI_RETCODE_ERROR;

    if(rc == SAJ_RETCODE_OK){
        result = (jint)gapi_dataReader_set_qos(dataReader, qos);
    }
    gapi_free(qos);

    return result;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniGetQos
 * Signature: (LDDS/DataReaderQosHolder;)V
 */
JNIEXPORT jint JNICALL
SAJ_FUNCTION(jniGetQos)(
    JNIEnv *env,
    jobject jdataReader,
    jobject jqosHolder)
{
    gapi_dataReaderQos* qos;
    gapi_returnCode_t result;
    saj_returnCode rc;
    jobject jqos;
    gapi_dataReader dataReader;

    if(jqosHolder != NULL){
        dataReader = (gapi_dataReader)saj_read_gapi_address(env, jdataReader);
        jqos = NULL;
        qos = gapi_dataReaderQos__alloc();
        result = gapi_dataReader_get_qos(dataReader, qos);

        if(result == GAPI_RETCODE_OK){
            rc = saj_DataReaderQosCopyOut(env, qos, &jqos);
            gapi_free(qos);

            if(rc == SAJ_RETCODE_OK){
                (*env)->SetObjectField(env, jqosHolder,
                        GET_CACHED(dataReaderQosHolder_value_fid), jqos);
                (*env)->DeleteLocalRef(env, jqos);
            } else {
                result = GAPI_RETCODE_ERROR;
            }
        }
    } else {
        result = GAPI_RETCODE_BAD_PARAMETER;
    }
    return (jint)result;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniSetListener
 * Signature: (LDDS/DataReaderListener;I)I
 */
JNIEXPORT jint JNICALL
SAJ_FUNCTION(jniSetListener)(
    JNIEnv *env,
    jobject jdataReader,
    jobject jlistener,
    jint jmask)
{
    struct gapi_dataReaderListener *listener;
    gapi_dataReader dataReader;
    gapi_returnCode_t grc;

    dataReader = (gapi_dataReader)saj_read_gapi_address(env, jdataReader);

    listener = saj_dataReaderListenerNew(env, jlistener);
    if(listener != NULL){
        saj_write_java_listener_address(env, dataReader, listener->listener_data);
    }
    grc = gapi_dataReader_set_listener(dataReader, listener,
                                                    (unsigned long int)jmask);

    if((grc != GAPI_RETCODE_OK) && (listener != NULL)){
        saj_listenerDataFree(env, saj_listenerData(listener->listener_data));
    }
    return (jint)grc;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniGetListener
 * Signature: ()LDDS/DataReaderListener;
 */
JNIEXPORT jobject JNICALL
SAJ_FUNCTION(jniGetListener)(
    JNIEnv *env,
    jobject jdataReader)
{
    jobject jlistener;
    struct gapi_dataReaderListener listener;
    gapi_dataReader dataReader;

    jlistener = NULL;
    dataReader = (gapi_dataReader)saj_read_gapi_address(env, jdataReader);
    listener = gapi_dataReader_get_listener(dataReader);

    jlistener = saj_read_java_listener_address(dataReader);

    return jlistener;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniGetTopicdescription
 * Signature: ()LDDS/TopicDescription;
 */
JNIEXPORT jobject JNICALL
SAJ_FUNCTION(jniGetTopicdescription)(
    JNIEnv *env,
    jobject jdataReader)
{
    jobject jdescription;
    gapi_topicDescription description;
    gapi_dataReader dataReader;

    jdescription = NULL;
    description = GAPI_OBJECT_NIL;

    dataReader = (gapi_dataReader)saj_read_gapi_address(env, jdataReader);
    description = gapi_dataReader_get_topicdescription(dataReader);

    if (description != GAPI_OBJECT_NIL){
        jdescription = saj_read_java_address(description);
    }
    return jdescription;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniGetSubscriber
 * Signature: ()LDDS/Subscriber;
 */
JNIEXPORT jobject JNICALL
SAJ_FUNCTION(jniGetSubscriber)(
    JNIEnv *env,
    jobject jdataReader)
{
    jobject jsubscriber;
    gapi_subscriber subscriber;
    gapi_dataReader dataReader;

    jsubscriber = NULL;
    subscriber = GAPI_OBJECT_NIL;

    dataReader = (gapi_dataReader)saj_read_gapi_address(env, jdataReader);
    subscriber = gapi_dataReader_get_subscriber(dataReader);

    if (subscriber != GAPI_OBJECT_NIL){
        jsubscriber = saj_read_java_address(subscriber);
    }
    return jsubscriber;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniGetSampleRejectedStatus
 * Signature: ()LDDS/SampleRejectedStatus;
 */
JNIEXPORT jint JNICALL
SAJ_FUNCTION(jniGetSampleRejectedStatus)(
    JNIEnv *env,
    jobject jdataReader,
    jobject jstatusHolder)
{
    gapi_dataReader dataReader;
    jobject jstatus;
    gapi_sampleRejectedStatus status;
    saj_returnCode rc;
    gapi_returnCode_t result;

    if(jstatusHolder){
        dataReader = (gapi_dataReader) saj_read_gapi_address(env, jdataReader);
        result = gapi_dataReader_get_sample_rejected_status(dataReader, &status);

        if(result == GAPI_RETCODE_OK){
            rc = saj_statusCopyOutSampleRejectedStatus(env, &status, &jstatus);

            if(rc == SAJ_RETCODE_OK){
                (*env)->SetObjectField(env, jstatusHolder,
                            GET_CACHED(sampleRejectedStatusHolder_value_fid), jstatus);
                (*env)->DeleteLocalRef(env, jstatus);
            } else {
                result = GAPI_RETCODE_ERROR;
            }
        }
    } else {
        result = GAPI_RETCODE_BAD_PARAMETER;
    }
    return (jint)result;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniGetLivelinessChangedStatus
 * Signature: ()LDDS/LivelinessChangedStatus;
 */
JNIEXPORT jint JNICALL
SAJ_FUNCTION(jniGetLivelinessChangedStatus)(
    JNIEnv *env,
    jobject jdataReader,
    jobject jstatusHolder)
{
    gapi_dataReader dataReader;
    jobject jstatus;
    gapi_returnCode_t result;
    gapi_livelinessChangedStatus status;
    saj_returnCode rc;

    if(jstatusHolder){
        dataReader = (gapi_dataReader) saj_read_gapi_address(env, jdataReader);
        result = gapi_dataReader_get_liveliness_changed_status(dataReader, &status);

        if(result == GAPI_RETCODE_OK){
            rc = saj_statusCopyOutLivelinessChangedStatus(env, &status, &jstatus);

            if(rc == SAJ_RETCODE_OK){
                (*env)->SetObjectField(env, jstatusHolder,
                            GET_CACHED(livelinessChangedStatusHolder_value_fid), jstatus);
                (*env)->DeleteLocalRef(env, jstatus);
            } else {
                result = GAPI_RETCODE_ERROR;
            }
        }
    } else {
        result = GAPI_RETCODE_BAD_PARAMETER;
    }
    return (jint)result;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniGetRequestedDeadlineMissedStatus
 * Signature: ()LDDS/RequestedDeadlineMissedStatus;
 */
JNIEXPORT jint JNICALL
SAJ_FUNCTION(jniGetRequestedDeadlineMissedStatus)(
    JNIEnv *env,
    jobject jdataReader,
    jobject jstatusHolder)
{
    gapi_dataReader dataReader;
    jobject jstatus;
    gapi_requestedDeadlineMissedStatus status;
    saj_returnCode rc;
    gapi_returnCode_t result;

    if(jstatusHolder){
        dataReader = (gapi_dataReader) saj_read_gapi_address(env, jdataReader);
        result = gapi_dataReader_get_requested_deadline_missed_status(dataReader, &status);

        if(result == GAPI_RETCODE_OK){
            rc = saj_statusCopyOutRequestedDeadlineMissedStatus(env, &status, &jstatus);

            if(rc == SAJ_RETCODE_OK){
                (*env)->SetObjectField(env, jstatusHolder,
                            GET_CACHED(requestedDeadlineMissedStatusHolder_value_fid), jstatus);
                (*env)->DeleteLocalRef(env, jstatus);
            } else {
                result = GAPI_RETCODE_ERROR;
            }
        }
    } else {
        result = GAPI_RETCODE_BAD_PARAMETER;
    }
    return (jint)result;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniGetRequestedIncompatibleQosStatus
 * Signature: ()LDDS/RequestedIncompatibleQosStatus;
 */
JNIEXPORT jint JNICALL
SAJ_FUNCTION(jniGetRequestedIncompatibleQosStatus)(
    JNIEnv *env,
    jobject jdataReader,
    jobject jstatusHolder)
{
    gapi_dataReader dataReader;
    jobject jstatus;
    gapi_requestedIncompatibleQosStatus* status;
    saj_returnCode rc;
    gapi_returnCode_t result;

    if(jstatusHolder){
        dataReader = (gapi_dataReader) saj_read_gapi_address(env, jdataReader);
        status = gapi_requestedIncompatibleQosStatus_alloc();
        result = gapi_dataReader_get_requested_incompatible_qos_status(dataReader, status);

        if(result == GAPI_RETCODE_OK){
            rc = saj_statusCopyOutRequestedIncompatibleQosStatus(env, status, &jstatus);

            if(rc == SAJ_RETCODE_OK){
                (*env)->SetObjectField(env, jstatusHolder,
                            GET_CACHED(requestedIncompatibleQosStatusHolder_value_fid), jstatus);
                (*env)->DeleteLocalRef(env, jstatus);
            } else {
                result = GAPI_RETCODE_ERROR;
            }
        }
        gapi_free(status);
    } else {
        result = GAPI_RETCODE_BAD_PARAMETER;
    }
    return (jint)result;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniGetSubscriptionMatchStatus
 * Signature: ()LDDS/SubscriptionMatchStatus;
 */
JNIEXPORT jint JNICALL
SAJ_FUNCTION(jniGetSubscriptionMatchedStatus)(
    JNIEnv *env,
    jobject jdataReader,
    jobject jstatusHolder)
{
    gapi_dataReader dataReader;
    jobject jstatus;
    gapi_subscriptionMatchedStatus status;
    saj_returnCode rc;
    gapi_returnCode_t result;

    if(jstatusHolder){
        dataReader = (gapi_dataReader) saj_read_gapi_address(env, jdataReader);
        result = gapi_dataReader_get_subscription_matched_status(dataReader, &status);

        if(result == GAPI_RETCODE_OK){
            rc = saj_statusCopyOutSubscriptionMatchStatus(env, &status, &jstatus);

            if(rc == SAJ_RETCODE_OK){
                (*env)->SetObjectField(env, jstatusHolder,
                            GET_CACHED(subscriptionMatchedStatusHolder_value_fid), jstatus);
                (*env)->DeleteLocalRef(env, jstatus);
            } else {
                result = GAPI_RETCODE_ERROR;
            }
        }
    } else {
        result = GAPI_RETCODE_BAD_PARAMETER;
    }
    return (jint)result;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniGetSampleLostStatus
 * Signature: ()LDDS/SampleLostStatus;
 */
JNIEXPORT jint JNICALL
SAJ_FUNCTION(jniGetSampleLostStatus)(
    JNIEnv *env,
    jobject jdataReader,
    jobject jstatusHolder)
{
    gapi_dataReader dataReader;
    jobject jstatus;
    gapi_sampleLostStatus status;
    saj_returnCode rc;
    gapi_returnCode_t result;

    if(jstatusHolder){
        dataReader = (gapi_dataReader) saj_read_gapi_address(env, jdataReader);
        result = gapi_dataReader_get_sample_lost_status(dataReader, &status);

        if(result == GAPI_RETCODE_OK){
            rc = saj_statusCopyOutSampleLostStatus(env, &status, &jstatus);

            if(rc == SAJ_RETCODE_OK){
                (*env)->SetObjectField(env, jstatusHolder,
                            GET_CACHED(sampleLostStatusHolder_value_fid), jstatus);
                (*env)->DeleteLocalRef(env, jstatus);
            } else {
                result = GAPI_RETCODE_ERROR;
            }
        }
    } else {
        result = GAPI_RETCODE_BAD_PARAMETER;
    }
    return (jint)result;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniWaitForHistoricalData
 * Signature: (LDDS/Duration_t;)I
 */
JNIEXPORT jint JNICALL
SAJ_FUNCTION(jniWaitForHistoricalData)(
    JNIEnv *env,
    jobject jdataReader,
    jobject jduration)
{
    gapi_dataReader dataReader;
    gapi_duration_t* duration;
    saj_returnCode rc;
    jint jresult;

    jresult = (jint)GAPI_RETCODE_BAD_PARAMETER;
    dataReader = (gapi_dataReader)saj_read_gapi_address(env, jdataReader);

    if(jduration != NULL){
        rc = SAJ_RETCODE_ERROR;
        duration = gapi_duration_t__alloc();
        rc = saj_durationCopyIn(env, jduration, duration);

        if(rc == SAJ_RETCODE_OK){
            jresult = (jint)gapi_dataReader_wait_for_historical_data(
                                                            dataReader, duration);
        }
        gapi_free(duration);
    }
    return jresult;
}

JNIEXPORT jint JNICALL
SAJ_FUNCTION(jniWaitForHistoricalDataWCondition)(
    JNIEnv *env,
    jobject jdataReader,
    jstring jfilterExpression,
    jobjectArray jfilterParameters,
    jobject jminSourceTimestamp,
    jobject jmaxSourceTimestamp,
    jobject jresourceLimits,
    jobject jduration)
{
    gapi_dataReader dataReader;
    gapi_duration_t duration;
    saj_returnCode rc;
    gapi_char* filterExpression;
    gapi_stringSeq* filterParameters;
    jint jresult;
    gapi_time_t minSourceTimestamp, maxSourceTimestamp;
    gapi_resourceLimitsQosPolicy resourceLimits;

    if(jduration && jminSourceTimestamp && jmaxSourceTimestamp && jresourceLimits){
        dataReader = (gapi_dataReader)saj_read_gapi_address(env, jdataReader);
        saj_timeCopyIn(env, jminSourceTimestamp, &minSourceTimestamp);
        saj_timeCopyIn(env, jmaxSourceTimestamp, &maxSourceTimestamp);
        saj_ResourceLimitsQosPolicyCopyIn(env, jresourceLimits, &resourceLimits);
        saj_durationCopyIn(env, jduration, &duration);

        if(jfilterParameters){
            filterParameters = gapi_stringSeq__alloc();

            if(filterParameters){
                rc = saj_stringSequenceCopyIn(env, jfilterParameters, filterParameters);
            } else {
                rc = SAJ_RETCODE_ERROR;
                jresult = (jint)GAPI_RETCODE_OUT_OF_RESOURCES;
            }
        } else {
            filterParameters = NULL;
            rc = SAJ_RETCODE_OK;
        }

        if(rc == SAJ_RETCODE_OK){
            if(jfilterExpression != NULL){
                filterExpression = (gapi_char*)
                    (*env)->GetStringUTFChars(env, jfilterExpression, 0);
            } else {
                filterExpression = NULL;
            }
            jresult = (jint)gapi_dataReader_wait_for_historical_data_w_condition(
                    dataReader, filterExpression, filterParameters,
                    &minSourceTimestamp, &maxSourceTimestamp,
                    &resourceLimits, &duration);

            if(jfilterExpression){
                (*env)->ReleaseStringUTFChars(env, jfilterExpression, filterExpression);
            }
            if(filterParameters){
                gapi_free(filterParameters);
            }
        }
    } else {
        jresult = (jint)GAPI_RETCODE_BAD_PARAMETER;
    }
    return jresult;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniGetMatchedPublications
 * Signature: (LDDS/InstanceHandleSeqHolder;)I
 */
JNIEXPORT jint JNICALL
SAJ_FUNCTION(jniGetMatchedPublications)(
    JNIEnv *env,
    jobject jdataReader,
    jobject jseqHolder)
{
    gapi_dataReader dataReader;
    gapi_instanceHandleSeq *publication_handles;
    saj_returnCode rc;
    jint jresult;
    jintArray jarray;

    if(jseqHolder != NULL){
        dataReader = (gapi_dataReader)saj_read_gapi_address(env, jdataReader);
        publication_handles = gapi_instanceHandleSeq__alloc();

        jresult = (jint)gapi_dataReader_get_matched_publications(
                                                dataReader, publication_handles);
        rc = saj_instanceHandleSequenceCopyOut(env, publication_handles, &jarray);
        gapi_free(publication_handles);

        if(rc == SAJ_RETCODE_OK){
            (*env)->SetObjectField(env, jseqHolder,
                    GET_CACHED(instanceHandleSeqHolder_value_fid), jarray);
            (*env)->DeleteLocalRef(env, jarray);
        } else {
            jresult = (jint)GAPI_RETCODE_ERROR;
        }
    } else {
        jresult = (jint)GAPI_RETCODE_BAD_PARAMETER;
    }
    return jresult;
}

/**
 * Class:     org_opensplice_dds_dcps_DataReaderImpl
 * Method:    jniGetMatchedPublicationData
 * Signature: (LDDS/PublicationBuiltinTopicDataHolder;I)I
 */
JNIEXPORT jint JNICALL
SAJ_FUNCTION(jniGetMatchedPublicationData)(
    JNIEnv *env,
    jobject jdataReader,
    jobject jdataHolder,
    jlong jhandle)
{
    gapi_dataReader dataReader;
    gapi_publicationBuiltinTopicData *publication_data;
    gapi_returnCode_t rc;
    jint jresult;
    jobjectArray jarray;

    if(jdataHolder != NULL){
        jarray = NULL;
        dataReader = (gapi_dataReader)saj_read_gapi_address(env, jdataReader);
        publication_data = gapi_publicationBuiltinTopicData__alloc();

        rc = gapi_dataReader_get_matched_publication_data(
                                        dataReader, publication_data,
                                        (const gapi_instanceHandle_t)jhandle);
        /**
         * @todo TODO: Copy publication_data to Java object array.
         */
        gapi_free(publication_data);

        if(rc == GAPI_RETCODE_OK){
            (*env)->SetObjectField(env, jdataHolder,
                    GET_CACHED(publicationBuiltinTopicDataHolder_value_fid), jarray);
            (*env)->DeleteLocalRef(env, jarray);
        }
        jresult = (jint)rc;
    } else {
        jresult = (jint)GAPI_RETCODE_BAD_PARAMETER;
    }
    return jresult;
}

#undef SAJ_FUNCTION