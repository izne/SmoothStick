/*
 * SPU.c
 * Smooth Processing Unit
 * Part of SPU X-Plane plugin
 *
 * Dimitar Angelov
 * Hamburg, Deutschland
 * März 2015
 */

#include "SPU.h"


void SmoothenValue(float *rawValue, float *smoothValue, float *smoothStep)
{
    if (*smoothValue != *rawValue)
    {
        if (*rawValue > 0.0f) // DEFLECTING RIGHT
        {
            if(*smoothValue < *rawValue)
            {
                *smoothValue += *smoothStep; // FURTHER RIGHT
            } else {
                *smoothValue -= *smoothStep; // BACK TO CENTER
            }

            if (*smoothValue > *rawValue) *smoothValue = *rawValue;
        }

        if (*rawValue < 0.0f) // DEFLECTING LEFT
        {
            if(*smoothValue > *rawValue)
            {
                *smoothValue -= *smoothStep; // FURTHER LEFT
            } else {
                *smoothValue += *smoothStep; // BACK TO CENTER
            }

            if (*smoothValue < *rawValue) *smoothValue = *rawValue;
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

