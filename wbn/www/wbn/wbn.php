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


require "wbnheader.php";
require "wbn-gameend.php";
$link = null;
switch ($messageType) {
	case 1:
  	/* Version Request */
		echo chr(1);
		echo chr($cfg_wbnversion_major);
		echo chr($cfg_wbnversion_minor);
		echo chr($cfg_wbnversion_revision);
		echo $cfg_wbn_motd;
		break;
	case 2:
	/* Server requests session key */
		$key = processServerKeyRequest($data);
		echo chr(1);
		echo $key;
		break;	
	case 3:
	/* Client requests a session key */
		$key = processClientKeyRequest($data);
		echo chr(1);
		echo $key;
		break;
	case 4:
	/* Server verify a client key */
		verifyClientKey($data);
		/* If we got here that is good. Send back ok response */
		echo chr(1);
		echo chr(1);
		break;
	case 5:
	/* Client has left this server game */
		processClientLeaveGame($data);
		echo chr(1);
		echo chr(1);
		break;
	case 6:
	/* Server has quit */
		processServerQuit($data);
		echo chr(1);
		echo chr(1);
		break;
	case 7:
		processServerUpdate($data);
		echo chr(1);
		echo chr(1);
		break;
	case 8:
		processTeams($data);
		break;
	case 9:
		processVersion($data);
		break;
	case 10:
		processLockMessage($data);
		break;
	default:
		writeError("Bad Request");
}

#############################################################################
# FUNCTION:     processLockMessage
# AUTHOR:       John Morrison
# DATE:         02/04/06
# RETURNS:
# PURPOSE:
# Processes a server lock/unlock message
#
# PARAMETERS:
# data - Input from server
#############################################################################
function processLockMessage($data) {
	$key = substr($data, 0, 32);
	$locked = ord($data[32]);
	if ($locked == 1) {
		$locked = 'T';
	} else {
		$locked = 'F';
	}
	$sqlKey = mysql_escape_string($key);
	openDatabase(true);
	$sql = "update game_server set gs_locked = '$locked' where gs_key = '$sqlKey'";
	mysql_query($sql);
	closeDatabase();
}

#############################################################################
# FUNCTION:     processVersion
# AUTHOR:       John Morrison
# DATE:         02/04/06
# RETURNS:
# PURPOSE:
# Processes a server version message
#
# PARAMETERS:
# data - Input from server
#############################################################################
function processVersion($data) {
	$key = substr($data, 0, 32);
	$ver1 = ord($data[32]);
	$ver2 = ord($data[33]);
	if ($ver1 != 1 || $ver2 < 4) {
		return;
	}
	$sqlKey = mysql_escape_string($key);
	$version = mysql_escape_string("1." . $ver1 . $ver2);
	openDatabase(true);
	$sql = "update game_server set gs_version = '$version' where gs_key = '$sqlKey'";
	mysql_query($sql);
	closeDatabase();
}

#############################################################################
# FUNCTION:     verifyClientKey
# AUTHOR:       John Morrison
# DATE:         02/04/06
# RETURNS:
# PURPOSE:
# A server is wanting to verify a client session key
#
# PARAMETERS:
# data - Input from server
#############################################################################
function verifyClientKey($data) {
	$ip = getenv ("REMOTE_ADDR");
	$serverKey = substr($data, 0, 32);
	$userNameLen = ord($data[32]);
	$userName =  substr($data, 33, $userNameLen);
	$pos = 33 + $userNameLen ;
	$clientKey = substr($data, $pos, 32);
	

	$serverKey = mysql_escape_string($serverKey);
	$clientKey = mysql_escape_string($clientKey);
	$userName = mysql_escape_string($userName);
	$query = "SELECT a.ap_pid from active_player a, users u where " . /* a.ap_left = 0 and */ " u.username = '$userName' and u.user_id = a.ap_pid and a.ap_serverkey = '$serverKey' and a.ap_key = '$clientKey'";
	openDatabase(true);
	$result = mysql_query($query);
	if (mysql_num_rows($result) != 1) {
		writeError("vcc - Does not have a valid session.");
	}
	closeDatabase();
}

#############################################################################
# FUNCTION:     processTeams
# AUTHOR:       John Morrison
# DATE:         03/02/21
# RETURNS:
# PURPOSE:
# A server is sending the teams list. Process it 
#
# PARAMETERS:
# data - Input from server
#############################################################################
function processTeams($data) {
	$numTeams = ord($data[0]);
	$serverKey = substr($data, 0, 32);
	
	$sqlServerKey = mysql_escape_string($serverKey);
	$sql = "select gs_key from game_server where gs_key='$sqlServerKey'";

	openDatabase(true);
	$result = mysql_query($sql);
	if (!$result) {
		writeError("Failed");
		return;
	}
	if (mysql_num_rows($result) != 1) {
		writeError("No Data");
		return;
	}
	# Valid game. Process
	$teamId = 0;
	$pos = 32;
	$numTeams = ord($data[$pos]);
	$pos++;
	$dataLen = strlen($data);
	while ($pos < $dataLen) {
		$value = ord($data[$pos]);
		if ( $value == 254) {
			# New Team marker
			$teamId++;
			$pos++;
		} else {
			$pid = "null";
			$playerKey = "null";
			if ($value != 0) {
				$playerKey = mysql_escape_string(substr($data, $pos, 32));
				$pos += 32;
				$sql = "select ap_pid from active_player where ap_key = '$playerKey' and ap_serverkey = '$sqlServerKey'";
				$result = mysql_query($sql);
				if ($result) {
					if (mysql_numrows($result) == 1) {
						$pid = mysql_result($result, 0, 0);
						$playerKey = "'$playerKey'";
					} else {
						writeError("No id");
					}
				} else {
					writeError("Error");
				}
			} else {
				$pos++;
			}
			$sql = "insert into active_team (at_serverkey, at_key, at_team_number, at_pid) values ('$sqlServerKey', $playerKey, '$teamId', $pid)";
			if (!mysql_query($sql)) {
				writeError("Error");
			}

		}
	}
}

#############################################################################
# FUNCTION:     processServerUpdate
# AUTHOR:       John Morrison
# DATE:         02/04/06
# RETURNS:
# PURPOSE:
# A server is sending an update packet. Update its last heard from time
# and add any events its sending to the game_event table
#
# PARAMETERS:
# data - Input from server
#############################################################################
function processServerUpdate($data) {
	global $link;
	$numPlayers = ord($data[0]);
	$numFreeBases = ord($data[1]);
	$numFreePills = ord($data[2]);
	$serverKey = substr($data, 3, 32);
	
	$sqlServerKey = mysql_escape_string($serverKey);
	/* Check Server key */
	$now = time();
	$query = "UPDATE game_server set gs_freepills = '$numFreePills', gs_freebases = '$numFreeBases', gs_numtanks = '$numPlayers', gs_lastheard = '$now' where gs_key = '$sqlServerKey'";
	openDatabase(true);
	$result = mysql_query($query);
	if (mysql_affected_rows($link) != 1) {
		writeError("Does not have a valid session");
	}
	$dataLen = strlen($data);
	if ($dataLen > 3 + 32) {
		$pos = 3 + 32; // 3 + 
		while ($pos < $dataLen) {
			$actionType = ord($data[$pos]);
			$actionType = mysql_escape_string($actionType);
			$pos++;
			$proceed = true;

			/* Check Player A Key */
			$playerKeyA = "null";
			if ($pos < $dataLen && ord($data[$pos]) > 0 ) {
				/* Must have a player key */
				$blah = substr($data, $pos-2);
				$playerKeyA = "'" . mysql_escape_string(substr($data, $pos, 32)) . "'";
				$pos += 32;
				/* Check Key */
				$sql = "SELECT ap_key from active_player where ap_key=$playerKeyA and ap_serverkey = '$sqlServerKey'"; // and ap_left = 0
				$result = mysql_query($sql);
				if (mysql_numrows($result) != 1) {
					$proceed = false;
					writeError("Bad" );
				} else if ($actionType == WINBOLO_NET_EVENT_PLAYER_JOIN) {
					$sql = "UPDATE active_player set ap_left = 0 where ap_serverkey = '$sqlServerKey' and ap_key = $playerKeyA";
					mysql_query($sql);
				}
			} else {
				$pos++;
			}

			/* Check Player B key */
			$playerKeyB = "null";
			if ($proceed == true && $pos < $dataLen && ord($data[$pos]) > 0 ) {
// Old code
// && ($actionType != WINBOLO_NET_EVENT_BASE_CAPTURE && $actionType != WINBOLO_NET_EVENT_PILL_CAPTURE &&  $actionType !=  WINBOLO_NET_EVENT_LGM_KILL && $actionType !=  WINBOLO_NET_EVENT_LGM_LOST && $actionType != WINBOLO_NET_EVENT_ALLY_LEAVE && $actionType != WINBOLO_NET_EVENT_WIN)
				/* Must have a player key */
				$playerKeyB = "'" . mysql_escape_string(substr($data, $pos, 32)) . "'";
				$pos += 32;
				/* check Key */
				$sql = "SELECT ap_key from active_player where ap_key=$playerKeyB and ap_serverkey = '$sqlServerKey'"; //  and ap_left = 0
				$result = mysql_query($sql);
				if (mysql_numrows($result) != 1) {
					$proceed = false;
					writeError("Bad");
				}
			} else {
				$pos++;
			}

			if ($proceed == true) {
				/* This is a good event - Import */
				$now = time();
				$sql = "INSERT into game_event (ge_serverkey, ge_eventtype, ge_playera, ge_playerb, ge_timestamp) values ('$sqlServerKey', '$actionType', $playerKeyA, $playerKeyB, '$now')";
				$result = mysql_query($sql);
				if (mysql_affected_rows($link) != 1) {
					writeError("bad jub jub");
				}
			}
		}
	}
	closeDatabase();
}

#############################################################################
# FUNCTION:     processClientLeaveGame
# AUTHOR:       John Morrison
# DATE:         02/04/06
# RETURNS:
# PURPOSE:
# A server is saying a client has left the game. Update their entry in the
# active_player table to being left
#
# PARAMETERS:
# data - Input from server
#############################################################################
function processClientLeaveGame($data) {
	if (strlen($data) != 67) { /* 2 Keys + Number of players + free bases + free pills  */
		writeError("Invalid Data");
	}
	$ip = getenv ("REMOTE_ADDR");
	$numPlayers = ord(substr($data, 0, 1));
	$pos++;
	$numBases = ord(substr($data, 1, 1));
	$pos++;
	$numPills = ord(substr($data, 2, 1));
	$pos = 3;
	$serverKey = substr($data, $pos, 32);
	$pos += 32;
	$clientKey = substr($data, $pos, 32);
	$serverKey = mysql_escape_string($serverKey);
	$clientKey = mysql_escape_string($clientKey);
	$query = "UPDATE active_player set ap_left = 1 where ap_serverkey = '$serverKey' and ap_key = '$clientKey'";
	openDatabase(true);
	$result = mysql_query($query);
	$updateSql = "UPDATE game_server set gs_numtanks = '$numPlayers', gs_freebases = '$numBases', gs_freepills = '$numPills' where gs_key = '$serverKey'";
	$result = mysql_query($updateSql);
	closeDatabase();
}

#############################################################################
# FUNCTION:     processClientKeyRequest
# AUTHOR:       John Morrison
# DATE:         02/04/06
# RETURNS:
# PURPOSE:
# A client is requesting a unique client session key
#
# PARAMETERS:
# data - Input from server
#############################################################################
function processClientKeyRequest($data) {
	$ip = getenv ("REMOTE_ADDR");
	$serverKey = substr($data, 0, 32);
	$userNameLen = ord($data[32]);
	$userName =  substr($data, 33, $userNameLen);
	$pos = 33 + $userNameLen;
	$passwordLen = ord($data[$pos]);
	$password = substr($data, $pos+1, $passwordLen);
	$userName = mysql_escape_string($userName);
	$password = md5($password);


	$serverKey = mysql_escape_string($serverKey);
	$userName = mysql_escape_string($userName);
	$password = mysql_escape_string($password);

	# Check User Name/Password
	$query = "SELECT user_id from users where lower(username) = '$userName' and user_password = '$password'";
	openDatabase(true);
	$result = mysql_query($query);
	if (mysql_numrows($result) != 1) {
		writeError("Invalid username/password combination");
	}
	$pid = mysql_result($result, 0, "user_id");

	#Check Server Key
	$query = "SELECT gs_key from game_server where gs_key = '$serverKey'";
	$result = mysql_query($query);
	if (mysql_numrows($result) != 1) {
		writeError("WBN: Sorry game does not exist");
	}

	#Check to see if we are already in game 
	$query = "SELECT ap_key, ap_left from active_player where ap_serverkey = '$serverKey' and ap_pid = '$pid'";
	$result = mysql_query($query);
	if (mysql_numrows($result) > 0 ) {
		# rejoin
		if (mysql_result($result, 0, "ap_left") == 0) {
			writeError("Sorry you are already in game. Please wait a minute and try again");
		} else {
			$key = mysql_result($result, 0, "ap_key");
			$sql = "UPDATE active_player set ap_left = 0 where ap_serverkey = '$serverKey' and ap_pid = '$pid'";
			mysql_query($sql);
			
			$event = WINBOLO_NET_EVENT_REJOIN;
			$now = time();
			$sql2 = "INSERT into game_event values ('$serverKey', '$event', '$key', NULL, '$now')";
			mysql_query($sql2);
		}
	} else {
		# Make ourselves a key and go!
		$key = md5(time() . $ip . $pid. $serverKey. $userName. $password . time());
		$sql = "INSERT into active_player (ap_pid, ap_key, ap_serverkey, ap_left) values ('$pid', '$key', '$serverKey', 1)";
		$result = mysql_query($sql);
		if (mysql_affected_rows() != 1) {
			writeError("Database error");
		}
		closeDatabase();
	}
	return $key;
}

#############################################################################
# FUNCTION:     getNewServerKey
# AUTHOR:       John Morrison
# DATE:         02/04/06
# RETURNS:	A unique server key
# PURPOSE:
# Checks to ensure that no keys have ever existed with the same name
#
# PARAMETERS:
# key - Key to test
#############################################################################
function getNewServerKey($key) {
	$finished = false;
	
	while ($finished == false) {
		$change = false;
		$sqlKey = mysql_escape_string($key);
		$sql = "select ags_key from archived_game_server where ags_key = '$sqlKey'";
		$result = mysql_query($sql);
		if (mysql_num_rows($result) > 0) {
			$change = true;
		} else {
			# Check current games
			$sql = "select gs_key from game_server where gs_key = '$sqlKey'";
			$result2 = mysql_query($sql);
			if (mysql_num_rows($result2) > 0) {
				$change = true;
			}
			mysql_free_result($result2);
		}
		mysql_free_result($result);

		#Do we need to try a new key?
		if ($change == true) {
			$key = md5($key . rand() . $key . time() . $key);
		} else {
			$finished = true;
		}
	}
	return $key;
}

#############################################################################
# FUNCTION:     processServerKeyRequest
# AUTHOR:       John Morrison
# DATE:         02/04/06
# RETURNS:
# PURPOSE:
# A server is requesting a unique server session key
#
# PARAMETERS:
# data - Input from server
#############################################################################
function processServerKeyRequest($data) {
	$ip = getenv ("REMOTE_ADDR");
	$gameType = ord($data[0]);
	$ai = ord($data[1]);
	$mines = ord($data[2]);
	$password = ord($data[3]);
	$freeBases = ord($data[4]);
	$freePills = ord($data[5]);
	$numPlayers = ord($data[6]);
	$numBases = ord($data[7]);
	$numPills = ord($data[8]);
	$mapLen = ord($data[9]);
	$mapName = substr($data, 10, $mapLen);
	$pos = 10 + $mapLen; 
	$port = unpack("nid", substr($data, $pos, 2));
	$port = $port[id];
	$pos += 2;
	$startTime = time();
	//$startTime = unpack("Nid", $startTime); 
	//$startTime = $startTime[id];

	openDatabase(true);
	$now = time();
	srand($now);
	$key = getNewServerKey(md5($now . rand() . $ip . rand() . $port . $mapName . rand() . $password . $mines . $startTime . time() . rand()));

	$mapName = mysql_escape_string($mapName);
	$ipAddress = $ip;
	$ip = gethostbyaddr($ip);
	# Fix for west
	if ($ip == "gatekeeper.nt4.com") {
		$ip = "west.winbolo.com";
	}
	#Fix for sheeps
	if ($ip == "ool-4351bf05.dyn.optonline.net") {
		$ip = "sheeps.winbolo.us";
	}
	#Fix for fermat
	if ($ip == "c-69-136-99-180.hsd1.pa.comcast.net") {
		$ip = "fermat.winbolo.us";
	}
	#Fix for canuck
	if ($ip == "CPE0002446032e4-CM000039fbb861.cpe.net.cable.rogers.com") {
		$ip = "winbolo.dhs.org";
	}
	#Fix for paniq
	if ($ipAddress == "207.88.53.107") {
		$ip = "paniq.winbolo.us";
	}
	#Fix for bluebot
	if ($ip == "pool-141-154-115-77.bos.east.verizon.net") {
		$ip = "bluebot.winbolo.us";
	}
	#Fix for renbot
	if ($ip == "dsl092-218-111.sfo2.dsl.speakeasy.net") {
		$ip = "renbot.winbolo.us";
	}

	if ($ip == "12.39.4.70") {
		$ip = "pawnyBot.winbolo.us";
	}

        #CCBot
	if ($ipAddress == "24.4.42.190") {
		$ip = "ccbot.winbolo.us";
	}

	#NYCBot
	if ($ip == "cpe-68-173-217-129.nyc.res.rr.com") {
		$ip = "nycbot.winbolo.us";
	}

	# Acrobot
	if ($ip == "cpe-24-195-41-47.nycap.res.rr.com") {
		$ip = "acrobot.winbolo.us";
	}

	# Alexmbot
	if ($ip == "chanxmail.swmc.org") {
		$ip = "alexmbot.winbolo.us";
	}

	#me
	if ($ip == "johnmorrison.name") {
		$ip = "winbolo.net";
	}
	
	$ip = mysql_escape_string($ip);
	$sql = "insert into game_server (gs_key, gs_ip, gs_port, gs_mapname, gs_password, gs_mines, gs_numbases, gs_numpills, gs_freebases, gs_freepills, gs_numtanks, gs_starttime, gs_firstheard, gs_lastheard, gs_ai, gs_gametype) values ('$key', '$ip', '$port', '$mapName', '$password', '$mines', '$numBases', '$numPills', '$freeBases', '$freePills', '$numPlayers', '$startTime', '$now', '$now', '$ai', '$gameType')";
	$result = mysql_query($sql);
	if (mysql_affected_rows() != 1) {
		writeError("Database error. ");
	}
	closeDatabase();
	return $key;
}

#############################################################################
# FUNCTION:     openDatabase
# AUTHOR:       John Morrison
# DATE:         01/06/17
# RETURNS:
# PURPOSE:
# Opens our conncetion to the databse
#
# PARAMETERS:
#############################################################################
function openDatabase($open) {
	global $link;
	global $cfg_winbolodb_user;
	global $cfg_winbolodb_pass;
	global $cfg_winbolodb;
	# declare some variables
	$site = "localhost";
        $username = $cfg_winbolodb_user;
        $password = $cfg_winbolodb_pass;
        $dbName = $cfg_winbolodb;
	if ($open == true) {
		$link = @mysql_connect($site, $username, $password);
		if (!$link) {
			writeError("WinBolo.net Temporarily Unavailable");
		}
		if (!mysql_select_db($dbName)) {
			writeError("WinBolo.net Temporarily Unavailable");
		}
	}
}

#############################################################################
# FUNCTION:     closeDatabase
# AUTHOR:       John Morrison
# DATE:         01/06/17
# RETURNS:
# PURPOSE:
# Closes our connection to the databse
#
# PARAMETERS:
#############################################################################
function closeDatabase() {
	mysql_close();
}

?>
