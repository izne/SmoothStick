/*
 * SPU.c
 * Smooth Processing Unit
 * Part of SmoothStick X-Plane plugin
 *
 * Dimitar Angelov <funkamateur@gmail.com>
 * Hamburg, Deutschland
 * März 2015
 */

#include "SPU.h"

void SmoothenRoll(float *rollValue, float *sRollValue, float *smoothStepRoll)
{
    if (*sRollValue != *rollValue)
    {
        if (*rollValue > 0.0f) // DEFLECTING RIGHT
        {
            if(*sRollValue < *rollValue)
            {
                *sRollValue += *smoothStepRoll; // FURTHER RIGHT
            } else {
                *sRollValue -= *smoothStepRoll; // BACK TO CENTER
            }

            if (*sRollValue > *rollValue) *sRollValue = *rollValue;
        }

        if (*rollValue < 0.0f) // DEFLECTING LEFT
        {
            if(*sRollValue > *rollValue)
            {
                *sRollValue -= *smoothStepRoll; // FURTHER LEFT
            } else {
                *sRollValue += *smoothStepRoll; // BACK TO CENTER
            }

            if (*sRollValue < *rollValue) *sRollValue = *rollValue;
        }
    }
}

void SmoothenPitch(float *pitchValue, float *sPitchValue, float *smoothStepPitch)
{
    if (*sPitchValue != *pitchValue)
    {
        if (*pitchValue > 0.0f) // DEFLECTING UP
        {
            if(*sPitchValue < *pitchValue)
            {
                *sPitchValue += *smoothStepPitch; // FURTHER UP
            } else {
                *sPitchValue -= *smoothStepPitch; // BACK TO CENTER
            }

            if (*sPitchValue > *pitchValue) *sPitchValue = *pitchValue;
        }
        else // DEFLECTING DOWN
        {
            if(*sPitchValue > *pitchValue)
            {
                *sPitchValue -= *smoothStepPitch; // FURTHER DOWN
            } else {
                *sPitchValue += *smoothStepPitch; // BACK TO CENTER
            }

            if (*sPitchValue < *pitchValue) *sPitchValue = *pitchValue;
        }
    }
}

void FindAxis(int axisType[100], int *indexElevatorAxis, int *indexAileronAxis)
{
    for (int i = 0; i < 100; i++)
    {
        if (axisType[i] == 1)
        {
            *indexElevatorAxis = i;
        }

        if (axisType[i] == 2)
        {
            *indexAileronAxis = i;
        }

        /* Rudder for future versions
        if (AxisType[i] == 3)
        {
            indexRudderAxis = i;
        }
        */
    }

}

