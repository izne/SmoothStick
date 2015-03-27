/*
 * SPU.h
 * Smooth Processing Unit
 * Part of SPU X-Plane plugin
 *
 * Dimitar Angelov
 * Hamburg, Deutschland
 * März 2015
 */

#ifndef SPU_H
#define SPU_H

void FindAxis(int axisType[100], int *indexElevatorAxis, int *indexAileronAxis);
void SmoothenValue(float *rawValue, float *smoothValue, float *smoothStep);

#endif // SPU_H
