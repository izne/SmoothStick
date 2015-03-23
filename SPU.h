/*
 * SPU.c
 * Smooth Processing Unit
 * Part of SmoothStick X-Plane plugin
 *
 * Dimitar Angelov <funkamateur@gmail.com>
 * Hamburg, Deutschland
 * März 2015
 */

#ifndef SPU_H
#define SPU_H

void FindAxis(int axisType[100], int *indexElevatorAxis, int *indexAileronAxis);
void SmoothenPitch(float *pitchValue, float *sPitchValue, float *smoothStepPitch);
void SmoothenRoll(float *rollValue, float *sRollValue, float *smoothStepRoll);

#endif // SPU_H
