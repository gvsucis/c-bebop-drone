#ifndef _ARDRONE3_CALLBACKS_H
#define _ARDRONE3_CALLBACKS_H

eARNETWORK_MANAGER_CALLBACK_RETURN arnetworkCmdCallback(int buffer_id, uint8_t *data, void *custom, eARNETWORK_MANAGER_CALLBACK_STATUS cause);

void batteryStateChangedCallback(uint8_t percent, void *custom);
void flatTrimChangedCallback(void *custom);
void maxAltitudeChangedCallback(float current, float min, float max, void *custom);
void maxTiltChangedCallback(float current, float min, float max, void *custom);
void stateMaxVerticalSpeedChangedCallback(float current, float min, float max, void *custom);
void stateMaxRotationSpeedChangedCallback(float current, float min, float max, void *custom);
void stateHullProtectionChangedCallback(uint8_t present, void *custom);
void stateOutdoorChangedCallback(uint8_t present, void *custom);

#endif /* _ARDRONE3_CALLBACKS_H */