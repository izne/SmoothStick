/*
 * SmoothStick.c
 *
 * Dimitar Angelov
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
    sprintf(outSig,  "angelov.SPU");
    sprintf(outDesc, "A plugin that smoothens the input of the joystick and adds progressive braking.");

    XPLMRegisterFlightLoopCallback(SmoothLoopCallback, -1, NULL); // FIXME: called back each second

    SmoothBreakCommand = XPLMCreateCommand(
                "sim/joystick/smoothbreak_enable",
                "Smooth breaking action"
                );

    XPLMRegisterCommandHandler(
                SmoothBreakCommand,
                SmoothBreakCommandHandler,
                0,
                NULL);



    gWindow = XPLMCreateWindow(
                    50, 700, 300, 600,			/* initial Area of the window. */
                    0,							/* Start visible. */
                    DrawWindowCallback,			/* Callbacks */
                    KeyCallback,
                    MouseClickCallback,
                    NULL);						/* Refcon - not used. */

    XPLMSetWindowIsVisible(gWindow, G_VISIBLE);

    smoothSubMenuItem = XPLMAppendMenuItem(
                        XPLMFindPluginsMenu(),
                        "SPU",
                        0,
                        1);

    smoothMenu = XPLMCreateMenu(
                        "SPU",
                        XPLMFindPluginsMenu(),
                        smoothSubMenuItem, 			// Menu Item to attach to.
                        smoothMenuHandlerCallback,	// The handler
                        0);						// Handler Ref


    XPLMAppendMenuItem(smoothMenu, "Toggle", (void *)0, 1);
    XPLMAppendMenuItem(smoothMenu, "Infowindow", (void *)1, 1);
    XPLMAppendMenuItem(smoothMenu, "Debug", (void *)2, 1);

    return 1;
}


PLUGIN_API void	XPluginStop(void)
{
    XPLMDestroyWindow(gWindow);
    XPLMUnregisterCommandHandler(SmoothBreakCommand, SmoothBreakCommandHandler, 0, 0);
    XPLMUnregisterFlightLoopCallback(SmoothLoopCallback, NULL);
}


PLUGIN_API void XPluginDisable(void)
{
    //XPLMDebugString("SmoothStick> Stopping plugin.\n");
    XPLMSetDatai(XPLMFindDataRef("sim/operation/override/override_joystick"), 0);
}


PLUGIN_API int XPluginEnable(void)
{
    //XPLMDebugString("SmoothStick> Starting plugin.\n");
    if (G_TOGGLE) XPLMSetDatai(XPLMFindDataRef("sim/operation/override/override_joystick"), 1);
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

    XPLMSetWindowIsVisible(gWindow, G_VISIBLE);

    /******* axis data processing *******/
    XPLMGetDatavf(XPLMFindDataRef("sim/joystick/joystick_axis_values"), AxisValue, 0, 100);
    // FIXME: when released, jumps to 0 (center)
    pitchValue = (AxisValue[idxElvAxis] - 0.5f) * 2.0f;
    rollValue =  (AxisValue[idxAilAxis] - 0.5f) * 2.0f;

    SmoothenValue(&pitchValue, &sPitchValue, &smoothStepPitch);
    SmoothenValue(&rollValue, &sRollValue, &smoothStepRoll);

    if (G_TOGGLE)
    {
        XPLMSetDataf(XPLMFindDataRef("sim/joystick/yoke_pitch_ratio"), sPitchValue);
        XPLMSetDataf(XPLMFindDataRef("sim/joystick/yoke_roll_ratio"), sRollValue);
    } else {
        XPLMSetDataf(XPLMFindDataRef("sim/joystick/yoke_pitch_ratio"), pitchValue);
        XPLMSetDataf(XPLMFindDataRef("sim/joystick/yoke_roll_ratio"), rollValue);
    }

    /************/

    return -1;
}


int	SmoothBreakCommandHandler(XPLMCommandRef     inCommand,
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

    if(G_DEBUG)
    {
        sprintf(pitchLabel,     "Input pitch: %f", pitchValue);
        sprintf(rollLabel,      "Input roll: %f",  rollValue);
        sprintf(sPitchLabel,    "Output pitch: %f", sPitchValue);
        sprintf(sRollLabel,     "Output roll: %f",  sRollValue);
        sprintf(axisDebug,      "Axis[%d]: %f - Axis[%d]: %f",
                idxAilAxis, (float)AxisValue[idxAilAxis],
                idxElvAxis, (float)AxisValue[idxElvAxis]);
    } else {
        sprintf(sPitchLabel, "");
        sprintf(sRollLabel, "");
        sprintf(axisDebug, "");

        if (G_TOGGLE)
        {
            sprintf(pitchLabel, "Sidestick: ON");
            sprintf(rollLabel,  "Breaking : ON");
        } else {
            sprintf(pitchLabel, "Sidestick: OFF");
            sprintf(rollLabel,  "Breaking : OFF");
        }

    }

    XPLMDrawString(colorWhite, left + 5, top - 35, pitchLabel, NULL, xplmFont_Basic);
    XPLMDrawString(colorWhite, left + 5, top - 50, rollLabel, NULL, xplmFont_Basic);
    XPLMDrawString(colorYellow, left + 5, top - 65, sPitchLabel, NULL, xplmFont_Basic);
    XPLMDrawString(colorYellow, left + 5, top - 80, sRollLabel, NULL, xplmFont_Basic);
    XPLMDrawString(colorYellow, left + 5, top - 95, axisDebug, NULL, xplmFont_Basic);
}



void smoothMenuHandlerCallback(
        void *               inMenuRef,
        void *               inItemRef)
{
    (void)inMenuRef;
    if (inItemRef == (void *)1)
    {
        XPLMDebugString("SmoothStick> Debug window visibility toggle.\n");
        if (G_VISIBLE) { G_VISIBLE = 0; } else { G_VISIBLE = 1; }
    }

    if (inItemRef == (void *)0)
    {
        XPLMDebugString("SmoothStick> SPU active toggle.\n");
        if (G_TOGGLE) { G_TOGGLE = 0; } else { G_TOGGLE = 1; }
    }

    if (inItemRef == (void *)2)
    {
        XPLMDebugString("SmoothStick> Axis debug info toggle.\n");
        if (G_DEBUG) { G_DEBUG = 0; } else { G_DEBUG = 1; }
    }
}











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
