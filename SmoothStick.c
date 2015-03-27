/*
 * SmoothStick.c
 *
 * Dimitar Angelov <funkamateur@gmail.com>
 * Hamburg, Deutschland
 * März 2015
 */

#include "SmoothStick.h"


PLUGIN_API int XPluginStart(
                        char *		outName,
                        char *		outSig,
                        char *		outDesc)
{
    sprintf(outName, "SmoothStick");
    sprintf(outSig,  "angelov.SmoothStick");
    sprintf(outDesc, "A plugin that smoothens the input of the joystick and adds progressive braking.");

    XPLMRegisterFlightLoopCallback(SmoothLoopCallback, -1, NULL); // FIXME: called back each second

    /* SmoothBreaking
    SmoothStickCommand = XPLMCreateCommand(
                "sim/joystick/smoothstick_enable",
                "Smoothen joystick input"
                );


    XPLMRegisterCommandHandler(
                SmoothStickCommand,
                SmoothStickCommandHandler,
                0,
                NULL);
                */


    gWindow = XPLMCreateWindow(
                    50, 700, 300, 600,			/* Area of the window. */
                    0,							/* Start visible. */
                    DrawWindowCallback,			/* Callbacks */
                    KeyCallback,
                    MouseClickCallback,
                    NULL);						/* Refcon - not used. */

   XPLMSetWindowIsVisible(gWindow, VISIBLE);

    smoothSubMenuItem = XPLMAppendMenuItem(
                        XPLMFindPluginsMenu(),
                        "SmoothStick",
                        0,						// Item Ref
                        1);						// Force English

    smoothMenu = XPLMCreateMenu(
                        "SmoothStick",
                        XPLMFindPluginsMenu(),
                        smoothSubMenuItem, 			// Menu Item to attach to.
                        smoothMenuHandlerCallback,	// The handler
                        0);						// Handler Ref


    XPLMAppendMenuItem(smoothMenu, "Toggle", (void *)0, 1);
    XPLMAppendMenuItem(smoothMenu, "Debug window", (void *)1, 1);
    XPLMAppendMenuItem(smoothMenu, "Settings", (void *)2, 1);

    idxElvAxis = 2;
    idxAilAxis = 3;

    return 1;
}


PLUGIN_API void	XPluginStop(void)
{
    XPLMDestroyWindow(gWindow);
    XPLMUnregisterCommandHandler(SmoothBreakingCommand, SmoothBreakingCommandHandler, 0, 0);
}


PLUGIN_API void XPluginDisable(void)
{
    XPLMDebugString("SmoothStick> Stopping plugin.\n");
    XPLMSetDatai(XPLMFindDataRef("sim/operation/override/override_joystick"), 0);
}


PLUGIN_API int XPluginEnable(void)
{
    XPLMDebugString("SmoothStick> Starting plugin.\n");
    XPLMSetDatai(XPLMFindDataRef("sim/operation/override/override_joystick"), 1);

    XPLMGetDatavi(XPLMFindDataRef("sim/joystick/joystick_axis_assignments"), AxisType, 0, 100);
    FindAxis(AxisType, &idxElvAxis, &idxAilAxis);

    return 1;
}


PLUGIN_API void XPluginReceiveMessage(
                    XPLMPluginID	inFromWho,
                    long			inMessage,
                    void *			inParam)
{
    (void)inFromWho;
    (void)inMessage;
    (void)inParam;
}




float SmoothLoopCallback(float elapsedMe, float elapsedSim, int counter, void * refcon)
{

    (void)elapsedMe;
    (void)elapsedSim;
    (void)counter;
    (void)refcon;

    XPLMSetWindowIsVisible(gWindow, VISIBLE);

    /******* processing code *******/
    XPLMGetDatavf(XPLMFindDataRef("sim/joystick/joystick_axis_values"), AxisValue, 0, 100);
    // FIXME: when released, jumps to 0 (center)
    pitchValue = (AxisValue[idxElvAxis] - 0.5f) * 2.0f;
    rollValue =  (AxisValue[idxAilAxis] - 0.5f) * 2.0f;

    SmoothenPitch(&pitchValue, &sPitchValue, &smoothStepPitch);
    SmoothenRoll(&rollValue, &sRollValue, &smoothStepRoll);

    XPLMSetDataf(XPLMFindDataRef("sim/joystick/yoke_pitch_ratio"), sPitchValue);
    XPLMSetDataf(XPLMFindDataRef("sim/joystick/yoke_roll_ratio"), sRollValue);
    /************/

    return 1;
}


int	SmoothBreakingCommandHandler(XPLMCommandRef     inCommand,
                              XPLMCommandPhase   inPhase,
                              void *             inRefcon)
{
    (void) inCommand;
    (void)inRefcon;

    if (inPhase == xplm_CommandBegin)
    {
        //XPLMSetDatai(gDataRefBreaking, 1);
    }

    if (inPhase == xplm_CommandContinue)
    {
        //read
        //smoothen
        //write
    }

    if (inPhase == xplm_CommandEnd)
    {
        //XPLMSetDatai(gDataRefBreaking, 0);
    }

    return 0;
}


void DrawWindowCallback(
                                   XPLMWindowID         inWindowID,
                                   void *               inRefcon)
{
    (void)inRefcon;
    int		left, top, right, bottom;

    XPLMGetWindowGeometry(inWindowID, &left, &top, &right, &bottom);
    XPLMDrawTranslucentDarkBox(left, top, right, bottom);
    XPLMDrawString(colorMagenta, left + 5, top - 20,
        versionStr, NULL, xplmFont_Basic);

    sprintf(pitchLabel, "pitch: %f", pitchValue);
    sprintf(rollLabel,  "roll: %f",  rollValue);
    sprintf(sPitchLabel, "smooth pitch: %f", sPitchValue);
    sprintf(sRollLabel,  "smooth roll: %f",  sRollValue);
    if(DEBUG)
    {
        sprintf(axisDebug,  "axis%d: %f :: axis%d: %f",
                idxAilAxis, (float)AxisValue[idxAilAxis],
                idxAilAxis, (float)AxisValue[idxElvAxis]);
    }

    XPLMDrawString(colorWhite, left + 5, top - 35, pitchLabel, NULL, xplmFont_Basic);
    XPLMDrawString(colorWhite, left + 5, top - 50, rollLabel, NULL, xplmFont_Basic);
    XPLMDrawString(colorYellow, left + 5, top - 65, sPitchLabel, NULL, xplmFont_Basic);
    XPLMDrawString(colorYellow, left + 5, top - 80, sRollLabel, NULL, xplmFont_Basic);
    XPLMDrawString(colorYellow, left + 5, top - 95, axisDebug, NULL, xplmFont_Basic);
}



void	smoothMenuHandlerCallback(
                                   void *               inMenuRef,
                                   void *               inItemRef)
{
    /* This is the menu callback.  We simply turn the item ref back
     * into a command ID and tell the sim to do it. */
    (void)inMenuRef;
    if (inItemRef == 0)
    {
        XPLMDebugString("SmoothStick> Menu item 0 selected.\n");
        if (VISIBLE) { VISIBLE = 0; } else { VISIBLE = 1; }
    }
}


/*
int	SmoothStickCommandHandler(
        XPLMCommandRef      inCommand,
        XPLMCommandPhase    inPhase,
        void *   			inRefcon)
{
    return 0;
}
*/











static void KeyCallback(
                                   XPLMWindowID         inWindowID,
                                   char                 inKey,
                                   XPLMKeyFlags         inFlags,
                                   char                 inVirtualKey,
                                   void *               inRefcon,
                                   int                  losingFocus)
{
    (void)inWindowID;
    (void)inKey;
    (void)inFlags;
    (void)inVirtualKey;
    (void)inRefcon;
    (void)losingFocus;
}

static int MouseClickCallback(
                                   XPLMWindowID         inWindowID,
                                   int                  x,
                                   int                  y,
                                   XPLMMouseStatus      inMouse,
                                   void *               inRefcon)
{
    (void)inWindowID;
    (void)x;
    (void)y;
    (void)inMouse;
    (void)inRefcon;

    return 0;
}
