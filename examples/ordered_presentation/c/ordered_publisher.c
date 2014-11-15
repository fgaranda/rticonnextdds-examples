/*******************************************************************************
 (c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/
/* ordered_publisher.c

 A publication of data of type ordered

 This file is derived from code automatically generated by the rtiddsgen
 command:

 rtiddsgen -language C -example <arch> ordered.idl

 Example publication of type ordered automatically generated by
 'rtiddsgen'. To test them follow these steps:

 (1) Compile this file and the example subscription.

 (2) Start the subscription with the command
 objs/<arch>/ordered_subscriber <domain_id> <sample_count>

 (3) Start the publication with the command
 objs/<arch>/ordered_publisher <domain_id> <sample_count>

 (4) [Optional] Specify the list of discovery initial peers and
 multicast receive addresses via an environment variable or a file
 (in the current working directory) called NDDS_DISCOVERY_PEERS.

 You can run any number of publishers and subscribers programs, and can
 add and remove them dynamically from the domain.


 Example:

 To run the example application on domain <domain_id>:

 On Unix:

 objs/<arch>/ordered_publisher <domain_id>
 objs/<arch>/ordered_subscriber <domain_id>

 On Windows:

 objs\<arch>\ordered_publisher <domain_id>
 objs\<arch>\ordered_subscriber <domain_id>


 modification history
 ------------ -------
 */

#include <stdio.h>
#include <stdlib.h>
#include "ndds/ndds_c.h"
#include "ordered.h"
#include "orderedSupport.h"

/* Delete all entities */
static int publisher_shutdown(DDS_DomainParticipant *participant,
        struct DDS_PublisherQos *publisher_qos) {
    DDS_ReturnCode_t retcode;
    int status = 0;

    if (participant != NULL) {
        retcode = DDS_DomainParticipant_delete_contained_entities(participant);
        if (retcode != DDS_RETCODE_OK) {
            printf("delete_contained_entities error %d\n", retcode);
            status = -1;
        }

        retcode = DDS_DomainParticipantFactory_delete_participant(
                DDS_TheParticipantFactory, participant);
        if (retcode != DDS_RETCODE_OK) {
            printf("delete_participant error %d\n", retcode);
            status = -1;
        }

        retcode = DDS_PublisherQos_finalize(publisher_qos);
        if (retcode != DDS_RETCODE_OK) {
            printf("publisherQos_finalize error %d\n", retcode);
            status = -1;
        }
    }

    /* RTI Connext provides finalize_instance() method on
     domain participant factory for people who want to release memory used
     by the participant factory. Uncomment the following block of code for
     clean destruction of the singleton. */
    /*
     retcode = DDS_DomainParticipantFactory_finalize_instance();
     if (retcode != DDS_RETCODE_OK) {
     printf("finalize_instance error %d\n", retcode);
     status = -1;
     }
     */

    return status;
}

static int publisher_main(int domainId, int sample_count) {
    DDS_DomainParticipant *participant = NULL;
    DDS_Publisher *publisher = NULL;
    DDS_Topic *topic = NULL;
    DDS_DataWriter *writer = NULL;
    orderedDataWriter *ordered_writer = NULL;
    ordered *instance0 = NULL;
    ordered *instance1 = NULL;
    DDS_ReturnCode_t retcode;
    DDS_InstanceHandle_t instance_handle0 = DDS_HANDLE_NIL;
    DDS_InstanceHandle_t instance_handle1 = DDS_HANDLE_NIL;
    const char *type_name = NULL;
    int count = 0;
    struct DDS_Duration_t send_period = { 1, 0 };
    struct DDS_PublisherQos publisher_qos = DDS_PublisherQos_INITIALIZER;

    /* To customize participant QoS, use 
     the configuration file USER_QOS_PROFILES.xml */
    participant = DDS_DomainParticipantFactory_create_participant(
            DDS_TheParticipantFactory, domainId, &DDS_PARTICIPANT_QOS_DEFAULT,
            NULL /* listener */, DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        printf("create_participant error\n");
        publisher_shutdown(participant, &publisher_qos);
        return -1;
    }

    publisher = DDS_DomainParticipant_create_publisher_with_profile(participant,
            "ordered_Library", "ordered_Profile_subscriber_instance",
            NULL /* listener */, DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
        printf("create_publisher error\n");
        publisher_shutdown(participant, &publisher_qos);
        return -1;
    }

    /* If you want to change the Publisher's QoS programmatically rather than
     * using the XML file, you will need to add the following lines to your
     * code and comment out the create_publisher call above.
     *
     * In this case, we set the presentation publish mode ordered in the topic.
     */
    /*
    retcode = DDS_DomainParticipant_get_default_publisher_qos(participant,
            &publisher_qos);
    if (retcode != DDS_RETCODE_OK) {
        printf("get_default_publisher_qos error\n");
        return -1;
    }

    publisher_qos.presentation.access_scope = DDS_TOPIC_PRESENTATION_QOS;
    publisher_qos.presentation.ordered_access = DDS_BOOLEAN_TRUE;

    publisher = DDS_DomainParticipant_create_publisher(participant,
            &publisher_qos, NULL, DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
        printf("create_publisher error\n");
        publisher_shutdown(participant, &publisher_qos);
        return -1;
    }
     */
    /* Register type before creating topic */
    type_name = orderedTypeSupport_get_type_name();
    retcode = orderedTypeSupport_register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        printf("register_type error %d\n", retcode);
        publisher_shutdown(participant, &publisher_qos);
        return -1;
    }

    /* To customize topic QoS, use 
     the configuration file USER_QOS_PROFILES.xml */
    topic = DDS_DomainParticipant_create_topic(participant, "Example ordered",
            type_name, &DDS_TOPIC_QOS_DEFAULT, NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        printf("create_topic error\n");
        publisher_shutdown(participant, &publisher_qos);
        return -1;
    }

    /* To customize data writer QoS, use 
     the configuration file USER_QOS_PROFILES.xml */
    writer = DDS_Publisher_create_datawriter(publisher, topic,
            &DDS_DATAWRITER_QOS_DEFAULT, NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (writer == NULL) {
        printf("create_datawriter error\n");
        publisher_shutdown(participant, &publisher_qos);
        return -1;
    }
    ordered_writer = orderedDataWriter_narrow(writer);
    if (ordered_writer == NULL) {
        printf("DataWriter narrow error\n");
        publisher_shutdown(participant, &publisher_qos);
        return -1;
    }
    /* Start changes for ordered_presentation */
    /* Create data sample for writing */

    instance0 = orderedTypeSupport_create_data_ex(DDS_BOOLEAN_TRUE);
    instance1 = orderedTypeSupport_create_data_ex(DDS_BOOLEAN_TRUE);
    if (instance0 == NULL || instance1 == NULL) {
        printf("orderedTypeSupport_create_data error\n");
        publisher_shutdown(participant, &publisher_qos);
        return -1;
    }

    /* For a data type that has a key, if the same instance is going to be
     written multiple times, initialize the key here
     and register the keyed instance prior to writing */

    instance0->id = 0;
    instance1->id = 1;

    instance_handle0 = orderedDataWriter_register_instance(ordered_writer,
            instance0);
    instance_handle1 = orderedDataWriter_register_instance(ordered_writer,
            instance1);

    /* Main loop */
    for (count = 0; (sample_count == 0) || (count < sample_count); ++count) {
        /* Modify the data to be written here */
        instance0->value = count;
        instance1->value = count;

        printf("Writing instance0, value->%d\n", instance0->value);
        retcode = orderedDataWriter_write(ordered_writer, instance0,
                &instance_handle0);
        if (retcode != DDS_RETCODE_OK) {
            printf("write instance0 error %d\n", retcode);
        }

        printf("Writing instance1, value->%d\n", instance1->value);
        retcode = orderedDataWriter_write(ordered_writer, instance1,
                &instance_handle1);
        if (retcode != DDS_RETCODE_OK) {
            printf("write instance0 error %d\n", retcode);
        }

        NDDS_Utility_sleep(&send_period);
    }

    retcode = orderedDataWriter_unregister_instance(ordered_writer, instance0,
            &instance_handle0);
    if (retcode != DDS_RETCODE_OK) {
        printf("unregister instance error %d\n", retcode);
    }

    retcode = orderedDataWriter_unregister_instance(ordered_writer, instance1,
            &instance_handle1);
    if (retcode != DDS_RETCODE_OK) {
        printf("unregister instance error %d\n", retcode);
    }

    /* Delete data sample */
    retcode = orderedTypeSupport_delete_data_ex(instance0, DDS_BOOLEAN_TRUE);
    if (retcode != DDS_RETCODE_OK) {
        printf("orderedTypeSupport_delete_data instance0 error %d\n", retcode);
    }

    retcode = orderedTypeSupport_delete_data_ex(instance1, DDS_BOOLEAN_TRUE);
    if (retcode != DDS_RETCODE_OK) {
        printf("orderedTypeSupport_delete_data instance1 error %d\n", retcode);
    }

    /* Cleanup and delete delete all entities */
    return publisher_shutdown(participant, &publisher_qos);
}

#if defined(RTI_WINCE)
int wmain(int argc, wchar_t** argv)
{
    int domainId = 0;
    int sample_count = 0; /* infinite loop */

    if (argc >= 2) {
        domainId = _wtoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = _wtoi(argv[2]);
    }

    /* Uncomment this to turn on additional logging
     NDDS_Config_Logger_set_verbosity_by_category(
     NDDS_Config_Logger_get_instance(),
     NDDS_CONFIG_LOG_CATEGORY_API,
     NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
     */

    return publisher_main(domainId, sample_count);
}
#elif !(defined(RTI_VXWORKS) && !defined(__RTP__)) && !defined(RTI_PSOS)
int main(int argc, char *argv[]) {
    int domainId = 0;
    int sample_count = 0; /* infinite loop */

    if (argc >= 2) {
        domainId = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    /* Uncomment this to turn on additional logging
     NDDS_Config_Logger_set_verbosity_by_category(
     NDDS_Config_Logger_get_instance(),
     NDDS_CONFIG_LOG_CATEGORY_API,
     NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
     */

    return publisher_main(domainId, sample_count);
}
#endif

#ifdef RTI_VX653
const unsigned char* __ctype = NULL;

void usrAppInit ()
{
#ifdef  USER_APPL_INIT
    USER_APPL_INIT; /* for backwards compatibility */
#endif

    /* add application specific code here */
    taskSpawn("pub", RTI_OSAPI_THREAD_PRIORITY_NORMAL, 0x8, 0x150000,
            (FUNCPTR)publisher_main, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

}
#endif
