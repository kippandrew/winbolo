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
 * Shells.java
 *
 * Created on October 9, 2003, 7:15 AM
 */

package com.jbolo.bolo;

import java.util.ArrayList;
import java.util.ListIterator;
import java.io.ByteArrayOutputStream;
import com.jbolo.bolo.map.BoloMap;

/** The shells object is used to hold the shells in the game world
 *
 * @author  John Morrison
 */
public class Shells {
    /** The start of when a shell explodes */
    private static int START_EXPLODE = 8;
    
    /** Position to add to offset the tanks location */
    private static int START_ADD = 5;
    
    /** ArrayList of Shell objects */
    private ArrayList<Shell> shs;
    
    /** GameWorld that created this shells */
    private GameWorld gw;
    
    /** Creates a new instance of Shells
     * @param gw The game world that created this Shells
     */
    public Shells(GameWorld gw) {
        shs = new ArrayList<Shell>(128);
        this.gw = gw;
    }
    
    /** When a new shell is fired it is added to the shells here
     * @param x X position of the shell in world unites
     * @param y Y position of the shell in world unites
     * @param angle Angle the shell is travelling in
     * @param len How far the shell is to travel in map squares
     * @param owner Owner of the shell
     * @param onBoat Did this shell originate from a boat
     */
    public void add(int x, int y, float angle, int len, int owner, boolean onBoat) {
        Point p;
        int self = gw.getPlayers().getSelf();
        int length;
        Shell shell;
        
        p = Utils.CalcDistance(angle, Shell.SPEED);
        x  = x + (START_ADD * p.getX());
        y  = y + (START_ADD * p.getY());
        
        
        length =  (1 + (Shell.LIFE * len) - (START_ADD));
        shell = new Shell(x, y, angle, owner, length, onBoat, self, false, false);
        shs.add(shell);
    }
    
    /**
     * Creates the list of shells that can be seen on the screen
     * @return ArrayList of ScreenItems
     * @param leftPos Left position of the screen
     * @param rightPos Right position of the screen
     * @param topPos Top position of the screen
     * @param bottomtPos Bottom position of the screen
     */
    public ArrayList<ScreenItem> getScreenItems(int leftPos, int rightPos, int topPos, int bottomPos) {
        ArrayList<ScreenItem> returnValue = new ArrayList<ScreenItem>(64); // Value to return
        ScreenItem si;
        int frame;

        
        for(Shell s : shs) { 
            if (s.getMX() >= leftPos && s.getMX() <= rightPos && s.getMY() >= topPos && s.getMY() <= bottomPos) {
                frame = Utils.GetDir(s.getAngle());
                frame += START_EXPLODE+1;
                si = new ScreenItem(s.getMX() - leftPos, s.getMY() - topPos, s.getPX(), s.getPY(), frame);
                returnValue.add(si);
            }
        }
        return returnValue;
    }
    
    /** Called once a game tick to update the positions of the shells
     * @param mp The worlds map
     * @param pbs The Pillboxes on the map
     * @param bss The Bases on the map
     * @param tanks ArrayList of tanks
     * @param isServer true if this is a server
     */
    public void update(BoloMap mp, Pillboxes pbs, Bases bss, ArrayList<Tank> tanks, boolean isServer) {
        int newX;
        int newY;
        Point update;
        int testPos;
        int count;
        Shell s;
        ListIterator<Shell> li = shs.listIterator();
        GameWorldItem hit;
        
        
        while (li.hasNext()) {
            s = li.next();
            if (s.getShellDead() == true && (s.getPackSent() == true || true )) { // FIXME netGetType() == netSingle
                li.remove();
            } else if (s.getShellDead() == true && s.getPackSent() == false) {
                // Do nothing. Keep the shell alive till we have atleast sent the packet
            } else if (s.getLength() > Shell.DEATH) {
                update = Utils.CalcDistance(s.getAngle(), s.SPEED);
                newX = s.getX() + update.getX();
                newY = s.getY() + update.getY();
                hit = calcCollision(s, tanks, gw.isServer());
                if (hit != null) {
                    /* Hit something. Explode */
                    gw.getExplosions().add(hit.getScreenMX(), hit.getScreenMY(), hit.getScreenPX(), hit.getScreenPY(), Explosion.START);
                    gw.getMinesExp().add(hit.getMX(), hit.getMY());
                    
                    
                    //FIXME
                    gw.getLGM().deathCheck(hit.getX(), hit.getY(), s.getOwner());
                    /* FIXME while (count < numTanks) {^M
                        lgmDeathCheck(screenGetLgmFromPlayerNum(screenGetTankPlayer(&tk[count])), mp, pb, bs, newX, newY, position->owner);^M
                         count++;
                    } */
                    if (s.getPackSent() == true) {
                        li.remove();
                    } else {
                        s.setShellDead(true);
                    }
                } else {
                    s.decrementLength();
                    s.setX(newX);
                    s.setY(newY);
                }
            } else {
                gw.getExplosions().add(s.getScreenMX(), s.getScreenMY(), s.getScreenPX(), s.getScreenPY(), Explosion.START);
                gw.getMinesExp().add(s.getMX(), s.getMY());
                if (gw.isServer() == true || Terrain.IsMinedTerrain(gw.getBoloMap().getPos(s.getMX(), s.getMY())) == true) {
                    gw.getMNT().add(NetChangeMNT.NMNT_MINEEXPLOSION, 0, /* FIXME screenGetTankPlayer(&tk) */ 0, s.getMX(), s.getMY()); // FIXME
                }
                /* FIXME while (count < numTanks) {^M
                    lgmDeathCheck(screenGetLgmFromPlayerNum(screenGetTankPlayer(&tk[count])), mp, pb, bs, newX, newY, position->owner);^M
                     count++;
                } */
                if (s.getPackSent() == true) {
                    li.remove();
                } else if (s.getShellDead() == false) {
                    s.setShellDead(true);
                }
            }
        }
    }
    
    /**
     * Checks to see if a collision has occured.
     * @return The posision of the collision as a game world item or null if no collision
     * @param s The shell we are working on
     * @param tanks List of tanks
     * @param isServer Are we are server
     */
    private GameWorldItem calcCollision(Shell s, ArrayList<Tank> tanks, boolean isServer) {
        GameWorldItem returnValue = null;
        boolean isMine = false;
        boolean basesExist = false;
        Pillboxes pbs;
        int pillNum;
        int mapX;
        int mapY;
        int count;
        BoloMap map;
        ListIterator<Tank> li;
        Tank tank;
        
        pbs = gw.getPills();
        map = gw.getBoloMap();
        mapX = s.getMX();
        mapY = s.getMY();
        if (pbs.isHit(mapX, mapY) == true) {
            returnValue = new GameWorldItem();
            returnValue.setX(Utils.GetWorldFromMap(mapX) + Utils.MAP_SQUARE_MIDDLE);
            returnValue.setY(Utils.GetWorldFromMap(mapY) + Utils.MAP_SQUARE_MIDDLE);
            if (gw.isServerOrSinglePlayerGame() == true	) { //FIXME or singlem player game
                if (pbs.damagePos(mapX, mapY, true, true) == true) {
                    pillNum = pbs.getPillNum(mapX, mapY, false, false);
                    gw.getPNB().add(NetChangePNB.NPNB_PILL_HIT, pillNum, pbs.getOwner(pillNum), s.getMX(), s.getMY());
                } else {
                    gw.getPNB().add(NetChangePNB.NPNB_PILL_HIT, 0, s.getOwner(), mapX, mapY);
                }
            } else if (s.getOwner() == 0) { // FIXME screenGetTankPlayer(&tk)
                pbs.damagePos(mapX, mapY, false, true);
            }
            gw.getSoundDist().soundDist(SoundDist.SHOT_BUILDING_NEAR, mapX, mapY);
        }
        
        
        if (returnValue == null) {
            li = tanks.listIterator();
            int th;
            while (li.hasNext()) {
                tank = li.next();
                if (tank.getPlayerNum() != s.getOwner()) {
                    th = tank.isTankHit(s.getX(), s.getY(), s.getAngle(), s.getOwner());
                    switch (th) {
                        case Tank.TH_HIT:
                            returnValue = new GameWorldItem(s.getX(), s.getY());
                            if (isServer == false) {
                                gw.getSoundDist().playSound(SoundDist.HIT_TANK_SELF);
                            }
                            if (isServer == false) {
                                if (map.getPos(tank.getMX(), tank.getMY()) == Terrain.DEEP_SEA) {
                                    /* We have drowned - They have killed us */
                                    //              if (owner != NEUTRAL) {
                                    gw.getMNT().add(NetChangeMNT.NMNT_KILLME, gw.getPlayers().getSelf(), tank.getPlayerNum(), s.getOwner(), 0xFF);
                                    //            } else {
                                    /* Was player get self for first owner parameter all the time */
                                    //            netMNTAdd(NMNT_KILLME, owner, screenGetTankPlayer(&(tk[count])), 0xFF, 0xFF);
                                    //              }
                                    
                                }
                            }
                            break;
                        case Tank.TH_KILL_SMALL:
                            returnValue = new GameWorldItem(s.getX(), s.getY());;
                            if (isServer == false) {
                                gw.getTankExplosion().add(s.getX(), s.getY(), s.getAngle(), TankExplosion.TK_EXPLODE_LENGTH, TankExplosion.SMALL_EXPLOSION);
                                //            if (owner != NEUTRAL) {
                                gw.getMNT().add(NetChangeMNT.NMNT_KILLME, gw.getPlayers().getSelf(), tank.getPlayerNum(), s.getOwner(), 0xFF);
                                //              } else {
                                //               netMNTAdd(NMNT_KILLME, owner, screenGetTankPlayer(&(tk[count])), 0xFF, 0xFF);
                                //              }
                                
                            }
                            if (isServer == false) {
                                gw.getSoundDist().playSound(SoundDist.HIT_TANK_SELF);
                            }
                            break;
                        case Tank.TH_KILL_BIG:
                            returnValue = new GameWorldItem(s.getX(), s.getY());;
                            if (isServer == false) {
                                gw.getTankExplosion().add(s.getX(), s.getY(), s.getAngle(), TankExplosion.TK_EXPLODE_LENGTH, TankExplosion.BIG_EXPLOSION);
                                //              if (owner != NEUTRAL) {
                                //              netMNTAdd(NMNT_KILLME, owner, screenGetTankPlayer(&(tk[count])), playersGetSelf(), 0);
                                //          } else {
                                gw.getMNT().add(NetChangeMNT.NMNT_KILLME, gw.getPlayers().getSelf(), tank.getPlayerNum(), s.getOwner(), 0xFF);
                                //         }
                                
                            }
                            if (isServer == false) {
                                gw.getSoundDist().playSound(SoundDist.HIT_TANK_SELF);
                            }
                            break;
                        case Tank.TH_MISSED:
                        default:
                            break;
                    }
                }
            }
        }
        
        /* Check for player hit */
        if (returnValue == null && isServer == false) {
            int playerHit = GameItem.NEUTRAL; //FIXME playersIsTankHit(*xValue, *yValue, angle, owner);
            if (playerHit != GameItem.NEUTRAL) {
                returnValue = new GameWorldItem(s.getX(), s.getY());
                gw.getSoundDist().soundDist(SoundDist.HIT_TANK_NEAR, mapX, mapY);
            }
        }
        
        
        if (returnValue == null) {
            Bases bss;
            bss = gw.getBases();
            basesExist = bss.existPos(mapX, mapY);
            if (basesExist == true) {
                if (s.getOnBoat() == true) {
                    returnValue = new GameWorldItem();
                    returnValue.setX(Utils.GetWorldFromMap(mapX) + Utils.MAP_SQUARE_MIDDLE);
                    returnValue.setY(Utils.GetWorldFromMap(mapY) + Utils.MAP_SQUARE_MIDDLE);
                    if (bss.canHit(mapX, mapY, s.getOwner()) == true) {
                        if (gw.isServerOrSinglePlayerGame() == true) {
                            bss.damagePos(mapX, mapY);
                            gw.getPNB().add(NetChangePNB.NPNB_BASE_HIT, 0, /* screenGetTankPlayer(tk) */ 0, mapX, mapY); // FIXME
                        }
                        pbs.baseHit(mapX, mapY, bss.getOwnerFromPos(mapX, mapY));
                    }
                    gw.getSoundDist().soundDist(SoundDist.SHOT_BUILDING_NEAR, mapX, mapY);
                } else if (bss.canHit(mapX, mapY, s.getOwner()) == true) {
                    returnValue = new GameWorldItem();
                    returnValue.setX(Utils.GetWorldFromMap(mapX) + Utils.MAP_SQUARE_MIDDLE);
                    returnValue.setY(Utils.GetWorldFromMap(mapY) + Utils.MAP_SQUARE_MIDDLE);
                    if (bss.canHit(mapX, mapY, s.getOwner()) == true) {
                        if (gw.isServerOrSinglePlayerGame() == true) {
                            bss.damagePos(mapX, mapY);
                            gw.getPNB().add(NetChangePNB.NPNB_BASE_HIT, 0, /* screenGetTankPlayer(tk) */ 0, mapX, mapY); // FIXME
                        }
                        pbs.baseHit(mapX, mapY, bss.getOwnerFromPos(mapX, mapY));
                    }
                    gw.getSoundDist().soundDist(SoundDist.SHOT_BUILDING_NEAR, mapX, mapY);
                }
            }
        }
        
        int terrain;
        int newTerrain;
        if (returnValue == null) {
            if (map.isPassable(mapX, mapY, s.getOnBoat()) == false && basesExist == false) {
                returnValue = new GameWorldItem();
                returnValue.setX(Utils.GetWorldFromMap(mapX) + Utils.MAP_SQUARE_MIDDLE);
                returnValue.setY(Utils.GetWorldFromMap(mapY) + Utils.MAP_SQUARE_MIDDLE);
                terrain = gw.getBoloMap().getPos(mapX, mapY);
                if (Terrain.IsMinedTerrain(terrain) == true) {
                    terrain -= Terrain.MINE_START;
                    isMine = true;
                }
                /* Update terrain and play sound */
                switch (terrain) {
                    case Terrain.BUILDING:
                        map.setPos(mapX, mapY, Terrain.HALFBUILDING, false, false);
                        gw.getSoundDist().soundDist(SoundDist.SHOT_BUILDING_NEAR, mapX, mapY);
                        break;
                    case Terrain.FOREST:
                        if (isMine == true) {
                            map.setPos(mapX, mapY, Terrain.GRASS+Terrain.MINE_SUBTRACT, false, false);
                            if (isServer == true) {
                                gw.getMNT().add(NetChangeMNT.NMNT_MINEEXPLOSION, 0, /* screenGetTankPlayer(tk) */ 0, mapX, mapY); // FIXME
                            }
                        } else {
                            map.setPos(mapX, mapY, Terrain.GRASS, false, false);
                        }
                        gw.getSoundDist().soundDist(SoundDist.SHOT_TREE_NEAR, mapX, mapY);
                        break;
                    case Terrain.HALFBUILDING:
                        map.setPos(mapX, mapY, gw.getBuildings().addItem(mapX, mapY), false, false);
                        gw.getSoundDist().soundDist(SoundDist.SHOT_BUILDING_NEAR, mapX, mapY);
                        break;
                    case Terrain.BOAT:
                        map.setPos(mapX, mapY, Terrain.RIVER, false, false);
                        gw.getSoundDist().soundDist(SoundDist.SHOT_BUILDING_NEAR, mapX, mapY);
                        break;
                    case Terrain.GRASS:
                        if (isMine == true) {
                            map.setPos(mapX, mapY, Terrain.GRASS+Terrain.MINE_SUBTRACT, false, false);
                            if (isServer == true) {
                                gw.getMNT().add(NetChangeMNT.NMNT_MINEEXPLOSION, 0, /* screenGetTankPlayer(tk)*/ 0, mapX, mapY); //FIXME
                            }
                        } else {
                            newTerrain = gw.getGrass().addItem(mapX, mapY);
                            map.setPos(mapX, mapY, newTerrain, false, false);
                            if (newTerrain == Terrain.RIVER) {
                                gw.getFloodFill().add(mapX, mapY);
                            }
                        }
                        break;
                    case Terrain.SWAMP:
                        if (isMine == true) {
                            map.setPos(mapX, mapY, Terrain.SWAMP+Terrain.MINE_SUBTRACT, false, false);
                            if (isServer == true) {
                                gw.getMNT().add(NetChangeMNT.NMNT_MINEEXPLOSION, 0, 0 /* screenGetTankPlayer(tk) */, mapX, mapY); // FIXME
                            }
                        } else {
                            newTerrain = gw.getSwamp().addItem(mapX, mapY);
                            map.setPos(mapX, mapY, newTerrain, false, false);
                            if (newTerrain == Terrain.RIVER) {
                                gw.getFloodFill().add(mapX, mapY);
                            }
                        }
                        break;
                    case Terrain.RUBBLE:
                        newTerrain = gw.getRubble().addItem(mapX, mapY);
                        map.setPos(mapX, mapY, newTerrain, false, false);
                        if (newTerrain == Terrain.RIVER) {
                            gw.getFloodFill().add(mapX, mapY);
                        }
                        break;
                    case Terrain.ROAD:
                        if (isMine == true) {
                            map.setPos(mapX, mapY, Terrain.ROAD+Terrain.MINE_SUBTRACT, false, false);
                            if (isServer == true) {
                                gw.getMNT().add(NetChangeMNT.NMNT_MINEEXPLOSION, 0, /* FIXME screenGetTankPlayer(tk) */ 0, mapX, mapY);
                            }
                        } else {
                            map.setPos(mapX, mapY, checkRoad(map, pbs, gw.getBases(), mapX, mapY, s.getAngle()), false, false);
                        }
                }
                
            }
        }
        
        if (returnValue != null) {
            gw.reCalc();
        }
        return returnValue;
    }
    
    public byte[] netMake(int playerNum, boolean sentState) {
        ByteArrayOutputStream bos = new ByteArrayOutputStream(shs.size() * (4+3+1));

        
        for (Shell s : shs) {
            if (s.getPackSent() == false && s.getCreator() != playerNum) {
                if (true) {  // FIXME screenTankInView(noPlayerNum, (BYTE) (q->x >> TANK_SHIFT_MAPSIZE), (BYTE) (q->y >> TANK_SHIFT_MAPSIZE)) == TRUE
                    bos.write(s.getX()); // FIXME ENdianisms
                    bos.write(s.getX());
                    bos.write(s.getY());
                    bos.write(s.getY());
                    //                    bos.write(s.getAngle());
                    //                    bos.write(s.getAngle());
                    //                    bos.write(s.getAngle());
                    bos.write(s.getLength());
                    bos.write(s.getOwner());
                    bos.write(Utils.BooleanToByte(s.getOnBoat()));
                    bos.write(s.getCreator());
                }
                /* We have no sent it */
                s.setPackSent(sentState);
            }
        }        
        return bos.toByteArray();
    }
    
    public void netExtract(byte data[], int dataLen, boolean isServer) {
        Shell s = null;
        int self = gw.getPlayers().getSelf();
        int x;
        int y;
        boolean onBoat;
        int owner;
        float angle;
        int length;
        boolean shouldAdd;
        int pos = 0;
        int creator;
        int amount;
        int twx;
        int twy;
        Tank tnk;
        Point p;
        boolean packSent;
        Pillboxes pbs = gw.getPills();
        int mx;
        int my;
        
        while (pos < dataLen) {
            shouldAdd = false;
            x = data[pos]; // FIXME endian
            pos += 2;
            y = data[pos];
            pos += 2;
            angle = data[pos];
            pos += 3;
            length = data[pos];
            pos++;
            owner = Utils.ConvertByteToInt(data[pos]);
            pos++;
            onBoat = Utils.ByteToBoolean(data[pos]);
            pos++;
            creator = data[pos];
            pos++;
            
            /* Check to see if we should add to it */
            if (isServer == true) {
                if (owner != GameItem.NEUTRAL) {
/*FIXME                    tnk = screenGetTankFromPlayer(creator);
                    tankGetWorld(tnk, &twx, &twy);
                    amount = tankGetShells(tnk);
                    if (amount > 0) {
                        if (utilIsItemInRange(twx, twy, wx, wy, 512, &dummy) == TRUE) {
                            amount--;
                            tankSetShells(tnk, amount);
                            shouldAdd = TRUE;
                        }
                    } */
                } else {
                    /* Fired from a pill - Check locality */
                    p = Utils.CalcDistance(angle, Shell.SPEED);
                    p.setX(p.getX() * 2);
                    p.setY(p.getY() * 2);
                    mx = Utils.GetMapFromWorld(x - p.getX());
                    my = Utils.GetMapFromWorld(y - p.getY());
                    if (pbs.existPos(mx, my) == true && pbs.deadPos(mx, my) == false) {
                        shouldAdd = true;
                    }
                }
            } else if (creator != self) {
                shouldAdd = true;
            }
            
            if (length > 68 && owner == GameItem.NEUTRAL) { /* Added length check to stop cheating */
                shouldAdd = false;
            } else if (length > 52 && owner != GameItem.NEUTRAL) {
                /* Tank max length */
                shouldAdd = false;
            }
            
            
            /* Add it if required */
            if (shouldAdd == true) {
                if (isServer == true) {
                    packSent = false;
                } else {
                    packSent = true;
                }
                s = new Shell(x, y, angle, owner, length, onBoat, creator, false, packSent);
                shs.add(s);
            }
        }
        
        /* Play the last sound event if exist */
        if (s != null) {
            gw.getSoundDist().soundDist(SoundDist.SHOOT_NEAR, s.getMX(), s.getMY());
        }
    }
    
    
    /**
     * A shell has been fired from a boat and has hit a road.
     *  This function returns whether the road should be kept
     *  as road or destroyed and replaced as river
     *
     * @param map - BoloMap
     * @param pbs Pillboxes
     * @param bss Bases
     * @param mapX X position of the hit
     * @param mapY Y position of the hit
     * @param angle - The angle the shell is travelling
     * @return the new terrain of the square
     */
    private int checkRoad(BoloMap map, Pillboxes pbs, Bases bss, int mapX, int mapY, float dir) {
        int returnValue = Terrain.ROAD;
        int dir16;
        
        dir16 = Utils.Get16Dir(dir);
        if (dir16 < Utils.BRADIANS_NEAST || dir16 >= Utils.BRADIANS_NWEST) {
            if (map.isLand(mapX, (mapY-1)) == false) {
                returnValue = Terrain.RIVER;
            }
        } else if (dir16 >= Utils.BRADIANS_NEAST && dir16 < Utils.BRADIANS_SEAST) {
            if (map.isLand((mapX+1), mapY) == false) {
                returnValue = Terrain.RIVER;
            }
        } else if (dir16 >= Utils.BRADIANS_SEAST && dir16 < Utils.BRADIANS_SWEST) {
            if (map.isLand(mapX, (mapY+1)) == false) {
                returnValue = Terrain.RIVER;
            }
        } else {
            if (map.isLand((mapX-1), mapY) == false) {
                returnValue = Terrain.RIVER;
            }
        }
        
        /* Play the sound if required */
        if (returnValue == Terrain.RIVER) {
            gw.getSoundDist().soundDist(SoundDist.SHOT_BUILDING_NEAR, mapX, mapY);
        }
        
        return returnValue;
    }
    
}
