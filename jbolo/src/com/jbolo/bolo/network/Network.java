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
 * Network.java
 *
 * Created on 14 June 2004, 03:34
 */

package com.jbolo.bolo.network;

import java.net.InetAddress;
import java.awt.Frame;
import com.jbolo.bolo.*;
import com.jbolo.winbolonet.WBN;
import com.jbolo.gui.NetClient;
import com.jbolo.gui.DNSLookups;
import com.jbolo.gui.DNSLookupItem;
import com.jbolo.gui.dialog.MessageBox;
import com.jbolo.gui.dialog.PasswordDialog;


/**
 * Network
 * @author John Morrison
 */
public class Network {
    
    /** Network game type - No network */
    public final static int NET_GAME_TYPE_NONE = 0;
    
    /** Network game type - Single Player Game */
    public final static int NET_GAME_TYPE_SINGLE = 1;
    
    /** Network game type - UDP Game */
    public final static int NET_GAME_TYPE_UDP = 2;
    
    /** Network state - Joining game */
    public final static int NET_STATE_JOINING = 0;
    
    /** Network state - Game running */
    public final static int NET_STATE_RUNNING = 1;
    
    /** Network state - Downloading starts  */
    private final static int NET_STATE_JOIN_START_DOWNLOAD = 2;
    
    /** Network state - Downloading bases */
    private final static int NET_STATE_JOIN_BASE_DOWNLOAD = 3;
    
    /** Network state - Downloading pillboxes */
    private final static int NET_STATE_JOIN_PILL_DOWNLOAD = 4;
    
    /** Network state - Downloading map (netYPos contains Y position up to downloading */
    private final static int NET_STATE_JOIN_MAP_DOWNLOAD = 5;
    
    /** Network state - Downloading start times */
    private final static int NET_STATE_JOIN_TIME_DOWNLOAD = 6;
    
    /** Network state - Failed */
    public final static int NET_STATE_FAILED = 7;
    
    
    /** Maximum retries for network things in joining game */
    private static int MAX_RETRIES  = 3;
    
    /** The type of game we are playing */
    private int networkGameType = NET_GAME_TYPE_NONE;
    /** Password in netgames */
    private String password = "";
    /** Network status */
    private int netStat = NET_STATE_RUNNING;
    /** Network status */
    private int oldNetStatus = NET_STATE_RUNNING;
    private int netFailedHigh = 0xFF;
    /** Y Position we want for data downloading */
    private int netYPos;
    
    /** Data for network status dialog box - Ring Delay */
    private int netRingDelay = 0;           /* Total Ring delay */
    /** Data for network status dialog box - Downstream time delay */
    private int netDownStreamDelay = 0;
    /** Data for network status dialog box - Number of packets per second */
    private int netPacketsPerSecond = 0;
    /** Data for network status dialog box - Number of network Errors */
    private int netNumErrors = 0;
    /** Data for network status dialog box - Number of network Retransmissions */
    private int netRetransmissions = 0;
    
    /** And Totals of for the last second */
    private int netTotPacketsPerSecond = 0;
    
    private long netLastTokenTime = 0;
    /** Time since last token */
    private long tknTime;
    /* Do we use the tracker or not */
    private boolean useTracker = false;
    /* Are we shutting down - Don't send packets */
    private boolean inNetShutdown;
    /** Last time we got a packet from the server */
    private long netLastHeard;
    
    /** Udp packets for our player */
    private UDPPackets udpp;
    
    /** Network time for CPU calculations */
    private int dwSysNetTotal = 0;
    /** Network time for CPU calculations */
    private int dwSysNet = 0;
    /** Network client transport layer object */
    private NetClient nc;
    /** DNS Lookups Object */
    private DNSLookups dns;
    /** GameWorld we belong to */
    private GameWorld gw;
    
    /** Info is generic byte array used to send data back to server. Private to prevent reinitalisation */
    private byte info[];
    
    
    /**
     * Creates a new instance of Network
     * @param gw GameWorld this network belongs to
     */
    public Network(GameWorld gw) {
        nc = null;
        dns = null;
        inNetShutdown = false;
        this.gw = gw;
        info = new byte[NetPacks.MAX_UDPPACKET_SIZE];
    }
    
    /**
     * Attempts to join a game. If it fails all network components are shutdown()
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
    public boolean setup(int netType, int myPort, String targetIp, int targetPort, String userName, String password, boolean usCreate, String trackerAddr, int trackerPort, boolean useTracker, boolean wantRejoin, boolean useWinboloNet, String wbnPassword) {
        boolean returnValue = true; // Value to return
        long startTime;
        
        inNetShutdown = false;
        netRetransmissions = 0;
        netStat = NET_STATE_RUNNING;
        startTime = 0;
        networkGameType = netType;
        this.password = password;
        udpp = new UDPPackets();
        
        
        // FIXME   Create join alliance window dlgAllianceWnd = CreateDialog(windowGetInstance(), MAKEINTRESOURCE(IDD_ALLIANCE), windowWnd(), dialogAllianceCallback);
        
        
        if (networkGameType == NET_GAME_TYPE_UDP) {
            nc = new NetClient(this, myPort);
            returnValue = nc.setup();
            netStat = NET_STATE_JOINING;
            dns = new DNSLookups(gw);
            dns.start();
            /* Try and set the tracker */
            this.useTracker = useTracker;
            if (returnValue == true && useTracker == true) {
                if (nc.setTracker(trackerAddr, trackerPort) == false) {
                    new MessageBox("NETERR_TRACKERDNS");
                }
                this.useTracker = false;
            }
            if (returnValue == true) {
                netYPos = 21;
                returnValue = join(targetIp, targetPort, wantRejoin, userName, wbnPassword, useWinboloNet, usCreate, password);
                if (returnValue == true) {
                    dns.start();
                    gw.netSetupTank(true);
                    netStat = NET_STATE_JOIN_START_DOWNLOAD;
                }
            }
        }
        if (returnValue == false) {
            shutdown();
        } else {
            if (this.networkGameType == NET_GAME_TYPE_UDP) {
                joinDataRequests();
                netLastHeard = System.currentTimeMillis();
            }
        }
        
        return returnValue;
    }
    
    /**
     * Shuts down networking. Shuts down internal packet driver, dns lookup thread etc
     */
    public void shutdown() {
        if (networkGameType == NET_GAME_TYPE_UDP) {
            inNetShutdown = true;
            sendQuitMessage();
            networkGameType = NET_GAME_TYPE_SINGLE;
            if (dns != null) {
                dns.stop();
            }
            dns = null;
            if (nc != null) {
                nc.shutdown();
            }
            nc = null;
            networkGameType = NET_GAME_TYPE_SINGLE;
            netStat = NET_STATE_RUNNING;
        }
    }
    
    public void check() {
        //FIXME
        nc.check();
    }
    
    public int getType() {
        return networkGameType;
    }
    
    public int getMapDownloadPosition() {
        return netYPos;
    }
    
    public int getStatus() {
        return netStat;
    }
    
    private int netSecond = 0;
    
    public void second() {
        dwSysNetTotal = dwSysNet;
        dwSysNet = 0;
        netTotPacketsPerSecond = netPacketsPerSecond;
        netPacketsPerSecond = 0;
        netSecond++;
        if (netSecond == 2 && networkGameType == NET_GAME_TYPE_UDP) {
            netSecond = 0;
            tknTime = System.currentTimeMillis();
            nc.sendServer(makePingResponse(), NetPacks.SIZE_PACKET_PING);
            netPacketsPerSecond++;
        }
    }
    
    private boolean inFix = false;
    
    
    /**
     * A packet has arrived. It is processed here
     *
     * @param buff Byte array of data that has arrived
     * @param len Length of the buffer
     * @param port Port this packet arrived on
     */
    public void packetArrive(byte buff[], int len, int port) {
        long tick;
        int sequenceNumber;
        
        tick = System.currentTimeMillis();
        makePacketHeader(info, (byte) 0);
        
        if (headerCorrect(buff, len) == false) {
            // error
            netNumErrors++;
            dwSysNet += System.currentTimeMillis() - tick;
            return;
        }
        
        if (len == NetPacks.SIZE_PACKET_PING && buff[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPACKET_PINGRESPONSE) {
            /* Ping packet */
            int pos;
            boolean okFix = true;
            
            netLastHeard = System.currentTimeMillis();
            if (buff[NetPacks.DATA_START] == gw.getPlayers().getSelf()) {
                if ((System.currentTimeMillis() - tknTime) >= 0) {
                    netRingDelay = (int) (System.currentTimeMillis() - tknTime);
                }
                /* Recovery Checks */
                pos = 1 + udpp.getOutSequenceNumber();
                if (pos == UDPPackets.MAX_UDP_SEQUENCE) {
                    pos = 0;
                }
                
                if (Utils.ConvertByteToInt(buff[NetPacks.SIZE_PACKET_PING-2]) != pos) {
                    /* Recover - Send server missing packets */
                    int upto;
                    int high;
                    upto = Utils.ConvertByteToInt(buff[NetPacks.SIZE_PACKET_PING-2]);
                    high = udpp.getOutSequenceNumber();
                    high++;
                    if (high == UDPPackets.MAX_UDP_SEQUENCE) {
                        high = 0;
                    }
                    okFix = false;
                }
                pos = 1 + Utils.ConvertByteToInt(buff[NetPacks.SIZE_PACKET_PING-1]);
                if (pos == UDPPackets.MAX_UDP_SEQUENCE) {
                    pos = 0;
                }
                if (udpp.getInSequenceNumber() != pos) {
                    /* Recover */
                    okFix = false;
                    if (netStat != NET_STATE_FAILED) {
                        oldNetStatus = netStat;
                        netStat = NET_STATE_FAILED;
                        makePacketHeader(info, NetPacks.BOLOPACKET_PACKETREREQUEST);
                        netFailedHigh = Utils.ConvertByteToInt(buff[NetPacks.SIZE_PACKET_PING-1]);
                        info[NetPacks.DATA_START+1] = (byte) UDPPackets.UDP_NON_RELIABLE_PACKET;
                        info[NetPacks.DATA_START] = (byte) udpp.getInSequenceNumber();
                        CRC.CRCCalcAppendArray(info, NetPacks.SIZE_PACKET_REREQUEST);
                        nc.sendServer(buff, NetPacks.SIZE_PACKET_REREQUEST+NetPacks.BOLO_PACKET_CRC_SIZE);
                    }
                }
            }
            if (okFix == true && netStat == NET_STATE_FAILED) {
                netStat = oldNetStatus;
            }
        } else if (len == NetPacks.BOLOPACKET_REQUEST_SIZE && buff[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPACKET_INFOREQUEST) {
            /* Make info response packet */
/*            INFO_PACKET h;
            memset(&h, 0, sizeof(h));
            netMakeInfoRespsonse(&h);
            memcpy(info, &h, sizeof(h));
            netClientSendUdpLast((BYTE *) info, sizeof(h)); */
            netPacketsPerSecond++;
        } else if (buff[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPOSITION_DATA) {
            /* It must be an position and stuff packet */
            /* Set last to be server */
            //            netDataPosPacket(buff, len, port);
        } else if (buff[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPACKET_PACKETREREQUEST && len == NetPacks.SIZE_PACKET_REREQUEST+NetPacks.BOLO_PACKET_CRC_SIZE) {
            if (CRC.CRCCheckArray(buff, len) == true) {
                int upto;
                int high;
                int count = 0;
                int us;
                upto = Utils.ConvertByteToInt(buff[NetPacks.BOLOPACKET_REQUEST_TYPEPOS+1]);
                high = udpp.getOutSequenceNumber();
                high++;
                len = NetPacks.BOLOPACKET_REQUEST_TYPEPOS+2;
                if (high == UDPPackets.MAX_UDP_SEQUENCE) {
                    high = 0;
                }
                info[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] = NetPacks.BOLOPACKET_RETRANSMITTED_PACKETS;
                while (upto != high && len < 1000) {
                    if (len + udpp.getOutBuffLength(upto) > 1000) {
                        upto = high;
                    } else {
                        /* Add it to our packet */
                        us = udpp.getOutBuffLength(upto);
                        Utils.AddShortToArray(info, us, len);
                        len += 2;
                        Utils.MemCopy(udpp.getOutBuff(upto), info, 0, len);
                        len += us;
                        upto++;
                        if (upto == UDPPackets.MAX_UDP_SEQUENCE) {
                            upto = 0;
                        }
                        count++;
                    }
                }
                info[NetPacks.BOLOPACKET_REQUEST_TYPEPOS+1] = (byte) count;
                CRC.CRCCalcAppendArray(info, len);
                nc.sendServer(info, len+NetPacks.CRC_SIZE);
            }
        } else if (buff[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPACKET_RETRANSMITTED_PACKETS) {
            int count;
            int us;
            int pos;
            byte data[] = new byte[NetPacks.MAX_UDPPACKET_SIZE];
            
            if (netStat == NET_STATE_FAILED) {
                if (CRC.CRCCheckArray(buff, len) == true) {
                    int numPacks = Utils.ConvertByteToInt(buff[NetPacks.BOLOPACKET_REQUEST_TYPEPOS+1]);
                    count = 0;
                    pos = NetPacks.BOLOPACKET_REQUEST_TYPEPOS+2;
                    inFix = true;
                    while (count < numPacks) {
                        us = Utils.GetShortFromArray(buff, pos);
                        pos += 2;
                        Utils.MemCopy(buff, data, us, pos);
                        packetArrive(data, us, 0);
                        pos += us;
                        count++;
                    }
                    inFix = false;
                } else {
                    System.err.println("CRC broken");
                }
            }
        } else {
            netLastHeard = System.currentTimeMillis();
            if (CRC.CRCCheckArray(buff, len) == true) {
                len -= 2;
                sequenceNumber = Utils.ConvertByteToInt(buff[len-1]);
                if (netStat != NET_STATE_FAILED) {
                    oldNetStatus = netStat;
                }
                if (sequenceNumber != udpp.getInSequenceNumber()) {
                    netStat = NET_STATE_FAILED;
                    netNumErrors++;
                    netFailedHigh = sequenceNumber;
                    if (inFix == false && (sequenceNumber > udpp.getInSequenceNumber() || (udpp.getInSequenceNumber() > 180 && (udpp.getInSequenceNumber() - sequenceNumber > 10)))) {
                        /* We want to ignore resends if the packet reappears */
                        makePacketHeader(info, NetPacks.BOLOPACKET_PACKETREREQUEST);
                        info[NetPacks.DATA_START+1] = (byte) UDPPackets.UDP_NON_RELIABLE_PACKET;
                        info[NetPacks.DATA_START] = (byte) udpp.getInSequenceNumber();
                        CRC.CRCCalcAppendArray(info, NetPacks.SIZE_PACKET_REREQUEST);
                        nc.sendServer(info, NetPacks.SIZE_PACKET_REREQUEST+NetPacks.CRC_SIZE);
                    }
                } else {
                    if (netStat == NET_STATE_FAILED) {
                        if (netFailedHigh == sequenceNumber) {
                            /* Back up to speed */
                            netStat = oldNetStatus;
                        } else if (inFix == false) {
                            /* Request Next */
                            makePacketHeader(info, NetPacks.BOLOPACKET_PACKETREREQUEST);
                            info[NetPacks.DATA_START+1] = (byte) UDPPackets.UDP_NON_RELIABLE_PACKET;
                            info[NetPacks.DATA_START] = (byte) udpp.getInSequenceNumber();
                            CRC.CRCCalcAppendArray(info, NetPacks.SIZE_PACKET_REREQUEST);
                            nc.sendServer(info, NetPacks.SIZE_PACKET_REREQUEST+NetPacks.CRC_SIZE);
                        }
                    }
                    udpp.getNextInSequenceNumber();
                    processPacket(buff, len-1);
                }
            }
        }
        dwSysNet += System.currentTimeMillis() - tick;
    }
    
    /**
     * A reliable packet has arrived. Its been verified as in sequence and CRC valid
     * Process it here
     *
     * @param buff Data buffer containing data
     * @param len Length of the data
     */
    private void processPacket(byte buff[], int len) {
        System.err.println("Received length = " + len + ", type = " + buff[NetPacks.BOLOPACKET_REQUEST_TYPEPOS]);
        if (len == (NetPacks.BOLOPACKET_REQUEST_SIZE + Bases.SIZEOF_BASES) && buff[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPACKET_BASESDATARESPONSE) {
            /* Here is the bases data */
            gw.setBaseNetData(buff, NetPacks.DATA_START);
            netStat = NET_STATE_JOIN_PILL_DOWNLOAD;
            netPacketsPerSecond++;
            joinDataRequests();
        } else if (len == (NetPacks.BOLOPACKET_REQUEST_SIZE + Starts.SIZEOF_STARTS) && buff[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPACKET_STARTSDATARESPONSE) {
            /* Here is the Starts data */
            gw.getStarts().setFromNetData(buff, NetPacks.DATA_START);
            netStat = NET_STATE_JOIN_BASE_DOWNLOAD;
            netPacketsPerSecond++;
            joinDataRequests();
        } else if (len == (NetPacks.BOLOPACKET_REQUEST_SIZE + Pillboxes.SIZEOF_PILLS) && buff[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPACKET_PILLSDATARESPONSE) {
            /* Here is the Pillbox data */
            gw.setPillNetData(buff, NetPacks.DATA_START);
            netStat = NET_STATE_JOIN_MAP_DOWNLOAD;
            netPacketsPerSecond++;
            joinDataRequests();
        } else if (buff[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPACKET_MAPDATARESPONSE && Utils.ConvertByteToInt(buff[NetPacks.BOLOPACKET_MAPDDOWNLOAD_YPOS]) == netYPos) {
            /* Here is the map download data */
            int dataLen;
            dataLen = len - 3 - 8;
            gw.getBoloMap().setMapNetRun(buff, NetPacks.DATA_START+1+2, netYPos, dataLen);
            netYPos +=5;
            if (netYPos == 241) {
                netStat = NET_STATE_JOIN_TIME_DOWNLOAD;
            }
            netPacketsPerSecond++;
            joinDataRequests();
        } else if (buff[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPACKET_MESSAGE || buff[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPACKET_MESSAGE_ALL_PLAYERS) {
            /* A Message packet */
            processMessagePacket(buff, len);
        } else if (len ==  NetPacks.SIZE_PACKET_TIME && buff[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPACKET_TIMERESPONSE) {
            /* Here is the time data */
            //FIXME            gw.setTimeLengths(startDelay, timeLimit);
            //            screenSetTimeLengths(tp.start_delay, tp.time_limit);
            netStat = NET_STATE_RUNNING;
            gw.netSetupTankGo();
        }
    }
    
    
    private void processMessagePacket(byte buff[], int len) {
        String msg = Utils.PByteArrayToString(buff, NetPacks.DATA_START+2);
        gw.incomingMessage(buff[NetPacks.DATA_START], Utils.PByteArrayToString(buff, NetPacks.DATA_START+2));
    }
    
    /**
     * Join initialisation. Attempts to find the game, validate password
     * if required, check player name availability.
     *
     * @param userName Player name to join with
     * @param ip Target IP Address to look for game
     * @param ip Target port to look for game
     * @param gamePassword Game password if we started the game
     * @param usCreate Did we start the server ourselves (if se we already know the password)
     * @return success
     */
    private boolean joinInit(String userName, String ip, int port, boolean usCreate, String gamePassword) {
        boolean returnValue = true; // Value to return
        byte data[] = new byte[NetPacks.MAX_UDPPACKET_SIZE];
        byte sendBuff[] = new byte[NetPacks.MAX_UDPPACKET_SIZE];
        int dataLen;
        InfoResponse ir = null;
        String joinPassword;
        byte joinPasswordBytes[] = null;
        int numTries;
        int packetLen;
        
        Utils.MemCopy(NetPacks.INFOREQUESTHEADER, data, NetPacks.INFOREQUESTHEADER.length);
        dataLen = nc.ping(ip, port, data, NetPacks.INFOREQUESTHEADER.length, false, false);
        try {
            ir = new InfoResponse(data, dataLen);
        } catch (Exception e) {
            new MessageBox("123 error");
            returnValue = false;
        }
        
        /* Check there is an availble player slot */
        if (returnValue == true && ir != null) {
            if (ir.getNumPlayers() == Players.MAX_PLAYERS) {
                returnValue = false;
                new MessageBox("NETERR_GAMEFULL");
            } else {
                /* Extract map name, AI Type and hidden mines and game start and originator */
                gw.setMapName(ir.getMapName());
                gw.setAIType(ir.getAI());
                gw.setAllowHiddenMines(ir.getHiddenMines());
                gw.setGameType(ir.getGameType());
                gw.setTimeGameCreated(ir.getStartTime());
                
                if (ir.getAddress().equals("0.0.0.0") == true) {
                    /* Last packet must be from server. */
                    nc.setServerAddress(nc.getLastAddress(), ir.getPort());
                }
                if (ir.getAddress().equals("127.0.0.1") == true) {
                    /* Set localhost address to our IP address */
                    try {
                        nc.setServerAddress(InetAddress.getLocalHost().getAddress(), ir.getPort());
                    } catch (Exception e) {
                        // Wont throw this as its calling get localhost and we have allready sent packets. But if we do we will fail
                        returnValue = false;
                    }
                }
            }
        }
        
        // Password if required
        if (returnValue == true) {
            if (ir.hasPassword() == true) {
                if (usCreate == false) {
                    joinPassword = new PasswordDialog(new Frame(), true).getPassword();
                } else {
                    // We created the game, password in gamePassword
                    joinPassword = gamePassword;
                }
                
                if (joinPassword == null) {
                    returnValue = false;
                } else {
                    joinPasswordBytes = Utils.StringToPByteArray(joinPassword);
                    this.password = joinPassword;
                }
                
                if (returnValue == true) {
                    makePacketHeader(sendBuff, NetPacks.BOLOPACKET_PASSWORDCHECK);
                    Utils.MemCopy(joinPasswordBytes, sendBuff, joinPasswordBytes.length, 0, NetPacks.DATA_START);
                    
                    numTries = 0;
                    Utils.MemCopy(sendBuff, data, NetPacks.SIZE_PASSWORD_PACKET);
                    packetLen = nc.ping(nc.getServerAddressString(), nc.getServerPort(), data, NetPacks.SIZE_PASSWORD_PACKET, true, true);
                    while (returnValue == true && numTries < MAX_RETRIES)
                        if (packetLen == NetPacks.SIZE_PACKETRESPONSE_WITHCRC && headerCorrect(data, packetLen) == true) {
                            if (data[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPACKET_PASSWORDACCEPT) {
                                numTries = MAX_RETRIES;
                            } else {
                                numTries = MAX_RETRIES;
                                returnValue = false;
                                new MessageBox("NETERR_PASSWORDWRONG");
                            }
                        }
                    numTries++;
                    if (numTries < MAX_RETRIES) {
                        Utils.MemCopy(sendBuff, data, NetPacks.SIZE_PASSWORD_PACKET);
                        packetLen = nc.ping(nc.getServerAddressString(), nc.getServerPort(), data, NetPacks.SIZE_PASSWORD_PACKET, true, true);
                        
                    }
                }
            }
        }
        
        // Check player name availabilty
        if (returnValue == true) {
            makePacketHeader(sendBuff, NetPacks.BOLOPACKET_NAMECHECK);
            Utils.StringToPByteArrayExisting(userName, sendBuff, NetPacks.DATA_START);
            
            numTries = 0;
            Utils.MemCopy(sendBuff, data, NetPacks.SIZE_NAMECHECK_PACKET);
            packetLen = nc.ping(nc.getServerAddressString(), nc.getServerPort(), data, NetPacks.SIZE_NAMECHECK_PACKET, true, true);
            while (numTries < MAX_RETRIES) {
                if (packetLen == NetPacks.SIZE_PACKETRESPONSE_WITHCRC && headerCorrect(data, packetLen) == true) {
                    if (data[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPACKET_NAMEACCEPT) {
                        // All good
                    } else if (data[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPACKET_NAMEFAIL) {
                        new MessageBox("NETERR_PLAYERNAMEINUSE");
                        returnValue = false;
                    } else {
                        new MessageBox("Sorry, the server player limit has been reached");
                        returnValue = false;
                    }
                    numTries = MAX_RETRIES;
                }
                
                if (numTries < MAX_RETRIES) {
                    Utils.MemCopy(sendBuff, data, NetPacks.SIZE_NAMECHECK_PACKET);
                    packetLen = nc.ping(nc.getServerAddressString(), nc.getServerPort(), data, NetPacks.SIZE_NAMECHECK_PACKET, true, true);
                }
            }
            
        }
        
        return returnValue;
    }
    
    /**
     * Join game finalisation. Gets our player number, tries rejoining
     * if required and gets the details of all other players in the game
     *
     * @param userName Player name to join the game with
     * @param password WBN Password
     * @param wantRejoin Do we want to attempt to rejoin the game
     * @param useWinboloNet Do we want to use WBN
     * @return success
     */
    private boolean joinFinalise(boolean wantRejoin, String userName, String password, boolean useWbn) {
        boolean returnValue = true; // Value to return
        int playerNum;
        byte data[] = new byte[NetPacks.MAX_UDPPACKET_SIZE];
        byte sendBuff[] = new byte[NetPacks.MAX_UDPPACKET_SIZE];
        int dataLen;
        int numTries;
        int packetLen;
        int count = 0;
        boolean finished = false;
        
        
        if (useWbn == true) {
            //FIXME            returnValue = wbnSetup(userName, password);
        }
        
        // Player Number request
        if (returnValue == true) {
            int test = 1;
            makePacketHeader(sendBuff, NetPacks.BOLOPACKET_PLAYERNUMREQUEST);
            // Next two bytes are port
            // Then userName
            Utils.StringToPByteArrayExisting(userName, sendBuff, NetPacks.DATA_START + 1);
            // Then WBN Key
            sendBuff[8+3+33] = 0;
            sendBuff[8+3+32] = 0;
            sendBuff[8+3+34] = 0;
            // Then password
            Utils.StringToPByteArrayExisting(password, sendBuff, (test + NetPacks.DATA_START + 2 + WBN.WINBOLONET_KEY_LEN + NetPacks.PLAYER_NAME_LEN));
            numTries = 0;
            Utils.MemCopy(sendBuff, data, NetPacks.SIZE_PACKET_PLAYERNUM_REQ);
            packetLen = nc.ping(nc.getServerAddressString(), nc.getServerPort(), data, test + NetPacks.SIZE_PACKET_PLAYERNUM_REQ, true, true);
            while (numTries < MAX_RETRIES && returnValue == true) {
                if (packetLen == NetPacks.SIZE_PACKET_PLAYERNUMRESPONSE && headerCorrect(data, packetLen) == true) {
                    if (data[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPACKET_PLAYERNUMRESPONSE && data[NetPacks.BOLOPACKET_PLAYERNUMPOS] < Players.MAX_PLAYERS && CRC.CRCCheckArray(data, packetLen) == true) {
                        if (WBN.GetInstance().isRunning() == true && useWbn == true) {
                            userName = "*" + userName;
                            //FIXME                            windowAllowPlayerNameChange(FALSE);
                        }
                        finished = true;
                        gw.getPlayers().setSelf(data[NetPacks.BOLOPACKET_PLAYERNUMPOS], userName);
                    } else {
                        returnValue = false;
                    }
                    numTries = MAX_RETRIES;
                }
                
                if (numTries < MAX_RETRIES) {
                    Utils.MemCopy(sendBuff, data, NetPacks.SIZE_PACKET_PLAYERNUM_REQ);
                    packetLen = nc.ping(nc.getServerAddressString(), nc.getServerPort(), data, test + NetPacks.SIZE_PACKET_PLAYERNUM_REQ, true, true);
                } else if (finished == false) {
                    returnValue = false;
                    new MessageBox("NETERR_CONNECTNOJOIN");
                }
            }
        }
        
        /* Rejoin if required */
        if (returnValue == true) {
            if (wantRejoin == true) {
                makePacketHeader(sendBuff, NetPacks.BOLOREJOINREQUEST);
                sendBuff[NetPacks.DATA_START] = data[NetPacks.BOLOPACKET_PLAYERNUMPOS];
                data[NetPacks.DATA_START] = (byte) UDPPackets.UDP_NON_RELIABLE_PACKET;
                CRC.CRCCalcAppendArray(sendBuff, NetPacks.DATA_START+1);
                nc.sendServer(sendBuff, NetPacks.DATA_START+1 + NetPacks.BOLO_PACKET_CRC_SIZE);
            }
        }
        
        /* Point of no return */
        if (returnValue == true) {
            /* Get each players details */
            makePacketHeader(sendBuff, NetPacks.BOLOPACKET_PLAYERDATAREQUEST);
            Utils.MemCopy(sendBuff, data, NetPacks.SIZE_PACKET_REQUEST+1);
            numTries = 0;
            packetLen = nc.ping(nc.getServerAddressString(), nc.getServerPort(), data, NetPacks.SIZE_PACKET_REQUEST+1, true, true);
            while (returnValue == true && numTries < MAX_RETRIES) {
                if (returnValue == true && packetLen == NetPacks.SIZE_PLAYERDATA_PACKET && headerCorrect(data, packetLen) == true && data[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] == NetPacks.BOLOPACKET_PLAYERDATARESPONSE && CRC.CRCCheckArray(data, packetLen) == true) {
                    /* Get the info from it */
                    String name;
                    String ip;
                    int playerOffset = NetPacks.DATA_START;
                    byte allies[] = new byte[Players.MAX_PLAYERS];
                    int numAllies;
                    int count2;
                    
                    for (count=0;count<Players.MAX_PLAYERS;count++) {
                        if (Utils.ByteToBoolean(data[playerOffset]) == true) {
                            name = Utils.PByteArrayToString(data, playerOffset+1);
                            numAllies = data[playerOffset+1+NetPacks.PLAYER_NAME_LEN];
                            for (count2=0;count2<numAllies;count2++) {
                                allies[count2] = data[playerOffset+1+NetPacks.PLAYER_NAME_LEN+1+count2];
                            }
                            ip = Utils.ConvertByteToInt(data[playerOffset+1+NetPacks.PLAYER_NAME_LEN+1+Players.MAX_PLAYERS+1]) + "." +
                            Utils.ConvertByteToInt(data[playerOffset+1+NetPacks.PLAYER_NAME_LEN+1+Players.MAX_PLAYERS+2]) + "." +
                            Utils.ConvertByteToInt(data[playerOffset+1+NetPacks.PLAYER_NAME_LEN+1+Players.MAX_PLAYERS+3]) + "." +
                            Utils.ConvertByteToInt(data[playerOffset+1+NetPacks.PLAYER_NAME_LEN+1+Players.MAX_PLAYERS+4]);
                            gw.getPlayers().setPlayer(count, name, ip, 0,0, 0, 0, 0, false, numAllies, allies);
                            dns.addRequest(new DNSLookupItem(ip, count));
                            /* One day add stuff to do with each players address */
                        }
                        playerOffset += 60;
                    }
                    numTries = MAX_RETRIES;
                } else if (returnValue == true) {
                    numTries++;
                    if (numTries < MAX_RETRIES) {
                        Utils.MemCopy(sendBuff, data, NetPacks.SIZE_PACKET_REQUEST+1);
                        packetLen = nc.ping(nc.getServerAddressString(), nc.getServerPort(), data, NetPacks.SIZE_PACKET_REQUEST+1, true, true);
                    }
                }
            }
        }
        
        
        return returnValue;
    }
    
    /**
     * Attempts to join a game
     * @param ip Target IP Address to look for game
     * @param port Target port to look for game
     * @param password WBN Password
     * @param usCreate Did we start the server ourselves (if se we already know the password)
     * @param wantRejoin Do we want to attempt to rejoin the game
     * @param useWBN Do we want to use WBN
     * @param gamePassword Password to join game with if usCreate is true
     * @return success
     */
    private boolean join(String ip, int port, boolean wantRejoin, String userName, String password, boolean useWbn, boolean usCreate, String gamePassword) {
        boolean returnValue; /* Value to return */
        
        returnValue = joinInit(userName, ip, port, usCreate, gamePassword);
        if (returnValue == true) {
            returnValue = joinFinalise(wantRejoin, userName, password, useWbn);
        }
        return returnValue;
    }
    
    /**
     * Writes a packet header into a byte array
     *
     * @param data Byte array to write into
     * @param packetType type of packet to write
     */
    private void makePacketHeader(byte data[], byte packetType) {
        Utils.MemCopy(NetPacks.HEADER, data, NetPacks.HEADER.length);
        data[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] = packetType;
    }
    
    /**
     * Checks to see if the header is correct
     *
     * @return if the header is valid
     * @param data Byte array to check
     * @param length Length of the data in the array
     */
    private boolean headerCorrect(byte data[], int length) {
        if (length < NetPacks.HEADER.length) {
            return false;
        }
        return Utils.CompareArrays(NetPacks.HEADER, data, NetPacks.HEADER.length);
    }
    
    /**
     * Sends a packet to the server using reliable packets
     *
     * @param data Date to send - Array must be atleast 3 bigger then length
     * @param len Length of the data to send
     */
    private void netSend(byte data[], int len) {
        data[len] = (byte) udpp.getNextOutSequenceNumber();
        len++;
        CRC.CRCCalcAppendArray(data, len);
        len += NetPacks.BOLO_PACKET_CRC_SIZE;
        nc.sendServer(data, len);
    }
    
    /**
     * Requests some join data to be downloaded
     */
    private void joinDataRequests() {
        byte data[] = new byte[512];
        int joinReq;
        makePacketHeader(data, (byte) 0);
        
        joinReq = netStat;
        if (netStat == NET_STATE_FAILED) {
            joinReq = oldNetStatus;
        }
        switch (joinReq) {
            case NET_STATE_JOIN_START_DOWNLOAD:
                data[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] = NetPacks.BOLOPACKET_STARTSDATAREQUEST;
                netSend(data, NetPacks.BOLOPACKET_REQUEST_SIZE);
                netPacketsPerSecond++;
                break;
            case NET_STATE_JOIN_BASE_DOWNLOAD:
                data[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] = NetPacks.BOLOPACKET_BASESDATAREQUEST;
                netSend(data, NetPacks.BOLOPACKET_REQUEST_SIZE);
                netPacketsPerSecond++;
                break;
            case NET_STATE_JOIN_PILL_DOWNLOAD:
                data[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] = NetPacks.BOLOPACKET_PILLSDATAREQUEST;
                netSend(data, NetPacks.BOLOPACKET_REQUEST_SIZE);
                netPacketsPerSecond++;
                break;
            case NET_STATE_JOIN_MAP_DOWNLOAD:
                data[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] = NetPacks.BOLOPACKET_MAPDATAREQUEST;
                data[NetPacks.BOLOPACKET_MAPDDOWNLOAD_YPOS] = (byte) netYPos;
                netSend(data, NetPacks.BOLOPACKET_REQUEST_SIZE+1);
                netPacketsPerSecond++;
                break;
            case NET_STATE_JOIN_TIME_DOWNLOAD:
                data[NetPacks.BOLOPACKET_REQUEST_TYPEPOS] = NetPacks.BOLOPACKET_TIMEREQUEST;
                netSend(data, NetPacks.BOLOPACKET_REQUEST_SIZE);
                netPacketsPerSecond++;
                break;
            default:
                break;
        }
    }
    
    /**
     * Sends a quit message to the server
     */
    private void sendQuitMessage() {
        if (netStat != NET_STATE_JOINING) {
            // Only send if we really are in the game
            byte buff[] = new byte[64];
            makePacketHeader(buff, NetPacks.BOLOPACKET_PACKETQUIT);
            netSend(buff, NetPacks.SIZE_PACKET_REQUEST);
        }
    }
    
    private byte[] makePingResponse() {
        byte data[] = new byte[NetPacks.SIZE_PACKET_PING];
        makePacketHeader(data, NetPacks.BOLOPACKET_PINGREQUEST);
        data[NetPacks.DATA_START] = (byte) gw.getPlayers().getSelf();
        Utils.MemSet(data, (byte) 0, 9, NetPacks.DATA_START+1);
        data[NetPacks.SIZE_PACKET_PING-2] = (byte) udpp.getInSequenceNumber();
        data[NetPacks.SIZE_PACKET_PING-1] = (byte) udpp.getOutSequenceNumber();
        return data;
    }
    
    private void processDataPosPacket(byte data[], int len, int port) {
        int sectionType;
        int sectionLen;
        int pos;
        
        if (CRC.CRCCheckArray(data, len) == false) {
            netNumErrors++;
        } else {
            /* Process player location data */
            nc.setServerPort(port);
            len -= NetPacks.CRC_SIZE;
            pos = NetPacks.DATA_START-1;
            while (pos < len) {
                sectionType = data[pos];
                pos++;
                sectionLen = data[pos];
                pos++;
                switch (sectionType) {
                    case NetPacks.BOLOPACKET_MAND_DATA:
//                        screenExtractPlayerData(ptr, sectionLen);
                        break;
                    case NetPacks.BOLO_PACKET_PNBDATA:
//                        screenExtractPNBData(ptr, sectionLen, FALSE);
                        break;
                    case NetPacks.BOLO_PACKET_MNTDATA:
//                        screenExtractMNTData(ptr, sectionLen, FALSE);
                        break;
                    case NetPacks.BOLO_PACKET_SHELLDATA:
                        System.err.println("Shells");
//                        screenExtractShellData(ptr, sectionLen);
                        break;
                    default:
                        break;
                        
                }
                
                
            }
            
        }
    }
}
