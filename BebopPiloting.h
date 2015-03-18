#ifndef _BEBOP_PILOTING_H_
#define _BEBOP_PILOTING_H_

#include <ihm.h>
#include "structs.h"

int ardiscoveryConnect (DEVICE_MANAGER_t *deviceManager);
eARDISCOVERY_ERROR ARDISCOVERY_Connection_SendJsonCallback (uint8_t *dataTx, uint32_t *dataTxSize, void *customData);
eARDISCOVERY_ERROR ARDISCOVERY_Connection_ReceiveJsonCallback (uint8_t *dataRx, uint32_t dataRxSize, char *ip, void *customData);

int startNetwork (DEVICE_MANAGER_t *deviceManager);
void onDisconnectNetwork (ARNETWORK_Manager_t *manager, ARNETWORKAL_Manager_t *alManager, void *customData);
void stopNetwork (DEVICE_MANAGER_t *deviceManager);

void registerARCommandsCallbacks (IHM_t *ihm);
void unregisterARCommandsCallbacks();

void *looperRun(void* data);
void *readerRun(void* data);

/* IHM callbacks: */
void onInputEvent (eIHM_INPUT_EVENT event, void *customData);
int customPrintCallback (eARSAL_PRINT_LEVEL level, const char *tag, const char *format, va_list va);

#endif /* _BEBOP_PILOTING_H_ */