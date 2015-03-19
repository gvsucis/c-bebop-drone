#ifndef _BEBOP_PILOTING_COMMANDS_HIGHLEVEL_H_
#define _BEBOP_PILOTING_COMMANDS_HIGHLEVEL_H_

#include <libARSAL/ARSAL.h>
#include <libARSAL/ARSAL_Print.h>
#include <libARNetwork/ARNetwork.h>
#include <libARNetworkAL/ARNetworkAL.h>
#include <libARCommands/ARCommands.h>
#include <libARDiscovery/ARDiscovery.h>

#include "structs.h"
#include "ARDrone3Settings.h"
#include "callbacks.h"

int ARDrone3SendSettingsAllSettings(DEVICE_MANAGER_t *deviceManager);
int ARDrone3SendCommonAllStates(DEVICE_MANAGER_t *deviceManager);
int ARDrone3SendTakeoffCommand(DEVICE_MANAGER_t *deviceManager);
int ARDrone3SendLandCommenad(DEVICE_MANAGER_t *deviceManager);
int ARDrone3SendPCMD(DEVICE_MANAGER_t *deviceManager);
int ARDrone3SendSpeedSettingsMaxVerticalSpeed(DEVICE_MANAGER_t *deviceManager, float current); 
int ARDrone3SendSpeedSettingsMaxRotationSpeed(DEVICE_MANAGER_t *deviceManager, float current);
int ARDrone3SendSendSpeedSettingsHullProtection(DEVICE_MANAGER_t *deviceManager, float present);
int ARDrone3SendPilotingSettingsMaxAltitude(DEVICE_MANAGER_t *deviceManager, float current);
int ARDrone3SendPilotingSettingsMaxTilt(DEVICE_MANAGER_t *deviceManager, float current);
int ARDrone3SendPilotingFlatTrim(DEVICE_MANAGER_t *deviceManager);
void ARDrone3SendYawRightCommand(DEVICE_MANAGER_t *deviceManager);
void ARDrone3SendYawLeftCommand(DEVICE_MANAGER_t *deviceManager);
void ARDrone3SendAscendCommand(DEVICE_MANAGER_t *deviceManager);
void ARDrone3SendDescendCommand(DEVICE_MANAGER_t *deviceManager);
void ARDrone3SendHoverCommand(DEVICE_MANAGER_t *deviceManager);

void ARDrone3SendMoveForwardCommand(DEVICE_MANAGER_t *deviceManager);
void ARDrone3SendMoveBackwardCommand(DEVICE_MANAGER_t *deviceManager);

#endif /* _BEBOP_PILOTING_COMMANDS_HIGHLEVEL_H_ */