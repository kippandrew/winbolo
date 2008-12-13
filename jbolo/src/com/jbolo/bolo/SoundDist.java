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
 * SoundDist.java
 *
 * Created on November 24, 2003, 5:16 PM
 */

package com.jbolo.bolo;

/**
 *  Responsible for differentiating between playing soft
 *  sound effects and loud sound effects.
 *
 * @author  John Morrison
 */
public class SoundDist {
    
    public static final int SHOOT_SELF = 0;
    public static final int SHOOT_NEAR = 1;
    public static final int SHOT_TREE_NEAR = 2;
    public static final int SHOT_TREE_FAR = 3;
    public static final int SHOT_BUILDING_NEAR = 4;
    public static final int SHOT_BUILDING_FAR = 5;
    public static final int HIT_TANK_NEAR = 6;
    public static final int HIT_TANK_FAR = 7;
    public static final int HIT_TANK_SELF = 8;
    public static final int BUBBLES = 9;
    public static final int TANK_SINK_NEAR = 10;
    public static final int TANK_SINK_FAR = 11;
    public static final int BIG_EXPLOSION_NEAR = 12;
    public static final int BIG_EXPLOSION_FAR = 13;
    public static final int FARMING_TREE_NEAR = 14;
    public static final int FARMING_TREE_FAR = 15;
    public static final int MAN_BUILDING_NEAR = 16;
    public static final int MAN_BUILDING_FAR = 17;
    public static final int MAN_DYING_NEAR = 18;
    public static final int MAN_DYING_FAR = 19;
    public static final int MAN_LAYING_MINE_NEAR = 20;
    public static final int MINE_EXPLOSION_NEAR = 21;
    public static final int MINE_EXPLOSION_FAR = 22;
    public static final int SHOOT_FAR = 23;
    
    /** If the distance from the tank to the event is greater then  15 map squares play the soft sound */
    private static final int SDIST_SOFT  = 15;
    
    /** If the distance from the tank to the event is greater then 40 map squares then play no sound */
    private static final int SDIST_NONE = 40;
    
    /** GameWorld that created this SoundDist */
    private GameWorld gw;
    
    /** Creates a new instance of SoundDist */
    public SoundDist(GameWorld gw) {
        this.gw = gw;
    }
    
    /**
     * Plays a sound
     *
     * @param value Sound to play
     */
    public void playSound(int value) {
        gw.getFrontEndSound().fePlaySound(value);
    }
    
    /** Calculates whether a soft sound of a loud sound should
     *  be played and passes paremeters to frontend
     *
     * @param value Sound effect to be played
     * @param mx Map X co-ordinatate for the sound origin
     * @param my Map Y co-ordinatate for the sound origin
     */
    public void soundDist(int value, int mx, int my) {
        int tankX; /* Tank X Map Co-ordinate */
        int tankY; /* Tank Y Map Co-ordinate */
        int gapX;  /* Distance from tank to sound */
        int  gapY;
        Tank tank;
        
        if (gw.getLog().isRecording() == true) {
            log(value, mx, my);
        }
        if (SoundDist.MINE_EXPLOSION_NEAR == value) {
            gapY = 0;
        }
        
        tank = gw.getTank();
        tankX = tank.getMX();
        tankY = tank.getMY();
        /* Get gap */
        gapX = Utils.GetGap(tankX, mx);
        gapY = Utils.GetGap(tankY, my);
        
        if (gapY < SDIST_NONE && gapX < SDIST_NONE) {
            /* Determine whether loud/soft sound should be played */
            switch (value) {
                case SoundDist.SHOOT_NEAR:
                    if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
                        gw.getFrontEndSound().fePlaySound(SHOOT_FAR);
                    } else {
                        gw.getFrontEndSound().fePlaySound(SHOOT_NEAR);
                    }
                    break;
                case SoundDist.SHOT_TREE_NEAR:
                    if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
                        gw.getFrontEndSound().fePlaySound(SHOT_TREE_FAR);
                    } else {
                        gw.getFrontEndSound().fePlaySound(SHOT_TREE_NEAR);
                    }
                    break;
                    
                case SoundDist.SHOT_BUILDING_NEAR:
                    if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
                        gw.getFrontEndSound().fePlaySound(SHOT_BUILDING_FAR);
                    } else {
                        gw.getFrontEndSound().fePlaySound(SHOT_BUILDING_NEAR);
                    }
                    break;
                case SoundDist.HIT_TANK_NEAR:
                    if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
                        gw.getFrontEndSound().fePlaySound(HIT_TANK_FAR);
                    } else {
                        gw.getFrontEndSound().fePlaySound(HIT_TANK_NEAR);
                    }
                    break;
                case SoundDist.TANK_SINK_NEAR:
                    if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
                        gw.getFrontEndSound().fePlaySound(TANK_SINK_FAR);
                    } else {
                        gw.getFrontEndSound().fePlaySound(TANK_SINK_NEAR);
                    }
                    break;
                case SoundDist.BIG_EXPLOSION_NEAR:
                    if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
                        gw.getFrontEndSound().fePlaySound(BIG_EXPLOSION_FAR);
                    } else {
                        gw.getFrontEndSound().fePlaySound(BIG_EXPLOSION_NEAR);
                    }
                    break;
                case SoundDist.FARMING_TREE_NEAR:
                    if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
                        gw.getFrontEndSound().fePlaySound(FARMING_TREE_FAR);
                    } else {
                        gw.getFrontEndSound().fePlaySound(FARMING_TREE_NEAR);
                    }
                    break;
                case SoundDist.MAN_BUILDING_NEAR:
                    if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
                        gw.getFrontEndSound().fePlaySound(MAN_BUILDING_FAR);
                    } else {
                        gw.getFrontEndSound().fePlaySound(MAN_BUILDING_NEAR);
                    }
                    break;
                case SoundDist.MAN_DYING_NEAR:
                    if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
                        gw.getFrontEndSound().fePlaySound(MAN_DYING_FAR);
                    } else {
                        gw.getFrontEndSound().fePlaySound(MAN_DYING_FAR);
                    }
                    break;
                case SoundDist.MINE_EXPLOSION_NEAR:
                    if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
                        gw.getFrontEndSound().fePlaySound(MINE_EXPLOSION_FAR);
                    } else {
                        gw.getFrontEndSound().fePlaySound(MINE_EXPLOSION_NEAR);
                    }
                    break;
                case MAN_LAYING_MINE_NEAR:
                    if (gapX <= SDIST_SOFT || gapY <= SDIST_SOFT) {
                        gw.getFrontEndSound().fePlaySound(MAN_LAYING_MINE_NEAR);
                    }
                    break;
                default:
                    break;
            }
        }
    }
    
    /**
     * Calculates the sound item to be logged
     * @param Sound effect to be played
     * @param mx Map X co-ordinatate for the sound origin
     * @param my Map Y co-ordinatate for the sound origin
     */
    private void log(int value, int mx, int my) {
        byte logMessageType = Log.log_SoundShoot; // Log type to log
        boolean wantLog = true;
        
        switch (value) {
            case SHOOT_SELF:
            case SHOOT_NEAR:
            case SHOOT_FAR:
                logMessageType = Log.log_SoundShoot;
                break;
                
            case SHOT_TREE_NEAR:
            case SHOT_TREE_FAR:
                logMessageType = Log.log_SoundFarm;
                break;
            case SHOT_BUILDING_NEAR:
            case SHOT_BUILDING_FAR:
//FIXME DOn't think this is right in the C code                logMessageType = Log.log_SoundHitWall;
                break;
            case HIT_TANK_NEAR:
            case HIT_TANK_FAR:
            case HIT_TANK_SELF:
            case BUBBLES:
            case TANK_SINK_NEAR:
            case TANK_SINK_FAR:
                wantLog = false;
                break;
            case BIG_EXPLOSION_NEAR:
                logMessageType = Log.log_SoundExplosion;
                break;
            case BIG_EXPLOSION_FAR:
                logMessageType = Log.log_SoundExplosion;
                break;
            case FARMING_TREE_NEAR:
            case FARMING_TREE_FAR:
                logMessageType = Log.log_SoundFarm;
                break;
            case MAN_BUILDING_NEAR:
            case MAN_BUILDING_FAR:
                logMessageType = Log.log_SoundBuild;
                break;
            case MAN_DYING_NEAR:
            case MAN_DYING_FAR:
                logMessageType = Log.log_SoundManDie;
                break;
                
            case MAN_LAYING_MINE_NEAR:
                logMessageType = Log.log_SoundMineLay;
                break;
                
            case MINE_EXPLOSION_NEAR:
            case MINE_EXPLOSION_FAR:
                logMessageType = Log.log_SoundMineExplode;
                break;
        }
        if (wantLog == true) {
            gw.getLog().addEvent(logMessageType, (byte) mx, (byte) my, (byte) 0, (byte) 0, (byte) 0, null);
        }
    }    
}
