<? class serverInfoClass {

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

var $key;
var $ip;
var $port;
var $mapName;
var $password;
var $mines;
var $ai;
var $gameType;
var $numBases;
var $numPills;
var $freeBases;
var $freePills;
var $numTanks;
var $startTime;
var $firstHeard;
var $lastHeard;
var $players;
var $version;
var $locked;
var $logAvailable;
var $logSize;
var $logRating;
var $logDownloads;
var $logNumRatings;

function setup($key, $ip, $port, $mapName, $password, $mines, $ai, $gameType, $numBases, $numPills, $freeBases, $freePills, $numTanks, $startTime, $firstHeard, $lastHeard, $players, $version, $locked) {
	$this->key = $key;
	$this->ip = $ip;
	$this->port = $port;
	$this->mapName = $mapName;
	$this->password = $password;
	$this->mines = $mines;
	$this->ai = $ai;
	$this->gameType= $gameType;
	$this->numBases = $numBases;
	$this->numPills = $numPills;
	$this->freeBases = $freeBases;
	$this->freePills = $freePills;
	$this->numTanks = $numTanks;
	$this->startTime = $startTime;
	$this->firstHeard = $firstHeard;
	$this->lastHeard = $lastHeard;
	$this->players = $players;

	if ($locked == 'T') {
		$this->locked = true;
	} else {
		$this->locked = false;
	}

	# Set version
	if ($version == "") {
		$version = "1.13";
	}
	$this->version = $version;
	$this->logAvailable = false;
}

function getKey() {
	return $this->key;
}

function getVersion() {
	return $this->version;
}

function getLocked() {
	return $this->locked;
}

function getIp() {
	return $this->ip;
}

function getPort() {
	return $this->port;
}

function getPassword() {
	if ($this->password == '1') {
		return "Yes";
	}
	return "No";
}

function getHiddenMines() {
	if ($this->mines== '1') {
		return "Yes";
	}
	return "No";
}

function getAi() {
	if ($this->ai == '1') {
		return "Yes";
	} else if ($this->ai == '2') {
		return "Yes, with an advantage";
	}
	return "No";
}

function getGameType() {
	if ($this->gameType == '1') {
		return "Open";
	} else if ($this->gameType == '2') {
		return "Tournament";
	}
	return "Strict Tournament";
}

function getNumPills() {
	return $this->numPills;
}

function getNumFreePills() {
	return $this->freePills;
}


function getNumPlayers() {
	return $this->numTanks;
}

function getNumFreeBases() {
	return $this->freeBases;
}

function getNumBases() {
	return $this->numBases;
}

function getMapName() {
	return $this->mapName;
}

function getPlayers() {
	return $this->players;
}

function getStartTime() {
	return $this->startTime;
}

function getFirstHeard() {
	return $this->firstHeard;
}

function getLastHeard() {
	return $this->lastHeard;
}

function setLogAvailable($val) {
	$this->logAvailable = false;
	if ($val == "Y") {
		$this->logAvailable = true;
	}
}

function getLogAvailable() {
	return $this->logAvailable;
}

function setLogSize($size) {
	if (!is_numeric($size)) {
		$size = 0;
	}
	$this->logSize = $size;
}

function getLogSize() {
	if ($this->logAvailable == false) {
		return "0K";
	}
	# Determine size in KB
	$size = floor($this->logSize / 1024);
	if ($size < 1) {
		$size = 1;
	}
	if ($size > 999) {
		$len = strlen($size);
		return substr($size, 0, $len-3) . "," . substr($size, $len-3,$len) . "K";
	}
	return $size . "K";
}

function setLogNumDownloads($num) {
	if (!is_numeric($num )) {
		$num = 0;
	}
	$this->logDownloads = $num;
}

function getLogNumDownloads() {
	return $this->logDownloads;
}

function setLogRating($num) {
	$this->logRating = $num;
}

function getLogRating() {
	return $this->logRating;
}

function setLogNumRatings($num) {
	$this->logNumRatings = $num;
}

function getLogNumRatings() {
	return $this->logNumRatings;
}

function prepareIPPortString() {
	return $this->getIP() . ":" . $this->getPort();
}

function writeRssEntryGamesCurrent() {
	echo "			<description>\n";
	echo "Type:    " . $this->getGameType() . "<br/>";
	if ($this->getLocked() == true) {
		echo "Locked:  " . "Yes" ."<br/>";	
	} else if ($this->getLocked() == false) {
		echo "Locked:  " . "No" . "<br/>";
	}
	echo "Version: " . $this->getVersion() . "<br/>";
	echo "Players: ";
	$players = $this->getPlayers();
	$playersCount = 0;
	if (sizeof($players) > 0) {
		while ($playersCount < sizeof($players)) {
			if ($playersCount != 0) {
				echo ", ";
			}
			#echo "<A href=\"playerinfo.php?name=" . urlencode($players[$playersCount]) . "\">" . $players[$playersCount] . "</A>";
			echo urlencode($players[$playersCount]);
			$playersCount++;
		}
		if (sizeof($players) != $this->getNumPlayers() && ($this->getNumPlayers() - sizeof($players) > 0)) {
			echo " + " . ($this->getNumPlayers() - sizeof($players)) . " non WinBolo.net participants";
		}
	}
	#Close up the players list
	echo "<br/>";
	echo "Neutral Bases: " . $this->getNumFreeBases() . "<br/>";
	echo "Neutral Pills: " . $this->getNumFreePills() . "<br/>";
	echo "			</description>\n";
}

}
?>
