#include "ARCommandsHighLevel.h"

int ARDrone3SendCommonAllStates(DEVICE_MANAGER_t *deviceManager)
{
    int sentStatus = 1;
    u_int8_t cmdbuf[128];
    int32_t actualSize = 0;
    eARCOMMANDS_GENERATOR_ERROR cmdError;
    eARNETWORK_ERROR netError = ARNETWORK_ERROR;

    // Send AllStates command
    cmdError = ARCOMMANDS_Generator_GenerateCommonCommonAllStates(cmdbuf, sizeof(cmdbuf), &actualSize);
    if (cmdError == ARCOMMANDS_GENERATOR_OK)
    {
        netError = ARNETWORK_Manager_SendData(deviceManager->netManager, JS_NET_CD_ACK_ID, cmdbuf, actualSize, NULL, &(arnetworkCmdCallback), 1);
    }
    
    if ((cmdError != ARCOMMANDS_GENERATOR_OK) || (netError != ARNETWORK_OK))
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Failed to send AllStates command.");
        sentStatus = 0;
    }

    return sentStatus;
}

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
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Failed to send TakeOff command.");
        sentStatus = 0;
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
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Failed to send Landing command.");
        sentStatus = 0;
    }

    return sentStatus;
}

int ARDrone3SendPilotingFlatTrim(DEVICE_MANAGER_t *deviceManager)
{
    int sentStatus = 1;
    u_int8_t cmdbuf[128];
    int32_t actualSize = 0;
    eARCOMMANDS_GENERATOR_ERROR cmdError;
    eARNETWORK_ERROR netError = ARNETWORK_ERROR;

    // Send FlatTrim command
    cmdError = ARCOMMANDS_Generator_GenerateARDrone3PilotingFlatTrim(cmdbuf, sizeof(cmdbuf), &actualSize);
    if (cmdError == ARCOMMANDS_GENERATOR_OK)
    {
        netError = ARNETWORK_Manager_SendData(deviceManager->netManager, JS_NET_CD_NONACK_ID, cmdbuf, actualSize, NULL, &(arnetworkCmdCallback), 1);
    }
    if ((cmdError != ARCOMMANDS_GENERATOR_OK) || (netError != ARNETWORK_OK))
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Failed to send FlatTrim command.");
    }

    return sentStatus;
}

int ARDrone3SendPCMD(DEVICE_MANAGER_t *deviceManager)
{
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
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Failed to send PCMD command.");
        sentStatus = 0;
    }

    return sentStatus;
}

int ARDrone3SendSpeedSettingsMaxVerticalSpeed(DEVICE_MANAGER_t *deviceManager, float current) 
{
    u_int8_t cmdbuf[128];
    int32_t actualSize = 0;
    eARCOMMANDS_GENERATOR_ERROR cmdError;
    eARNETWORK_ERROR netError = ARNETWORK_ERROR;
    int sentStatus = 1;

    // Send MaxVerticalSpeed command
    cmdError = ARCOMMANDS_Generator_GenerateARDrone3SpeedSettingsMaxVerticalSpeed(cmdbuf, sizeof(cmdbuf), &actualSize, current);

    if (cmdError == ARCOMMANDS_GENERATOR_OK)
    {
        netError = ARNETWORK_Manager_SendData(deviceManager->netManager, JS_NET_CD_NONACK_ID, cmdbuf, actualSize, NULL, &(arnetworkCmdCallback), 1);
    }

    if ((cmdError != ARCOMMANDS_GENERATOR_OK) || (netError != ARNETWORK_OK))
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Failed to send MaxVerticalSpeed command.");
        sentStatus = 0;
    }

    return sentStatus;   
}

int ARDrone3SendSpeedSettingsMaxRotationSpeed(DEVICE_MANAGER_t *deviceManager, float current)
{
    u_int8_t cmdbuf[128];
    int32_t actualSize = 0;
    eARCOMMANDS_GENERATOR_ERROR cmdError;
    eARNETWORK_ERROR netError = ARNETWORK_ERROR;
    int sentStatus = 1;

    // Send MaxVerticalSpeed command
    cmdError = ARCOMMANDS_Generator_GenerateARDrone3SpeedSettingsMaxRotationSpeed(cmdbuf, sizeof(cmdbuf), &actualSize, current);

    if (cmdError == ARCOMMANDS_GENERATOR_OK)
    {
        netError = ARNETWORK_Manager_SendData(deviceManager->netManager, JS_NET_CD_NONACK_ID, cmdbuf, actualSize, NULL, &(arnetworkCmdCallback), 1);
    }

    if ((cmdError != ARCOMMANDS_GENERATOR_OK) || (netError != ARNETWORK_OK))
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Failed to send MaxVerticalSpeed command.");
        sentStatus = 0;
    }

    return sentStatus;      
}

int ARDrone3SendSendSpeedSettingsHullProtection(DEVICE_MANAGER_t *deviceManager, float present)
{
    u_int8_t cmdbuf[128];
    int32_t actualSize = 0;
    eARCOMMANDS_GENERATOR_ERROR cmdError;
    eARNETWORK_ERROR netError = ARNETWORK_ERROR;
    int sentStatus = 1;

    // Send MaxVerticalSpeed command
    cmdError = ARCOMMANDS_Generator_GenerateARDrone3SpeedSettingsHullProtection(cmdbuf, sizeof(cmdbuf), &actualSize, present);

    if (cmdError == ARCOMMANDS_GENERATOR_OK)
    {
        netError = ARNETWORK_Manager_SendData(deviceManager->netManager, JS_NET_CD_NONACK_ID, cmdbuf, actualSize, NULL, &(arnetworkCmdCallback), 1);
    }

    if ((cmdError != ARCOMMANDS_GENERATOR_OK) || (netError != ARNETWORK_OK))
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Failed to send MaxVerticalSpeed command.");
        sentStatus = 0;
    }

    return sentStatus;         
}

int ARDrone3SendPilotingSettingsMaxAltitude(DEVICE_MANAGER_t *deviceManager, float current)
{
    u_int8_t cmdbuf[128];
    int32_t actualSize = 0;
    eARCOMMANDS_GENERATOR_ERROR cmdError;
    eARNETWORK_ERROR netError = ARNETWORK_ERROR;
    int sentStatus = 1;

    // Send MaxVerticalSpeed command
    cmdError = ARCOMMANDS_Generator_GenerateARDrone3PilotingSettingsMaxAltitude(cmdbuf, sizeof(cmdbuf), &actualSize, current);

    if (cmdError == ARCOMMANDS_GENERATOR_OK)
    {
        netError = ARNETWORK_Manager_SendData(deviceManager->netManager, JS_NET_CD_NONACK_ID, cmdbuf, actualSize, NULL, &(arnetworkCmdCallback), 1);
    }

    if ((cmdError != ARCOMMANDS_GENERATOR_OK) || (netError != ARNETWORK_OK))
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Failed to send MaxVerticalSpeed command.");
        sentStatus = 0;
    }

    return sentStatus;   
}

int ARDrone3SendPilotingSettingsMaxTilt(DEVICE_MANAGER_t *deviceManager, float current)
{
    u_int8_t cmdbuf[128];
    int32_t actualSize = 0;
    eARCOMMANDS_GENERATOR_ERROR cmdError;
    eARNETWORK_ERROR netError = ARNETWORK_ERROR;
    int sentStatus = 1;

    // Send MaxVerticalSpeed command
    cmdError = ARCOMMANDS_Generator_GenerateARDrone3PilotingSettingsMaxTilt(cmdbuf, sizeof(cmdbuf), &actualSize, current);

    if (cmdError == ARCOMMANDS_GENERATOR_OK)
    {
        netError = ARNETWORK_Manager_SendData(deviceManager->netManager, JS_NET_CD_NONACK_ID, cmdbuf, actualSize, NULL, &(arnetworkCmdCallback), 1);
    }

    if ((cmdError != ARCOMMANDS_GENERATOR_OK) || (netError != ARNETWORK_OK))
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Failed to send MaxVerticalSpeed command.");
        sentStatus = 0;
    }

    return sentStatus;   
}

void ARDrone3SendYawRightCommand(DEVICE_MANAGER_t *deviceManager)
{
    deviceManager->dataPCMD.active = 1;
    deviceManager->dataPCMD.yaw = 100;
}

void ARDrone3SendYawLeftCommand(DEVICE_MANAGER_t *deviceManager)
{   
    deviceManager->dataPCMD.active = 1;
    deviceManager->dataPCMD.yaw = -100;
}

void ARDrone3SendAscendCommand(DEVICE_MANAGER_t *deviceManager)
{   
    deviceManager->dataPCMD.gaz = 30;
}

void ARDrone3SendDescendCommand(DEVICE_MANAGER_t *deviceManager)
{   
    deviceManager->dataPCMD.gaz = -30;
}

void ARDrone3SendHoverCommand(DEVICE_MANAGER_t *deviceManager)
{
    deviceManager->dataPCMD.active = 0;
    deviceManager->dataPCMD.roll = 0.0;
    deviceManager->dataPCMD.pitch = 0.0;
    deviceManager->dataPCMD.yaw = 0.0;
    deviceManager->dataPCMD.gaz = 0.0;
    deviceManager->dataPCMD.heading = 0.0;
}

void ARDrone3SendMoveForwardCommand(DEVICE_MANAGER_t *deviceManager)
{    
    deviceManager->dataPCMD.active = 1;
    deviceManager->dataPCMD.pitch = 30; //Speed
}

void ARDrone3SendMoveBackwardCommand(DEVICE_MANAGER_t *deviceManager)

    deviceManager->dataPCMD.active = 1;
    deviceManager->dataPCMD.pitch = -30; //Speed
}