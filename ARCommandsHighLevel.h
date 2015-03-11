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

int ARDrone3SendTakeoffCommand(DEVICE_MANAGER_t *deviceManager);
int ARDrone3SendLandCommenad(DEVICE_MANAGER_t *deviceManager);
int ARDrone3SendPCMD(DEVICE_MANAGER_t *deviceManager);

#endif /* _BEBOP_PILOTING_COMMANDS_HIGHLEVEL_H_ */