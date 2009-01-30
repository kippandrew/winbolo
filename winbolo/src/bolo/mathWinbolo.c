

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
 * Filename:      mathWinbolo.c
 * Author:        Chris Lesnieski
 * Creation Date: 2008-12-16
 * Last Modified: 2008-12-16
 *
 * Purpose: Provides functions that the WinBolo maths need.  You might
 * be able to write lots of #defines for these but I'd rather have
 * them as funtions.
 *
 */

/* Inludes */

#include <math.h>
#include <string.h>
#include "bases.h"
#include "bolo_map.h"
#include "global.h"
#include "pillbox.h"
#include "tank.h"
#include "util.h"
#include "types.h"
#include "mathWinbolo.h"



/*
 * TODO: add a parameter that let's someone specify the number of places they want after the decimal.
 */
float mathConvertRadianToDegree(float rad)
{
	return (float) (rad * RADIAN_TO_DEGREE_FACTOR);
}

/*
 */
float mathConvertDegreeToRadian(float deg)
{
	float a;
	a = (float) (deg * DEGREE_TO_RADIAN_FACTOR);
	return a;
}


/*
 * Usually, this will be used when converting tank direction to some other sort of direction.
 * Tank direction is stored as binary radians (256 slices of a circle).
 */
int mathConvertBradianToDegree(float brad)
{
	float a;
	a = mathConvertBradianToRadian(brad);
	a = mathConvertRadianToDegree(a);
	return (int) a;
}


/*
 * 
 */
float mathConvertDegreeToBradian(float deg)
{
	float a = mathConvertDegreeToRadian(deg);
	a = mathConvertRadianToBradian(a);
	return a;
}

/*
 * This function is not working right.
 */
float mathConvertBradianToRadian(float brad)
{
	/* Shift the origin so that it lines up with normal trig planes */
	brad += BRADIANS_EAST;

	if (brad > BRADIANS_MAX)
	{
		brad -= BRADIANS_MAX;
	}

	/* This will arrange it so that the growth from 0 -> 256 is
	 * counter-clockwise just like radians. */
	brad = (float) (BRADIANS_MAX - brad);

	/* This will give us a radian */
	brad *= (float) (BRADIAN_TO_RADIAN_FACTOR);

	brad -= (float) (RADIANS_MAX / 2);

	if (brad < 0)
	{
		brad += (float) RADIANS_MAX;
	}

	return brad;
}

/*
 * This function seems to be functioning properly.
 */
float mathConvertRadianToBradian(float rad)
{
	/* Multiply the radian so that it is now in bradian units */
	rad *= (float) RADIAN_TO_BRADIAN_FACTOR;

	/* Arrange the bradian back to it's post-right hand shift */
	rad = (float) (BRADIANS_MAX - rad);

	/* Subtract the rotation so that the origin is due north again */

	rad += BRADIANS_EAST;

	if (rad > BRADIANS_MAX)
	{
		rad -= BRADIANS_MAX;
	}

	return rad;
}

/*
 * Gets the X velocity of the body in question.
 */
SPEEDTYPE mathComponentXSpeed(SPEEDTYPE speed, TURNTYPE angle)
{
	float rad = mathConvertBradianToRadian(angle);
	return (SPEEDTYPE) floor((cos(rad) * speed) + 0.5);
}


/*
 * Gets the Y velocity of the body in question.  Since WinBolo's Y direction is
 * flip-flopped over the X-axis as to what it really should be, we need to take
 * that into account when we send the result back.  That's what the -1.0 is for.
 */
SPEEDTYPE mathComponentYSpeed(SPEEDTYPE speed, TURNTYPE angle)
{
	float a = (float) mathConvertBradianToDegree(angle);
	float rad = mathConvertDegreeToRadian(a);
	return (float) (floor((sin(rad) * speed) + 0.5) * -1);
}


/*
 * massA: mass of body A
 * speedA: speed of body A in x or y direction BEFORE collision
 * massB: mass of body B
 * speedB: speed of body B in x or y direction BEFORE collision
 *
 * This will return the component (X or Y) speed after a collision.
 * If you're looking for the Y component be sure to multiply this result by -1.
 * Returns a integer that will denote that component's speed.
 */
SPEEDTYPE mathCollisionPostComponentSpeed(int massA, SPEEDTYPE speedA, int massB, SPEEDTYPE speedB)
{
	return (((speedA * (massA-massB)) + (2 * massB * speedB))/(massA+massB));
}


/*
 * This will return, in degrees, the direction of the body after a collision.
 * Please pass in the WinBolo Y-component so that the degrees returned is accurate.
 */
int mathAngleTravelUsingComponents(float speedX, float speedY)
{
	float a;
 
	/* Tank is not moving horizontally */
	if (speedX == 0)
	{
		/* Tank is not moving vertically either, thus, not moving at all */
		if (speedY == 0)
		{
			return 0;
		}
		/* WinBolo means the Y axis is inverted. */
		else if (speedY > 0)
		{
			return 270;
		}
		else
		{
			return 90;
		}
	}
	/* Tank is not moving vertically */
	else if (speedY == 0)
	{
		/* Tank is not moving horizontally either, thus, not moving at all */
		if (speedX == 0)
		{
			return 0;
		}
		else if (speedX > 0)
		{
			return 0;
		}
		else
		{
			return 180;
		}
	}
	/* This body has movement in both the X and Y plane */
	else
	{
		/* Tank is moving NW */
		if ((speedX < 0) && (speedY < 0))
		{
			a = 180 + mathConvertRadianToDegree((float) atan((speedY * -1)/speedX));
		}
		/* Tank is moving SW */
		else if ((speedX < 0) && (speedY > 0))
		{
			a = (float) (180 + (float) (mathConvertRadianToDegree((float) atan((speedY * -1)/speedX))));
		}
		/* Tank is moving SE */
		else if ((speedX > 0) && (speedY > 0))
		{
			a = mathConvertRadianToDegree((float) atan((speedY * -1)/speedX));
		}
		/* Tank is moving NE */
		else if ((speedX > 0) && (speedY < 0))
		{
			a = mathConvertRadianToDegree((float) atan((speedY * -1)/speedX));
		}
		return (int) a;
	}

	/* Should never get to this point.. */
	return 0;
}


/*
 * The only way to truly set the angle property correctly is to take
 * the last set of tank coordinates and compare them to the current set
 * of tank coordinates.  If we use the tank's ANGLE property, that simply
 * states in what direction is the tank pointing in, not necessarily
 * where it's actually moving.
 *
 * Something's wrong with the speedX and speedY components.  They're somehow
 * not being set correctly, I believe.  When the tank is travelling due east
 * it's speedX will be negative when it should be positive.
 */
void mathUpdateVectorBody(struct vectorBody *vB, SPEEDTYPE speed, WORLD currentX, WORLD currentY, WORLD prevX, WORLD prevY, TURNTYPE angle)
{
	int i;
	float actualTankSpeed;

	/* This is just here for debugging purposes.  It allows me to
	 * catch the tanks speed, angle, and component speeds. */
	if (speed > 10.0)
	{
		if ((currentX - prevX < 0) || ((*vB).speedX < 0))
		{
			i = 0;
		}
	}

	actualTankSpeed = mathGetDistanceBetweenTwoPoints(currentX, currentY, prevX, prevY);

	(*vB).speedX_prev = (*vB).speedX;
	(*vB).speedY_prev = (*vB).speedY;
	(*vB).angle_prev = (*vB).angle;

	/* There is no difference between the current and previous coordinates or the previous coordinates aren't
	 * set to the map origin, which means the tank is brand new.
	 */
	if (((currentX-prevX == 0.0) && (currentY-prevY == 0.0)) || ((prevX == 0) && (prevY == 0)))
	{
		(*vB).angle = angle;
		(*vB).speedX = 0.0;
		(*vB).speedY = 0.0;
	}
	else
	{
		(*vB).angle = (TURNTYPE)mathConvertDegreeToBradian((float) mathAngleTravelUsingComponents((float) currentX-prevX, (float) currentY-prevY));
		(*vB).speedX = mathComponentXSpeed(actualTankSpeed, (*vB).angle);
		(*vB).speedY = mathComponentYSpeed(actualTankSpeed, (*vB).angle);
	}
}

/*
 *
 */
float mathGetDistanceBetweenTwoPoints(WORLD x1, WORLD y1, WORLD x2, WORLD y2)
{
	return (float) sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}
