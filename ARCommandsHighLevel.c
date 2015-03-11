#include "ARCommandsHighLevel.h"

int ARDrone3SendTakeoffCommand(DEVICE_MANAGER_t *deviceManager)
{
    int sentStatus = 1;
    u_int8_t cmdbuf[128];
    int32_t actualSize = 0;
    eARCOMMANDS_GENERATOR_ERROR cmdError;
    eARNETWORK_ERROR netError = ARNETWORK_ERROR;

    // Send TakeOff command
    cmdError = ARCOMMANDS_Generator_GenerateARDrone3PilotingTakeOff(cmdbuf, sizeof(cmdbuf), &actualSize);
    if (cmdError == ARCOMMANDS_GENERATOR_OK)
    {
        netError = ARNETWORK_Manager_SendData(deviceManager->netManager, JS_NET_CD_ACK_ID, cmdbuf, actualSize, NULL, &(arnetworkCmdCallback), 1);
    }

    if ((cmdError != ARCOMMANDS_GENERATOR_OK) || (netError != ARNETWORK_OK))
    {
        sentStatus = 0;
    }

    if (!sentStatus)
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Failed to send TakeOff command.");
    }

    return sentStatus;
}

int ARDrone3SendLandCommenad(DEVICE_MANAGER_t *deviceManager) {
    int sentStatus = 1;
    u_int8_t cmdbuf[128];
    int32_t actualSize = 0;
    eARCOMMANDS_GENERATOR_ERROR cmdError;
    eARNETWORK_ERROR netError = ARNETWORK_ERROR;

    // Send Landing command
    cmdError = ARCOMMANDS_Generator_GenerateARDrone3PilotingLanding(cmdbuf, sizeof(cmdbuf), &actualSize);
    if (cmdError == ARCOMMANDS_GENERATOR_OK)
    {
        netError = ARNETWORK_Manager_SendData(deviceManager->netManager, JS_NET_CD_ACK_ID, cmdbuf, actualSize, NULL, &(arnetworkCmdCallback), 1);
    }

    if ((cmdError != ARCOMMANDS_GENERATOR_OK) || (netError != ARNETWORK_OK))
    {
        sentStatus = 0;
    }

    if (!sentStatus)
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Failed to send Landing command.");
    }

    return sentStatus;
}

int ARDrone3SendPCMD(DEVICE_MANAGER_t *deviceManager)
{
    // int sentStatus = 1;
    // u_int8_t cmdBuffer[128];
    // int32_t cmdSize = 0;
    // eARCOMMANDS_GENERATOR_ERROR cmdError;
    // eARNETWORK_ERROR netError = ARNETWORK_ERROR;
    
    // // Send Posture command
    // cmdError = ARCOMMANDS_Generator_GenerateJumpingSumoPilotingPCMD(cmdBuffer, sizeof(cmdBuffer), &cmdSize, deviceManager->dataPCMD.flag, deviceManager->dataPCMD.speed, deviceManager->dataPCMD.turn);
    // if (cmdError == ARCOMMANDS_GENERATOR_OK)
    // {
    //     // The commands sent in loop should be sent to a buffer not acknowledged ; here JS_NET_CD_NONACK_ID
    //     netError = ARNETWORK_Manager_SendData(deviceManager->netManager, JS_NET_CD_NONACK_ID, cmdBuffer, cmdSize, NULL, &(arnetworkCmdCallback), 1);
    // }
    
    // if ((cmdError != ARCOMMANDS_GENERATOR_OK) || (netError != ARNETWORK_OK))
    // {
    //     sentStatus = 0;
    // }
    
    // return sentStatus;

    u_int8_t cmdbuf[128];
    int32_t actualSize = 0;
    eARCOMMANDS_GENERATOR_ERROR cmdError;
    eARNETWORK_ERROR netError = ARNETWORK_ERROR;
    int sentStatus = 1;

    // Send PCMD command
    cmdError = ARCOMMANDS_Generator_GenerateARDrone3PilotingPCMD(cmdbuf, sizeof(cmdbuf), &actualSize, 
            deviceManager->dataPCMD.active, 
            deviceManager->dataPCMD.roll, 
            deviceManager->dataPCMD.pitch, 
            deviceManager->dataPCMD.yaw, 
            deviceManager->dataPCMD.gaz, 
            deviceManager->dataPCMD.heading);
    if (cmdError == ARCOMMANDS_GENERATOR_OK)
    {
        netError = ARNETWORK_Manager_SendData(deviceManager->netManager, JS_NET_CD_NONACK_ID, cmdbuf, actualSize, NULL, &(arnetworkCmdCallback), 1);
    }

    if ((cmdError != ARCOMMANDS_GENERATOR_OK) || (netError != ARNETWORK_OK))
    {
        sentStatus = 0;
    }

    if (!sentStatus)
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Failed to send PCMD command.");
    }

    return sentStatus;
}

// [ARDrone3ARNetworkConfig c2dNackId] 
//     withSendPolicy:ARNETWORK_SEND_POLICY_DROP 
//     withCompletionBlock:nil 

//     withFlag:(uint8_t)(localState.pilotingData.active ? 1 : 0) 
//     withRoll:(int8_t)(localState.pilotingData.roll * 100.f) 
//     withPitch:(int8_t)(localState.pilotingData.pitch * 100.f) 
//     withYaw:(int8_t)(localState.pilotingData.yaw * 100.f) 
//     withGaz:(int8_t)(localState.pilotingData.gaz * 100.f) 
//     withPsi:localState.pilotingData.heading];