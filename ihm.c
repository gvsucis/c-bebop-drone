#include <stdlib.h>
#include <curses.h>
#include <string.h>

#include <libARSAL/ARSAL.h>

#include "ihm.h"


#define HEADER_X 0
#define HEADER_Y 0

#define INFO_X 0
#define INFO_Y 2

#define FLAT_TRIM_X 0
#define FLAT_TRIM_Y 4

#define BATTERY_X 0
#define BATTERY_Y 6

#define MAX_ALTITUDE_STATE_X 0
#define MAX_ALTITUDE_STATE_Y 8

#define MAX_TILT_STATE_X 0
#define MAX_TILT_STATE_Y 10

#define MAX_VERTICAL_SPEED_STATE_X 0
#define MAX_VERTICAL_SPEED_STATE_Y 12

#define MAX_ROTATION_SPEED_STATE_X 0
#define MAX_ROTATION_SPEED_STATE_Y 14

#define HULL_PROTECTION_X 0
#define HULL_PROTECTION_Y 16

#define OUTDOOR_X 0
#define OUTDOOR_Y 18


void *IHM_InputProcessing(void *data);


IHM_t *IHM_New (IHM_onInputEvent_t onInputEventCallback)
{
    int failed = 0;
    IHM_t *newIHM = NULL;
    
    // check parameters
    if (onInputEventCallback == NULL)
    {
        failed = 1;
    }
    
    if (!failed)
    {
        //  Initialize IHM
        newIHM = malloc(sizeof(IHM_t));
        if (newIHM != NULL)
        {
            //  Initialize ncurses
            newIHM->mainWindow = initscr();
            newIHM->inputThread = NULL;
            newIHM->run = 1;
            newIHM->onInputEventCallback = onInputEventCallback;
            newIHM->customData = NULL;
        }
        else
        {
            failed = 1;
        }
    }
    
    if (!failed)
    {
        raw();                  // Line buffering disabled
        keypad(stdscr, TRUE);
        noecho();               // Don't echo() while we do getch
        timeout(100);
        
        refresh();
    }
    
    if (!failed)
    {
        //start input thread
        if(ARSAL_Thread_Create(&(newIHM->inputThread), IHM_InputProcessing, newIHM) != 0)
        {
            failed = 1;
        }
    }
    
    if (failed)
    {
        IHM_Delete (&newIHM);
    }

    return  newIHM;
}

void IHM_Delete (IHM_t **ihm)
{
    //  Clean up

    if (ihm != NULL)
    {
        if ((*ihm) != NULL)
        {
            (*ihm)->run = 0;
            
            if ((*ihm)->inputThread != NULL)
            {
                ARSAL_Thread_Join((*ihm)->inputThread, NULL);
                ARSAL_Thread_Destroy(&((*ihm)->inputThread));
                (*ihm)->inputThread = NULL;
            }
            
            delwin((*ihm)->mainWindow);
            (*ihm)->mainWindow = NULL;
            endwin();
            refresh();
            
            free (*ihm);
            (*ihm) = NULL;
        }
    }
}

void IHM_setCustomData(IHM_t *ihm, void *customData)
{
    if (ihm != NULL)
    {
        ihm->customData = customData;
    }
}

void *IHM_InputProcessing(void *data)
{
    IHM_t *ihm = (IHM_t *) data;
    int key = 0;
    
    if (ihm != NULL)
    {
        while (ihm->run)
        {
            key = getch();
            
            if (key =='q')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_EXIT, ihm->customData);
                }
            }
            else if(key == KEY_UP)
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_FORWARD, ihm->customData);
                }
            }
            else if(key == KEY_DOWN)
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_BACK, ihm->customData);
                }
            }
            else if(key == 'a')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_LEFT, ihm->customData);
                }
            }
            else if(key == 'd')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_RIGHT, ihm->customData);
                }
            }
            else if(key == ' ')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_JUMP, ihm->customData);
                }
            }
            else if(key == 'w')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_ASCEND, ihm->customData);
                }
            }
            else if(key == 's')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_DESCEND, ihm->customData);
                }
            }
            else if(key == 27)
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_HOVER, ihm->customData);
                }   
            }
            else
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_NONE, ihm->customData);
                }
            }
            
            usleep(10);
        }
    }
    
    return NULL;
}

void IHM_PrintHeader(IHM_t *ihm, char *headerStr)
{
    if (ihm != NULL)
    {
        move(HEADER_Y, 0);   // move to begining of line
        clrtoeol();          // clear line
        mvprintw(HEADER_Y, HEADER_X, headerStr);
    }
}

void IHM_PrintInfo(IHM_t *ihm, char *infoStr)
{
    if (ihm != NULL)
    {
        move(INFO_Y, 0);    // move to begining of line
        clrtoeol();         // clear line
        mvprintw(INFO_Y, INFO_X, infoStr);
    }
}

void IHM_PrintMaxAltitudeStateInfo(IHM_t *ihm, float current, float min, float max)
{
    if (ihm != NULL)
    {
        move(MAX_ALTITUDE_STATE_Y, 0);    // move to begining of line
        clrtoeol();              // clear line
        mvprintw(MAX_ALTITUDE_STATE_Y, MAX_ALTITUDE_STATE_X, "Max Altitude Changed: (current: %.2f m/s) (min: %.2f m/s) (max: %.2f m/s)", current, min, max );
    }
}

void IHM_PrintMaxTiltStateInfo(IHM_t *ihm, float current, float min, float max)
{
    if (ihm != NULL)
    {
        move(MAX_TILT_STATE_Y, 0);    // move to begining of line
        clrtoeol();                   // clear line
        mvprintw(MAX_TILT_STATE_Y, MAX_TILT_STATE_X, "Max Tilt Changed: (current: %.2f degrees) (min: %.2f degrees) (max: %.2f degrees)", current, min, max );
    }    
}
void IHM_PrintBattery(IHM_t *ihm, uint8_t percent)
{
    if (ihm != NULL)
    {
        move(BATTERY_Y, 0);     // move to begining of line
        clrtoeol();             // clear line
        mvprintw(BATTERY_Y, BATTERY_X, "Battery: %d", percent);
    }
}

void IHM_PrintFlatTrim(IHM_t *ihm, char *infoStr)
{
    if (ihm != NULL)
    {
        move(FLAT_TRIM_Y, 0);    // move to begining of line
        clrtoeol();         // clear line
        mvprintw(FLAT_TRIM_Y, FLAT_TRIM_X, infoStr);
    }    
}

void IHM_PrintMaxVerticalSpeedStateInfo(IHM_t *ihm, float current, float min, float max)
{
    if (ihm != NULL)
    {
        move(MAX_VERTICAL_SPEED_STATE_Y, 0);
        clrtoeol();
        mvprintw(MAX_VERTICAL_SPEED_STATE_Y, MAX_VERTICAL_SPEED_STATE_X, "Max Vertical Speed Changed: (current: %.2f m/s) (min: %.2f m/s) (max: %.2f m/s)", current, min, max );
    } 
}

void IHM_PrintMaxRotationSpeedStateInfo(IHM_t *ihm, float current, float min, float max)
{
    if (ihm != NULL)
    {
        move(MAX_ROTATION_SPEED_STATE_Y, 0);
        clrtoeol();
        mvprintw(MAX_ROTATION_SPEED_STATE_Y, MAX_ROTATION_SPEED_STATE_X, "Max Rotation Speed Changed: (current: %.2f degree/s) (min: %.2f degree/s) (max: %.2f degree/s)", current, min, max );
    } 
}

void IHM_PrintHullProtectionPresence(IHM_t *ihm, char *infoStr)
{
    if (ihm != NULL)
    {
        move(HULL_PROTECTION_Y, 0);
        clrtoeol();
        mvprintw(HULL_PROTECTION_Y, HULL_PROTECTION_X, infoStr);
    }  
}

void IHM_PrintOutdoor(IHM_t *ihm, char *infoStr)
{
    if (ihm != NULL)
    {
        move(OUTDOOR_Y, 0);
        clrtoeol();
        mvprintw(OUTDOOR_Y, OUTDOOR_X, infoStr);
    }  
}