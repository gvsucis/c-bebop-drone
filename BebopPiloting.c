/**
 * @file BebopPiloting.c
 * @date 10/03/2015
 */

#include <stdlib.h>
#include <curses.h>
#include <string.h>

#include <libARSAL/ARSAL.h>
#include <libARSAL/ARSAL_Print.h>
#include <libARNetwork/ARNetwork.h>
#include <libARNetworkAL/ARNetworkAL.h>
#include <libARCommands/ARCommands.h>
#include <libARDiscovery/ARDiscovery.h>

#include "BebopPiloting.h"
#include "ihm.h"
#include "ARCommandsHighLevel.h"
#include "ARDrone3Settings.h"
#include "callbacks.h" 

static ARNETWORK_IOBufferParam_t c2dParams[] = {
   {
        .ID = JS_NET_CD_NONACK_ID,
        .dataType = ARNETWORKAL_FRAME_TYPE_DATA,
        .sendingWaitTimeMs = 20,
        .ackTimeoutMs = ARNETWORK_IOBUFFERPARAM_INFINITE_NUMBER,
        .numberOfRetry = ARNETWORK_IOBUFFERPARAM_INFINITE_NUMBER,
        .numberOfCell = 2,
        .dataCopyMaxSize = 128,
        .isOverwriting = 1,
    },
    /* Acknowledged commands. */
    {
        .ID = JS_NET_CD_ACK_ID,
        .dataType = ARNETWORKAL_FRAME_TYPE_DATA_WITH_ACK,
        .sendingWaitTimeMs = 20,
        .ackTimeoutMs = 500,
        .numberOfRetry = 3,
        .numberOfCell = 20,
        .dataCopyMaxSize = 128,
        .isOverwriting = 0,
    },
    /* Emergency commands. */
    {
        .ID = BD_NET_CD_EMERGENCY_ID,
        .dataType = ARNETWORKAL_FRAME_TYPE_DATA_WITH_ACK,
        .sendingWaitTimeMs = 10,
        .ackTimeoutMs = 100,
        .numberOfRetry = ARNETWORK_IOBUFFERPARAM_INFINITE_NUMBER,
        .numberOfCell = 1,
        .dataCopyMaxSize = 128,
        .isOverwriting = 0,
    }
};
static const size_t numC2dParams = sizeof(c2dParams) / sizeof(ARNETWORK_IOBufferParam_t);

static ARNETWORK_IOBufferParam_t d2cParams[] = {
    {
        .ID = ((ARNETWORKAL_MANAGER_WIFI_ID_MAX / 2) - 1),
        .dataType = ARNETWORKAL_FRAME_TYPE_DATA,
        .sendingWaitTimeMs = 20,
        .ackTimeoutMs = -1,
        .numberOfRetry = -1,
        .numberOfCell = 10,
        .dataCopyMaxSize = 128,
        .isOverwriting = 0,
    },
    {
        .ID = ((ARNETWORKAL_MANAGER_WIFI_ID_MAX / 2) - 2),
        .dataType = ARNETWORKAL_FRAME_TYPE_DATA_WITH_ACK,
        .sendingWaitTimeMs = 20,
        .ackTimeoutMs = 500,
        .numberOfRetry = 3,
        .numberOfCell = 20,
        .dataCopyMaxSize = 128,
        .isOverwriting = 0,
    }
};
static const size_t numD2cParams = sizeof(d2cParams) / sizeof(ARNETWORK_IOBufferParam_t);

static int commandBufferIds[] = {
    JS_NET_DC_NONACK_ID,
    JS_NET_DC_ACK_ID,
};
static const size_t numOfCommandBufferIds = sizeof(commandBufferIds) / sizeof(int);

static int idToIndex(ARNETWORK_IOBufferParam_t* params, size_t num_params, int id)
{
    int i = 0;
    for (i = 0; i < num_params; i ++)
    {
        if (params[i].ID == id)
        {
            return i;
        }
    }
    return -1;
}

int gIHMRun = 1;
char gErrorStr[ERROR_STR_LENGTH];

int main (int argc, char *argv[])
{
    // local declarations
    int failed = 0;
    DEVICE_MANAGER_t *deviceManager = NULL;
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "-- Bebop Piloting --");
    IHM_t *ihm = IHM_New (&onInputEvent);
    if (ihm != NULL)
    {
        gErrorStr[0] = '\0';
        ARSAL_Print_SetCallback (customPrintCallback); //use a custom callback to print, for not disturb ncurses IHM
        
        IHM_PrintHeader(ihm, "-- Bebop Piloting --");
        registerARCommandsCallbacks (ihm);
    }
    else
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Creation of IHM failed.");
        failed = 1;
    }
    
    if (!failed)
    {
        deviceManager = malloc(sizeof(DEVICE_MANAGER_t));
        if (deviceManager != NULL)
        {
            // initialize deviceManager
            deviceManager->alManager = NULL;
            deviceManager->netManager = NULL;
            deviceManager->rxThread = NULL;
            deviceManager->txThread = NULL;
            deviceManager->d2cPort = JS_D2C_PORT;
            deviceManager->c2dPort = JS_C2D_PORT; //deviceManager->c2dPort = 0; // should be read from json
            
            deviceManager->looperThread = NULL;
            deviceManager->readerThreads = NULL;
            deviceManager->readerThreadsData = NULL;
            
            deviceManager->run = 1;
            
            deviceManager->dataPCMD.active = 0;
            deviceManager->dataPCMD.roll = 0.0;
            deviceManager->dataPCMD.pitch = 0.0;
            deviceManager->dataPCMD.yaw = 0.0;
            deviceManager->dataPCMD.gaz = 0.0;
            deviceManager->dataPCMD.heading = 0.0;
            
            IHM_setCustomData(ihm, deviceManager);
        }
        else
        {
            failed = 1;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "deviceManager alloc error !");
        }
    }

    if (!failed)
    {
        IHM_PrintInfo(ihm, "Connecting ...");
        failed = ardiscoveryConnect (deviceManager);
    }
    
    if (!failed)
    {
        // start network
        failed = startNetwork (deviceManager);
    }
    
    if (!failed)
    {
        // Create and start looper thread.
        if (ARSAL_Thread_Create(&(deviceManager->looperThread), looperRun, deviceManager) != 0)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Creation of looper thread failed.");
            failed = 1;
        }
    }
    
    if (!failed)
    {
        // allocate reader thread array.
        deviceManager->readerThreads = calloc(numOfCommandBufferIds, sizeof(ARSAL_Thread_t));
        
        if (deviceManager->readerThreads == NULL)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Allocation of reader threads failed.");
            failed = 1;
        }
    }
    
    if (!failed)
    {
        // allocate reader thread data array.
        deviceManager->readerThreadsData = calloc(numOfCommandBufferIds, sizeof(READER_THREAD_DATA_t));
        
        if (deviceManager->readerThreadsData == NULL)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Allocation of reader threads data failed.");
            failed = 1;
        }
    }
    
    if (!failed)
    {
        // Create and start reader threads.
        int readerThreadIndex = 0;
        for (readerThreadIndex = 0 ; readerThreadIndex < numOfCommandBufferIds ; readerThreadIndex++)
        {
            // initialize reader thread data
            deviceManager->readerThreadsData[readerThreadIndex].deviceManager = deviceManager;
            deviceManager->readerThreadsData[readerThreadIndex].readerBufferId = commandBufferIds[readerThreadIndex];
            
            if (ARSAL_Thread_Create(&(deviceManager->readerThreads[readerThreadIndex]), readerRun, &(deviceManager->readerThreadsData[readerThreadIndex])) != 0)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Creation of reader thread failed.");
                failed = 1;
            }
        }
    }
    
    if (!failed)
    {
        ARDrone3SendCommonAllStates(deviceManager);
        sleep(1);
        ARDrone3SendPilotingFlatTrim(deviceManager);
        sleep(1);
        ARDrone3SendSpeedSettingsMaxVerticalSpeed(deviceManager, 0.5); //Max Vertical speed in m/s
        sleep(1);
        ARDrone3SendSpeedSettingsMaxRotationSpeed(deviceManager, 51); //Max Rotation speed in degree/s
        sleep(1);
        ARDrone3SendSendSpeedSettingsHullProtection(deviceManager, 1); // Presence of hull protection -   1 if present, 0 if not present
        sleep(1);
        ARDrone3SendPilotingSettingsMaxAltitude(deviceManager, 2); //Max Altitude in meters
        sleep(1);
        ARDrone3SendPilotingSettingsMaxTilt(deviceManager, 6); //Max tilt in degree
        sleep(1);

        IHM_PrintInfo(ihm, "Running ... (Arrow up & down to move forward/backward ; Spacebar to takeoff ; 'w' to ascend, 's' to descend, 'a' to yaw left, 'd' to yaw right and 'q' to land)");
        
        while (gIHMRun)
        {
            usleep(50);
        }
        
        IHM_PrintInfo(ihm, "Disconnecting ...");
    }
    
    if (deviceManager != NULL)
    {
        deviceManager->run = 0; // break threads loops
        
        // Stop looper Thread
        if (deviceManager->looperThread != NULL)
        {
            ARSAL_Thread_Join(deviceManager->looperThread, NULL);
            ARSAL_Thread_Destroy(&(deviceManager->looperThread));
            deviceManager->looperThread = NULL;
        }
        
        if (deviceManager->readerThreads != NULL)
        {
            // Stop reader Threads
            int readerThreadIndex = 0;
            for (readerThreadIndex = 0 ; readerThreadIndex < numD2cParams ; readerThreadIndex++)
            {
                if (deviceManager->readerThreads[readerThreadIndex] != NULL)
                {
                    ARSAL_Thread_Join(deviceManager->readerThreads[readerThreadIndex], NULL);
                    ARSAL_Thread_Destroy(&(deviceManager->readerThreads[readerThreadIndex]));
                    deviceManager->readerThreads[readerThreadIndex] = NULL;
                }
            }
            
            // free reader thread array
            free (deviceManager->readerThreads);
            deviceManager->readerThreads = NULL;
        }
        
        if (deviceManager->readerThreadsData != NULL)
        {
            // free reader thread data array
            free (deviceManager->readerThreadsData);
            deviceManager->readerThreadsData = NULL;
        }
        
        // Stop Network
        stopNetwork (deviceManager);
        
        // free deviceManager
        free (deviceManager);
        deviceManager = NULL;
    }
    
    if (ihm != NULL)
    {
        unregisterARCommandsCallbacks ();
        
        IHM_Delete (&ihm);
        ARSAL_Print_SetCallback (NULL); //reset the callback to print
        
        /* print error */
        if (gErrorStr[0] != '\0')
        {
            //ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "%s", gErrorStr);
            printf("Error: %s", gErrorStr);
        }
    }
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "-- END --");
    
    return EXIT_SUCCESS;
}

/*****************************************
 *
 *             private implementation:
 *
 ****************************************/

int ardiscoveryConnect (DEVICE_MANAGER_t *deviceManager)
{
    int failed = 0;
    
    //ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- ARDiscovery Connection");
    
    eARDISCOVERY_ERROR err = ARDISCOVERY_OK;
    ARDISCOVERY_Connection_ConnectionData_t *discoveryData = ARDISCOVERY_Connection_New (ARDISCOVERY_Connection_SendJsonCallback, ARDISCOVERY_Connection_ReceiveJsonCallback, deviceManager, &err);
    if (discoveryData == NULL || err != ARDISCOVERY_OK)
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Error while creating discoveryData : %s", ARDISCOVERY_Error_ToString(err));
        failed = 1;
    }
    
    if (!failed)
    {
        eARDISCOVERY_ERROR err = ARDISCOVERY_Connection_ControllerConnection(discoveryData, JS_DISCOVERY_PORT, JS_IP_ADDRESS);
        if (err != ARDISCOVERY_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Error while opening discovery connection : %s", ARDISCOVERY_Error_ToString(err));
            failed = 1;
        }
    }
    
    ARDISCOVERY_Connection_Delete(&discoveryData);
    
    return failed;
}

int startNetwork (DEVICE_MANAGER_t *deviceManager)
{
    int failed = 0;
    eARNETWORK_ERROR netError = ARNETWORK_OK;
    eARNETWORKAL_ERROR netAlError = ARNETWORKAL_OK;
    int pingDelay = 0; // 0 means default, -1 means no ping
        
    // Create the ARNetworkALManager
    deviceManager->alManager = ARNETWORKAL_Manager_New(&netAlError);
    if (netAlError != ARNETWORKAL_OK)
    {
        failed = 1;
    }
    
    if (!failed)
    {
        // Initilize the ARNetworkALManager
        netAlError = ARNETWORKAL_Manager_InitWifiNetwork(deviceManager->alManager, JS_IP_ADDRESS, JS_C2D_PORT, JS_D2C_PORT, 1);
        if (netAlError != ARNETWORKAL_OK)
        {
            failed = 1;
        }
    }
    
    if (!failed)
    {
        // Create the ARNetworkManager.
        deviceManager->netManager = ARNETWORK_Manager_New(deviceManager->alManager, numC2dParams, c2dParams, numD2cParams, d2cParams, pingDelay, onDisconnectNetwork, NULL, &netError);
        if (netError != ARNETWORK_OK)
        {
            failed = 1;
        }
    }

    if (!failed)
    {
        // Create and start Tx and Rx threads.
        if (ARSAL_Thread_Create(&(deviceManager->rxThread), ARNETWORK_Manager_ReceivingThreadRun, deviceManager->netManager) != 0)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Creation of Rx thread failed.");
            failed = 1;
        }
        
        if (ARSAL_Thread_Create(&(deviceManager->txThread), ARNETWORK_Manager_SendingThreadRun, deviceManager->netManager) != 0)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Creation of Tx thread failed.");
            failed = 1;
        }
    }
    
    // Print net error
    if (failed)
    {
        if (netAlError != ARNETWORKAL_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "ARNetWorkAL Error : %s", ARNETWORKAL_Error_ToString(netAlError));
        }
        
        if (netError != ARNETWORK_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "ARNetWork Error : %s", ARNETWORK_Error_ToString(netError));
        }
    }
    
    return failed;
}

void stopNetwork (DEVICE_MANAGER_t *deviceManager)
{
    int failed = 0;
    eARNETWORK_ERROR netError = ARNETWORK_OK;
    eARNETWORKAL_ERROR netAlError = ARNETWORKAL_OK;
    int pingDelay = 0; // 0 means default, -1 means no ping
        
    // ARNetwork cleanup
    if (deviceManager->netManager != NULL)
    {
        ARNETWORK_Manager_Stop(deviceManager->netManager);
        if (deviceManager->rxThread != NULL)
        {
            ARSAL_Thread_Join(deviceManager->rxThread, NULL);
            ARSAL_Thread_Destroy(&(deviceManager->rxThread));
            deviceManager->rxThread = NULL;
        }
        
        if (deviceManager->txThread != NULL)
        {
            ARSAL_Thread_Join(deviceManager->txThread, NULL);
            ARSAL_Thread_Destroy(&(deviceManager->txThread));
            deviceManager->txThread = NULL;
        }
    }
    
    if (deviceManager->alManager != NULL)
    {
        ARNETWORKAL_Manager_Unlock(deviceManager->alManager);
        
        ARNETWORKAL_Manager_CloseWifiNetwork(deviceManager->alManager);
    }
    
    ARNETWORK_Manager_Delete(&(deviceManager->netManager));
    ARNETWORKAL_Manager_Delete(&(deviceManager->alManager));
}

void registerARCommandsCallbacks (IHM_t *ihm)
{
    ARCOMMANDS_Decoder_SetARDrone3PilotingStateFlatTrimChangedCallback(flatTrimChangedCallback, ihm); 
    ARCOMMANDS_Decoder_SetCommonCommonStateBatteryStateChangedCallback(batteryStateChangedCallback, ihm);
    ARCOMMANDS_Decoder_SetARDrone3PilotingSettingsStateMaxAltitudeChangedCallback(maxAltitudeChangedCallback, ihm);
    ARCOMMANDS_Decoder_SetARDrone3PilotingSettingsStateMaxTiltChangedCallback(maxTiltChangedCallback, ihm);
    ARCOMMANDS_Decoder_SetARDrone3SpeedSettingsStateMaxVerticalSpeedChangedCallback(stateMaxVerticalSpeedChangedCallback, ihm);
    ARCOMMANDS_Decoder_SetARDrone3SpeedSettingsStateMaxRotationSpeedChangedCallback(stateMaxRotationSpeedChangedCallback, ihm);
    ARCOMMANDS_Decoder_SetARDrone3SpeedSettingsStateHullProtectionChangedCallback(stateHullProtectionChangedCallback, ihm);
    ARCOMMANDS_Decoder_SetARDrone3SpeedSettingsStateOutdoorChangedCallback(stateOutdoorChangedCallback, ihm);
}

void unregisterARCommandsCallbacks ()
{
    ARCOMMANDS_Decoder_SetARDrone3PilotingStateFlatTrimChangedCallback(NULL, NULL);
    ARCOMMANDS_Decoder_SetCommonCommonStateBatteryStateChangedCallback (NULL, NULL);
    ARCOMMANDS_Decoder_SetARDrone3PilotingSettingsStateMaxAltitudeChangedCallback(NULL, NULL);
    ARCOMMANDS_Decoder_SetARDrone3PilotingSettingsStateMaxTiltChangedCallback(NULL, NULL);
    ARCOMMANDS_Decoder_SetARDrone3SpeedSettingsStateMaxVerticalSpeedChangedCallback(NULL, NULL);
    ARCOMMANDS_Decoder_SetARDrone3SpeedSettingsStateMaxRotationSpeedChangedCallback(NULL, NULL);
    ARCOMMANDS_Decoder_SetARDrone3SpeedSettingsStateHullProtectionChangedCallback(NULL, NULL);
    ARCOMMANDS_Decoder_SetARDrone3SpeedSettingsStateOutdoorChangedCallback(NULL, NULL);
}

void onDisconnectNetwork (ARNETWORK_Manager_t *manager, ARNETWORKAL_Manager_t *alManager, void *customData)
{
    // Network Disconnected
    gIHMRun = 0; //stop IHM
}

void *looperRun (void* data)
{
    DEVICE_MANAGER_t *deviceManager = (DEVICE_MANAGER_t *)data;
    
    if(deviceManager != NULL)
    {
        while (deviceManager->run)
        {
            ARDrone3SendPCMD(deviceManager);            
            usleep(25000);
        }
    }

    return NULL;
}

void *readerRun (void* data)
{
    DEVICE_MANAGER_t *deviceManager = NULL;
    int bufferId = 0;
    int failed = 0;
    
    // Allocate some space for incoming data.
    const size_t maxLength = 128 * 1024;
    void *readData = malloc (maxLength);
    if (readData == NULL)
    {
        failed = 1;
    }
    
    if (!failed)
    {
        // get thread data.
        if (data != NULL)
        {
            bufferId = ((READER_THREAD_DATA_t *)data)->readerBufferId;
            deviceManager = ((READER_THREAD_DATA_t *)data)->deviceManager;
            
            if (deviceManager == NULL)
            {
                failed = 1;
            }
        }
        else
        {
            failed = 1;
        }
    }
    
    if (!failed)
    {
        while (deviceManager->run)
        {
            eARNETWORK_ERROR netError = ARNETWORK_OK;
            int length = 0;
            int skip = 0;
            
            // read data
            netError = ARNETWORK_Manager_ReadDataWithTimeout (deviceManager->netManager, bufferId, readData, maxLength, &length, 1000);
            if (netError != ARNETWORK_OK)
            {
                if (netError != ARNETWORK_ERROR_BUFFER_EMPTY)
                {
                    ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "ARNETWORK_Manager_ReadDataWithTimeout () failed : %s", ARNETWORK_Error_ToString(netError));
                }
                skip = 1;
            }
            
            if (!skip)
            {
                // Forward data to the CommandsManager
                eARCOMMANDS_DECODER_ERROR cmdError = ARCOMMANDS_DECODER_OK;
                cmdError = ARCOMMANDS_Decoder_DecodeBuffer ((uint8_t *)readData, length);
                if ((cmdError != ARCOMMANDS_DECODER_OK) && (cmdError != ARCOMMANDS_DECODER_ERROR_NO_CALLBACK))
                {
                    char msg[128];
                    ARCOMMANDS_Decoder_DescribeBuffer ((uint8_t *)readData, length, msg, sizeof(msg));
                    ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "ARCOMMANDS_Decoder_DecodeBuffer () failed : %d %s", cmdError, msg);
                }
            }
        }
    }
    
    if (readData != NULL)
    {
        free (readData);
        readData = NULL;
    }
    
    return NULL;
}


eARDISCOVERY_ERROR ARDISCOVERY_Connection_SendJsonCallback (uint8_t *dataTx, uint32_t *dataTxSize, void *customData)
{
    DEVICE_MANAGER_t *deviceManager = (DEVICE_MANAGER_t *)customData;
    eARDISCOVERY_ERROR err = ARDISCOVERY_OK;
    
    if ((dataTx != NULL) && (dataTxSize != NULL) && (deviceManager != NULL))
    {
        *dataTxSize = sprintf((char *)dataTx, "{ \"%s\": %d,\n \"%s\": \"%s\",\n \"%s\": \"%s\" }",
        ARDISCOVERY_CONNECTION_JSON_D2CPORT_KEY, deviceManager->d2cPort,
        ARDISCOVERY_CONNECTION_JSON_CONTROLLER_NAME_KEY, "name",
        ARDISCOVERY_CONNECTION_JSON_CONTROLLER_TYPE_KEY, "type") + 1;
    }
    else
    {
        err = ARDISCOVERY_ERROR;
    }
    
    return err;
}

eARDISCOVERY_ERROR ARDISCOVERY_Connection_ReceiveJsonCallback (uint8_t *dataRx, uint32_t dataRxSize, char *ip, void *customData)
{
    DEVICE_MANAGER_t *deviceManager = (DEVICE_MANAGER_t *)customData;
    eARDISCOVERY_ERROR err = ARDISCOVERY_OK;
    
    if ((dataRx != NULL) && (dataRxSize != 0) && (deviceManager != NULL))
    {
        char *json = malloc(dataRxSize + 1);
        strncpy(json, (char *)dataRx, dataRxSize);
        json[dataRxSize] = '\0';
        
        //ARSAL_PRINT(ARSAL_PRINT_DEBUG, TAG, "    - ReceiveJson:%s ", json);
        
        //normally c2dPort should be read from the json here.
        
        free(json);
    }
    else
    {
        err = ARDISCOVERY_ERROR;
    }
    
    return err;
}

// IHM callbacks: 

void onInputEvent (eIHM_INPUT_EVENT event, void *customData)
{
    // Manage IHM input events
    DEVICE_MANAGER_t *deviceManager = (DEVICE_MANAGER_t *)customData;
    
    switch (event)
    {
        case IHM_INPUT_EVENT_EXIT:
            if(deviceManager != NULL)
            {
                ARDrone3SendLandCommenad(deviceManager);
            }
            gIHMRun = 0;
            break;
        case IHM_INPUT_EVENT_JUMP:
            if(deviceManager != NULL)
            {
                ARDrone3SendTakeoffCommand(deviceManager);
            }
            break;
        case IHM_INPUT_EVENT_FORWARD:
            if(deviceManager != NULL)
            {
                ARDrone3SendMoveForwardCommand(deviceManager);
            }
            break;
        case IHM_INPUT_EVENT_BACK:
            if(deviceManager != NULL)
            {
                ARDrone3SendMoveBackwardCommand(deviceManager);
            }
            break;
        case IHM_INPUT_EVENT_RIGHT:
            if(deviceManager != NULL)
            {
                ARDrone3SendYawRightCommand(deviceManager);
            }
            break;
        case IHM_INPUT_EVENT_LEFT:
            if(deviceManager != NULL)
            {
                ARDrone3SendYawLeftCommand(deviceManager);
            }
            break;
        case IHM_INPUT_EVENT_ASCEND:
            if(deviceManager != NULL)
            {
                ARDrone3SendAscendCommand(deviceManager);
            }
            break;
        case IHM_INPUT_EVENT_DESCEND:
            if(deviceManager != NULL)
            {
                ARDrone3SendDescendCommand(deviceManager);
            }
            break;
        case IHM_INPUT_EVENT_HOVER:
            if(deviceManager != NULL)
            {
                ARDrone3SendHoverCommand(deviceManager);
            }
            break;
        case IHM_INPUT_EVENT_NONE:
            if(deviceManager != NULL)
            {
                // deviceManager->dataPCMD.flag = 0;
                // deviceManager->dataPCMD.speed = 0;
                // deviceManager->dataPCMD.turn = 0;
            }
            break;
        default:
            break;
    }
}

int customPrintCallback (eARSAL_PRINT_LEVEL level, const char *tag, const char *format, va_list va)
{
    // Custom callback used when ncurses is runing for not disturb the IHM
    
    if ((level == ARSAL_PRINT_ERROR) && (strcmp(TAG, tag) == 0))
    {
        // save the last Error
        vsnprintf(gErrorStr, (ERROR_STR_LENGTH - 1), format, va);
        gErrorStr[ERROR_STR_LENGTH - 1] = '\0';
    }
    
    return 1;
}

