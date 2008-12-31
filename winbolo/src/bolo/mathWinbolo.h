

/*
 * $Id$
 *
 * Copyright (c) 1998-2008 John Morrison.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


/*
 * Filename:      mathWinbolo.h
 * Author:        Chris Lesnieski
 * Creation Date: 2008-12-16
 * Last Modified: 2008-12-16
 * Purpose: Provides functions that the WinBolo maths
 *
 */

#ifndef MATHWINBOLO_H
#define MATHWINBOLO_H

struct vectorBody {
	TURNTYPE  angle;
	TURNTYPE  angle_prev;
	SPEEDTYPE speedX;
	SPEEDTYPE speedY;
	SPEEDTYPE speedX_prev;
	SPEEDTYPE speedY_prev;
};




/*
 * TODO: add a parameter that let's someone specify the number of places they want after the decimal.
 */
float mathConvertRadianToDegree(float);


/*
 */
float mathConvertDegreeToRadian(float);


/*
 * Usually, this will be used when converting tank direction to some other sort of direction.
 * Tank direction is stored as binary radians (256 slices of a circle).
 */
int mathConvertBradianToDegree(float);


/*
 * Is this shit even right?
 */
float mathConvertDegreeToBradian(float);


/*
 * Gets a radian when passed a bradian
 */
float mathConvertBradianToRadian(float);


/*
 * Gets a bradian when passed a radian
 */
float mathConvertRadianToBradian(float);


/*
 * Gets the X speed of the body in question.
 */
SPEEDTYPE mathComponentXSpeed(SPEEDTYPE, TURNTYPE);


/*
 * Gets the Y speed of the body in question.  Since WinBolo's Y direction is
 * flip-flopped over the X-axis as to what it really should be, we need to take
 * that into account when we send the result back.  That's what the -1.0 is for.
 */
SPEEDTYPE mathComponentYSpeed(SPEEDTYPE, TURNTYPE);


/*
 * This will return the component (X or Y) speed after a collision.
 * If you're looking for the Y component be sure to multiply by -1.
 * Returns a integer that will denote that component's speed.
 */
SPEEDTYPE mathCollisionPostComponentSpeed(int massA, SPEEDTYPE speedA, int massB, SPEEDTYPE speedB);


/*
 * This will return, in degrees, the direction of the body after a collision.
 * Please pass in the WinBolo Y-component so that the degrees returned is accurate.
 */
int mathAngleTravelUsingComponents(float, float);

/*
 * Gonna be lots of explanation for this one, I suspect...
 */
void mathUpdateVectorBody(struct vectorBody*, SPEEDTYPE, WORLD, WORLD, WORLD, WORLD, TURNTYPE);

/*
 * Returns the distance between two points based on WORLD coordinates.
 */
float mathGetDistanceBetweenTwoPoints(WORLD x1, WORLD y1, WORLD x2, WORLD y2);


#endif /* MATHWINBOLO_H */