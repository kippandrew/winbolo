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
	
class eventServerClass extends eventClass {
	var $ip;
	var $port;
	var $hostName;
	var $gameType;
	var $key;
	var $locked;
	var $version;

	function setVersion($version) {
		if ($version == "") {
			$this->version = "1.13";
		}
		$this->version = $version;
	}

	function getVersion() {
		return $this->version;
	}

	function setLocked($locked) {
		if ($locked == 'T') {
			$this->locked = true;
		} else {
			$this->locked = false;
		}
	}

	function getLocked() {
		return $this->locked;
	}

	function setIp($ip, $port, $hostName) {
		$this->ip = $ip;
		$this->port = $port;
		$this->hostName = $hostName;
	}

	function setKey($key) {
		$this->key = $key;
	}

	function setGameType($gameType) {
		$this->gameType = $gameType;
	}

	function getIp() {
		return $this->ip;
	}

	function getPort() {
		return $this->port;
	}

	function getKey() {
		return $this->key;
	}
	
	function getHostName() {
		if ($this->getHostName != null) {
			return $this->hostName;
		}
		return $this->ip;
	}

	function getGameType() {
        	if ($this->gameType == '1') {
	        	return "Open";
		} else if ($this->gameType == '2') {
			return "Tournament";
		}
		return "Strict Tournament";
	}

	function prepareHostString() {
		return $this->getHostName() . ":" . $this->port;
	}
}

?>
