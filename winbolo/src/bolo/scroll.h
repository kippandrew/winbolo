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


/*********************************************************
*Name:          scroll
*Filename:      scroll.h
*Author:        John Morrison
*Creation Date:  16/1/99
*Last Modified: 19/11/99
*Purpose:
*  Handles scrolling on the screen. Auto scrolling and
*  keeping the object in the centre of the screen
*********************************************************/

#ifndef SCROLL_H
#define SCROLL_H

#include "global.h"
#include "screen.h"
#include "types.h"


/* Defines */

/* Center Offset from left edge of the screen is 4 map units */
#define SCROLL_CENTER 8
/* Amount to do division by */
#define SCROLL_DIVIDE 2.5

/* The distance from the edge the tank has to beed to scroll when autoscroll is off */
#define NO_SCROLL_EDGE 2

/* Prototypes */

/*********************************************************
*NAME:          scrollSetScrollType
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 16/1/99
*PURPOSE:
*  Modifies state of autoscrolling
*
*ARGUMENTS:
*  isAuto - Set to on or off?
*********************************************************/
void scrollSetScrollType(bool isAuto);

/*********************************************************
*NAME:          scrollCenterObject
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 16/1/99
*PURPOSE:
*  Centres the screen on the object
*
*ARGUMENTS:
*  xValue - Pointer to hold new X co-ordinate
*  yValue - Pointer to hold new Y co-ordinate
*  objectX - Object to centre on X co-ordinate 
*  objectY - Object to centre on Y co-ordinate 
*********************************************************/
void scrollCenterObject(BYTE *xValue, BYTE *yValue, BYTE objectX, BYTE objectY);

/*********************************************************
*NAME:          scrollUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 16/1/99
*PURPOSE:
*  Called every game tick. Checks to see if the screen 
*  is required to be moved because the tank has moved
*  etc.
*  If the object is not a tank the last 3 parameters
*  are ignored. It returns if a recalculation is needed
*
*ARGUMENTS:
*  pb        - Pointer to the pillboxes structure
*  xValue    - Pointer to hold new X co-ordinate
*  yValue    - Pointer to hold new Y co-ordinate
*  objectX   - Object to centre on X co-ordinate 
*  objectY   - Object to centre on Y co-ordinate 
*  isTank    - Is the object a tank
*  gunsightX - The gunsights X position
*  gunsightY - The gunishgts Y position
*  speed     - The speed of the tank
*  armour    - Amount of armour on the tank
*  angle     - Tank travelling angle
*  manual    - Is it manual move (ie by keys, not tank)
*********************************************************/
bool scrollUpdate(pillboxes *pb, BYTE *xValue, BYTE *yValue, BYTE objectX, BYTE objectY, bool isTank, BYTE gunsightX, BYTE gunsightY, BYTE speed, BYTE armour, TURNTYPE angle, bool manual);

/*********************************************************
*NAME:          scrollCheck
*AUTHOR:        John Morrison
*CREATION DATE: 19/11/99
*LAST MODIFIED: 19/11/99
*PURPOSE:
*  Returns whether an item is on screen or not
*
*ARGUMENTS:
*  xValue    - Current X co-ordinate
*  yValue    - Current Y co-ordinate
*  objectX   - Objects X co-ordinate 
*  objectY   - Objects Y co-ordinate 
*********************************************************/
bool scrollCheck(BYTE xValue, BYTE yValue, BYTE objectX, BYTE objectY);

/*********************************************************
*NAME:          scrollManual
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 10/06/01
*PURPOSE:
*  Movement scroll keys have been pressed. Returns if a 
*  movement occurs
*
*ARGUMENTS:
*  xValue    - Pointer to hold new X co-ordinate
*  yValue    - Pointer to hold new Y co-ordinate
*  objectX   - Objects X co-ordinate 
*  objectY   - Objects Y co-ordinate 
*  angle     - Items Angle
*********************************************************/
bool scrollManual(BYTE *xValue, BYTE *yValue, BYTE objectX, BYTE objectY, TURNTYPE angle);

/*********************************************************
*NAME:          scrollNoAutoScroll
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 19/11/99
*PURPOSE:
*  Checks to see if the screen is required to be moved 
*  because the object is moving off screen. Doesn't 
*  use autoscrolling features. Returns if a recalculation
*  of the screen is needed
*
*ARGUMENTS:
*  xValue    - Pointer to hold new X co-ordinate
*  yValue    - Pointer to hold new Y co-ordinate
*  objectX   - Object to centre on X co-ordinate 
*  objectY   - Object to centre on Y co-ordinate 
*  angle     - Turntype angle
*********************************************************/
bool scrollNoAutoScroll(BYTE *xValue, BYTE *yValue, BYTE objectX, BYTE objectY, TURNTYPE angle);

/*********************************************************
*NAME:          scrollAutoScroll
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 16/1/99
*PURPOSE:
*  Checks to see if the screen is required to be moved 
*  because the object is moving off screen. Uses the
*  autoscrolling features. Returns if a recalculation
*  of the screen is needed
*
*ARGUMENTS:
*  pb        - Pointer to the pillboxes structure
*  xValue    - Pointer to hold new X co-ordinate
*  yValue    - Pointer to hold new Y co-ordinate
*  objectX   - Object to centre on X co-ordinate 
*  objectY   - Object to centre on Y co-ordinate 
*  gunsightX - The gunsights X position
*  gunsightY - The gunishgts Y position
*  speed     - The speed of the tank
*  angle     - Angle of the tank
*********************************************************/
bool scrollAutoScroll(pillboxes *pb, BYTE *xValue, BYTE *yValue, BYTE objectX, BYTE objectY, BYTE gunsightX, BYTE gunsightY, BYTE speed, TURNTYPE angle);

#endif /* SCROLL_H */

