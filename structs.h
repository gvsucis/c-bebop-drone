#ifndef _BEBOP_STRUCTS_H
#define _BEBOP_STRUCTS_H


typedef struct
{
    uint8_t flag;
    int8_t  speed;
    int8_t  turn;
}JS_PCMD_t;

typedef struct READER_THREAD_DATA_t READER_THREAD_DATA_t;

typedef struct ARDrone3PilotingData_t
{
    int active;
    float roll;
    float pitch;
    float yaw;
    float gaz;
    float heading;
} ARDrone3PilotingData_t;

typedef struct
{
    ARNETWORKAL_Manager_t *alManager;
    ARNETWORK_Manager_t *netManager;
    ARSAL_Thread_t rxThread;
    ARSAL_Thread_t txThread;
    int d2cPort;
    int c2dPort;
    
    ARSAL_Thread_t looperThread;
    ARSAL_Thread_t *readerThreads;
    READER_THREAD_DATA_t *readerThreadsData;
    
    int run;
    ARDrone3PilotingData_t dataPCMD;
    
}DEVICE_MANAGER_t;

struct READER_THREAD_DATA_t
{
    DEVICE_MANAGER_t *deviceManager;
    int readerBufferId;
};

#endif /* _BEBOP_STRUCTS_H */