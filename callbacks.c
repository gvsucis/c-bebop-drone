#include <libARSAL/ARSAL.h>
#include <libARSAL/ARSAL_Print.h>
#include <libARNetwork/ARNetwork.h>
#include <libARNetworkAL/ARNetworkAL.h>
#include <libARCommands/ARCommands.h>
#include <libARDiscovery/ARDiscovery.h>

#include "ihm.h"
#include "callbacks.h"

eARNETWORK_MANAGER_CALLBACK_RETURN arnetworkCmdCallback(int buffer_id, uint8_t *data, void *custom, eARNETWORK_MANAGER_CALLBACK_STATUS cause)
{
    eARNETWORK_MANAGER_CALLBACK_RETURN retval = ARNETWORK_MANAGER_CALLBACK_RETURN_DEFAULT;

    if (cause == ARNETWORK_MANAGER_CALLBACK_STATUS_TIMEOUT)
    {
        retval = ARNETWORK_MANAGER_CALLBACK_RETURN_DATA_POP;
    }
    
    return retval;
}

void flatTrimChangedCallback(void *custom)
{
    // callback that flat trim is correctly processed
    IHM_t *ihm = (IHM_t *) custom;
    
    if (ihm != NULL)
    {
        IHM_PrintFlatTrim(ihm, "Flat trim processed correctly ...");   
    }
}

void batteryStateChangedCallback (uint8_t percent, void *custom)
{
    // callback of changing of battery level
    IHM_t *ihm = (IHM_t *) custom;
    
    if (ihm != NULL)
    {
        IHM_PrintBattery (ihm, percent);
    }
}

void maxAltitudeChangedCallback(float current, float min, float max, void *custom)
{
    // Max altitude is changed callback
    IHM_t *ihm = (IHM_t *) custom;
    
    if (ihm != NULL)
    {
        IHM_PrintMaxAltitudeStateInfo(ihm, current, min, max);   
    }
}

void maxTiltChangedCallback(float current, float min, float max, void *custom)
{
    // Max tilt is changed callback
    IHM_t *ihm = (IHM_t *) custom;
    
    if (ihm != NULL)
    {
        IHM_PrintMaxTiltStateInfo(ihm, current, min, max);   
    } 
}

void stateMaxVerticalSpeedChangedCallback(float current, float min, float max, void *custom)
{
	// Max Vertical Speed is changed callback
    IHM_t *ihm = (IHM_t *) custom;
    
    if (ihm != NULL)
    {
        IHM_PrintMaxVerticalSpeedStateInfo(ihm, current, min, max);   
    }
}

void stateMaxRotationSpeedChangedCallback(float current, float min, float max, void *custom)
{
	// Max Rotation Speed is changed callback
    IHM_t *ihm = (IHM_t *) custom;
    
    if (ihm != NULL)
    {
        IHM_PrintMaxRotationSpeedStateInfo(ihm, current, min, max);   
    }
}

void stateHullProtectionChangedCallback(uint8_t present, void *custom)
{
	// callback that hull protection is present
    IHM_t *ihm = (IHM_t *) custom;
    
    if (ihm != NULL)
    {
    	if (present == 1)
        	IHM_PrintHullProtectionPresence(ihm, "Hull Protection is present.");   
        else
        	IHM_PrintHullProtectionPresence(ihm, "Hull Protection is not present.");   
    }
}

void stateOutdoorChangedCallback(uint8_t present, void *custom)
{
	// callback that hull protection is present
    IHM_t *ihm = (IHM_t *) custom;
    
    if (ihm != NULL)
    {
    	if (present == 1)
        	IHM_PrintOutdoor(ihm, "This is an outdoor flight.");   
        else
        	IHM_PrintOutdoor(ihm, "This is not an outdoor flight.");   
    }
}