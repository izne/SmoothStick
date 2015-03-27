/*
 * SmoothStick.c
 *
 * Dimitar Angelov <funkamateur@gmail.com>
 * Hamburg, Deutschland
 * März 2015
 */



#include <stdio.h>

#include "XPLMDefs.h"
#include "XPLMPlanes.h"
#include "XPLMCamera.h"


#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"
#include "XPLMGraphics.h"
#include "XPLMDisplay.h"
#include "XPLMMenus.h"
#include "XPLMProcessing.h"

#include "SPU.h"




/*
 * Global
 */

char version[8] = "0.71";
char versionStr[256] = "SmoothStick v0.71";

XPLMWindowID	gWindow = NULL;
XPLMMenuID      smoothMenu;
XPLMCommandRef  SmoothBreakingCommand = NULL;
int             smoothSubMenuItem;

char            pitchLabel[256], rollLabel[256];
char            sPitchLabel[256], sRollLabel[256];
char            axisDebug[256];

float           pitchValue, rollValue, sPitchValue, sRollValue;
float           colorWhite[]                = { 1.0f, 1.0f, 1.0f };
float           colorYellow[]               = { 1.0f, 1.0f, 0.0f };
float           colorMagenta[]              = { 1.0f, 0.5f, 1.0f };
float           smoothStepPitch             = 0.005f; //0.02f
float           smoothStepRoll              = 0.005f; //0.02f

int             AxisType[100];
float           AxisValue[100];
int             idxAilAxis, idxElvAxis;


int TOGGLE = 1;
int VISIBLE = 1;
int DEBUG = 1;


float SmoothLoopCallback(float elapsedMe, float elapsedSim, int counter, void * refcon);

int	SmoothBreakingCommandHandler(
        XPLMCommandRef    	inCommand,
        XPLMCommandPhase  	inPhase,
        void *            	inRefcon);


static void DrawWindowCallback(
        XPLMWindowID         inWindowID,
        void *               inRefcon);

void	smoothMenuHandlerCallback(
                                   void *               inMenuRef,
                                   void *               inItemRef);

static void KeyCallback(
                                   XPLMWindowID         inWindowID,
                                   char                 inKey,
                                   XPLMKeyFlags         inFlags,
                                   char                 inVirtualKey,
                                   void *               inRefcon,
                                   int                  losingFocus);

static int MouseClickCallback(
                                   XPLMWindowID         inWindowID,
                                   int                  x,
                                   int                  y,
                                   XPLMMouseStatus      inMouse,
                                   void *               inRefcon);

