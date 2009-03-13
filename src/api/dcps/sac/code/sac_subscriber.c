
#include <gapi.h>

#include "dds_dcps.h"
#include "sac_structured.h"

/*     DataReader
 *     create_datareader(
 *         in TopicDescription a_topic,
 *         in DataReaderQos qos,
 *         in DataReaderListener a_listener);
 */
    DDS_DataReader
    DDS_Subscriber_create_datareader (
        DDS_Subscriber this,
        const DDS_TopicDescription a_topic,
        const DDS_DataReaderQos *qos,
        const struct DDS_DataReaderListener *a_listener,
        const DDS_StatusMask mask
        )
{
    struct gapi_dataReaderListener gListener;
    struct gapi_dataReaderListener *pListener = NULL;

    if ( a_listener ) {
        sac_copySacDataReaderListener(a_listener, &gListener);
        pListener = &gListener;
    }

    return (DDS_DataReader)
	gapi_subscriber_create_datareader (
	    (gapi_subscriber)this,
	    (gapi_topicDescription)a_topic,
	    (const gapi_dataReaderQos *)qos,
	    (const struct gapi_dataReaderListener *)pListener,
	    (gapi_statusMask) mask
	);
}

/*     ReturnCode_t
 *     delete_datareader(
 *         in DataReader a_datareader);
 */
DDS_ReturnCode_t
DDS_Subscriber_delete_datareader (
    DDS_Subscriber this,
    const DDS_DataReader a_datareader
    )
{
    return (DDS_ReturnCode_t)
	gapi_subscriber_delete_datareader (
	    (gapi_subscriber)this,
	    (gapi_dataReader)a_datareader
	);
}

/*     ReturnCode_t
 *     delete_contained_entities();
 */
DDS_ReturnCode_t
DDS_Subscriber_delete_contained_entities (
    DDS_Subscriber this
    )
{
    return (DDS_ReturnCode_t)
	gapi_subscriber_delete_contained_entities (
	    (gapi_subscriber)this, NULL, NULL
	);
}

/*     DataReader
 *     lookup_datareader(
 *         in string topic_name);
 */
DDS_DataReader
DDS_Subscriber_lookup_datareader (
    DDS_Subscriber this,
    const DDS_char *topic_name
    )
{
    return (DDS_DataReader)
	gapi_subscriber_lookup_datareader (
	    (gapi_subscriber)this,
	    (const gapi_char *)topic_name
	);
}

/*     ReturnCode_t
 *     get_datareaders(
 *         out DataReaderSeq readers,
 *         in SampleStateMask sample_states,
 *         in ViewStateMask view_states,
 *         in InstanceStateMask instance_states);
 */
DDS_ReturnCode_t
DDS_Subscriber_get_datareaders (
    DDS_Subscriber this,
    DDS_DataReaderSeq **readers,
    const DDS_SampleStateMask sample_states,
    const DDS_ViewStateMask view_states,
    const DDS_InstanceStateMask instance_states
    )
{
    return (DDS_ReturnCode_t)
	gapi_subscriber_get_datareaders (
	    (gapi_subscriber)this,
	    (gapi_dataReaderSeq **)readers,
	    (gapi_sampleStateMask)sample_states,
	    (gapi_viewStateMask)view_states,
	    (gapi_instanceStateMask)instance_states
	);
}

/*     ReturnCode_t
 *     notify_datareaders();
 */
    DDS_ReturnCode_t

DDS_Subscriber_notify_datareaders (
    DDS_Subscriber this
    )
{
    return (DDS_ReturnCode_t)
	gapi_subscriber_notify_datareaders (
	(gapi_subscriber)this
    );
}

/*     ReturnCode_t
 *     set_qos(
 *         in SubscriberQos qos);
 */
DDS_ReturnCode_t
DDS_Subscriber_set_qos (
    DDS_Subscriber this,
    const DDS_SubscriberQos *qos
    )
{
    return (DDS_ReturnCode_t)
	gapi_subscriber_set_qos (
	    (gapi_subscriber)this,
	    (const gapi_subscriberQos *)qos
	);
}

/*     ReturnCode_t
 *     get_qos(
 *         inout SubscriberQos qos);
 */
DDS_ReturnCode_t
DDS_Subscriber_get_qos (
    DDS_Subscriber this,
    DDS_SubscriberQos *qos
    )
{
    return (DDS_ReturnCode_t)
    gapi_subscriber_get_qos (
	(gapi_subscriber)this,
	(gapi_subscriberQos *)qos
    );
}

/*     ReturnCode_t
 *     set_listener(
 *         in SubscriberListener a_listener,
 *         in StatusKindMask mask);
 */
DDS_ReturnCode_t
DDS_Subscriber_set_listener (
    DDS_Subscriber this,
    const struct DDS_SubscriberListener *a_listener,
    const DDS_StatusMask mask
    )
{
    struct gapi_subscriberListener gListener;
    struct gapi_subscriberListener *pListener = NULL;

    if ( a_listener ) {
        sac_copySacSubscriberListener(a_listener, &gListener);
        pListener = &gListener;
    }
    
    return (DDS_ReturnCode_t)
	gapi_subscriber_set_listener (
	    (gapi_subscriber)this,
	    (const struct gapi_subscriberListener *)pListener,
	    (gapi_statusMask)mask
	);
}

/*     SubscriberListener
 *     get_listener();
 */
struct DDS_SubscriberListener
DDS_Subscriber_get_listener (
    DDS_Subscriber this
    )
{
    struct DDS_SubscriberListener d;
    struct gapi_subscriberListener s;

    s = gapi_subscriber_get_listener ((gapi_subscriber)this);
    sac_copyGapiSubscriberListener (&s, &d);

    return d;
}

/*     ReturnCode_t
 *     begin_access();
 */
DDS_ReturnCode_t
DDS_Subscriber_begin_access (
    DDS_Subscriber this
    )
{
    return (DDS_ReturnCode_t)
	gapi_subscriber_begin_access (
	    (gapi_subscriber)this
	);
}

/*     ReturnCode_t
 *     end_access();
 */
DDS_ReturnCode_t
DDS_Subscriber_end_access (
    DDS_Subscriber this
    )
{
    return (DDS_ReturnCode_t)
	gapi_subscriber_end_access (
	    (gapi_subscriber)this
	);
}

/*     DomainParticipant
 *     get_participant();
 */
DDS_DomainParticipant
DDS_Subscriber_get_participant (
    DDS_Subscriber this
    )
{
    return (DDS_DomainParticipant)
	gapi_subscriber_get_participant (
	    (gapi_subscriber)this
	);
}

/*     ReturnCode_t
 *     set_default_datareader_qos(
 *         in DataReaderQos qos);
 */
DDS_ReturnCode_t
DDS_Subscriber_set_default_datareader_qos (
    DDS_Subscriber this,
    const DDS_DataReaderQos *qos
    )
{
    return (DDS_ReturnCode_t)
	gapi_subscriber_set_default_datareader_qos (
	    (gapi_subscriber)this,
	    (const gapi_dataReaderQos *)qos
	);
}

/*     ReturnCode_t
 *     get_default_datareader_qos(
 *         inout DataReaderQos qos);
 */
DDS_ReturnCode_t
DDS_Subscriber_get_default_datareader_qos (
    DDS_Subscriber this,
    DDS_DataReaderQos *qos
    )
{
    return (DDS_ReturnCode_t)
    gapi_subscriber_get_default_datareader_qos (
	(gapi_subscriber)this,
	(gapi_dataReaderQos *)qos
    );
}

/*     ReturnCode_t
 *     copy_from_topic_qos(
 *         inout DataReaderQos a_datareader_qos,
 *         in TopicQos a_topic_qos);
 */
DDS_ReturnCode_t
DDS_Subscriber_copy_from_topic_qos (
    DDS_Subscriber this,
    DDS_DataReaderQos *a_datareader_qos,
    const DDS_TopicQos *a_topic_qos
    )
{
    return (DDS_ReturnCode_t)
	gapi_subscriber_copy_from_topic_qos (
	    (gapi_subscriber)this,
	    (gapi_dataReaderQos *)a_datareader_qos,
	    (const gapi_topicQos *)a_topic_qos
	);
}