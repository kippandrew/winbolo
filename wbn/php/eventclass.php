<?
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
	
class eventClass {
var $eventType;
var $timestamp;
var $playera;
var $playerb;

function setup($eventType, $timestamp, $playera, $playerb) {
	$this->eventType= $eventType;
	$this->timestamp = $timestamp;
	$this->playera = $playera;
	$this->playerb = $playerb;
}

function getEventType() {
	return $this->eventType;
}

function getTimestamp() {
	return $this->timestamp;
}

function getPlayerA() {
	return $this->playera;
}

function getPlayerB() {
	return $this->playerb;
}

function prepareName($name, $makeUrl) {
        if ($name == null) {
                return "Non Winbolo.net participant";
        }
	if ($makeUrl == true) {
        	return "<a href=\"playerinfo.php?name=" . urlencode($name) . "\"> " . $name . "</a>";
	} else {
		return $name;
	}
}



function writeRssEntry() {
#echo " ".  $this->eventType . ": A=" . $this->playera . " - B=" . $this->playerb . "---->";
	echo "		<description>" . date("G:i:s", $this->getTimestamp()) . ": ";
	switch ($this->eventType) {
		case WINBOLO_NET_EVENT_ALLY_JOIN:
		echo $this->prepareName($this->playera, false) . " just joined alliance with " . $this->prepareName($this->playerb, false) . ".";
		break;
		case WINBOLO_NET_EVENT_ALLY_LEAVE:
		echo $this->prepareName($this->playera, false) . " just left alliance.";
		break;
		case WINBOLO_NET_EVENT_PILL_CAPTURE:
		echo $this->prepareName($this->playera, false) . " just captured a neutral pillbox.";
		break;
		case WINBOLO_NET_EVENT_BASE_CAPTURE:
		echo $this->prepareName($this->playera, false) . " just captured a neutral base.";

		break;
		case WINBOLO_NET_EVENT_TANK_KILL:
		echo $this->prepareName($this->playera, false) . " just killed " . $this->prepareName($this->playerb, false) . ".";

		break;
		case WINBOLO_NET_EVENT_PLAYER_LEAVE:
		echo $this->prepareName($this->playera, false) . " just left game.";
		break;
		case WINBOLO_NET_EVENT_PLAYER_JOIN:
		echo $this->prepareName($this->playera, false) . " just joined game.";
		break;
		case WINBOLO_NET_EVENT_LGM_KILL:
		echo $this->prepareName($this->playera, true) . " just killed " . $this->prepareName($this->playerb, true) . "'s builder.";
		break;
		case WINBOLO_NET_EVENT_LGM_LOST:
		echo $this->prepareName($this->playera, false) . " just lost their builder.";
		break;
		case WINBOLO_NET_EVENT_BASE_STEAL:
		echo $this->prepareName($this->playera, false) . " just stole base from " . $this->prepareName($this->playerb, false) .   ".";
		break;
		case WINBOLO_NET_EVENT_PILL_STEAL:
		echo $this->prepareName($this->playera, false) . " just stole pillbox from " . $this->prepareName($this->playerb, false) .   ".";
		break;
		case WINBOLO_NET_EVENT_REJOIN:
		echo $this->prepareName($this->playera, false) . " just rejoined game.";
		break;
		case WINBOLO_NET_EVENT_QUITTING:
		echo $this->prepareName($this->playera, false) . " just quit game.";
		break;
		default:
		echo "  - $thisType";
		break;
	} 
	echo "</description>\n";
}



function writeEntry() {
#echo " ".  $this->eventType . ": A=" . $this->playera . " - B=" . $this->playerb . "---->";
	switch ($this->eventType) {
		case WINBOLO_NET_EVENT_ALLY_JOIN:
		echo "<img src=\"/images/join.gif\"> " . $this->prepareName($this->playera, true) . " just joined alliance with " . $this->prepareName($this->playerb, true) . ".";
		break;
		case WINBOLO_NET_EVENT_ALLY_LEAVE:
		echo "<img src=\"/images/leave.gif\"> " . $this->prepareName($this->playera, true) . " just left alliance.";
		break;
		case WINBOLO_NET_EVENT_PILL_CAPTURE:
		echo "<img src=\"/images/pill.gif\"> " . $this->prepareName($this->playera, true) . " just captured a neutral pillbox.";
		break;
		case WINBOLO_NET_EVENT_BASE_CAPTURE:
		echo  "<img src=\"/images/base.gif\"> " . $this->prepareName($this->playera, true) . " just captured a neutral base.";

		break;
		case WINBOLO_NET_EVENT_TANK_KILL:
		echo "<img src=\"/images/tank.gif\"> " . $this->prepareName($this->playera, true) . " just killed " . $this->prepareName($this->playerb, true) . ".";

		break;
		case WINBOLO_NET_EVENT_PLAYER_LEAVE:
		echo "<img src=\"/images/tank.gif\"> " . $this->prepareName($this->playera, true) . " just left game.";
		break;
		case WINBOLO_NET_EVENT_PLAYER_JOIN:
		echo "<img src=\"/images/tank.gif\"> " . $this->prepareName($this->playera, true) . " just joined game.";
		break;
		case WINBOLO_NET_EVENT_LGM_KILL:
		echo "<img src=\"/images/lgm-killed.gif\"> " . $this->prepareName($this->playera, true) . " just killed " . $this->prepareName($this->playerb, true) . "'s builder.";
		break;
		case WINBOLO_NET_EVENT_LGM_LOST:
		echo "<img src=\"/images/lgm.gif\"> " . $this->prepareName($this->playera, true) . " just lost their builder.";
		break;
		case WINBOLO_NET_EVENT_BASE_STEAL:
		echo  "<img src=\"/images/base.gif\"> " . $this->prepareName($this->playera, true) . " just stole base from " . $this->prepareName($this->playerb, true) .   ".";
		break;
		case WINBOLO_NET_EVENT_PILL_STEAL:
		echo  "<img src=\"/images/pill.gif\"> " . $this->prepareName($this->playera, true) . " just stole pillbox from " . $this->prepareName($this->playerb, true) .   ".";
		break;
		case WINBOLO_NET_EVENT_REJOIN:
		echo "<img src=\"/images/tank.gif\"> " . $this->prepareName($this->playera, true) . " just rejoined game.";
		break;
		case WINBOLO_NET_EVENT_QUITTING:
		echo "<img src=\"/images/tank.gif\"> " . $this->prepareName($this->playera, true) . " just quit game.";
		break;

		default:
		echo "  - " . $this->eventType .  " basd";
		break;
	} 
}


}

?>
