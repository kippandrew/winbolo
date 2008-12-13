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
 * Tank.java
 *
 * Created on October 11, 2003, 9:19 PM
 */

package com.jbolo.bolo;

import com.jbolo.bolo.map.BoloMap;
import java.util.ArrayList;
import java.util.ListIterator;

/** Tank represents a tank in the game
 * @author John Morrison
 */
public class Tank extends GameWorldItem {
    
    /** Amount to subtract from the tank middle to get the left co-ordinate for drawing */
    public static final int TANK_SUBTRACT = 128;
    /** Full amount of armour in the tank */
    public static final int ARMOUR_FULL = 40;
    /** Full amount of shells in the tank */
    public static final int SHELLS_FULL = 40;
    /** Full amount of mines in the tank */
    public static final int MINES_FULL = 40;

    /** The tank has 16 frames */
    public static final int TANK_FRAMES = 16;
    
    /** The square to test to see if the tank is hit */
    public static final int TANK_SUBTRACT_SHELL_HIT_X1 = 144;
    /** The square to test to see if the tank is hit */
    public static final int TANK_SUBTRACT_SHELL_HIT_X2 = 128;
    /** The square to test to see if the tank is hit */
    public static final int TANK_SUBTRACT_SHELL_HIT_Y = 128;
    
    /** Tank has been missed by a shell */
    public static final int TH_MISSED = 0;
    /** Tank has been hit by a shell but not killed */
    public static final int TH_HIT = 1;
    /** The tank has been hit by a shell, killsed and isn't carrying a lot of stuff so a small explosion */
    public static final int TH_KILL_SMALL = 2;
    /* The tank has been hit by a shell, killsed and is carrying a lot of stuff so a big explosion */
    public static final int TH_KILL_BIG = 3;
    
    /** Tank autoslowdown rate */
    private static float TANK_AUTOSLOW_SPEED = 0.2f;
    /** Maximuns lengths for the gunsight (in halve map squares) */
    private static int GUNSIGHT_MAX = 14;
    /** Minimum lengths for the gunsight (in halve map squares) */
    private static int GUNSIGHT_MIN = 2;
    /** Amount to modify move position to check surround terrain when the tank is on a boat */
    private static int TANK_MOVE_BOAT_SUB = 64;
    /** Amount to modify move position to check surround terrain when the tank is on land */
    private static int TANK_MOVE_LAND_SUB = 128;
    /** The time taken for a tank to reload after firing */
    private static int RELOAD_TIME = 13;
    /** The time taken in water before reducing stocks */
    private static int WATER_TIME = 15;
    /** Amount of armour damage done wben hitting a mine */
    private static int MINE_DAMAGE = 10;
    /** Amount of armour damage done when hit but a shell */
    private static int DAMAGE = 5;
    /** The middle pixel of a map square is 8 */
    private static int MIDDLE_PIXEL = 8;
    /** Amount to slow down when we are coliding witha wall */
    private static int TANK_WALL_SLOW_DOWN = 1;
    /** Shells + mines must be greater then 20 for a big explosion */
    private static int BIG_EXPLOSION_THRESHOLD = 20;
    /** Amount the tank slows down when the slow key is pressed */
    private static float TANK_SLOWKEY_RATE = 0.35f;
    /** Amount of slowdown the tank does when it comes across a slower terrain */
    private static float TANK_TERRAIN_DECEL_RATE = 0.2f;
    /** Amoiunt of acceleration the tank does when it is accelerator */
    private static float TANK_ACCELERATE_RATE = 0.5f;
    
    /** Speed we exit the boat at */
    public static int BOAT_EXIT_SPEED = 16;
    
    /** Amount of armour this tank has */
    private int armour;
    /** Amount of shells this tank has */
    private int shells;
    /** Amount of mines this tank has */
    private int mines;
    /** Amount of trees this tank has */
    private int trees;
    /** Angle this tank is travelling */
    private float angle;
    /** Speed of the tank */
    private float speed;
    /** Reloading time between firing */
    private int reload;
    /** Is this tank on a boat*/
    private boolean onBoat;
    /** Is this tank showing its gunsight */
    private boolean showSight;
    /** The gunsight length */
    private int sightLen;
    /** Number of kills this tank has */
    private int numKills;
    /** Number of deaths this tank has */
    private int numDeaths;
    /** Time between respawns after the tank dies */
    private int deathWait;
    /** Amount of time the tank has spent in water (Terrain.RIVER) */
    private int waterCount;
    /** Is this tank obstructed */
    private boolean obstructed;
    /** Is this a new tank */
    private boolean newTank;
    /** Does this tank have autoslowdown enabled */
    private boolean autoSlowdown;
    /** Has this tank got auto show/hide gunsight enabled */
    private boolean autoHideGunsight;
    /** Has this tank just fired */
    private boolean justFired;
    
    /** An array of Integer objects that this tank is carrying. Refers to itemNum
     * in the Pillboxes object */
    private ArrayList carriedPills;
    /** Is this tank shutting down */
    private boolean shuttingDown;
    
    /** This tanks player number */
    private int playerNum;
    
    /** How long the left key has been held down */
    private int firstLeft;
    /** How long the right key has been held down */
    private int firstRight;
    
    private int tankHitCount;
    
    /** The game world that created this Tnak */
    private GameWorld gw;
    
    /** Creates a new instance of Tank
     * @param gw The game world that created this Tank
     * @param gt The game type of this game
     * @param sts The starts object
     */
    public Tank(GameWorld gw, GameType gt, Starts sts) {
        super(0, 0);
        Start s;
        
        this.gw = gw;
        shuttingDown = false;
        firstLeft = 0;
        firstRight = 0;
        armour = gt.getArmour();
        shells = gt.getShells();
        mines = gt.getMines();
        trees = gt.getTrees();
        onBoat = true;
        showSight = true;
        sightLen = GUNSIGHT_MAX;
        numKills = 0;
        numDeaths = 0;
        reload = 0;
        speed = 0;
        waterCount = 0;
        deathWait = 0;
        carriedPills = new ArrayList(16);
        obstructed = false;
        newTank = true;
        autoSlowdown = false;
        autoHideGunsight = false;
        justFired = false;
        tankHitCount = 0;
        
        s = sts.getStart();
        x = Utils.GetWorldFromMap(s.getX());
        y = Utils.GetWorldFromMap(s.getY());
        x += BoloMap.MAP_SQUARE_MIDDLE;
        y += BoloMap.MAP_SQUARE_MIDDLE;
        angle = s.getStartDir();
        
        gw.centerTank();
    }
    
    /** Sets this tanks Player Number
     * @param playerNum Player number to set
     */
    public void setPlayerNum(int playerNum) {
        this.playerNum = playerNum;
    }
    
    /** Returns the player number of this tank
     * @return Player number
     */
    public int getPlayerNum() {
        return playerNum;
    }
    
    /**
     * Sets whether autoslowdown is enabled for this tank
     * @param state State to set
     */
    public void setAutoslowdownState(boolean state) {
        autoSlowdown = state;
    }
    
    /**
     * Sets whether autohide gunsight is enabled for this tank
     * @param state State to set
     */
    public void setAutohideGunsight(boolean state) {
        autoHideGunsight = state;
    }
    
    /**
     * Gets whether autoslowdown is enabled for this tank
     * @return if autoslowdown is enabled
     */
    public boolean getAutoslowdownState() {
        return autoSlowdown;
    }
    
    /**
     * Gets the gunsight length of this tank
     *
     * @return int
     */
    public int getGunsightLength() {
        return sightLen;
    }
    
    /**
     * Gets the reload time of this tank
     *
     * @return int
     */
    public int getReloadTime() {
        return reload;
    }
    
    /**
     * Gets whether autohide gunsight is enabled
     * @return if autohide gunsight is enabled
     */
    public boolean getAutohideGunsight() {
        return autoHideGunsight;
    }
    
    
    /**
     * Increases the tanks gunsight range
     */
    public void increaseGunsightRange() {
        if (sightLen < this.GUNSIGHT_MAX) {
            sightLen++;
        } else if (autoHideGunsight == true && showSight == true) {
            showSight = false;
            if (gw.isServer() == false) {
                gw.getFrontEndClient().feShowGunsight(false);
            }
        }
    }
    
    /**
     * Decreases the tanks gunsight range
     */
    public void decreaseGunsightRange() {
        if (sightLen > GUNSIGHT_MIN) {
            sightLen--;
        }
        if (showSight == false && autoHideGunsight == true) {
            showSight = true;
            if (gw.isServer() == false) {
                gw.getFrontEndClient().feShowGunsight(true);
            }
        }
    }
    
    /** Called every game tick to update the tanks position, fire shells etc
     * @param mp - The map
     * @param pbs - The pillboxes on this map
     * @param bss - The bases on this map
     * @param shs - The shells on this map
     * @param sts - The starts on this map
     * @param tb - The TankButtons pressed
     * @param tankShoot - Is this tank shoting
     * @param inBrain - Is this bbrain is being run by a brain
     */
    public void update(BoloMap mp, Bases bss, Pillboxes pbs, Shells shs, Starts sts, int tb, boolean tankShoot, boolean inBrain) {
        int bmx; /* Tank X & Y map co-ordinates */
        int bmy;
        
        obstructed = false;
        justFired = false;
        
        bmx = getMX();
        bmy = getMY();
        /* Update the reload count */
        if (reload > 0) {
            reload--;
        }
        
        /* Shoot if required */
        if (tankShoot == true && reload == 0 && shells > 0 && isDead() == false) {
            double a;
            double b = 2;
            double c;
            
            a = sightLen;
            c = a / b;
            shs.add(x, y, angle, (int) c, playerNum, onBoat);
            reload = RELOAD_TIME;
            shells--;
            justFired = true;
            if (gw.isServer() == false) {
                gw.getSoundDist().playSound(SoundDist.SHOOT_SELF);
                gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, armour, trees);
            }
        }
        
        if (deathWait > 0) {
            deathWait--;
            if (deathWait == 0) {
                newTank = true;
            }
        } else if (isDead() == true) {
            if (gw.getInStartFind() == false) {
                death(sts);
            }
        } else if (onBoat == false && mp.getPos(bmx, bmy) == Terrain.DEEP_SEA) {
            /* Check for death */
            gw.getSoundDist().soundDist(SoundDist.TANK_SINK_NEAR, bmx, bmy);
            gw.getMessages().add(Messages.ASSISTANT_MESSAGE, "Online Assistant", "Tank Sunk in Deep Sea"); //FIXME text
            gw.getMNT().add(NetChangeMNT.NMNT_KILLME, 0, playerNum, (int) 0xFF, (int) 0xFF);
            dropPills(mp, pbs, bss);
            armour = -1;
            deathWait = 100;
        } else if (onBoat == true) {
            /* Tank movement on boat */
            newTank = false;
            moveOnBoat(mp, pbs, bss, bmx, bmy, tb, inBrain);
        } else {
            newTank = false;
            moveOnLand(mp, pbs, bss, bmx, bmy, tb, inBrain);
        }
    }
    
    
    /** Responsible for moving the tank when it is on a boat
     * @param mp - The map
     * @param pbs - The pillboxes on this map
     * @param bss - The bases on this map
     * @param shs - The shells on this map
     * @param bmx - Tank X map positino
     * @param bmy - Tank Y map positoin
     * @param tb - The TankButtons pressed
     * @param inBrain - Is this bbrain is being run by a brain
     */
    private void moveOnLand(BoloMap mp, Pillboxes pbs, Bases bss, int bmx, int bmy, int tb, boolean inBrain) {
        boolean slowDown;
        int newx;
        int newy;
        int newmx;
        int newmy;
        Point p = new Point(0,0);
        
        newmx = bmx;
        newmy = bmy;
        /* Check terrain is clear */
        checkGroundClear(mp, pbs, bss);
        
        if (gw.isServer() == false) {
            tankTurn(mp, pbs, bss, bmx, bmy, tb);
        }
        tankAccel(mp, pbs, bss, bmx, bmy, tb);
        slowDown = false;
        
        /* Update location if speed > 0 */
        if (speed > 0) {
            /* If we have autoslowdown turned on - SLOW DOWN! */
            if (autoSlowdown == true && inBrain == false) {
                if (tb != TankButtons.TDECEL && tb != TankButtons.TLEFTDECEL && tb != TankButtons.TRIGHTDECEL && tb != TankButtons.TACCEL && tb != TankButtons.TLEFTACCEL && tb != TankButtons.TRIGHTACCEL) {
                    speed -= TANK_AUTOSLOW_SPEED;
                    if (speed < 0) {
                        speed = 0;
                    }
                }
            }
            int ang = Utils.Get16Dir(angle);
            p = Utils.CalcDistance(ang, speed);
            /* Check to make sure updating isn't going to run into something. If not update co-ordinates */
            newx = (x + p.getX());
            newy = (y + p.getY());
            
            if (p.getY() > 0) {
                newy += TANK_MOVE_LAND_SUB+32;
            } else if (p.getY() < 0){
                newy -= TANK_MOVE_LAND_SUB-32;
            }  else if (ang > Utils.BRADIANS_EAST && ang < Utils.BRADIANS_WEST && ang != Utils.BRADIANS_SOUTH) {
                newy += TANK_MOVE_LAND_SUB+32;
            } else if (ang != Utils.BRADIANS_NORTH) {
                newy -= TANK_MOVE_LAND_SUB-32;
            }
            
            if (p.getX() >= 0) {
                newx += TANK_MOVE_LAND_SUB+32;
            } else if (p.getX() < 0) {
                newx -= TANK_MOVE_LAND_SUB-32;
            } else if (ang > Utils.BRADIANS_NORTH && ang < Utils.BRADIANS_SOUTH && ang != Utils.BRADIANS_EAST) {
                newx += TANK_MOVE_LAND_SUB+32;
            } else if (ang != Utils.BRADIANS_WEST) {
                newx -= TANK_MOVE_LAND_SUB-32;
            }
            
            
            newmx = Utils.GetMapFromWorld(newx);
            newmy = Utils.GetMapFromWorld(newy);
            
            if (mp.getSpeed(pbs, bss, bmx, newmy, gw.getPlayers().getSelf(), onBoat) > 0) {
                y = y + p.getY();
            } else {
                slowDown = true;
                obstructed = true;
            }
            
            if (mp.getSpeed(pbs, bss, newmx, bmy, gw.getPlayers().getSelf(), onBoat) > 0) {
                x = x + p.getX();
            } else if (slowDown == true) {
                speed -= TANK_WALL_SLOW_DOWN;
                if (speed < 0) {
                    speed =0;
                }
                obstructed = true;
            } else {
                obstructed = true;
            }
            
            newmx = getMX();
            newmy = getMY();
            if (newmy < bmy) {
                if (false) { // FIXME frontEndTutorial(newbmy) == true
                    speed = 0;
                }
            }
            
            /* Check for entering Boat */
            if ((mp.getPos(newmx, newmy)) == Terrain.BOAT) {
                if (gw.isServer() == false) { // FIXME || netGetType() == netSingle
                    gw.getMNT().add(NetChangeMNT.NMNT_DROPBOAT, 0, playerNum, newmx, newmy);
                    mp.setPos(newmx,newmy, Terrain.RIVER, true, false);
                }
                onBoat = true;
                gw.reCalc();
            }
            
            /* Check for hit mine */
            if (newmx != bmx || newmy != bmy) { /* && isServer == FALSE */
                if (mp.isMine(newmx, newmy) == true) {
                    gw.getMNT().add(NetChangeMNT.NMNT_MINEEXPLOSION, 0, playerNum, newmx, newmy);
                    gw.getMinesExp().add(newmx, newmy);
                }
            }
        }
        
        
        /* Check for tank in water */
        if (mp.getPos(bmx, bmy) == Terrain.RIVER && speed <= BoloMap.MAP_SPEED_TRIVER && onBoat == false) {
            if (bss.existPos(bmx, bmy) == false) {
                waterCount++;
                if (waterCount == WATER_TIME) {
                    waterCount = 0;
                    inWater();
                }
            }
        }
        
        /* Base Capture */
        if (gw.isServerOrSinglePlayerGame() == true) {
            if (bss.isCapturable(bmx, bmy) == true) {
                if (bss.amOwner(bmx, bmy, playerNum) == false) {
                    int itemNum = bss.getItemNumFromPos(bmx, bmy);
                    bss.setOwner(bmx, bmy, playerNum, false);
                    gw.getPNB().add(NetChangePNB.NPNB_BASE_CAPTURE, (byte) itemNum, (byte) playerNum, (byte) bmx, (byte) bmy);
                    gw.getFrontEndDraw().feDrawStatusBase(itemNum, bss.getAllianceNum(itemNum));
                    gw.reCalc();
                }
            }
        }
        
        /* Check for pill capture */
        if (gw.isServerOrSinglePlayerGame() == true) {
            checkPillCapture(pbs);
        }
        
        /* Check for near mines */
        nearMines(bmx, bmy);
        
    }
    
    
    /** Responsible for moving the tank when it is on a boat
     * @param mp - The map
     * @param pbs - The pillboxes on this map
     * @param bss - The bases on this map
     * @param shs - The shells on this map
     * @param bmx - Tank X map positino
     * @param bmy - Tank Y map positoin
     * @param tb - The TankButtons pressed
     * @param inBrain - Is this bbrain is being run by a brain
     */
    private void moveOnBoat(BoloMap mp, Pillboxes pbs, Bases bss, int bmx, int bmy, int tb, boolean inBrain) {
        int newx; /* New world co-ordinates */
        int newy;
        int newbmx;
        int newbmy;
        Point p;
        int ang;
        int boatExitSquare;
        
        checkGroundClear(mp, pbs, bss);
        if (gw.isServer() == false) {
            tankTurn(mp, pbs, bss, bmx, bmy, tb);
        }
        tankAccel(mp, pbs, bss, bmx, bmy, tb);
        /* Update location if speed > 0 */
        if (speed > 0) {
            /* If we have autoslowdown turned on - SLOW DOWN! */
            if (autoSlowdown == true && inBrain == false) {
                if (tb != TankButtons.TDECEL && tb != TankButtons.TLEFTDECEL && tb != TankButtons.TRIGHTDECEL && tb != TankButtons.TACCEL && tb != TankButtons.TLEFTACCEL && tb != TankButtons.TRIGHTACCEL) {
                    speed -= TANK_AUTOSLOW_SPEED;
                    if (speed < 0) {
                        speed = 0;
                    }
                }
            }
            ang = Utils.Get16Dir(angle);
            p = Utils.CalcDistance(ang, speed);
            newx = x + p.getX();
            newy = y + p.getY();
            if (p.getY() > 0) {
                newy += TANK_MOVE_BOAT_SUB;
            } else {
                newy -= TANK_MOVE_BOAT_SUB;
            }
            
            if (p.getX() >= 0) {
                newx += TANK_MOVE_BOAT_SUB;
            } else {
                newy -= TANK_MOVE_BOAT_SUB;
            }
            newbmx = Utils.GetMapFromWorld(newx);
            newbmy = Utils.GetMapFromWorld(newy);
            
            if (mp.isLand(bmx, newbmy) == false) {
                y += p.getY();
            } else if (mp.getSpeed(pbs, bss, bmx, newbmy, playerNum, onBoat) > 0 && speed >= BOAT_EXIT_SPEED) {
                y += p.getY();
            } else {
                newbmy = bmy;
            }
            if (mp.isLand(newbmx, newbmy) == false) {
                x += p.getX();
            } else if (mp.getSpeed(pbs, bss, newbmx, newbmy, playerNum, onBoat) > 0 && speed >= BOAT_EXIT_SPEED) {
                x += p.getX();
            } else {
                newbmx = bmx;
            }
            /* Reget MX & MY */
            newbmx = Utils.GetMapFromWorld(x);
            newbmy = Utils.GetMapFromWorld(y);
            if (newbmy < bmy) {
                // FIXME
                if (false) { // frontEndTutorial(newbmy) == TRUE
                    speed = 0;
                    
                }
            }
            
            if (gw.isServerOrSinglePlayerGame() == true) {
                checkPillCapture(pbs);
            }
            if (mp.isLand(newbmx, newbmy) == true) {
                boatExitSquare = mp.getPos(newbmx, newbmy);
                if (boatExitSquare == Terrain.BOAT) {
                    if (gw.isServer() == false) {
                        gw.getMNT().add(NetChangeMNT.NMNT_DROPBOAT, 0, gw.getPlayers().getSelf(), newbmx, newbmy);
                        gw.getMNT().add(NetChangeMNT.NMNT_RUNOVERBOAT, 0, gw.getPlayers().getSelf(), newbmx, newbmy);
                        mp.setPos(newbmx,newbmy,Terrain.RIVER, true, false);
                    }
                    
                    gw.getExplosions().add(newbmx, newbmy, 0, 0, Explosion.START);
                    gw.getSoundDist().soundDist(SoundDist.MAN_BUILDING_NEAR, newbmx, newbmy);
                    gw.reCalc();
                } else if (boatExitSquare != Terrain.BUILDING && boatExitSquare != Terrain.HALFBUILDING) {
                    if (mp.getPos(bmx, bmy) == Terrain.RIVER) {
                        if (gw.isServer() == false) {
                            mp.setPos(bmx, bmy, Terrain.BOAT, true, false);
                            gw.getMNT().add(NetChangeMNT.NMNT_BUILDBOAT, 0, playerNum, bmx, bmy);
                        }
                    }
                    onBoat = false;
                    gw.reCalc();
                }
                /* OK We have successfully left the boat */
                if (onBoat == false) {
                    /* Check for Mine hit */
                    if (mp.isMine(newbmx, newbmy) == true) {
                        gw.getMNT().add(NetChangeMNT.NMNT_MINEEXPLOSION, 0, playerNum, newbmx, newbmy);
                        gw.getMinesExp().add(newbmx, newbmy);
                    }
                }
            }
            /* Check for hit mine on outer map edges */
            if (mp.isMine(bmx, bmy) == true) {
                gw.getSoundDist().soundDist(SoundDist.MINE_EXPLOSION_NEAR, bmx, bmy);
                gw.getExplosions().add(bmx, bmy, 0, 0, Explosion.START);
                onBoat = false;
                speed = 0;
            }
        }
        nearMines(bmx, bmy);
    }
    
    /**
     * Checks for pillbox captures. If it does capture
     * one then it updates everything.
     *
     * @param pbs Pillboxes
     */
    private void checkPillCapture(Pillboxes pbs) {
        int bmx;
        int bmy;
        int pillNum;
        
        if (isDead() == false && gw.isServerOrSinglePlayerGame() == true) {
            bmx = getMX();
            bmy = getMY();
            if (bmx !=0 && bmy != 0 && pbs.isCapturable(bmx, bmy) == true) {
                pillNum = pbs.getPillNum(bmx, bmy, true, false);
                while (pillNum != -1) {
                    gw.getPNB().add(NetChangePNB.NPNB_PILL_PICKUP, (byte) pillNum, playerNum, 0, 0);
                    pbs.setInTank(pillNum, true);
                    gw.getPNB().add(NetChangePNB.NPNB_PILL_CAPTURE, (byte) pillNum, playerNum, 0, 0);
                    if (gw.isServer() == false) {
                        gw.getFrontEndDraw().feDrawStatusPillbox(pillNum,  pbs.getAllianceNum(pillNum));
                    }
                    carriedPills.add(new Integer(pillNum+1));
                    if (pbs.getOwner(pillNum) != playerNum) {
                        pbs.setOwner(pillNum, playerNum, false);
                    }
                    if (pbs.existPos(bmx, bmy) == true) {
                        pillNum = pbs.getPillNum(bmx, bmy, true, false);
                    } else {
                        pillNum = -1;
                    }
                }
                gw.reCalc();
            }
        }
    }
    
    /** Responsible for turning the tank
     * @param mp - The map
     * @param pbs - The pillboxes on this map
     * @param bss - The bases on this map
     * @param shs - The shells on this map
     * @param bmx - Tank X map positino
     * @param bmy - Tank Y map positoin
     * @param tb - The TankButtons pressed
     */
    private void tankTurn(BoloMap mp, Pillboxes pbs, Bases bss, int bmx, int bmy, int tb) {
        float turnAmount; /* Amount to turn */
        
        /* Left turn */
        if (tb == TankButtons.TLEFT || tb == TankButtons.TLEFTACCEL || tb == TankButtons.TLEFTDECEL) {
            turnAmount = mp.getTurnRate(pbs, bss, bmx, bmy, playerNum, onBoat);
            if (firstLeft < 10) {
                firstLeft++;
                turnAmount /= 2.0;
                
            }
            angle -= turnAmount;
            if (angle < 0) {
                angle = Utils.BRADIANS_MAX + angle;
            }
        } else {
            firstLeft = 0;
        }
        /* Right turn */
        if (tb == TankButtons.TRIGHT || tb == TankButtons.TRIGHTACCEL || tb == TankButtons.TRIGHTDECEL) {
            turnAmount = mp.getTurnRate(pbs, bss, bmx, bmy, playerNum, onBoat);
            if (firstRight < 10) {
                firstRight++;
                turnAmount /= 2.0;
                
            }
            angle += turnAmount;
            if (angle > Utils.BRADIANS_MAX) {
                angle = angle - Utils.BRADIANS_MAX;
            }
        } else {
            firstRight = 0;
        }
    }
    
    
    /** Responsible for moving the tank
     * @param mp - The map
     * @param pbs - The pillboxes on this map
     * @param bss - The bases on this map
     * @param shs - The shells on this map
     * @param bmx - Tank X map positino
     * @param bmy - Tank Y map positoin
     * @param tb - The TankButtons pressed
     */
    private void tankAccel(BoloMap mp, Pillboxes pbs, Bases bss, int bmx, int bmy, int tb) {
        int displace;
        float subAmount;
        
        displace = mp.getSpeed(pbs, bss, bmx, bmy, playerNum, onBoat);
        if ((tb == TankButtons.TDECEL || tb == TankButtons.TLEFTDECEL || tb == TankButtons.TRIGHTDECEL) || speed > displace)  {
            subAmount = speed;
            if (speed > displace) {
                subAmount = speed - TANK_TERRAIN_DECEL_RATE;
            }
            if (tb == TankButtons.TDECEL || tb == TankButtons.TLEFTDECEL || tb == TankButtons.TRIGHTDECEL) {
                subAmount -= (float) TANK_SLOWKEY_RATE;
            }
            if (subAmount > speed) {
                speed = 0;
            } else {
                speed = subAmount;
                if (speed < 0) {
                    speed = 0;
                }
            }
        } else if (speed < displace && (tb == TankButtons.TACCEL || tb == TankButtons.TLEFTACCEL || tb == TankButtons.TRIGHTACCEL))  {
            speed += TANK_ACCELERATE_RATE;
            if (speed > displace) {
                speed = displace;
            }
        }
    }
    
    /** Checks that the ground beneath the tank is clear (ie no
     * one has built under our tank) If it is not it moves the
     * tank.
     * @param mp - The map
     * @param pbs - The pillboxes on this map
     * @param bss - The bases on this map
     */
    private void checkGroundClear(BoloMap mp, Pillboxes pbs, Bases bss) {
        int modAmount = 1; /* Used for conversions */
        int bmx;     /* Tank map offsets */
        int  bmy;
        int  px;      /* Tank pixel offsets */
        int py;
        int terrain; /* The terrain we are on */
        boolean needFix; /* True if location is obstructed and we must move the tank */
        int leftPos;     /* Used in checking for tank collisions */
        int downPos;
        int owner;
        
        needFix = false;
        bmx = getMX();
        bmy = getMY();
        /* Pill check */
        if (pbs.existPos(bmx, bmy) == true) {
            /* Check to make sure its not dead */
            if (pbs.deadPos(bmx, bmy) == false) {
                needFix = true;
            }
        }
        
        /* Base check */
        if (bss.existPos(bmx, bmy) == true) {
            owner = bss.getOwnerFromPos(bmx, bmy);
            if (owner != GameItem.NEUTRAL && gw.getPlayers().isAlly(owner, playerNum) == false) {
                needFix = true;
                obstructed = true;
            } else {
                /* Dont check terrain underneath because we are on a base! */
                return;
            }
        }
        
        /* Terrain Check */
        if (needFix == false) {
            terrain = mp.getPos(bmx, bmy);
            if (terrain == Terrain.BUILDING || terrain == Terrain.HALFBUILDING) {
                obstructed = true;
                needFix = true;
            }
        }
        
        /* Fix if required */
        if (needFix == true) {
            px = getPX();
            py = getPY();
            if (px >= MIDDLE_PIXEL) {
                x++;
            } else {
                x--;
            }
            
            if (py >= MIDDLE_PIXEL) {
                py++;
            } else {
                py--;
            }
            speed--;
            if (speed < 0) {
                speed = 0;
            }
        }
        
        /* Check for tanks */
        if (needFix == false && gw.isServer() == false && false) { // FIXME playersCheckCollision(screenGetTankPlayer(value), (*value)->x, (*value)->y, &leftPos, &downPos) == TRUE) {
            obstructed = true;
            speed = 0;
            if (leftPos > 0) {
                x += 8;
            } else if (leftPos < 0) {
                x -= 8;
            } else {
                int rnd = (int) Math.random() % 6;
                if (rnd < 3 ) {
                    x = x + (int) (Math.random() % modAmount);
                } else {
                    x = x - (int) (Math.random() % modAmount);
                }
                modAmount++;
            }
            if (downPos > 0) {
                y = y - (int) (Math.random() % modAmount);
            } else if (downPos < 0) {
                y = y + (int) (Math.random() % modAmount);
            } else {
                int rnd = (int) Math.random() % 6;
                if (rnd < 3 ) {
                    y = y + (int) (Math.random() % modAmount);
                } else {
                    y = (int) (y - Math.random() % modAmount);
                }
                modAmount++;
            }
        } else {
            modAmount = 1;
        }
    }
    
    /* getFrame returns the draing frame of this tank
     *
     * @return drawing frame
     */
    /** Gets the drawing frame for this tank
     * @return The drawing TileNum
     */
    public int getFrame() {
        int returnValue =  TileNum.TANK_TRANSPARENT;
        if (armour > ARMOUR_FULL) {
            returnValue =  TileNum.TANK_TRANSPARENT;
        } else {
            returnValue = Utils.GetDir(angle);
            if (onBoat == true) {
                returnValue += TileNum.TANK_BOAT_ADD;
            }
        }
        
        return returnValue;
    }
    
    /**
     * The tank has died. Reinit its location and increment the death count
     *
     * @param sts Starts in the game world
     */
    public void death(Starts sts) {
        GameType gt = gw.getGameTypeObject();
        Start s;
        
        if (true || gw.isServer() == false) { // FIXME netGetType() == netSingle ||  {
            //FIXME LGM tank died            gw.getLGM().tankDied();
            //FIXME lgmTankDied(screenGetLgmFromPlayerNum(screenGetTankPlayer(value)));
            if (gw.isServer() == false) {
                if (false) { // FIXME netGetType() == netUdp
                    gw.setInStartFind(true);
                    //FIXME netRequestStartPosition()
                } else {
                    armour = gt.getArmour();
                    mines = gt.getMines();
                    shells = gt.getShells();
                    trees = gt.getTrees();
                    tankHitCount = 0;
                    gw.setInStartFind(true);
                    s = sts.getStart(playerNum);
                    x = Utils.GetWorldFromMap(s.getX()) + Utils.MAP_SQUARE_MIDDLE;
                    y = Utils.GetWorldFromMap(s.getY()) + Utils.MAP_SQUARE_MIDDLE;
                    angle = s.getStartDir();
                    gw.tankView();
                    gw.setInStartFind(false);
                    gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, armour, trees);
                }
                onBoat = true;
                numDeaths++;
                reload = 0;
                speed = 0;
                waterCount = 0;
                gw.getFrontEndDraw().feWriteKillsDeaths(numKills, numDeaths);
            } else if (gw.isServer() == true) { // FIXME from WinBolo
                armour = 10;
            }
        }
    }
    
    /**
     * Gets the number of kills this tank has had
     * @return Number of kills this tank has had
     */
    public int getKills() {
        return numKills;
    }
    
    /**
     * Gets the number of deaths this tank has had
     * @return Number of deaths this tank has had
     */
    public int getDeaths() {
        return numDeaths;
    }
    
    /**
     * Gets if the tank has just fired
     * @return If the tank has just fired
     */
    public boolean getJustFired() {
        return justFired;
    }
    
    /** tankIsMoving returns if the tank is moving
     *
     * @return boolean
     */
    public boolean tankIsMoving() {
        if (speed != 0) {
            return true;
        }
        return false;
    }
    
    public boolean isObstructed() {
        return obstructed;
    }
    
    
    public boolean isNewTank() {
        return newTank;
    }
    
    /**
     * Gets if this tank is on a boat or not
     * @return if this tank is on a boat or not
     */
    public boolean isOnBoat() {
        return onBoat;
    }
    
    /** getAngle returns the angle this tank is travelling at
     *
     * @return float
     */
    public float getAngle() {
        return angle;
    }
    
    /** getDir returns the angel the tank is travelling at converted to a
     * 0-15 value
     *
     * @return int between 0-15
     */
    public int getDir() {
        return Utils.GetDir(angle);
    }
    
    /* get256Dir returns the angle the tank is travelling at cvnverted to a
     * 0-255 value
     *
     * @return int between 0-255
     */
    /** Gets the tank direction as a bradian
     * @return a bradian indicating the tanks direction
     */
    public int get256Dir() {
        return (int) angle;
    }
    
    /** getTravelAngle is another method for getDir
     *
     * @return getDir() return
     * @see getDir
     */
    public int getTravelAngle() {
        return this.getDir();
    }
    
    /** getSpeed returns the travelling speed of the tank
     *
     * @return int
     */
    public int getSpeed() {
        return (int) speed;
    }
    
    /** getActualSpeed returns the travelling speed of the tank
     *
     * @return float
     */
    public float getActualSpeed() {
        return speed;
    }
    
    /** getArmour returns the amount of armour this tank currently has
     *
     * @return int
     */
    public int getArmour() {
        return armour;
    }
    
    /** getScreenMX returns the screen map x co-ordinate of the tank
     *
     * @return int
     */
    public int getScreenMX() {
        return Utils.GetMapFromWorld(x - TANK_SUBTRACT);
    }
    
    /** getScreenMY returns the screen map y co-ordinate of the tank
     *
     * @return int
     */
    public int getScreenMY() {
        return Utils.GetMapFromWorld(y - TANK_SUBTRACT);
    }
    
    /** getScreenPX returns the screen pixel x co-ordinate of the tank
     *
     * @return int
     */
    public int getScreenPX() {
        return Utils.GetPixelFromWorld(x - TANK_SUBTRACT);
    }
    
    /** getScreenOY returns the screen pixel y co-ordinate of the tank
     * @return int
     */
    public int getScreenPY() {
        return Utils.GetPixelFromWorld(y - TANK_SUBTRACT);
    }
    
    /** getShells returns the amount of shells this tank currently has
     * @return int
     */
    public int getShells() {
        return shells;
    }
    /** getMines returns the number of mines this tank currently has
     * @return int
     */
    public int getMines() {
        return mines;
    }
    
    /** getTrees returns the number of trees this tank currently has
     * @return int
     */
    public int getTrees() {
        return trees;
    }
    
    public void setShells(int amount) {
        if (amount > ARMOUR_FULL) {
            amount = ARMOUR_FULL;
        }
        shells = amount;
    }
    
    public void setMines(int amount) {
        if (amount > ARMOUR_FULL) {
            amount = ARMOUR_FULL;
        }
        mines = amount;
    }
    
    public void setArmour(int amount) {
        if (amount > ARMOUR_FULL) {
            amount = ARMOUR_FULL;
        }
        armour = amount;
    }
    
    public void setTrees(int amount) {
        if (amount > ARMOUR_FULL) {
            amount = ARMOUR_FULL;
        }
        trees = amount;
    }
    
    /** setStats sets the number shells, mines and trees this tank has
     *
     * @param shellsAmount Number of shells to set
     * @param minesAmount Number of mines to set
     * @param armourAmount Amount of armour to set
     * @param treesAmount Number of trees to set
     */
    public void setStats(int shellsAmount, int minesAmount, int armourAmount, int treesAmount) {
        shells = shellsAmount;
        mines = minesAmount;
        armour = armourAmount;
        trees = treesAmount;
    }
    
    /** isGunsightShowing returns if this tanks gunsight is showing
     *
     * @return boolean
     */
    public boolean isGunsightShowing() {
        return this.showSight;
    }
    
    public void setGunsightShowing(boolean value) {
        this.showSight = value;
        if (showSight == false) {
            sightLen = GUNSIGHT_MAX;
        }
    }
    
    
    public void setWorld(int x, int y, float angle, boolean setResources) {
        GameType gt;
        
        this.x = x;
        this.y = y;
        this.angle = angle;
        
        if (setResources == true) {
            gt = gw.getGameTypeObject();
            armour = gt.getArmour();
            shells = gt.getShells();
            mines = gt.getMines();
            trees = gt.getTrees();
            if (gw.isServer() == false) {
                gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, armour, trees);
            }
        }
        
    }
    
    /** getGunsight returns the current position of the gunsight as a GameWorldItem.
     * If the tank is dead it the position is at 0,0
     *
     * @return the position of the gunsight
     * @see GameWorldItem
     */
    public GameWorldItem getGunsight() {
        GameWorldItem returnValue = new GameWorldItem();
        Point p;
        
        if (isDead() == false) {
            int speed = sightLen;
            speed <<= 7;
            p = Utils.CalcDistance(angle, speed);
            returnValue.setX(x + p.getX() - TANK_SUBTRACT);
            returnValue.setY(y + p.getY() - TANK_SUBTRACT);
        }
        return returnValue;
    }
    
    /** getGunsight returns the current position of the gunsight as a GameWorldItem.
     * If the tank is dead it returns null
     * @return the position of the gunsight or null if not on screen
     * @see GameWorldItem
     * @param left Left offset of the screen
     * @param right Right offset of the screen
     * @param top Top offset of the screen
     * @param bottom Bottom offset of the screen
     */
    public ScreenItem getGunsight(int left, int right, int top, int bottom) {
        ScreenItem returnValue = null;
        GameWorldItem gwi;
        if (showSight == true && isDead() == false) {
            gwi = getGunsight();
            if (gwi.getMX() >= left && gwi.getMX() <= right && gwi.getMY() >= top && gwi.getMY() <= bottom) {
                returnValue = new ScreenItem(gwi.getMX() - left, gwi.getMY() - top, gwi.getPX() , gwi.getPY(), 0);
            }
        }
        
        return returnValue;
    }
    
    /**
     * We just killed a player. Add it here and update the  frontend repectively.
     */
    public void addKill() {
        numKills++;
        gw.getFrontEndDraw().feWriteKillsDeaths(numKills, numDeaths);
    }
    
    /**
     * Someone else has picked up a pill. We should check that
     * we aren't carrying it ourselves and if so drop it (The
     * server said so) because this can lead to desync problems.
     *
     * @param pillNum Pillbox to drop if we have it
     */
    public void stopCarryingPill(int pillNum) {
        Integer i = new Integer(pillNum);
        carriedPills.remove(i);
    }
    
    /**
     * Gets the first available carried pill. If none are
     * avaiable it returns LGM.LGM_NO_PILL
     *
     * @return a pillbox number that the pill is carrying or LGM.LGM_NO_PILL if no pills are on this tank
     * @param perform Whether to remove the pillbox from the tank or not
     */
    public int getCarriedPill(boolean perform) {
        int returnValue = LGM.LGM_NO_PILL;
        Integer val;
        if (carriedPills.size() > 0) {
            val = (Integer) carriedPills.get(0);
            returnValue = val.intValue();
            if (perform == true) {
                carriedPills.remove(0);
            }
        }
        return returnValue;
    }
    
    public void getCarriedPillNum(int pillNum) {
        carriedPills.remove(new Integer(pillNum));
    }
    
    
    /**
     * Puts a pillbox in the tank (Comes from LGM)
     *
     * @param pillNum The pillbox number to add
     */
    public void putCarriedPill(int pillNum) {
        Integer val = new Integer(pillNum);
        carriedPills.add(val);
    }
    
    /**
     * Returns the number of pillboxes this tank is carrying
     *
     * @return int
     */
    public int getNumCarriedPills() {
        return carriedPills.size();
    }
    
    /**
     * Puts a pillbox in a tank
     *
     * @param pillNum Pillbox to put in the tank
     */
    public void putPill(int pillNum) {
        Integer i = new Integer(pillNum+1);
        carriedPills.add(i);
        gw.getFrontEndDraw().feDrawStatusPillbox(pillNum, gw.getPills().getAllianceNum(pillNum-1));
    }
    
    /**
     * Adds the amount given in the amount to the tanks stocks and updates the display
     *
     * @param amount Amount of trees to add
     */
    public void giveTrees(int amount) {
        trees += amount;
        if (trees > ARMOUR_FULL) {
            trees = ARMOUR_FULL;
        }
        if (gw.isServer() == false) {
            if (armour <= ARMOUR_FULL) {
                gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, armour, trees);
            } else {
                gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, 0, trees);
            }
        }
    }
    
    /**
     * Adds the amount given in the amount to the tanks stocks and updates the display
     *
     * @param amount Amount of trees to add
     */
    public void giveMines(int amount) {
        addMines(amount);
    }
    
    /**
     * Returns whether the amount of mines request is availble
     * if it is then it subtracts the amount then updates the
     * display
     *
     * @return if there enough mines to perform the operation
     * @param amount Amount of mines to check
     * @param perform Whether to perform the deduction or just to check
     */
    public boolean getLgmMines(int amount, boolean perform) {
        if (mines - amount >= 0) {
            if (perform == true) {
                mines -= amount;
                if (gw.isServer() == false) {
                    if (isDead() == false) {
                        gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, armour, trees);
                    } else {
                        gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, 0, trees);
                    }
                }
            }
            return true;
        }
        return false;
        
    }
    
    /**
     * Sets this tanks location data
     */
    public void setLocationData(int x, int y, float angle, float speed, boolean onBoat) {
        this.x = x;
        this.y = y;
        this.angle = angle;
        this.speed = speed;
        this.onBoat = onBoat;
    }
    
    /**
     * Returns whether the amount of trees request is availble
     * if it is then it subtracts the amount then updates the
     * display
     *
     * @return if there enough trees to perform the operation
     * @param amount Amount of trees to check
     * @param perform Whether to perform the deduction or just to check
     */
    public boolean getLgmTrees(int amount, boolean perform) {
        if (trees - amount >= 0) {
            if (perform == true) {
                trees -= amount;
                if (gw.isServer() == false) {
                    if (isDead() == false) {
                        gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, armour, trees);
                    } else {
                        gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, 0, trees);
                    }
                }
            }
            return true;
        }
        return false;
    }
    
    /**
     * Returns if this tank is dead or not
     * @return if this tank is dead or not
     */
    public boolean isDead() {
        if (armour < 0 || armour > ARMOUR_FULL) {
            return true;
        }
        return false;
    }
    
    /**
     * Adds the amount of armour specified to the tank
     *
     * @param value Amount to add
     */
    public void addArmour(int value) {
        armour += value;
        if (armour > ARMOUR_FULL) {
            armour = ARMOUR_FULL;
        }
        if (gw.isServer() == false) {
            gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, armour, trees);
        }
    }
    
    /**
     * Adds the amount of shells specified to the tank
     *
     * @param value Amount to add
     */
    public void addShells(int value) {
        shells += value;
        if (shells > ARMOUR_FULL) {
            shells = ARMOUR_FULL;
        }
        if (gw.isServer() == false) {
            gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, armour, trees);
        }
    }
    
    /**
     * Adds the amount of mines specified to the tank
     *
     * @param value Amount to add
     */
    public void addMines(int value) {
        mines += value;
        if (mines > ARMOUR_FULL) {
            mines = ARMOUR_FULL;
        }
        if (gw.isServer() == false) {
            gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, armour, trees);
        }
    }
    
    /**
     * Tank has been requested to lay a mine
     */
    public void layMine() {
        int terrain;
        int x = getMX();
        int y = getMY();
        Mines ms = gw.getMines();
        BoloMap mp = gw.getBoloMap();
        
        if (isDead() == false && mp.isMine(x, y) == false) {
            terrain = mp.getPos(x, y);
            if (isDead() == false && onBoat == false && mines > 0 && terrain != Terrain.BUILDING && terrain != Terrain.HALFBUILDING && terrain != Terrain.BOAT && terrain != Terrain.RIVER && terrain < Terrain.MINE_START && gw.getPills().existPos(x, y) == false && gw.getBases().existPos(x, y) == false) {
                mines--;
                mp.setPos(x, y, terrain + Terrain.MINE_SUBTRACT, false, false);
                gw.getMNT().add(NetChangeMNT.NMNT_MINETANKPLACE, 0, playerNum, x, y);
                gw.getSoundDist().soundDist(SoundDist.MAN_LAYING_MINE_NEAR, x, y);
                if (isDead() == false) {
                    gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, armour, trees);
                } else {
                    gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, 0, trees);
                }
                gw.reCalc();
            }
        }
    }
    
    /**
     * Check to see if the tank is near any hidden mines
     * @param mx X Map position
     * @param my Y Map position
     */
    private void nearMines(int mx, int my) {
        boolean needRecalc = false; // Does the screen need to recalc
        BoloMap map = gw.getBoloMap(); // Bolo Map
        Mines mines = gw.getMines(); // Mines we can see
        
        if (map.isMine(mx, my) == true) {
            if ((mines.add(mx, my)) == false) {
                needRecalc = true;
            }
        }
        if (map.isMine((mx-1), (my-1)) == true) {
            if ((mines.add((mx-1), (my-1))) == false) {
                needRecalc = true;
            }
        }
        if (map.isMine((mx-1), my) == true) {
            if ((mines.add((mx-1), my)) == false) {
                needRecalc = true;
            }
        }
        
        if (map.isMine((mx-1), (my+1)) == true) {
            if ((mines.add((mx-1), (my+1))) == false) {
                needRecalc = true;
            }
        }
        if (map.isMine(mx, (my-1)) == true) {
            if ((mines.add(mx, (my-1))) == false) {
                needRecalc = true;
            }
        }
        if (map.isMine(mx, (my+1)) == true) {
            if ((mines.add(mx, (my+1))) == false) {
                needRecalc = true;
            }
        }
        if (map.isMine((mx+1), (my-1)) == true) {
            if ((mines.add((mx+1), (my-1))) == false) {
                needRecalc = true;
            }
        }
        if (map.isMine((mx+1), my) == true) {
            if (mines.add((mx+1), my) == false) {
                needRecalc = true;
            }
        }
        if (map.isMine((mx+1), (my+1)) == true) {
            if (mines.add((mx+1), (my+1)) == false) {
                needRecalc = true;
            }
        }
        
        if (needRecalc == true) {
            gw.reCalc();
        }
    }
    
    /**
     * The tank is in water. Reduce shells and mines count
     */
    private void inWater() {
        boolean modsMade = false; // Have any modifications been made
        
        if (shells > 0) {
            shells--;
            modsMade = true;
        }
        if (mines > 0) {
            mines--;
            modsMade = true;
        }
        
        if (modsMade == true) {
            /* Update view and play sound */
            gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, armour, trees);
            gw.getFrontEndSound().fePlaySound(SoundDist.BUBBLES);
        }
    }
    
    /**
     * A mine has gone off at map position x,y Check to see if the tank has been damaged
     * @param mx Map X position of the mine explosion
     * @param my Map Y position of the mine explosion
     */
    public void checkMineDamage(int mx, int my) {
        int mineX;
        int mineY;
        int diffX;
        int diffY;
        
        mineX = Utils.GetWorldFromMap(mx) + Utils.MAP_SQUARE_MIDDLE;
        mineY = Utils.GetWorldFromMap(my) + Utils.MAP_SQUARE_MIDDLE;
        
        if (mineX > x) {
            diffX = mineX - x;
        } else {
            diffX = x - mineX;
        }
        
        if (mineY > y) {
            diffY = mineY - y;
        } else {
            diffY = y - mineY;
        }
        
        if (diffX < 384 && diffY < 384 && isDead() == false) {
            armour -= MINE_DAMAGE;
            gw.getMNT().add(NetChangeMNT.NMNT_TANKHIT, playerNum, playerNum, 2, armour);
            if (isDead() == true) {
                if (shells + mines > BIG_EXPLOSION_THRESHOLD) {
                    gw.getTankExplosion().add(x, y, angle, (int) speed, TankExplosion.BIG_EXPLOSION);
                } else {
                    gw.getTankExplosion().add(x, y, angle, (int) speed, TankExplosion.SMALL_EXPLOSION);
                }
                deathWait = 255;
                dropPills(gw.getBoloMap(), gw.getPills(), gw.getBases());
            }
            if (onBoat == true) {
                onBoat = false;
                speed = 0;
            }
            if (isDead() == true) {
                gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, 0, trees);
            } else {
                gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, armour, trees);
            }
            gw.reCalc();
            
        }
    }
    
    /**
     * Drops all the pillboxes at the tanks current location.  Called when tank dies
     * @param map The BoloMap to drop onto
     * @param pbs The pillboxes in this game world
     * @param bss The bases in this game world
     */
    private void dropPills(BoloMap map, Pillboxes pbs, Bases bss) {
        Pillbox p;
        int numPills;
        Integer i;
        ListIterator li;
        int pillNum = 0;
        int bmx;
        int bmy;
        int width;
        int count;
        int setX;
        int setY;
        int pos;
        boolean wantNext = true;
        
        numPills = carriedPills.size();
        
        if (numPills > 0 && gw.isServerOrSinglePlayerGame() == true) {
            count = 0;
            bmx = getMX();
            bmy = getMY();
            /* Width for depositing them */
            width = (int) Math.sqrt((double) numPills);
            bmx -= width / 2;
            bmy -= width / 2;
            
            if (bmx >= 0 && bmx <= BoloMap.MAP_MINE_EDGE_LEFT) {
                bmx = BoloMap.MAP_MINE_EDGE_LEFT+1;
            } else if (bmx >= BoloMap.MAP_MINE_EDGE_RIGHT) {
                bmx = BoloMap.MAP_MINE_EDGE_RIGHT - 10;
            }
            
            if (bmy >= 0 && bmy <= BoloMap.MAP_MINE_EDGE_TOP) {
                bmy = BoloMap.MAP_MINE_EDGE_TOP+1;
            } else if (bmy >= BoloMap.MAP_MINE_EDGE_BOTTOM) {
                bmy = BoloMap.MAP_MINE_EDGE_BOTTOM - 10;
            }
            
            wantNext = true;
            li = carriedPills.listIterator();
            while (li.hasNext()) {
                if (wantNext == true) {
                    i = (Integer) li.next();
                    pillNum = i.intValue()-1;
                    wantNext = false;
                }
                p = (Pillbox) pbs.getItem(pillNum);
                if (gw.isServerOrSinglePlayerGame() == true) {
                    setX = bmx;
                    setY = bmy+count;
                    if (setX > BoloMap.MAP_MINE_EDGE_LEFT && setX < BoloMap.MAP_MINE_EDGE_RIGHT && setY > BoloMap.MAP_MINE_EDGE_TOP && setY < BoloMap.MAP_MINE_EDGE_BOTTOM) {
                        pos = gw.getBoloMap().getPos(setX, setY);
                        if (pbs.existPos(setX, setY) == false && gw.getBases().existPos(setX, setY) == false && pos != Terrain.BUILDING && pos != Terrain.HALFBUILDING && pos != Terrain.BOAT) {
                            if (gw.isServerOrSinglePlayerGame() == true) {
                                p.setX(setX);
                                p.setY(setY);
                                p.setArmour((byte) 0);
                                p.setInTank(false);
                                p.setOwner(playerNum);
                                p.setJustSeen(false);
                                
                                gw.getPNB().add(NetChangePNB.NPNB_PILL_DEAD, pillNum, playerNum, setX, setY);
                            }
                            if (gw.isServer() == false) {
                                gw.getFrontEndDraw().feDrawStatusPillbox(pillNum,  p.getAlliance(gw));
                            }
                            wantNext = true;
                        }
                    }
                    count++;
                    if (count == width) {
                        count = 0;
                        bmx++;
                    }
                    
                    
                }
            }
        }
        carriedPills.clear();
        gw.reCalc();
    }
    
    /**
     * Returns whether the tank has been hit or not, if it it is killed etc.
     *  Also updates its location if hit but not dead.
     *
     * @param x X co-ord of the shell
     * @param y Y co-ord of the shell
     * @param angle The direction the shell came from
     * @param owner The shell owner
     * @return the tank hit type
     */
    public int isTankHit(int shellX, int shellY, float angle, int owner) {
        int returnValue = TH_MISSED;
        Point p;
        int bmx;
        int bmy;
        int newmx;
        int newmy;
        int newbmx;
        int newbmy;
        boolean needSend = false;
        BoloMap mp = gw.getBoloMap();
        Pillboxes pbs = gw.getPills();
        Bases bss = gw.getBases();
        
        if (gw.isServer() == false) {
            if (owner == playerNum) {
                return TH_MISSED;
            }
        }
        
        if (Math.abs(x - shellX) < 128 && Math.abs(y - shellY) < 128  && isDead() == false) {
            returnValue = TH_HIT;
            needSend = true;
            if (gw.isServer() == false) { /* Fix Me */
                armour -= DAMAGE;
            }
            if (onBoat == true) {
                onBoat = false;
                speed = 0;
                gw.reCalc();
            }
            if (isDead() == true) {
                if (shells + mines > BIG_EXPLOSION_THRESHOLD) {
                    returnValue = TH_KILL_BIG;
                } else {
                    returnValue = TH_KILL_SMALL;
                }
                deathWait = 255;
                dropPills(mp, pbs, bss);
            } else { /* if ((*value)->armour <= TANK_FULL_ARMOUR)  */
                p = Utils.CalcDistance(angle, BoloMap.MAP_SQUARE_MIDDLE);
                /* Check for Colisions */
                bmx = getMX();
                bmy = getMY();
                
                newmx = x + p.getX();
                newmy = y + p.getY();
                
                newbmx = Utils.GetMapFromWorld(newmx);
                newbmy = Utils.GetMapFromWorld(newmy);
                
                if (mp.getSpeed(pbs, bss, bmx, newbmy, playerNum, onBoat) > 0) {
                    y = y + p.getY();
                    bmy = newbmy;
                }
                if (mp.getSpeed(pbs, bss, newbmx, bmy, playerNum, onBoat) > 0) {
                    x = x + p.getX();
                    bmx = newbmx;
                }
                /* Check for scroll of screen */
                //FIXME scrolling                screenTankScroll();
            }
            if (isDead() == false) {
                if (gw.isServer() == false) {
                    gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, armour, trees);
                    if (needSend == true) {
                        /* Send hit to network */
                        gw.getMNT().add(NetChangeMNT.NMNT_TANKHIT, playerNum, playerNum, 1 , armour / 5);
                    }
                }
            } else {
                if (gw.isServer() == false) {
                    gw.getFrontEndDraw().feDrawStatusTankBars(shells, mines, 0, trees);
                }
            }
        }
        
        
        return returnValue;
    }
    
    public void resetHitCount() {
        tankHitCount = 0;
    }
    
    public void addHitCount(int amount) {
            if (tankHitCount == 0 && amount < 0) {
                return;
            }
            
            if (amount > -2 || amount < 3) {
                tankHitCount += amount;
                if ((tankHitCount > 10 && amount > 1) || (tankHitCount > 9 && amount == 1)) {
                    /* Cheater */
//                    printf("Cheat received form of armour!...\n");             
//FIXME                    netPlayersSetCheater((netPlayers *) serverNetGetNetPlayers(), (BYTE) serverCoreGetTankPlayer(value));
                }
            } else {
                /* Cheater? */
                /* Do nothing for now */
                
            }
            
        }
        
        public void setOnBoat(boolean value) {
            this.onBoat = value;
            
        }
    }
