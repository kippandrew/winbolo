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
 * GameWorld.java
 *
 * Created on October 5, 2003, 7:49 PM
 */
package com.jbolo.bolo;

import com.jbolo.gui.*;
import com.jbolo.bolo.map.BoloMap;
import com.jbolo.bolo.network.Network;
import java.util.ArrayList;

/**
 *
 * @author  John Morrison
 */
public class GameWorld implements GameClient {
    
    /** The bases in this game world */
    protected Bases bss;
    /** The pillboxes in this game world */
    protected Pillboxes pbs;
    /** The starts in this game world */
    protected Starts sss;
    /** The BoloMap in this game world */
    protected BoloMap mp;
    /** The mines in this game world */
    protected Mines mines;
    /** The labels in this game world */
    protected Labels labels;
    /** The messages in this game world */
    protected Messages messages;
    
    /** The FloodFillTimedTerrain in this game world */
    protected FloodFillTimedTerrain floodFill;
    /** The mine explosions in this game map */
    protected MinesExplosionTimedTerrain minesExp;
    /** The players in this game world */
    protected Players players;
    /** The shells in this game world */
    protected Shells shs;
    /** The swamp in this game world */
    protected ChangeableTerrain swamp;
    /** The grass in this game world */
    protected ChangeableTerrain grass;
    /** The buildings in this world */
    protected ChangeableTerrain building;
    /** The rubble in this world */
    protected ChangeableTerrain rubble;
    /** The sound dist in this game world */
    protected SoundDist soundDist;
    /** The net brain map in this game world */
    protected BrainMap brainMap;
    /** The net change mnt in this game world */
    protected NetChangeMNT mnt;
    /** The net change pnb in this game world */
    protected NetChangePNB pnb;
    /** The log in this game world */
    protected Log log;
    /** The explosions in this game world */
    protected Explosions explosions;
    /** The game type of this game world */
    protected GameType gmeType;
    /** The scroll interface of this game world */
    protected Scroll scroll;
    /** Tank Explosions in this game world */
    protected TankExplosion tankExplosions;
    /** The tree grow on this game world */
    protected TreeGrow treeGrow;
    /** Interface to the drawing system */
    protected FrontEndDraw fed;
    /** Interface to the sound system */
    protected FrontEndSound fes;
    /** Interface to misc frontend functions */
    protected FrontEndClient fec;
    /* The time game was created */
    protected long timeStart;
    /** Game Start Delay */
    protected int gmeStartDelay;
    /** The length this game has been running */
    protected long gmeLength;
    /** This clients LGM */
    protected LGM lgm;
    
    
    /** Time the game was started */
    protected long startTime;
    
    /* The offset from the top and left of the map */
    private int xOffset;
    private int yOffset;
    private int[][] screenItem;
    private boolean[][] screenMines;
    private Tank tank = null;
    
    private boolean inPillView;
    private int messageTime;
    private int pillViewX;
    private int pillViewY;
    
    private String mapName;
    
    /** Are we running? */
    private boolean screenGameRunning;
    private boolean needScreenRecalc;
    
    /** Cursor X position */
    private int cursorPosX;
    /** Cursor Y position */
    private int cursorPosY;
    
    private boolean inStartFind;
    
    private int allowComputerTanks = 0;
    
    private Network network;
    
    /** Creates a new instance of GameWorld */
    public GameWorld(int gameType, boolean allowHiddenMines, FrontEndDraw fed, FrontEndSound fes, FrontEndClient fec) {
        this.fed = fed;
        this.fes = fes;
        this.fec = fec;
        
        network = new Network(this);
        mp = new BoloMap(this);
        pbs = new Pillboxes(this);
        bss = new Bases(this);
        sss = new Starts(this);
        mines = new Mines(this, allowHiddenMines);
        labels = new Labels();
        messages = new Messages(this);
        minesExp = new MinesExplosionTimedTerrain(this);
        floodFill = new FloodFillTimedTerrain(this);
        players = new Players(this);
        gmeType = new GameType(this, gameType);
        shs = new Shells(this);
        scroll = new Scroll(this);
        brainMap = new BrainMap();
        mnt = new NetChangeMNT(this);
        pnb = new NetChangePNB(this);
        treeGrow = new TreeGrow(this);
        tankExplosions = new TankExplosion(this);
        log = new Log();
        explosions = new Explosions();
        
        swamp = new ChangeableTerrain(5, Terrain.RIVER, Terrain.SWAMP);
        grass = new ChangeableTerrain(5, Terrain.SWAMP, Terrain.GRASS);
        building = new ChangeableTerrain(4, Terrain.RUBBLE, Terrain.HALFBUILDING);
        rubble = new ChangeableTerrain(5, Terrain.RIVER, Terrain.RUBBLE);
        soundDist = new SoundDist(this);
        
        messageTime = 0;
        inPillView = false;
        xOffset = 15;
        yOffset = 15;
        pillViewX = 0;
        pillViewY = 0;
        mapName = new String();
        screenGameRunning = true;//FIXME false
        needScreenRecalc = true;
        gmeStartDelay = 0;
        gmeLength = -1;
        cursorPosX = 0;
        cursorPosY = 0;
        screenItem = new int[MAIN_BACK_BUFFER_SIZE_X][MAIN_BACK_BUFFER_SIZE_Y];
        screenMines = new boolean[MAIN_BACK_BUFFER_SIZE_X][MAIN_BACK_BUFFER_SIZE_Y];
    }
    
    /**
     * Gets the explosions in this game world
     * @return The explosions in this game world
     */
    public Explosions getExplosions() {
        return explosions;
    }
    
    /**
     * Gets the tank explosions in this game world
     * @return The explosions in this game world
     */
    public TankExplosion getTankExplosion() {
        return tankExplosions;
    }
    
    
    /**
     * Gets the swamp in this game world
     * @return The swamp in this game world
     */
    public ChangeableTerrain getSwamp() {
        return swamp;
    }
    
    /**
     * Gets the grass in this game world
     * @return The grass in this game world
     */
    public ChangeableTerrain getGrass() {
        return grass;
    }
    
    /**
     * Gets the game type object
     * @return the game type of this game world
     */
    public GameType getGameTypeObject() {
        return gmeType;
    }
    
    /**
     * Gets the NetChangeMNT in this game world
     * @return The NetChangeMNT in this game world
     */
    public NetChangeMNT getMNT() {
        return mnt;
    }
    
    /**
     * Gets the NetChangePNB in this game world
     * @return The NetChangePNB in this game world
     */
    public NetChangePNB getPNB() {
        return pnb;
    }
    
    /**
     * Gets the FloodFillTimedTerrain in this game world
     * @return The FloodFillTimedTerrain in this game world
     */
    public FloodFillTimedTerrain getFloodFill() {
        return floodFill;
    }
    
    /**
     * Gets the rubble in this game world
     * @return The rubble in this game world
     */
    public ChangeableTerrain getRubble() {
        return rubble;
    }
    
    /**
     * Gets the buildings in this game world
     * @return The buildings in this game world
     */
    public ChangeableTerrain getBuildings() {
        return building;
    }
    
    
    /**
     * Gets the messages in this game world
     * @return The messages in this game world
     */
    public Messages getMessages() {
        return messages;
    }
    
    /**
     * Gets the brain map in this game world
     * @return The BrainMap in this game world
     */
    public BrainMap getBrainMap() {
        return brainMap;
    }
    
    /**
     * Gets the Log in this game world
     * @return The Log in this game world
     */
    public Log getLog() {
        return log;
    }
    
    /**
     * Gets the mines in this game world
     * @return The mines in this game world
     */
    public Mines getMines() {
        return mines;
    }
    
    /**
     * Gets the BoloMap in this game world
     * @return The BoloMap in this game world
     */
    public BoloMap getBoloMap() {
        return mp;
    }
    
    /**
     * Gets the tank in this game world
     * @return The tank in this game world
     */
    public Tank getTank() {
        return tank;
    }
    
    public LGM getLGM() {
        return lgm;
    }
    
    /**
     * Gets the MinesExplosionTimedTerrain in this game world
     * @return The MinesExplosionTimedTerrain in this game world
     */
    public MinesExplosionTimedTerrain getMinesExp() {
        return minesExp;
    }
    
    /**
     * Gets the sound distance in this game world
     * @return The SoundDist in this game world
     */
    public SoundDist getSoundDist() {
        return soundDist;
    }
    
    /**
     * Gets the starts in this game world
     * @return The starts in this game world
     */
    public Starts getStarts() {
        return sss;
    }
    
    /**
     * Gets the pillboxes in this game world
     * @return The pillboxes in this game world
     */
    public Pillboxes getPills() {
        return pbs;
    }
    
    /**
     * Gets the bases in this game world
     * @return The bases in this game world
     */
    public Bases getBases() {
        return bss;
    }
    
    /**
     * Gets the labels in this game world
     * @return The labels in this game world
     */
    public Labels getLabels() {
        return labels;
    }
    
    /**
     * Gets the players in this game world
     * @return The players in this game world
     */
    public Players getPlayers() {
        return players;
    }
    
    /**
     * Gets the FrontEndDraw in this game world
     * @return The FrontEndDraw in this game world
     */
    public FrontEndDraw getFrontEndDraw() {
        return fed;
    }
    
    /**
     * Gets the FrontEndSound in this game world
     * @return The FrontEndSound in this game world
     */
    public FrontEndSound getFrontEndSound() {
        return fes;
    }
    
    /**
     * Gets the FrontEndMisc in this game world
     * @return The FrontEndMisc in this game world
     */
    public FrontEndClient getFrontEndClient() {
        return fec;
    }
    
    
    public void setTimeLengths(long startTime, long timeLimit) {
        //FIXME
        //        this.gmeStartDelay = (int) startTime;
        //        this.gmeLength = timeLimit;
    }
    
    /**
     * Saves the map in its current state
     *
     * @return success of saving file
     * @param fileName Filename to save to
     */
    public boolean saveMap(String fileName) {
        return mp.write(fileName, sss, bss, pbs);
    }
    
    /**
     * Attempts to join a game.
     *
     * @param netType Type. Either netSingle for single player game or netUdp for network game
     * @param myPort Port to start on
     * @param targetIp Target IP Address to look for game
     * @param targetPort Target port to look for game
     * @param userName Player name to join with
     * @param password Game password if we started the game
     * @param usCreate Did we start the server ourselves (if se we already know the password)
     * @param trackerAddr Tracker address to send game updates to
     * @param trackerPort Tracker port to send updates to
     * @param useTracker Do we want to send updates to the tracker
     * @param wantRejoin Do we want to attempt to rejoin the game
     * @param useWinboloNet Do we want to use WBN
     * @param wbnPassword Our WBN password
     * @return Success of setting up (and joining a game if netType is netUDP)
     */
    public boolean joinGame(int netType, int myPort, String targetIp, int targetPort, String userName, String password, boolean usCreate, String trackerAddr, int trackerPort, boolean useTracker, boolean wantRejoin, boolean useWinboloNet, String wbnPassword) {
        return network.setup(netType, myPort, targetIp, targetPort, userName, password, usCreate, trackerAddr, trackerPort, useTracker, wantRejoin, useWinboloNet, wbnPassword);
    }
    
    public void shutdown() {
        network.shutdown();
    }
    
    /**
     * reads in a map file
     * @return success, fails on map load error
     * @param fileName Map File to load
     */
    public boolean read(String fileName) {
        boolean returnValue;
        mp = new BoloMap(this);
        pbs = new Pillboxes(this);
        bss = new Bases(this);
        sss = new Starts(this);
        
        returnValue = mp.read(fileName, sss, bss, pbs);
        if (returnValue == true) {
            mapName = Utils.GetMapNameFromFileName(fileName);
            tank = new Tank(this, gmeType, sss);
            lgm = new LGM(this, players.getSelf(), tank);
            players.setSelf(0, "Elvis"); //FIXME
            centerTank();
        }
        return returnValue;
    }
    
    /** Is this game instance a game server or client
     * @return true if it is a server
     */
    public boolean isServer() {
        return false;
    }
    
    /**
     * Gets if this tank is dead
     *
     * @return if the tank is dead or not
     */
    public boolean isTankDead() {
        return tank.isDead();
    }
    
    public boolean getGunsight() {
        return tank.isGunsightShowing();
    }
    
    public void setGunsightShowing(boolean value) {
        tank.setGunsightShowing(value);
    }
    
    /** Is this game instance a game server or single player game or network client
     * @return true if it is a server/single player game
     */
    public boolean isServerOrSinglePlayerGame() {
        return true;
    }
    
    public Network getNetwork() {
        return network;
    }
    
    /**
     * Gets the number of kills this tank has had
     * @return Number of kills this tank has had
     */
    public int getTankKills() {
        return tank.getKills();
    }
    
    /**
     * Gets the number of deaths this tank has had
     * @return Number of deaths this tank has had
     */
    public int getTankDeaths() {
        return tank.getDeaths();
    }
    
    boolean busy = false;
    int waitTime = 0;
    
    public void update(TankButtons tb) {
        boolean isServer = true;
        
        
        if (busy == true) {
// haha            System.err.println("Syncronization is stupid");
            return;
        }
        
        busy = true;
        if (network.getType() == Network.NET_GAME_TYPE_UDP) {
            waitTime++;
            if (waitTime == 10) {
                network.check();
                waitTime = 0;
            }
        }
        
        try {
            
            if (this.screenGameRunning == false || network.getStatus() != Network.NET_STATE_RUNNING) { // FIXME: or ngs = netFailed
                busy = false;
                return;
            }
            
            if (gmeStartDelay > 0) {
                gmeStartDelay--;
                messageTime++;
                if (messageTime == Messages.MESSAGE_SCROLL_TIME) {
                    messages.update();
                    messageTime = 0;
                }
                return;
            }
            if (gmeLength > 0) {
                gmeLength--;
                if (gmeLength == 0) {
                    /* Game Times Up. */
                    screenGameRunning = false;
                    //FIXME frontEndGameOver();^M
                    return;
                }
            }
            
            pbs.update(mp, bss, players, shs, tank);
            bss.update();
            
            if (false == true) { //FIXME ngs != netRunning
                /* Messaging */
                messageTime++;
                if (messageTime == Messages.MESSAGE_SCROLL_TIME) {
                    messages.update();
                    messageTime = 0;
                }
                return;
            }
            
            /* Do the brain translation if required */
            //FIXME if (isBrain == TRUE) {^M
            //    tb = screenTranslateBrainButtons(&tankShoot, TRUE);^M
            //  }
            
            
            tank.update(mp, bss, pbs, shs, sss, tb.getHeldButtonsuttons() , tb.getShooting(), false);
            
            // Update the scroll
            if (tank.getSpeed() > 0) {
                if (inPillView == false) {
                    int oldXOffset = xOffset;
                    int oldYOffset = yOffset;
                    GameWorldItem gunsight = tank.getGunsight();
                    
                    Point p = new Point(xOffset, yOffset);
                    if (scroll.update(p, pbs, tank.getScreenMX(), tank.getScreenMY(), true, gunsight.getMX(), gunsight.getMY(), tank.getSpeed(), tank.getArmour(), tank.getTravelAngle(),  false) == true) {
                        if (oldXOffset < p.getX()) {
                            cursorPosX--;
                        } else if (oldXOffset > p.getX()) {
                            cursorPosX++;
                        }
                        if (oldYOffset < p.getY()) {
                            cursorPosY--;
                        } else if (oldYOffset > p.getY()) {
                            cursorPosY++;
                        }
                        xOffset = p.getX();
                        yOffset = p.getY();
                        reCalc();
                    }
                }
            }
            
            /* Check we are still allowed to be in pillbox view */
            if (inPillView == true) {
                if (pbs.checkPillView(pillViewX, pillViewY, players.getSelf()) == false) {
                    tankView();
                }
            }
            
            int baseNum;
            Base base;
            
            
            baseNum = bss.getClosest(tank.getWorldX(), tank.getWorldY());
            if (baseNum != GameItems.NOT_FOUND) {
                base = (Base) bss.getItem(baseNum);
                fed.feDrawStatusBaseBars(base.getShells(), base.getMines(), base.getArmour(), false);
            } else {
                fed.feDrawStatusBaseBars(0, 0, 0, false);
            }
            
            
            ArrayList tanks = new ArrayList();
            tanks.add(tank);
            shs.update(mp, pbs, bss, tanks, isServer);
            lgm.update();
  /*
  test = &lgman;^M
  tkExplosionUpdate(&mp, &pb, &bs, (lgm **) &test, 1);^M
   */
            tankExplosions.update(mp, pbs, bss);
            explosions.update();
            minesExp.update(mp, pbs, bss);
            floodFill.update(mp, pbs, bss);
            treeGrow.update();
            mp.update();
   /* Update network LGM frames as the are client controlled *
  playersGameTickUpdate();^M
    */
            /* Messaging */
            messageTime++;
            if (messageTime == Messages.MESSAGE_SCROLL_TIME) {
                messages.update();
                messageTime = 0;
            }
            busy = false;
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    /** tankView centers the screen on our tank
     */
    public void tankView() {
        inPillView = false;
        centerTank();
    }
    
    /**
     * reCalc tells us that we need to recalculate the scren
     */
    public void reCalc() {
        needScreenRecalc = true;
    }
    
    public void screenUpdate(int updateType) {
        ArrayList tanks;
        ArrayList shells;
        ArrayList lgms;
        Point cursorPos = null;
        int x; /* X and Y co-ords of the tank */
        int y;
        int px; /* Pixel X and Y co-ords of the tank */
        int py;
        int left, right, top, bottom;
        long time = System.currentTimeMillis();
        int ns = network.getStatus();
        
        if (ns != Network.NET_STATE_RUNNING && ns != Network.NET_STATE_FAILED) {
            fed.feDrawDownloadScreen(false, network.getMapDownloadPosition());
            return;
        }
        
        
        
        
        if (updateType == 1) { /* Up */
            
            this.yOffset--;
            reCalc();
            return;
        } else if (updateType == 2) { /* Down */
            this.yOffset++;
            reCalc();
            return;
        } else if (updateType == 3) { /* Left */
            this.xOffset--;
            reCalc();
            return;
        } else if (updateType == 4) { /* Right */
            this.xOffset++;
            reCalc();
            return;
        }
        updateView();
        
        left = xOffset;
        right = xOffset + MAIN_BACK_BUFFER_SIZE_X;
        top = yOffset;
        bottom = yOffset + MAIN_BACK_BUFFER_SIZE_Y;
        if (cursorPosX >= left && cursorPosX <= right && cursorPosY >= top && cursorPosY <= bottom) {
            cursorPos = new Point(cursorPosX - xOffset, cursorPosY - yOffset);
        }
        
        shells = shs.getScreenItems(left, right, top, bottom);
        shells.addAll(explosions.getScreenItems(left, right, top, bottom));
        shells.addAll(tankExplosions.getScreenItems(left, right, top, bottom));
        tanks = prepareTanks();
        lgms = prepareLgms();
        //	System.err.println("Preparation took " + (System.currentTimeMillis() - time));
        fed.feDrawMainView(screenItem, screenMines, tanks, shells, lgms, tank.getGunsight(left, right, top, bottom), cursorPos, -1);
        
    }
    
    /**
     * Prepares the list of tanks to be displayed on the screen
     *
     * @return displays the list of Screen
     */
    private ArrayList prepareLgms() {
        ArrayList returnValue;
        ScreenItem si;
        int mx, my, px, py, frame;
        int count = 0;
        int right;
        int bottom;
        
        right = xOffset + MAIN_BACK_BUFFER_SIZE_X;
        bottom = yOffset + MAIN_BACK_BUFFER_SIZE_Y;
        
        returnValue = players.makeScreenLgms(xOffset, right, yOffset, bottom, tank.getX(), tank.getY());
        si = lgm.getScreenItem(xOffset, right, yOffset, bottom);
        if (si != null) {
            returnValue.add(si);
        } else {
        }
        return returnValue;
    }
    
    /**
     * Prepares the list of tanks to be displayed on the screen
     * @return ArrayList of ScreenTankItems to display
     */
    private ArrayList prepareTanks() {
        ArrayList returnValue; // Value to return
        ScreenTankItem st;
        int mx, my, px, py, frame;
        int count = 0;
        int right;
        int bottom;
        
        right = xOffset + MAIN_BACK_BUFFER_SIZE_X;
        bottom = yOffset + MAIN_BACK_BUFFER_SIZE_Y;
        
        returnValue = players.makeScreenTanks(xOffset, right, yOffset, bottom, tank.getX(), tank.getY());
        mx = tank.getScreenMX();
        my = tank.getScreenMY();
        if (mx >= xOffset && mx <= right && my >= yOffset && my <= bottom) {
            px = tank.getScreenPX();
            py = tank.getScreenPY();
            if (tank.isDead() == false) {
                st = new ScreenTankItem(mx - xOffset, my - yOffset, px, py, tank.getFrame(), tank.getPlayerNum(), "Me");
                returnValue.add(st);
            }
        }
        return returnValue;
    }
    
    private void updateView() {
        int count;
        int count2;
        if (needScreenRecalc == true) {
            for (count=0;count<MAIN_BACK_BUFFER_SIZE_X;count++) {
                for (count2=0;count2<MAIN_BACK_BUFFER_SIZE_Y;count2++) {
                    screenItem[count][count2] = screenCalcSquare((count+xOffset), (count2+yOffset), count, count2);
                    brainMap.setPos(count+xOffset, count2+ yOffset, mp.getPos((count+xOffset), (count2+yOffset)), mines.existPos((count+xOffset), (count2+yOffset)));
                }
            }
            needScreenRecalc = false;
        }
    }
    
    
    
    /**
     * Removes mines for terrain squares and converts bases to roads
     * @param xValue The x co-ordinate to calculate for
     * @param yValue The Y co-ordinate to calculate for
     * @return the Terrain of this square
     */
    private int getCalcSquareItem(int xValue, int yValue) {
        int returnValue;
        if (bss.existPos(xValue, yValue) == true) {
            returnValue = Terrain.ROAD;
        } else {
            returnValue = Terrain.TerrainWithoutMine(mp.getPos(xValue, yValue));
        }
        return returnValue;
        
    }
    
    /**
     * Calculates the terrain type for a given location
     * @param xValue The x co-ordinate to calculate for
     * @param yValue The Y co-ordinate to calculate for
     * @return the TileNum to draw
     */
    private int screenCalcSquare(int xValue, int yValue, int srcX, int srcY) {
        int ba;  /* The allience of a base */
        int returnValue; /* Value to return */
        int currentPos;
        int aboveLeft;
        int above;
        int aboveRight;
        int leftPos;
        int rightPos;
        int belowLeft;
        int below;
        int belowRight;
        
        screenMines[srcX][srcY] = false;
        /* Set up Items */
        if ((pbs.existPos(xValue,yValue)) == true) {
            returnValue = pbs.getScreenHealth(xValue, yValue);
        } else if (bss.existPos(xValue, yValue) == true) {
            returnValue = TileNum.BASE_GOOD;
            switch (bss.getAlliancePos(xValue, yValue)) {
                case GameItem.GAME_ITEM_OWN_GOOD:
                    returnValue = TileNum.BASE_GOOD;
                    break;
                case GameItem.GAME_ITEM_ALLY_GOOD:
                    returnValue = TileNum.BASE_GOOD;
                    break;
                case GameItem.GAME_ITEM_NEUTRAL:
                    returnValue = TileNum.BASE_NEUTRAL;
                    break;
                case GameItem.GAME_ITEM_DEAD:
                    if (bss.amOwner(players.getSelf(), xValue, yValue) == true) {
                        returnValue = TileNum.BASE_GOOD;
                    } else {
                        returnValue = TileNum.BASE_EVIL;
                    }
                    break;
                case GameItem.GAME_ITEM_EVIL:
                default:
                    /* Base Evil */
                    returnValue = TileNum.BASE_EVIL;
            }
        }  else {
            currentPos = mp.getPos(xValue, yValue);
            if (mp.isMine(xValue,  yValue) == true) {
                screenMines[srcX][srcY] = mines.existPos(xValue, yValue);
                if (currentPos != Terrain.DEEP_SEA) {
                    currentPos = currentPos - Terrain.MINE_SUBTRACT;
                }
            } else {
                screenMines[srcX][srcY] = false;
            }
            aboveLeft = getCalcSquareItem((xValue-1), (yValue-1));
            above = getCalcSquareItem(xValue, (yValue-1));
            aboveRight  = getCalcSquareItem((xValue+1), (yValue-1));
            leftPos = getCalcSquareItem((xValue-1), yValue);
            rightPos = getCalcSquareItem((xValue+1), yValue);
            belowLeft = getCalcSquareItem((xValue-1), (yValue+1));
            below = getCalcSquareItem(xValue, (yValue+1));
            belowRight = getCalcSquareItem((xValue+1), (yValue+1));
            
            switch (currentPos) {
                case Terrain.ROAD:
                    returnValue = ScreenCalc.ScreenCalcRoad(aboveLeft, above, aboveRight, leftPos, rightPos, belowLeft, below, belowRight);
                    break;
                case Terrain.BUILDING:
                    returnValue = ScreenCalc.ScreenCalcBuilding(aboveLeft, above, aboveRight, leftPos, rightPos, belowLeft, below, belowRight);
                    break;
                case Terrain.FOREST:
                    returnValue = ScreenCalc.ScreenCalcForest(aboveLeft, above, aboveRight, leftPos, rightPos, belowLeft, below, belowRight);
                    break;
                case Terrain.RIVER:
                    returnValue = ScreenCalc.ScreenCalcRiver(aboveLeft, above, aboveRight, leftPos, rightPos, belowLeft, below, belowRight);
                    break;
                case Terrain.DEEP_SEA:
                    returnValue = ScreenCalc.ScreenCalcDeepSea(aboveLeft, above, aboveRight, leftPos, rightPos, belowLeft, below, belowRight);
                    break;
                case Terrain.BOAT:
                    returnValue = ScreenCalc.ScreenCalcBoat(aboveLeft, above, aboveRight, leftPos, rightPos, belowLeft, below, belowRight);
                    break;
                case Terrain.CRATER:
                    returnValue = ScreenCalc.ScreenCalcCrater(aboveLeft, above, aboveRight, leftPos, rightPos, belowLeft, below, belowRight);
                    break;
                default:
                    returnValue = currentPos;
                    break;
            }
        }
        return returnValue;
    }
    
    /**
     * Gets the number of bases in this game client
     * @return the number of bases in this game
     */
    public int getNumBases() {
        return bss.getNumItems();
    }
    
    /**
     * Gets the number of pillboxes in this game client
     * @return the number of pillboxes in this game
     */
    public int getNumPillboxes() {
        return pbs.getNumItems();
    }
    
    /**
     * Gets the base status for a base in this game client
     * @return the base status for a base in this game client
     * @param itemNum to get (0-15)
     * @see GameItem
     */
    public int getBaseStatus(int itemNum) {
        return bss.getAllianceNum(itemNum);
    }
    
    /**
     * Gets the pillbox status for a pillbox in this game client
     * @return the pillbox status for a pillbox in this game client
     * @param itemNum to get (0-15)
     * @see GameItem
     */
    public int getPillboxStatus(int itemNum) {
        return pbs.getAllianceNum(itemNum);
    }
    
    /**
     * Gets the number of players in this game
     * @return the number of players in this game client
     */
    public int getNumPlayers() {
        return players.getNumPlayers();
    }
    
    /**
     * Gets the tank status for a tank in this game client
     * @return the tank status for a tank in this game client
     * @param itemNum to get (0-15)
     * @see GameItem
     */
    public int getTankStatus(int itemNum) {
        if (itemNum == players.getSelf()) {
            return GameItem.GAME_ITEM_OWN_GOOD;
        }
        return GameItem.GAME_ITEM_BLANK; // FIXME
    }
    
    /**
     * Request for tank to lay mine has occured
     */
    public void tankLayMine() {
        tank.layMine();
    }
    
    public void centerTank() {
        Point p;
        if (tank != null) {
            if (tank.isDead() == false) {
                p = scroll.centerObject(tank.getMX(), tank.getMY());
                xOffset = p.getX();
                yOffset = p.getY();
            }
            reCalc();
        }
    }
    
    /**
     * Checks for tank damage done by a mine explosion
     * @param x X map position of the mine explosion
     * @param y Y map position of the mine explosion
     */
    public void checkTankMineDamage(int x, int y) {
        tank.checkMineDamage(x, y);
    }
    
    /**
     * Sets if we are in the find a start position
     * @param value Value to set
     */
    public void setInStartFind(boolean value) {
        inStartFind = value;
    }
    
    /**
     * Gets if we are in the find a start position
     * @return boolean indicating if we are finding a start position
     */
    public boolean getInStartFind() {
        return inStartFind;
    }
    
    /**
     * Gets the map name
     * @return The map name in this game
     */
    public String getMapName() {
        return mapName;
    }
    
    /**
     * Returns the game type
     * @return the game type
     */
    public int getGameType() {
        return gmeType.getType();
    }
    
    /**
     * Gets if the game allows computer tanks
     * @return if the game allows computer tanks
     */
    public int getComputerTanksAllowed() {
        return allowComputerTanks;
    }
    
    /**
     * Gets the number of players in this game
     * @return the number of players in this game
     */
    public int getNumberPlayers() {
        return players.getNumPlayers();
    }
    
    /**
     * Gets if hidden mines are allowed in this game
     * @return if the game allows hidden mines
     */
    public boolean isHiddenMinesAllowed() {
        return mines.getAllowHiddenMines();
    }
    
    /**
     * Gets the amount of time left in the game in game ticks
     * @return the amount of time left in a game or -1 for unlimitted
     */
    public long getGameTimeLeft() {
        return gmeLength;
    }
    
    /**
     * Attempts to set our player name in this game
     * @return if the name change is allowed
     * @param newName Name to change to
     */
    public boolean setPlayerName(String newName) {
        return players.setPlayerName(players.getSelf(), newName);
    }
    
    /**
     * Gets our tanks current player name
     * @return The tanks current name
     */
    public String getPlayerName() {
        return players.getPlayerName(players.getSelf());
    }
    
    /**
     * Changes the gunsight range of our tank
     * @param increaseRange If true our range is increased
     */
    public void changeGunsightRange(boolean increaseRange) {
        if (increaseRange == true) {
            tank.increaseGunsightRange();
        } else {
            tank.decreaseGunsightRange();
        }
    }
    
    /**
     * Sets whether newswire messages are shown
     * @param show Are these messages shown or not
     */
    public void showNewswireMessages(boolean show) {
        messages.setNewswire(show);
    }
    
    /**
     * Sets whether assistant messages are shown
     * @param show Are these messages shown or not
     */
    public void showAssistantMessages(boolean show) {
        messages.setAssistant(show);
    }
    
    /**
     * Sets whether brain messages are shown
     * @param show Are these messages shown or not
     */
    public void showBrainMessages(boolean show) {
        messages.setAI(show);
    }
    
    /**
     * Sets whether network status messages are shown
     * @param show Are these messages shown or not
     */
    public void showNetworkMessages(boolean show) {
        messages.setNetStatus(show);
    }
    
    /**
     * Sets whether network debug messages are shown
     * @param show Are these messages shown or not
     */
    public void showDebugMessages(boolean show) {
        messages.setNetwork(show);
    }
    
    /**
     * Sets the players checked state from the menu
     * @param playerNum Player number to set
     * 2param state The state to set
     */
    public void setPlayerCheckedState(int playerNum, boolean state) {
        players.setPlayerCheckedState(playerNum, state);
    }
    
    /**
     * Gets the number of checked players on the players menu
     * @return number of checked players
     */
    public int getNumberCheckedPlayers(int selectType) {
        int returnValue; // Value to return
        
        switch (selectType) {
            case GameClient.MESSAGE_ALL_ALLIES:
                returnValue =  players.getNumAllies(players.getSelf());
                break;
            case GameClient.MESSAGE_ALL_PLAYERS:
                returnValue = players.getNumPlayers();
                break;
            case GameClient.MESSAGE_ALL_NEARBY:
                returnValue = players.getNumNearby(tank.getMX(), tank.getMY());
                break;
            default:
                returnValue = players.getNumChecked();
        }
        return returnValue;
    }
    
    /**
     * Selects all players on the players menu
     */
    public void selectAll() {
        players.selectAll();
    }
    
    /**
     * Selects no players on the players menu
     */
    public void selectNone() {
        players.selectNone();
    }
    
    /**
     * Selects allies on the players menu
     */
    public void selectAllies() {
        players.selectAllies();
    }
    
    /**
     * Selects nearby tanks on the players menu
     */
    public void selectNearby() {
        players.selectNearby(tank.getMX(), tank.getMY());
    }
    
    /**
     * Sends a message
     * @param messageType The type of message to send
     * @param message The message to send
     */
    public void sendMessage(int messageType, String message) {
        boolean needSendSelf = true; // Do we need to send this message ourself
        String name; // Our name
        
        //FIXME Sending to other people
        switch (messageType) {
            case GameClient.MESSAGE_ALL_ALLIES:
                break;
            case GameClient.MESSAGE_ALL_PLAYERS:
                break;
            case GameClient.MESSAGE_ALL_NEARBY:
                break;
            default:
                needSendSelf = false;
        }
        
        if (needSendSelf == true) {
            name = players.makeMessageName(players.getSelf());
            messages.add(players.getSelf() + Messages.PLAYER_MESSAGE_OFFSET, name, message);
        }
    }
    /**
     * Gets whether our tank has autoslowdown turned on
     * @return if autoslowdown is enabled on this tank
     */
    public boolean getAutoslowState() {
        return tank.getAutoslowdownState();
    }
    
    /**
     * Gets if auto show hide gunsight is turned on
     * @return if auto show hide gunsight is turned on
     */
    public boolean getAutoShowHideGunsight() {
        return tank.getAutohideGunsight();
    }
    
    /**
     * Sets if auto slowdown is turned
     * @param state Stete to set
     */
    public void setAutoslowState(boolean state) {
        tank.setAutoslowdownState(state);
    }
    
    /**
     * Sets if auto showhide is turned
     * @param state Stete to set
     */
    public void setAutoShowHideGunsight(boolean state) {
        tank.setAutohideGunsight(state);
    }
    
    /**
     * The mouse has been clicked indicating a build operation
     * is requested. buildType is one of LGM_TREE_REQUEST,
     * LGM_ROAD_REQUEST,LGM_BUILDING_REQUEST,LGM_PILL_REQUEST LGM_MINE_REQUEST;
     *
     * @param buildType The building type request
     */
    public void buildRequest(int buildType) {
        lgm.addRequest(cursorPosX, cursorPosY, buildType);
    }
    
    /**
     * Sets the current cursor position in the game client in map
     * squares from 1,1 in the visiable corner of the screen
     *
     * @param x X map position
     * @param y Y map position
     */
    public void setCursorPos(int x, int y) {
        cursorPosX = xOffset + x;
        cursorPosY = yOffset + y;
    }
    
    /**
     * Sets a players DNS address
     *
     * @param ip IP Address to set for
     * @param hostname Hostname to set it to
     */
    public void setLocation(String ip, String hostname) {
        players.setLocation(ip, hostname);
    }
    
    /**
     * Sets whether autoscrolling is enabled or not
     *
     * @param state State to set
     */
    public void setAutoScroll(boolean state) {
        scroll.setType(state);
    }
    
    /**
     * Sets the map name
     *
     * @param name Value to set to
     */
    public void setMapName(String name) {
        mapName = name;
    }
    
    /**
     * Sets whether AI players are allowed
     *
     * @param aiType to AI type to set
     */
    public void setAIType(int aiType) {
        allowComputerTanks = aiType;
    }
    
    /**
     * Sets whether hidden mines are allowed in this game
     *
     * @param value Value to set
     */
    public void setAllowHiddenMines(boolean value) {
        mines.setAllowHiddenMines(value);
    }
    
    /**
     * Sets the game type
     *
     * @param value Value to set
     */
    public void setGameType(int value) {
        gmeType.set(value);
    }
    
    /**
     * Sets the game start time
     *
     * @param value Value to set to
     */
    public void setTimeGameCreated(long value) {
        startTime = value;
    }
    
    /**
     * Sets up the tank so it exists while we are download map
     *
     * @param Are we in In start mode?
     */
    public void netSetupTank(boolean inStarts) {
        this.inStartFind = inStarts;
        tank = new Tank(this, this.gmeType, sss);
        tank.setPlayerNum(players.getSelf());
        lgm = new LGM(this, players.getSelf(), tank);
    }
    
    /**
     * Sets up the base data from the network
     * Also refreshes the screen and sets the menus up
     *
     * @param buff Data array
     * @param offset Offset into the array
     */
    public void setBaseNetData(byte buff[], int offset) {
        int count = 0;
        //         bss.setFromNetData(buff, offset);
        /*
        while (count < bss.getNumItems()) {
            fed.feDrawStatusBase(count, bss.getAllianceNum(count));
            count++;
        }
         
        // Set our player name in the menu
        count = players.getSelf();
        fec.feSetPlayer(count, players.getPlayerName(count));
        // Set The other players in the menu
        players.setMenuItems();
         */
    }
    
    /**
     * Sets up the pill data from the network
     * Also refreshes the screen
     *
     * @param buff Data array
     * @param offset Offset into the array
     */
    public void setPillNetData(byte buff[], int offset) {
        int count = 0;
        
        pbs.setFromNetData(buff, offset);
        while (count < pbs.getNumItems()) {
            fed.feDrawStatusPillbox(count, pbs.getAllianceNum(count));
            count++;
        }
    }
    
    /**
     * A message has come in from the network
     *
     * @param playerNum Player the message come from
     * @param message The message text
     */
    public void incomingMessage(int playerNum, String message) {
        messages.add(playerNum, players.makeMessageName(playerNum), message);
    }
    
    public void netSetupTankGo() {
        int count;
        int count2;
        Start s;
        setInStartFind(true);
        
        s = sss.getStart(tank.getPlayerNum());
        tank.setWorld(Utils.GetWorldFromMap(s.getX()) + Utils.MAP_SQUARE_MIDDLE, Utils.GetWorldFromMap(s.getY()) + Utils.MAP_SQUARE_MIDDLE, s.getStartDir(), true);
        centerTank();
        setInStartFind(false);
        
/*        for (count=0;count<MAIN_BACK_BUFFER_SIZE_X;count++) {
            for (count2=0;count2<MAIN_BACK_BUFFER_SIZE_Y;count2++) {
                mine
                (*mineView).mineItem[count][count2] = FALSE;
            }
        } */
        
        reCalc();
        
    }
}

