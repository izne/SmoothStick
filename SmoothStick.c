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
    strcpy(outName, "SmoothStick");
    strcpy(outSig,  "angelov.SmoothStick");
    strcpy(outDesc, "A plugin that smoothens the input of the joystick.");

/*
    SmoothStickCommand = XPLMCreateCommand(
                "sim/flight_controls/smoothstick_joystick",
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
                    1,							/* Start visible. */
                    DrawWindowCallback,			/* Callbacks */
                    KeyCallback,
                    MouseClickCallback,
                    NULL);						/* Refcon - not used. */

    smoothSubMenuItem = XPLMAppendMenuItem(
                        XPLMFindPluginsMenu(),
                        "SmoothStick",
                        0,						// Item Ref
                        1);						// Force English

    smoothMenu = XPLMCreateMenu(
                        "",
                        XPLMFindPluginsMenu(),
                        smoothSubMenuItem, 			// Menu Item to attach to.
                        smoothMenuHandlerCallback,	// The handler
                        0);						// Handler Ref


     // For each command, we set the item refcon to be the key command ID we wnat
     // to run.   Our callback will use this item refcon to do the right command.
     // This allows us to write only one callback for the menu.
    XPLMAppendMenuItem(smoothMenu, "Toggle", (void *) xplm_key_pause, 1);
    XPLMAppendMenuItem(smoothMenu, "Debug window", (void *) xplm_key_revthrust, 1);
    XPLMAppendMenuItem(smoothMenu, "Settings", (void *) xplm_key_jettison, 1);

    idxElvAxis = 2;
    idxAilAxis = 3;

    return 1;
}


PLUGIN_API void	XPluginStop(void)
{
    XPLMDestroyWindow(gWindow);
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
    return 1;
}


PLUGIN_API void XPluginReceiveMessage(
                    XPLMPluginID	inFromWho,
                    long			inMessage,
                    void *			inParam
        )
{
    (void)inFromWho;
    (void)inMessage;
    (void)inParam;
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


    // processing code
    XPLMGetDatavi(XPLMFindDataRef("sim/joystick/joystick_axis_assignments"), AxisType, 0, 100);
    XPLMGetDatavf(XPLMFindDataRef("sim/joystick/joystick_axis_values"), AxisValue, 0, 100);
    FindAxis(AxisType, &idxElvAxis, &idxAilAxis);

    // FIXME: when released...
    pitchValue = (AxisValue[idxElvAxis] - 0.5f) * 2.0f;
    rollValue =  (AxisValue[idxAilAxis] - 0.5f) * 2.0f;

    SmoothenPitch(&pitchValue, &sPitchValue, &smoothStepPitch);
    SmoothenRoll(&rollValue, &sRollValue, &smoothStepRoll);

    XPLMSetDataf(XPLMFindDataRef("sim/joystick/yoke_pitch_ratio"), sPitchValue);
    XPLMSetDataf(XPLMFindDataRef("sim/joystick/yoke_roll_ratio"), sRollValue);



    // Display code
    sprintf(pitchLabel, "pitch: %f", pitchValue);
    sprintf(rollLabel,  "roll: %f",  rollValue);
    sprintf(sPitchLabel, "smooth pitch: %f", sPitchValue);
    sprintf(sRollLabel,  "smooth roll: %f",  sRollValue);
    sprintf(axisDebug,  "axis%d: %f :: axis%d: %f", idxAilAxis, (float)AxisValue[idxAilAxis], idxAilAxis, (float)AxisValue[idxElvAxis]);

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
    XPLMCommandKeyStroke((XPLMCommandKeyID) inItemRef);
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
