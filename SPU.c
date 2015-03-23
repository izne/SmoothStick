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
