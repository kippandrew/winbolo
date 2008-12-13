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
 * Scroll.java
 *
 * Created on November 16, 2003, 6:01 PM
 */

package com.jbolo.bolo;

/**
 *  Handles scrolling on the screen. Auto scrolling and
 *  keeping the object in the centre of the screen
 *
 * @author  John Morrison
 */
public class Scroll {
    
    /** Center Offset from left edge of the screen is 8 map units */
    private static int SCROLL_CENTER = 8;
    
    /** Amount to do division by */
    private static float SCROLL_DIVIDE = 2.5f;
    
    /** The distance from the edge the tank has to beed to scroll when autoscroll is
     * off */
    private static int NO_SCROLL_EDGE = 2;
    
    /** Is autoscrolling on or off */
    private boolean autoScroll;
    
    private int scrollX;
    
    private int scrollY;
    
    private boolean xPositive;
    
    private boolean yPositive;
    
    private boolean autoScrollOverRide;
    
    private boolean mods;
    
    /** Gameworld we belong to */
    private GameWorld gw;
    
    /** Creates a new instance of Scroll
     *
     * @param gw GameWorld we belong to
     */
    public Scroll(GameWorld gw) {
        this.gw = gw;
        autoScroll = false;
        scrollX = 0;
        scrollY = 0;
        xPositive = true;
        yPositive = true;
        autoScrollOverRide = false;
        mods = false;
    }
    
    
    /**
     * Modifies the autoscrolling state
     *
     * @param isAuto Are we turning autoscrolling on
     */
    public void setType(boolean isAuto) {
        autoScroll = isAuto;
        autoScrollOverRide = false;
        if (isAuto == false) {
            scrollX = 0;
            scrollY = 0;
            mods = false;
        }
    }
    
    /**
     * Centres object at x,y
     *
     * @param x X map position of object
     * @param y Y map position of object
     * @return Point containing the screen X,Y co-ordinates
     */
    public Point centerObject(int x, int y) {
        autoScrollOverRide = false;
        return new Point(x - SCROLL_CENTER, y - SCROLL_CENTER);
    }
    
    public boolean update(Point point, Pillboxes pbs, int objectX, int objectY, boolean isTank, int gunsightX, int gunsightY, int speed, int armour, float angle, boolean manual) {
        boolean returnValue = true; // Value to return
         if (gw.isTankDead() == true) {
            returnValue = false;
        } else if (manual == true) {
            returnValue = scrollManual(point, objectX, objectY, angle);
        } else if (autoScroll == true && isTank == true && armour <= Tank.ARMOUR_FULL && autoScrollOverRide == false) {
            /* Calculate using the autoscroll function */
            returnValue = scrollAutoScroll(pbs, point, objectX, objectY, gunsightX, gunsightY, speed, angle);
        } else {
            /* calculate not using auto scroll functions */
            returnValue = scrollNoAutoScroll(point, objectX, objectY, angle);
        }
        return returnValue;
    }
    
    public boolean check(int xValue, int yValue, int objectX, int objectY) {
        boolean returnValue = true;
        /* Check to see if moving towards screen edge */
        if ((objectX - xValue) > GameWorld.MAIN_SCREEN_SIZE_X) {
            xValue++;
            returnValue = false;
        }
        if ((objectX-1) < xValue) {
            xValue--;
            returnValue = false;
        }
        if ((objectY - yValue) > GameWorld.MAIN_SCREEN_SIZE_Y) {
            returnValue = false;
        }
        if (objectY <= yValue) {
            returnValue = false;
        }
        return returnValue;
    }
    
    private boolean scrollManual(Point point, int objectX, int objectY, float angle) {
        boolean returnValue; /* Used internally */
        boolean leftPos;
        boolean rightPos;
        boolean upPos;
        boolean downPos;
        
        leftPos = false;
        rightPos = false;
        upPos = false;
        downPos = false;
        returnValue = false;
        autoScrollOverRide = true;
        
        if (angle >= Utils.BRADIANS_SSWEST && angle <= Utils.BRADIANS_NNWEST) {
            rightPos = true;
        }
        if (angle >= Utils.BRADIANS_NNEAST && angle <= Utils.BRADIANS_SSEAST) {
            leftPos = true;
        }
        if (angle >= Utils.BRADIANS_SEASTE && angle <= Utils.BRADIANS_SWESTW) {
            downPos = true;
        }
        if (angle <= Utils.BRADIANS_NEASTE || angle >= Utils.BRADIANS_NWESTW) {
            upPos = true;
        }
        
        /* Check to see if moving towards screen edge */
        if ((objectX - point.getX()) > GameWorld.MAIN_SCREEN_SIZE_X && leftPos == true) {
            point.setX(point.getX()+1);
            returnValue = true;
        }
        if ((objectX-2) < point.getX() && rightPos == false) {
            point.setX(point.getX()-1);
            returnValue = true;
        }
        if ((objectY - point.getY()) > GameWorld.MAIN_SCREEN_SIZE_Y && downPos == true) {
            point.setY(point.getY()+1);
            returnValue = true;
        }
        if (objectY <= point.getY() && upPos == true) {
            point.setY(point.getY()-1);
            returnValue = true;
        }
        if (mods == true && returnValue == true) {
            mods = false;
        }
        return returnValue;
    } 
    
    
    private boolean scrollNoAutoScroll(Point point, int objectX, int objectY, float angle) {
        boolean returnValue; /* Value to return */
        boolean leftPos;
        boolean rightPos;
        boolean upPos;
        boolean downPos;
        
        leftPos = false;
        rightPos = false;
        upPos = false;
        downPos = false;
        returnValue = false;
        
        if (angle >= Utils.BRADIANS_SSWEST && angle <= Utils.BRADIANS_NNWEST) {
            rightPos = true;
        }
        if (angle >= Utils.BRADIANS_NNEAST && angle <= Utils.BRADIANS_SSEAST) {
            leftPos = true;
        }
        if (angle >= Utils.BRADIANS_SEASTE  && angle <= Utils.BRADIANS_SWESTW) {
            downPos = true;
        }
        if (angle <= Utils.BRADIANS_NEASTE || angle >= Utils.BRADIANS_NWESTW) {
            upPos = true;
        }
        /* Check to see if moving towards screen edge */
        if ((objectX - point.getX()) >= (GameWorld.MAIN_SCREEN_SIZE_X-NO_SCROLL_EDGE) && leftPos == true) {
            point.setX(point.getX()+1);
            returnValue = true;
        }
        if ((objectX-1) < point.getX()+NO_SCROLL_EDGE && rightPos == true) {
            point.setX(point.getX()-1);
            returnValue = true;
        }
        
        if ((objectY - point.getY()) >= (GameWorld.MAIN_SCREEN_SIZE_Y-NO_SCROLL_EDGE) && downPos == true) {
            point.setY(point.getY()+1);
            returnValue = true;
        }
        if (objectY <= point.getY()+NO_SCROLL_EDGE && upPos == true) {
            point.setY(point.getY()-1);
            returnValue = true;
        }
        if (mods == true && returnValue == true) {
            mods = false;
        }
        if (returnValue == true) {
            autoScrollOverRide = false;
        }
        return returnValue;
    }
    
    private boolean scrollAutoScroll(Pillboxes pbs, Point point, int objectX, int objectY, int gunsightX, int gunsightY, int speed, float angle) {
        boolean returnValue;        /* Value to return */
        
        returnValue = false;
        
        if (scrollX == 0 && scrollY == 0) {
            /* Check to see if moving towards screen edge */
            if (((gunsightX-1) - point.getX()) >= (GameWorld.MAIN_SCREEN_SIZE_X)) {
                scrollX = (int) (speed / SCROLL_DIVIDE);
                if (scrollX == 0) {
                    scrollX = 1;
                }
                xPositive = true;
                mods = true;
            }
            if ((gunsightX) < point.getX()) {
                scrollX = (int) (speed / SCROLL_DIVIDE);
                if (scrollX == 0) {
                    scrollX = 1;
                }
                xPositive = false;
                mods = true;
            }
            
            if (((gunsightY-1)- point.getY()) >= (GameWorld.MAIN_SCREEN_SIZE_Y)) {
                scrollY = (int) (speed / SCROLL_DIVIDE);
                if (scrollY == 0) {
                    scrollY = 1;
                }
                
                yPositive = true;
                mods = true;
            }
            if (gunsightY < point.getY()) {
                scrollY = (int) (speed / SCROLL_DIVIDE);
                if (scrollY == 0) {
                    scrollY = 1;
                }
                
                yPositive = false;
                mods = true;
            }
            
        }
        if (scrollX > 0) {
            returnValue = true;
            scrollX--;
            if (xPositive == true) {
                point.setX(point.getX()+1);
            } else {
                point.setX(point.getX()-1);
            }
        }
        if (scrollY > 0) {
            scrollY--;
            returnValue = true;
            if (yPositive == true) {
                point.setY(point.getY()+1);
            } else {
                point.setY(point.getY()-1);
            }
        }
        if (scrollX == 0 && scrollY == 0) {
            mods = false;
        }
        return returnValue;
    }
    
    
}