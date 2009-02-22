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


if ($BASE_FILES != "") {
  require "$BASE_FILES/teamclass.php";
  require "$BASE_FILES/mapcollectionitemclass.php";
  require "$BASE_FILES/maplevelclass.php";
  require "$BASE_FILES/mapinfoclass.php";
  require "$BASE_FILES/newsclass.php";
  require "$BASE_FILES/playerclass.php";
  require "$BASE_FILES/serverinfoclass.php";
  require "$BASE_FILES/eventclass.php";
  require "$BASE_FILES/eventserverclass.php";
  require "$BASE_FILES/postclass.php";
  require "$BASE_FILES/partialserverlistclass.php";
  include "$BASE_FILES/winbolonetcfg.php";
  include "$BASE_FILES/forumfunctions.php";
  require "$BASE_FILES/mapcollectioninfoclass.php";
  require "$BASE_FILES/mapcollectioncommentclass.php";
  include "$BASE_FILES/mapcollectionfunctions.php";
}

#############################################################################
# FUNCTION:	openDatabase	
# AUTHOR:	John Morrison
# DATE:		01/06/17
# RETURNS:	
# PURPOSE:	
# Opens our conncetion to the databse
#
# PARAMETERS:
#############################################################################
function openDatabase($open) {
	global $cfg_winbolodb;
	global $cfg_winbolodb_user;
	global $cfg_winbolodb_pass;
	# declare some variables
	$site = "localhost"; 
	$username = $cfg_winbolodb_user; 
	$password = $cfg_winbolodb_pass; 
	$dbName = $cfg_winbolodb;
	
	if ($open == true) {
	 	if (!@mysql_connect($site, $username, $password)) {
			include ("offline.html"); 
			exit;
		}
		mysql_select_db($dbName);
	}

}

#############################################################################
# FUNCTION:	closeDatabase	
# AUTHOR:	John Morrison
# DATE:		01/06/17
# RETURNS:	
# PURPOSE:	
# Closes our connection to the databse
#
# PARAMETERS:
#############################################################################
function closeDatabase() {
	mysql_close();
}

#############################################################################
# FUNCTION:	doesPlayerExist	
# AUTHOR:	John Morrison
# DATE:		01/06/17
# RETURNS:	true if the player exists else false
# PURPOSE:	
# Looks up to see if a login name is in the winbolo.net database. 
#
# PARAMETERS:
# playerName -	The WinBolo.net login
#############################################################################
function doesPlayerExist($playerName) {
	$playerName = mysql_escape_string($playerName);
	$query = "SELECT user_id from users where lower(username) = '$playerName'";
	$result = mysql_query($query);
	if (mysql_numrows($result) == 0) {
		return false;
	}
	return true;
}

#############################################################################
# FUNCTION:	getPlayerMap
# AUTHOR:	John Morrison
# DATE:		01/06/17
# RETURNS:	returns a map info class containing the map details or false 
#		if the user has not uploaded a map 
# PURPOSE:	
# Checks to see if a player has uploaded a map 
#
# PARAMETERS:
# pid 	- Player Id
#############################################################################
function getPlayerMap($pid) {
	$query = "SELECT name, comment, bases, pills, starts from map where pid = " . $pid;
	$result = mysql_query($query);
	if (mysql_numrows($result) == 0) {
		return false;
	}

	$mi = new MapInfoClass;
	$mi->setup(mysql_result($result, 0, "name"), mysql_result($result, 0, "bases"), mysql_result($result, 0, "pills"), mysql_result($result, 0, "starts"), mysql_result($result, 0, "comment"));
	return $mi;
}

#############################################################################
# FUNCTION:	logHasCommented
# AUTHOR:	John Morrison
# DATE:		04/09/23
# RETURNS:	returns if a player has commented on a log file
#		
# PURPOSE:	
# Test to see if a player has already commented on a log file
#
# PARAMETERS:
# key - Game key to test
# pid - Player ID
#############################################################################
function logHasCommented($key, $pid) {
	$pid = mysql_escape_string($pid);
	$key = mysql_escape_string($key);
	$sql = "select lc_pid from log_comment where lc_pid = '$pid' and lc_key = '$key'";
	$result = mysql_query($sql);
	if (mysql_num_rows($result) > 0) {
		mysql_free_result($result);
		return true;
	}
	mysql_free_result($result);
	return false;
}

#############################################################################
# FUNCTION:	logCalculate Rating
# AUTHOR:	John Morrison
# DATE:		04/09/23
# RETURNS:	
#		
# PURPOSE:	
# Calcualtes the ratings for a log file and updates the table
#
# PARAMETERS:
# key - Game key to calculate ratings for
#############################################################################
function logCalculateRating($key) {
	$key = mysql_escape_string($key);
	$sql = "select avg(lc_rating) as avg, count(lc_rating) as total from log_comment where lc_key = '$key' and lc_rating > 0";
	$result = mysql_query($sql);
	$rating = mysql_result($result, 0, 0);
	$total = mysql_result($result, 0, 1);
	mysql_free_result($result);
	$sql = "UPDATE archived_game_server set ags_log_rating = $rating, ags_log_numratings = $total where ags_key = '$key'";
	mysql_query($sql);
}

#############################################################################
# FUNCTION:	getNewsItems
# AUTHOR:	John Morrison
# DATE:		03/01/25
# RETURNS:	returns an array of news events
#		
# PURPOSE:	
# Retrieves an array of news events objects 
#
# PARAMETERS:
# numItems - The number of items to return
#############################################################################
function getNewsItems($numItems) {
	$nis = array();
	$query = "select t.topic_id, t.topic_title, u.username, p.post_time, pt.bbcode_uid, pt.post_text FROM users u, topics t, posts p, posts_text pt where t.forum_id=5 and t.topic_first_post_id = p.post_id and pt.post_id = p.post_id and u.user_id = p.poster_id order by p.post_time desc limit $numItems";
	$result = mysql_query($query);
	$count = 0;
	$numRows = mysql_numrows($result);
	while ($count < $numRows) {
		$ni = new newsItemClass;
		$ni->setup(mysql_result($result, $count, "topic_title"), mysql_result($result, $count, "username"), mysql_result($result, $count, "post_time"), mysql_result($result, $count, "post_text"), mysql_result($result, $count, "bbcode_uid"), mysql_result($result, $count, "topic_id"));
		$nis[$count] = $ni;
		$count++;
	}
	return $nis;
}

#############################################################################
# FUNCTION:	getJournalEntries
# AUTHOR:	John Morrison
# DATE:		04/09/18
# RETURNS:	returns Partial Server list containing games played 
# PURPOSE:	
# Retrieves the list of games the player has played in.
#
# PARAMETERS:
# username - User name to retrive names for
# start    - Start item
#############################################################################
function getJournalEntries($username, $start) {
        global $cfg_ServersPerPage;
        global $cfg_MaxServersSelect;

	$pid = getPid($username);
	if ($pid == 0) {
		return null;
	}
        $ag = array();
        $count = $min;
	$min = $start;
	$max = $min+$cfg_ServersPerPage;


	$sql = "select straight_join j.j_pid, a.* from journal j, archived_game_server a where j.j_key = a.ags_key and j.j_pid = $pid order by a.ags_starttime desc";
	$result = mysql_query($sql);
	$numRows = mysql_numrows($result);
	if ($numRows == 0) {
		return null;
	}
	if ($min > $numRows) {
		$min = $numRows-1;
		$max = $min+1;
	} else {
	        if ($numRows < $max) {
			$max = $numRows;
		}
	}
        $arrayIndex = 0;
        $count = $min;
        while ($count < $max) {
                # Get Players
                $key = mysql_result($result, $count, "ags_key");
		$sql = "select u.username from users u, journal j where u.user_id = j.j_pid and j.j_key = '$key'";
		$playersResult = mysql_query($sql);
                $players = array();
		$count2 = 0;
		while ($count2 < mysql_numrows($playersResult)) {
			$players[$count2] = mysql_result($playersResult, $count2, "username");
			$count2++;
		}
		mysql_free_result($playersResult);
		$cs = new ServerInfoClass();
		$cs->setup(mysql_result($result, $count, "ags_key"), mysql_result($result, $count, "ags_ip"), mysql_result($result, $count, "ags_port"),mysql_result($result, $count, "ags_mapname"),mysql_result($result, $count, "ags_password"),mysql_result($result, $count, "ags_mines"),mysql_result($result, $count, "ags_ai"),mysql_result($result, $count, "ags_gametype"),  mysql_result($result, $count, "ags_numbases"),mysql_result($result, $count, "ags_numpills") , mysql_result($result, $count, "ags_freebases"),mysql_result($result, $count, "ags_freepills"), mysql_result($result, $count, "ags_numtanks"), mysql_result($result, $count, "ags_starttime"), mysql_result($result, $count, "ags_firstheard"), mysql_result($result, $count, "ags_lastheard"), $players, mysql_result($result, $count, "ags_version"), 'F');
		$cs->setLogRating(mysql_result($result, $count, "ags_log_rating"));
		$cs->setLogNumRatings(mysql_result($result, $count, "ags_log_numratings"));
		$ag[$arrayIndex] = $cs;
		$count++;
		$arrayIndex++;
        }
	$ret = new PartialServerListClass();
        $ret->setup($numRows, $ag);
	mysql_free_result($result);
        return $ret;
}


#############################################################################
# FUNCTION:	getRank
# AUTHOR:	John Morrison
# DATE:		01/06/17
# RETURNS:	returns the rank of something or -1 on error 
# PURPOSE:	
# Gets the rank of a score of a particular table. The parameters section
# details more information
#
# PARAMETERS:
# table    - The table name that contains the scores
# rankType - The field that has the scores to compare
# score    - Our score we are comparing too
# id       - Player / Team id
# idField  - The id field in the database ie pid or tid
# prefix   - Prefix of tables
#############################################################################
function getRank($table, $rankType, $score, $id, $idField, $prefix) {
        #Check to see if won/lost a game
        $wins = $prefix . "_wins";
        $loses = $prefix . "_loses";
        $sql = "select $idField from $table where $idField = $id and $wins = 0 and $loses = 0";
        $results = mysql_query($sql);
        if (mysql_num_rows($results) == 1) {
                return "N/A";
        }

        #It has played a game, lets find its rank
        $sql = "select $rankType as score from $table where ($wins > 0 || $loses > 0) and $rankType >= '$score' order by $rankType desc";
	$results = mysql_query($sql);
	if (!$results) {
		return -1;
	}
	$count = 0;
	$rank = 0;
	$lastScore = 1000000;
	while ($count < mysql_numrows($results)) {
		$score = mysql_result($results, $count, "score");
		if ($score < $lastScore) {
			$rank++;
			$lastScore = $score;
		}
		$count++;
	}
	return $rank;
}

#############################################################################
# FUNCTION:	getLockedHtml
# AUTHOR:	John Morrison
# DATE:		04/01/13
# RETURNS:	String containing the html to load an image
# PURPOSE:	
# Gets the html string <img src='images/locked.png'> if the game is locked
# and is an appropriate version
#
# PARAMETERS:
# gs  -	The game server to get foR 
#############################################################################
function getLockedHtml($gs) {
	if ($gs->getVersion() != "1.13" && $gs->locked == true) {
		return "<img src='images/locked.gif'>";
	}
	return "";
}

#############################################################################
# FUNCTION:	doesTeamExist	
# AUTHOR:	John Morrison
# DATE:		01/07/21
# RETURNS:	true if the team exists else false
# PURPOSE:	
# Looks up to see if a team name is in the winbolo.net database. 
#
# PARAMETERS:
# mame -	The WinBolo.net team name to cheack
#############################################################################
function doesTeamExist($name) {
	$playerName = mysql_escape_string($name);
	$query = "SELECT tid from stats_team where lower(name) = '$name'";
	$result = mysql_query($query);
	if (mysql_numrows($result) == 0) {
		return false;
	}
	return true;
}

#############################################################################
# FUNCTION:	getPlayerName
# AUTHOR:	John Morrison
# DATE:		01/06/17
# RETURNS:	The players name null if not found
# PURPOSE:	
# Retrieves a players name from a pid
#
# PARAMETERS:
# pid - The player name
#############################################################################
function getPlayerName($pid) {
	$pid = mysql_escape_string($pid);
	$query = "SELECT username from users where user_id = '$pid'";
	$result = mysql_query($query);
	if (mysql_num_rows($result) == 0) {
		return null;
	}
	return mysql_result($result, 0, "username");
}


#############################################################################
# FUNCTION:	getPid
# AUTHOR:	John Morrison
# DATE:		01/06/17
# RETURNS:	The players player id or 0 if not found
# PURPOSE:	
# Retrieves a login id's pid
#
# PARAMETERS:
# playerName -	The WinBolo.net login
#############################################################################
function getPid($playerName) {
	$playerName = mysql_escape_string($playerName);
	$query = "SELECT user_id from users where lower(username) = '$playerName'";
	$result = mysql_query($query);
	if (mysql_num_rows($result) == 0) {
	echo mysql_error();
		return 0;
	}
	return mysql_result($result, 0, "user_id");
}

#############################################################################
# FUNCTION:	getTid
# AUTHOR:	John Morrison
# DATE:		01/07/21
# RETURNS:	The teams id or -1 if not found
# PURPOSE:	
# Retrieves a teams id
#
# PARAMETERS:
# name -	The WinBolo.net taam name
#############################################################################
function getTid($name) {
	$name = mysql_escape_string($name);
	$query = "SELECT tid from stats_team where lower(name) = '$name'";
	$result = mysql_query($query);
	if (mysql_numrows($result) == 0) {
		return -1;
	}
	return mysql_result($result, 0, "tid");
}

#############################################################################
# FUNCTION:	getTeams
# AUTHOR:	John Morrison
# DATE:		01/07/22
# RETURNS:	The number of players that matcha a string
# PURPOSE:	
# Retrieves a number of players in an array.
#
# PARAMETERS:
# mame 	- The WinBolo.net team name 
# limit  -  The limit of the number of players to get
#############################################################################
function getTeams($name, $limit) {
	$name = mysql_escape_string($name);
	$query = "SELECT name from stats_team where lower(name) like '%$name%' order by name limit $limit";
	$result = mysql_query($query);
	$teams = array();
	$count = 0;
	while ($count <mysql_numrows($result)) {
		$teams[$count] = mysql_result($result, $count, 0);
		$count++;
	}
	return $teams;
}

#############################################################################
# FUNCTION:	getPlayers
# AUTHOR:	John Morrison
# DATE:		01/06/17
# RETURNS:	The number of players that matcha a string
# PURPOSE:	
# Retrieves a number of players in an array.
#
# PARAMETERS:
# playerName -	The WinBolo.net login
# limit      -  The limit of the number of players to get
#############################################################################
function getPlayers($playerName, $limit) {
	$playerName = mysql_escape_string($playerName);
	$query = "SELECT username from users where lower(username) like '%$playerName%' and user_id > 1 order by username limit $limit";
	$result = mysql_query($query);
	$players = array();
	$count = 0;
	while ($count <mysql_numrows($result)) {
		$players[$count] = mysql_result($result, $count, 0);
		$count++;
	}
	return $players;
}

#############################################################################
# FUNCTION:     setEmailForward
# AUTHOR:       John Morrison
# DATE:         04/09/19
# RETURNS:      "OK" if leave succeeded or database error
# PURPOSE:
# To set a player email forward on or off. 
#
# PARAMETERS:
# playerName - The player name to set
# enable    - true to enable, false to disable
#############################################################################
function setEmailForward($playerName, $enable) {
	global $cfg_aliasadd_app;
	global $cfg_aliasremove_app;
	if (strstr($playerName, "@") != false || strstr($playerName, "|") != false || strstr($playerName, "'") != false || strstr($playerName, '"') != false || strstr($playerName, ";") != false) {
		return "Your player name prevents you from having an email forward.";
	}

	$pc = getPlayerClass($playerName);
	if ($pc == 0) {
		return "PC Error (DB?)";
	}
	$email = $pc->getEmail();
	$id = $pc->getId();
	$alias = strtolower($playerName) . "@winbolo.net";
	if ($enable == true) {
		$val = "1";
	} else {
		$val = "0";
	}
	if (!ereg("^[a-zA-Z0-9_@.-]+$", $playerName)) {
		return "Your player name prevents you from having an email forward.";
	}
	if ((!ereg(".+\@.+\..+", $email)) || (!ereg("^[a-zA-Z0-9_@.-]+$", $email))) {
		return "Your Email address is not valid. Please correct the problem and try again.";
	}
	if (($pc->getEmailForward() == 1 && $enabled == true) || ($pc->getEmailForward() == 0 && $enable == false)) {
		return "Email forward already set.";
	}

	$alias = escapeshellarg($alias);
	$email = escapeshellarg($email);

	if ($enable == true) {
		$res = exec("$cfg_aliasadd_app $email $alias 2>&1");
	} else {
		$res = exec("$cfg_aliasremove_app $alias 2>&1");
	}
	if (strlen($res) != 0) {
		return "Error setting alias.";
	}
	
	$sql = "UPDATE users set user_emailforward = $val where user_id = $id";
	mysql_query($sql);

	if ($enable == false) {
		$sql = "UPDATE users set user_showlforward = 0 where user_id = $id";
		mysql_query($sql);
	}
	
	return "OK";
}

#############################################################################
# FUNCTION:     doJoinTeam
# AUTHOR:       John Morrison
# DATE:         01/07/22
# RETURNS:      "OK" if join succeeded or error message
# PURPOSE:
#  To join a player to a team
#
# PARAMETERS:
# playerName - The player name
# team       - The team
#############################################################################
function doJoinTeam($playerName, $team, $password) {
	if (checkPassword($team, $password, "JOINTEAM") == false) {
		return "Password is incorrect";
	}
	if (isPlayerInTeam($playerName, $team) == true) {
		return "You can't join the same team twice you know?";
	}
	$pid = getPid($playerName);
	if ($pid == 0) {
		return "Error";
	}
	$tid = getTid($team);
	$query = "insert into team_members values ('$tid', '$pid')";
	$result = mysql_query($query);
	if (mysql_affected_rows() != 1) {
		return "Database error";
	}
	return "OK";
}


#############################################################################
# FUNCTION:     doModifyTeamMembers
# AUTHOR:       John Morrison
# DATE:         01/09/09
# RETURNS:      "OK" if succeeded or error message
# PURPOSE:
#  To removes 1 or more team members from a team 
#
# PARAMETERS:
# team    - The team
# players - Array of players to remove 
#############################################################################
function doModifyTeamMembers($team, $players) {
	if (sizeof($players) < 1) {
		return "nothing to do";
	}

	# Get tid
	$tid = getTid($team);
        if ($tid == -1) {
                return "Error2";
        }

	# Get owner
	$query = "select owner from stats_team where tid = '$tid'";
        $result = mysql_query($query);
	$ownerPid = mysql_result($result, 0, "owner");

	# Get pids because mysql is lame and soesn't support subselects
	$count = 0;
	$sql = "select user_id from users where ";
	while ($count < sizeof($players)) {
		$current = mysql_escape_string($players[$count]);
		if ($count > 0) {
			$sql .= " or ";
		}
		$sql .= "username = '$current' ";
		$count++;
	}
	$result = mysql_query($sql);

	if (mysql_numrows($result) < 1) {
		return "nothing to do";
	}

	#now do delete
	$sql = "delete from team_members where tid = '$tid' and pid != '$ownerPid' and pid in (";
	$count = 0;
        while ($count <mysql_numrows($result)) {
		if ($count > 0) {
			$sql .= ", ";
		}
		$sql .= mysql_result($result, $count, "user_id");
		$count++;
	}
	$sql .= ")";
	if (!mysql_query($sql)) {
		return "Database error";
	}
	return "OK";
}

#############################################################################
# FUNCTION:     doLeaveTeam
# AUTHOR:       John Morrison
# DATE:         01/07/22
# RETURNS:      "OK" if leave succeeded or database error
# PURPOSE:
#  To remove a player from a team 
#
# PARAMETERS:
# playerName - The player name
# team       - The team
#############################################################################
function doLeaveTeam($playerName, $team) {
	$pid = getPid($playerName);
	if ($pid == 0) {
		return "Error";
	}

	$tid = getTid($team);
	if ($tid == -1) {
		return "Error2";
	}

	$query = "select owner from stats_team where tid = '$tid'";
	$result = mysql_query($query);
	if (mysql_result($result, 0, "owner") == $pid) {
		return "Team creators can not leave teams";
	}
	$query = "delete from team_members where pid = '$pid' and tid = '$tid'";
	$result = mysql_query($query);
	if (mysql_affected_rows() != 1) {
		return "Database error";
	}
	return "OK";
}

#############################################################################
# FUNCTION: 	doPlayerModify
# AUTHOR:	John Morrison
# DATE:		01/06/17
# RETURNS:	The string "OK" on success other wise an error message 
#		to display
# PURPOSE:	
# Updates a players profile. 
#
# PARAMETERS:
# username -	The WinBolo.net login
# password -	The new password (or blank for unchanged)
# password2 -	The new password retyped
# gametype -	The players favourite game type
# blurb	-	Players blurb
# showemail -	Whether to show the players Email Address or not
# showblurb - 	Whether to show the blurb or not 
# sig       -	User sig
# website   -	Website url
# icq       -	Players ICQ
# aim       -	Players aim
# yahoo	    -   Players yahoo
# msn	    -   Player msn
# occupation -  Players occupation
# showforward -  Display Email Forward
#############################################################################
function doPlayerModify($userName, $email, $password, $password2, $gametype, $blurb, $showemail, $showblurb, $sig, $website, $icq, $aim, $yahoo, $msn, $occ, $showforward) {
	$pid = getPid($userName);

        $email = stripslashes($email);
	$password = stripslashes($password);
	$password2 = stripslashes($password2);
	$gametype = stripslashes($gametype);
	$blurb = stripslashes($blurb);
	$showemail = stripslashes($showemail);
	$showblurb = stripslashes($showblurb);
	$sig = stripslashes($sig);
	$icq = stripslashes($icq);
	$aim = stripslashes($aim);
	$yahoo = stripslashes($yahoo);
	$msn = stripslashes($msn);
	$occ = stripslashes($occ);
	$showforard = stripslashes($showforward);

	# Do check of changing email address if forward is set and disallow it
	$pc = getPlayerClass($userName);
	if ($pc == null) {
		return "Error retrieving details";
	}
	if ($pc->getEmailForward() == "1" && $email != $pc->getEmail()) {
		return "You have requested to change your email address and have an email forward enabled. You must first disable your email forward before changing your email address then re-enable it afterwards.";
	}

	if ($showforward != "1") {
		$showforward = "0";
	}

        if ((!ereg(".+\@.+\..+", $email)) || (!ereg("^[a-zA-Z0-9_@.-]+$", $email))) {
               return "Email address is not valid: $email";
        }

	if ($pid == 0) {
		return "Unexpected Error";
	}
	if ($password != "") {
		if ($password != $password2) {
			return "Sorry, your passwords do not match";
		}
		$passwd = md5($password);
		$query = "update users set user_password = \"" . $passwd . "\" where user_id = " . $pid;
		mysql_query($query);
	}
        $sig = chop($sig); // Strip all trailing whitespace.
        $sig = addslashes($sig);
        $website = trim($website);
        if(substr(strtolower($website), 0, 7) != "http://") {
                $website = "http://" . $website;
        }
        if($website == "http://") {
                $website = "";
        }
        $website = addslashes($website);
        // Check if the ICQ number only contains digits
        $icq = (ereg("^[0-9]+$", $icq)) ? $icq : '';
        $aim = addslashes($aim);
        $yim = addslashes($yahoo);
        $msnm = addslashes($msn);
	$occ = addslashes($occ);

	$blurb = mysql_escape_string($blurb);
        if($showemail == "1") {
                $sqlviewemail = "1";
	} else {
		$sqlviewemail = "0";
	}
	if ($showblurb == "1") {
		$sqlviewblurb = "1";
	} else {
		$sqlviewblurb = "0";
	}
	$sql ="update users set user_email = '$email', user_icq = '$icq', user_website = '$website', user_aim = '$aim', user_viewemail = '$sqlviewemail', user_yim='$yim', user_msnm='$msnm', user_game_type = '$gametype', user_viewblurb='$sqlviewblurb', user_blurb='$blurb', user_sig='$sig', user_occ='$occ', user_showforward = '$showforward' where user_id='$pid'";
	mysql_query($sql);
	return "OK";
}

function doActivatePassword($actkey) {
	$actkey = mysql_escape_string($actkey);
	$query = "update users set user_password = user_newpasswd where user_actkey = '$actkey'";
	mysql_query($query);
	if (mysql_affected_rows() != 1) {
		return "Sorry, this activation key is invalid.";
	}
	return "OK";
}

function doLostPassword($username, $email) {
	$sqlusername = mysql_escape_string($username);
	$sqlemail = mysql_escape_string($email);
	$query = "SELECT user_id from users where lower(username) = '$sqlusername'  and lower(user_email) = '$sqlemail'";
	$result = mysql_query($query);
	if (mysql_numrows($result) != 1) {
		return "Sorry, The email address you entered does not match the one stored in our database.";
	} else {
		doForumsLostPassword(mysql_result($result, 0, "user_id"), $username, $email);
	}
	return "OK";

}

#############################################################################
# FUNCTION:     isPlayerInTeam 
# AUTHOR:       John Morrison
# DATE:         01/07/22
# RETURNS:      If TRUE if a player is a member of a team or not 
# PURPOSE:
#  To determine if a player is a member of a team
#
# PARAMETERS:
# playerName - The player name to test
# team       - The team to test against.
#############################################################################
function isPlayerInTeam($playerName, $team) {
	$pid = getPid($playerName);
	if ($pid == 0 ) {
		return false;
	}
	$tid = getTid($team);
	if ($tid == -1) {
		return false;
	}
	
	$query = "SELECT pid from team_members where pid = '$pid' and tid = '$tid'";
	$result = mysql_query($query);
	if (mysql_numrows($result) == 1) {
		return true;
	}
	return false;
}

#############################################################################
# FUNCTION:     checkPassword
# AUTHOR:       John Morrison
# DATE:         01/06/24
# RETURNS:      true if valid, false otherwise
# PURPOSE:
# Checks the login id and password to see if they are a valid combination
#
# PARAMETERS:
# name     - name to check
# password - Password to check against name
#############################################################################
function checkPassword($name, $password, $passwordType) {
	$name = mysql_escape_string($name);
	$password = stripslashes($password);
	if ($passwordType == "PLAYER") {
		$query = "SELECT user_id from users where lower(username) = '$name' and user_password = \"" . md5($password) . "\"";
	} elseif ($passwordType == "JOINTEAM") {
		$password = mysql_escape_string($password);
		$query = "select tid from stats_team where lower(name) = '$name' and join_password = '$password'";
	} else {
		// Edit team
		$password = mysql_escape_string($password);
		$query = "select tid from stats_team where lower(name) = '$name' and owner_password = '$password'";
	}
	$result = mysql_query($query);
	if (mysql_numrows($result) == 1) {
		return true; 
	}
	return false;
}

##############################################################################
# FUNCTION:     getGameLogWithServer
# AUTHOR:       John Morrison
# DATE:         02/04/10
# RETURNS:
# PURPOSE:
# Gets a gameEventClass for a server key. 
#
# PARAMETERS:
# limit - Maximum number of games to get 
#############################################################################
function getGameLogWithServer($limit) {
	processRemoveStaleServers();
	$ea = array();
	$time = time() - (5 * 60);
	$query = "SELECT game_server.gs_ip, game_server.gs_locked, game_server.gs_version, game_server.gs_gametype, game_event.ge_serverkey, game_server.gs_port, game_event.ge_eventtype, game_event.ge_timestamp, game_event.ge_playera, game_event.ge_playerb, active_player.ap_pid, users.username as playera from game_event left join active_player on game_event.ge_playera = active_player.ap_key left join users on users.user_id = active_player.ap_pid left join game_server on game_event.ge_serverkey = gs_key where game_event.ge_eventtype <> " . WINBOLO_NET_EVENT_WIN . " and game_event.ge_timestamp >= $time order by game_event.ge_timestamp desc limit $limit";
	$result = mysql_query($query);
	$numRows = mysql_numrows($result);
	$count = 0;
	while ($count < $numRows) {
		$event = new EventServerClass();
		$playerBKey = mysql_result($result, $count, "ge_playerb");
		$playerB = null;
		$key = mysql_result($result, $count, "ge_serverkey");
		if ($playerBKey != null) {
			$sql = "SELECT u.username from users u, active_player p where u.user_id = p.ap_pid and p.ap_key = '$playerBKey' and p.ap_serverkey = '$key'";
			$playerBResult = mysql_query($sql);
			$playerB = mysql_result($playerBResult, 0, "username");
			mysql_free_result($playerBResult);
		}
		$event->setup(mysql_result($result, $count, "ge_eventtype"), mysql_result($result, $count, "ge_timestamp"), mysql_result($result, $count, "playera"), $playerB);
		$event->setIp(mysql_result($result, $count, "gs_ip"), mysql_result($result, $count, "gs_port"), null);
		$event->setLocked(mysql_result($result, $count, "gs_locked"));
		$event->setVersion(mysql_result($result, $count, "gs_version"));
		$event->setKey($key);
		$event->setGameType(mysql_result($result, $count, "gs_gametype"));
		$ea[$count] = $event;
		$count++;
	}
	return $ea;
}

##############################################################################
# FUNCTION:     getGameLog
# AUTHOR:       John Morrison
# DATE:         02/04/07
# RETURNS:
# PURPOSE:
# Gets a gameEventClass for a server key. 
#
# PARAMETERS:
# key  - Server key to get from 
#############################################################################
function getGameLog($key) {
	$ea = array();
	$key = mysql_escape_string($key);
	$query = "SELECT game_event.ge_eventtype, game_event.ge_timestamp, game_event.ge_playera, game_event.ge_playerb, active_player.ap_pid, users.username as playera from game_event left join active_player on game_event.ge_playera = active_player.ap_key left join users on users.user_id = active_player.ap_pid  where game_event.ge_serverkey='$key' and game_event.ge_eventtype <> " . WINBOLO_NET_EVENT_WIN . " order by game_event.ge_timestamp desc";
	$result = mysql_query($query);
	$numRows = mysql_numrows($result);
	$count = 0;
	while ($count < $numRows) {
		$event = new EventClass();
		$playerB = mysql_result($result, $count, "ge_playerb");
		if ($playerB != null) {
			$sql = "SELECT u.username from users u, active_player p where u.user_id = p.ap_pid and p.ap_key = '$playerB' and p.ap_serverkey = '$key'";
			$playerBResult = mysql_query($sql);
			$playerB = mysql_result($playerBResult, 0, "username");
			mysql_free_result($playerBResult);
		}
		$event->setup(mysql_result($result, $count, "ge_eventtype"), mysql_result($result, $count, "ge_timestamp"), mysql_result($result, $count, "playera"), $playerB);
		$ea[$count] = $event;
		$count++;
	}

	return $ea;
}

##############################################################################
# FUNCTION:     getGameComments
# AUTHOR:       John Morrison
# DATE:         04/09/23
# RETURNS:
# PURPOSE:
# Gets the list of game comments for this game server
#
# PARAMETERS:
# key  - Server key to get from 
#############################################################################
function getGameComments($key) {
	$key = mysql_escape_string($key);
	$rv = array();
	$count = 0;
	$sql = "select l.*, u.username from log_comment l, users u where u.user_id = l.lc_pid and l.lc_key = '$key' order by l.lc_timestamp asc";
	$result = mysql_query($sql);
	$total = mysql_num_rows($result);
	while ($count < $total) {
		$v = new mapCollectionCommentClass();
		$v->setup(mysql_result($result, $count, "lc_timestamp"), mysql_result($result, $count, "lc_comment"), mysql_result($result, $count, "lc_pid"), mysql_result($result, $count, "lc_rating"));
		$v->setName(mysql_result($result, $count, "username"));
		$rv[$count] = $v;
		$count++;
	}
	return $rv;
}


##############################################################################
# FUNCTION:     getArchivedGameLog
# AUTHOR:       John Morrison
# DATE:         03/02/04
# RETURNS:
# PURPOSE:
# Gets a gameEventClass for a server key from the archived table. 
#
# PARAMETERS:
# key  - Server key to get from 
#############################################################################
function getArchivedGameLog($key) {
	$ea = array();
	$key = mysql_escape_string($key);
	$query = "SELECT archived_game_event.age_eventtype, archived_game_event.age_timestamp, archived_game_event.age_playera, archived_game_event.age_playerb , users.username as playera from archived_game_event left join users on users.user_id = archived_game_event.age_playera  where archived_game_event.age_serverkey='$key' and archived_game_event.age_eventtype <> " . WINBOLO_NET_EVENT_WIN . " order by archived_game_event.age_timestamp desc";
	$result = mysql_query($query);
	if (!$result) {
		return null;
	}
	$numRows = mysql_num_rows($result);
	$count = 0;
	while ($count < $numRows) {
		$event = new EventClass();
		$playerB = mysql_result($result, $count, "age_playerb");
		if ($playerB != null) {
			$sql = "SELECT username from users where user_id = '$playerB'";
			$playerBResult = mysql_query($sql);
			$playerB = mysql_result($playerBResult, 0, "username");
			mysql_free_result($playerBResult);
		}
		$event->setup(mysql_result($result, $count, "age_eventtype"), mysql_result($result, $count, "age_timestamp"), mysql_result($result, $count, "playera"), $playerB);
		$ea[$count] = $event;
		$count++;
	}

	return $ea;
}
##############################################################################
# FUNCTION:     getLogViewerGameInfo
# AUTHOR:       John Morrison
# DATE:         03/08/10
# RETURNS:
# PURPOSE:
# Gets a serverInfoClass for a log viewer link request on $key. Really just a
# copy and past of getArchivedGameInfo() function with changes to ignore time
# and unneeded host/port combination. Note: One day really make it a wrapper
#
# PARAMETERS:
# key  - Key of the game
#############################################################################
function getLogViewerGameInfo($key) {
	$sqlKey = mysql_escape_string($key);
	$query = "SELECT * from archived_game_server where ags_key='$sqlKey'";
	$result = mysql_query($query);
	if (mysql_numrows($result) != 1) {
		return null;
	}
	$playerSql = "SELECT distinct age.age_playera, u.username from archived_game_event age, users u where u.user_id = age.age_playera and age.age_serverkey = '$key'";
	$playersResult = mysql_query($playerSql);
	$players = array();
	$count2 = 0;
	while ($count2 < mysql_numrows($playersResult)) {
		$players[$count2] = mysql_result($playersResult, $count2, "username");
		$count2++;
	}
	$cs = new ServerInfoClass();
	$cs->setup($key, mysql_result($result, 0, "ags_ip"), mysql_result($result, 0, "ags_port"),mysql_result($result, 0, "ags_mapname"),mysql_result($result, 0, "ags_password"),mysql_result($result, 0, "ags_mines"),mysql_result($result, 0, "ags_ai"),mysql_result($result, 0, "ags_gametype"),  mysql_result($result, 0, "ags_numbases"),mysql_result($result, 0, "ags_numpills") , mysql_result($result, 0, "ags_freebases"),mysql_result($result, 0, "ags_freepills"), mysql_result($result, 0, "ags_numtanks"), mysql_result($result, 0, "ags_starttime"), mysql_result($result, 0, "ags_firstheard"), mysql_result($result, 0, "ags_lastheard"), $players, mysql_result($result, 0, "ags_version"), 'F');
	$cs->setLogAvailable(mysql_result($result, 0, "ags_log_available"));
	$cs->setLogSize(mysql_result($result, 0, "ags_log_size"));
	$cs->setLogNumDownloads(mysql_result($result, 0, "ags_log_downloads"));
	$cs->setLogRating(mysql_result($result, 0, "ags_log_rating"));
	$cs->setLogNumRatings(mysql_result($result, 0, "ags_log_numratings"));
	return $cs;
}

##############################################################################
# FUNCTION:     getArchivedGameInfo
# AUTHOR:       John Morrison
# DATE:         03/02/03
# RETURNS:
# PURPOSE:
# Gets a serverInfoClass for activer server on $ip and $port. Returns null
# on error.
#
# PARAMETERS:
# key  - Key of the game
#############################################################################
function getArchivedGameInfo($key) {
	$cs = new ServerInfoClass();
	$key = mysql_escape_string($key);
	$query = "SELECT * from archived_game_server where ags_key='$key'";

	$result = mysql_query($query);
	if (mysql_numrows($result) != 1) {
		return null;
	}
	$playerSql = "SELECT distinct age.age_playera, u.username from archived_game_event age, users u where u.user_id = age.age_playera and age.age_serverkey = '$key'";
	$playersResult = mysql_query($playerSql);
	$players = array();
	$count2 = 0;
	while ($count2 < mysql_numrows($playersResult)) {
		$players[$count2] = mysql_result($playersResult, $count2, "username");
		$count2++;
	}

	$cs->setup($key, mysql_result($result, 0, "ags_ip"), mysql_result($result, 0, "ags_port"),mysql_result($result, 0, "ags_mapname"),mysql_result($result, 0, "ags_password"),mysql_result($result, 0, "ags_mines"),mysql_result($result, 0, "ags_ai"),mysql_result($result, 0, "ags_gametype"),  mysql_result($result, 0, "ags_numbases"),mysql_result($result, 0, "ags_numpills") , mysql_result($result, 0, "ags_freebases"),mysql_result($result, 0, "ags_freepills"), mysql_result($result, 0, "ags_numtanks"), mysql_result($result, 0, "ags_starttime"), mysql_result($result, 0, "ags_firstheard"), mysql_result($result, 0, "ags_lastheard"), $players, mysql_result($result, 0, "ags_version"), 'F');
	$cs->setLogAvailable(mysql_result($result, 0, "ags_log_available"));
	$cs->setLogSize(mysql_result($result, 0, "ags_log_size"));
	$cs->setLogNumDownloads(mysql_result($result, 0, "ags_log_downloads"));
	$cs->setLogRating(mysql_result($result, 0, "ags_log_rating"));
	$cs->setLogNumRatings(mysql_result($result, 0, "ags_log_numratings"));
	return $cs;
}

##############################################################################
# FUNCTION:     getGameInfo
# AUTHOR:       John Morrison
# DATE:         02/04/07
# RETURNS:
# PURPOSE:
# Gets a serverInfoClass for activer server on $ip and $port. Returns null
# on error.
#
# PARAMETERS:
# ip   - Ip or the server to lookup
# port - Port of the server to get
#############################################################################
function getGameInfo($ip, $port, $key) {
	$port = mysql_escape_string($port);
	$ip = mysql_escape_string($ip);
	$key = mysql_escape_string($key);
	$cs = new ServerInfoClass();
	$query = "SELECT * from game_server where gs_ip='$ip' and gs_port='$port' and gs_key = '$key'";
	$result = mysql_query($query);
	if (mysql_numrows($result) != 1) {
		return null;
	}
	$playerSql = "SELECT p.ap_pid, u.username from active_player p, users u where u.user_id = p.ap_pid and p.ap_serverkey = '$key' and p.ap_left = 0";
	$playersResult = mysql_query($playerSql);
	$players = array();
	$count2 = 0;
	while ($count2 < mysql_numrows($playersResult)) {
		$players[$count2] = mysql_result($playersResult, $count2, "username");
		$count2++;
	}

	$cs->setup($key, mysql_result($result, 0, "gs_ip"), mysql_result($result, 0, "gs_port"),mysql_result($result, 0, "gs_mapname"),mysql_result($result, 0, "gs_password"),mysql_result($result, 0, "gs_mines"),mysql_result($result, 0, "gs_ai"),mysql_result($result, 0, "gs_gametype"),  mysql_result($result, 0, "gs_numbases"),mysql_result($result, 0, "gs_numpills") , mysql_result($result, 0, "gs_freebases"),mysql_result($result, 0, "gs_freepills"), mysql_result($result, 0, "gs_numtanks"), mysql_result($result, 0, "gs_starttime"), mysql_result($result, 0, "gs_firstheard"), mysql_result($result, 0, "gs_lastheard"), $players, mysql_result($result, 0, "gs_version"), mysql_result($result, 0, "gs_locked"));
	return $cs;
}

##############################################################################
# FUNCTION:     getActiveGames
# AUTHOR:       John Morrison
# DATE:         02/03/31
# RETURNS:
# PURPOSE:
# Gets a list of active games tracked on WinBolo.net
#
# PARAMETERS:
#
#############################################################################
function getActiveGames() {
	processRemoveStaleServers();
	$ag = array();
	$count = 0;
	$cs = new ServerInfoClass();
	
	$query = "SELECT * from game_server";
	$result = mysql_query($query);	
        while ($count <mysql_numrows($result)) {
		# Get Players
		$key = mysql_result($result, $count, "gs_key");
		$playerSql = "SELECT p.ap_pid, u.username from active_player p, users u where u.user_id = p.ap_pid and p.ap_serverkey = '$key' and p.ap_left = 0";
		$playersResult = mysql_query($playerSql);
		$players = array();
		$count2 = 0;
		while ($count2 < mysql_numrows($playersResult)) {
			$players[$count2] = mysql_result($playersResult, $count2, "username");
			$count2++;
		}
		$cs = new ServerInfoClass();
		$cs->setup(mysql_result($result, $count, "gs_key"), mysql_result($result, $count, "gs_ip"), mysql_result($result, $count, "gs_port"),mysql_result($result, $count, "gs_mapname"),mysql_result($result, $count, "gs_password"),mysql_result($result, $count, "gs_mines"),mysql_result($result, $count, "gs_ai"),mysql_result($result, $count, "gs_gametype"),  mysql_result($result, $count, "gs_numbases"),mysql_result($result, $count, "gs_numpills") , mysql_result($result, $count, "gs_freebases"),mysql_result($result, $count, "gs_freepills"), mysql_result($result, $count, "gs_numtanks"), mysql_result($result, $count, "gs_starttime"), mysql_result($result, $count, "gs_firstheard"), mysql_result($result, $count, "gs_lastheard"), $players, mysql_result($result, $count, "gs_version"), mysql_result($result, $count, "gs_locked"));
		$ag[$count] = $cs;
		$count++;
	}
							
	return $ag;
}

##############################################################################
# FUNCTION:     getArchivedGames
# AUTHOR:       John Morrison
# DATE:         03/02/03
# RETURNS:
# PURPOSE:
# Gets a list of games that have finished but are played in the last 24 hours
# and tracked on WinBolo.net
# Now returns a PartialServerList so that the number of games in the entire
# archived games list can be returned also.  This number is required to
# print out the nifty page selector
#
# PARAMETERS:
# $start - start index
#############################################################################
function getArchivedGames($start) {
	$ag = array();
	$count = $min;
	$cs = new ServerInfoClass();

	global $cfg_ServersPerPage;
	global $cfg_MaxServersSelect;

	$min = $start;
	$max = $start+$cfg_ServersPerPage;

	$time = time() - (60 * 60 * 24);
	$query = "SELECT * from archived_game_server where ags_lastheard >= '$time' order by ags_lastheard desc limit 0,$cfg_MaxServersSelect";
	$result = mysql_query($query);	

	$numRows = mysql_numrows($result);
	if ($numRows == 0) {
		return null;
	}
	if ($min > $numRows) {
		$min = $numRows-1;
		$max = $min+1;
	} else {
		if ($numRows < $max) {
			$max = $numRows;
		}
	}

	$arrayIndex = 0;
	$count = $min;
        while ($count < $max) {
		# Get Players
		$key = mysql_result($result, $count, "ags_key");
		$playerSql = "SELECT distinct age.age_playera, u.username from archived_game_event age, users u where u.user_id = age.age_playera and age.age_serverkey = '$key'";
		$playersResult = mysql_query($playerSql);
		$players = array();
		$count2 = 0;
		while ($count2 < mysql_numrows($playersResult)) {
			$players[$count2] = mysql_result($playersResult, $count2, "username");
			$count2++;
		}
		mysql_free_result($playersResult);
		$cs = new ServerInfoClass();
		$cs->setup(mysql_result($result, $count, "ags_key"), mysql_result($result, $count, "ags_ip"), mysql_result($result, $count, "ags_port"),mysql_result($result, $count, "ags_mapname"),mysql_result($result, $count, "ags_password"),mysql_result($result, $count, "ags_mines"),mysql_result($result, $count, "ags_ai"),mysql_result($result, $count, "ags_gametype"),  mysql_result($result, $count, "ags_numbases"),mysql_result($result, $count, "ags_numpills") , mysql_result($result, $count, "ags_freebases"),mysql_result($result, $count, "ags_freepills"), mysql_result($result, $count, "ags_numtanks"), mysql_result($result, $count, "ags_starttime"), mysql_result($result, $count, "ags_firstheard"), mysql_result($result, $count, "ags_lastheard"), $players, mysql_result($result, $count, "ags_version"), 'F');
		$cs->setLogAvailable(mysql_result($result, $count, "ags_log_available"));
		$cs->setLogSize(mysql_result($result, $count, "ags_log_size"));
		$cs->setLogNumDownloads(mysql_result($result, $count, "ags_log_downloads"));
		$cs->setLogRating(mysql_result($result, $count, "ags_log_rating"));
		$cs->setLogNumRatings(mysql_result($result, $count, "ags_log_numratings"));
		$ag[$arrayIndex] = $cs;
		$count++;
		$arrayIndex++;
	}
	$ret = new PartialServerListClass();
	$ret->setup($numRows, $ag);
	mysql_free_result($result);
	return $ret;
}

#############################################################################
# FUNCTION:	getTeamClass
# AUTHOR:	John Morrison
# DATE:		01/07/22
# RETURNS:	The teamClass or NULL if the team doesn't exist 
# PURPOSE:	
# Retrieves a teams details 
#
# PARAMETERS:
# team -	The WinBolo.net team 
#############################################################################
function getTeamClass($team) {
	$tid = getTid($team);
	if ($tid == -1) {
		return 0;
	}
	$tournRank = 1;
	$strictRank = 1;
	$query = "SELECT t.name, u.username, t.blurb, t.open_num_games, t.open_num_bases, t.open_num_pills, t.open_num_tanks, t.tournnum_games, t.tourn_score, t.tourn_wins, t.tourn_loses, t.tourn_num_bases, t.tourn_num_pills, t.tourn_num_tanks, t.strict_num_games, t.strict_wins, t.strict_loses, t.strict_num_bases, t.strict_num_pills, t.strict_num_tanks, t.strict_score, t.regdate, t.website from stats_team t, users u where t.owner = u.user_id and t.tid = " . $tid;
	$result = mysql_query($query);
	$tc = new TeamInfoClass;
	$tc->setStuff(mysql_result($result, 0, "name"), mysql_result($result, 0, "username"), mysql_result($result, 0, "blurb"), mysql_result($result, 0, "open_num_games"), mysql_result($result, 0, "open_num_pills"), mysql_result($result, 0, "open_num_bases"), mysql_result($result, 0, "open_num_tanks"), mysql_result($result, 0, "tournnum_games"), mysql_result($result, 0, "tourn_wins"), mysql_result($result, 0, "tourn_loses"), mysql_result($result, 0, "tourn_score"), mysql_result($result, 0, "tourn_num_pills"), mysql_result($result, 0, "tourn_num_bases"), mysql_result($result, 0, "tourn_num_tanks"), $tournRank, mysql_result($result, 0, "strict_num_games"), mysql_result($result, 0, "strict_score"), mysql_result($result, 0, "strict_wins"), mysql_result($result, 0, "strict_loses"), mysql_result($result, 0, "strict_num_pills"), mysql_result($result, 0, "strict_num_bases"), mysql_result($result, 0, "strict_num_tanks"), $strictRank, mysql_result($result, 0, "website"), mysql_result($result, 0, "regdate"));
	$players = array();
	$query = "select u.username from users u, team_members m where u.user_id = m.pid and m.tid = '$tid' order by u.username";
	$result = mysql_query($query);
        $count = 0;
	while ($count <mysql_numrows($result)) {
		$players[$count] = mysql_result($result, $count, "username");
		$count++;
	}
	$tc->setPlayers($players);
	$tc->setTournRank(getRank("stats_team", "tourn_score", $tc->getTournScore(), $tid, "tid", "tourn"));
        $tc->setStrictRank(getRank("stats_team", "strict_score", $tc->getStrictScore(), $tid, "tid", "strict"));

	$tc->setPageViews(getPageViews("view_team", $tid));
	return $tc;	
}

#############################################################################
# FUNCTION:	getPageViews
# AUTHOR:	John Morrison
# DATE:		01/06/17
# RETURNS:	The number of times this page has been viewed. 
# PURPOSE:	
# Increments and returns the page view count for this page. 
#
# PARAMETERS:
# table - Table name to extract from
# pid   - Id to get
#############################################################################
function getPageViews($table, $id) {
	$sql = "update $table set view_count = view_count + 1 where id = $id";
	mysql_query($sql);
	$sql = "select view_count from $table where id = $id";
	$res = mysql_query($sql);
	if ($res) {
		if (mysql_num_rows($res) == 1) {
			return mysql_result($res, 0, 0);
		}
	}

	return 0;
}

#############################################################################
# FUNCTION:	getPlayerClass
# AUTHOR:	John Morrison
# DATE:		01/06/17
# RETURNS:	The playerClass or NULL if the player doesn't exist 
# PURPOSE:	
# Retrieves a login id's pid
#
# PARAMETERS:
# playerName -	The WinBolo.net login
#############################################################################
function getPlayerClass($playerName) {
	$pid = getPid($playerName);
	if ($pid == 0) {
		return 0;
	}
	$tournRank = 1; 
	$strictRank = 1;
	$query = "SELECT u.username, u.user_onirc, u.user_email, u.user_game_type, u.user_blurb, u.user_viewblurb, u.user_viewemail, s.open_num_games, s.open_num_bases, s.open_num_pills, s.open_num_tanks, s.tourn_num_games, s.tourn_score, s.tourn_wins, s.tourn_loses, s.tourn_num_bases, s.tourn_num_pills, s.tourn_num_tanks, s.strict_num_games, s.strict_wins, s.strict_loses, s.strict_num_bases, s.strict_num_pills, s.strict_num_tanks, s.strict_score, u.user_regdate, u.user_icq, u.user_aim, u.user_website, u.user_occ, u.user_from, u.user_yim, u.user_msnm, u.user_sig, u.user_emailforward, u.user_showforward from stats_player s, users u where s.pid = u.user_id and u.user_id = " . $pid;
	$result = mysql_query($query);
	$pc = new PlayerInfoClass;

	$pc->setStuff($pid, mysql_result($result, 0, "username"), mysql_result($result, 0, "user_email"), mysql_result($result, 0, "user_game_type"), mysql_result($result, 0, "user_blurb"), mysql_result($result, 0, "user_viewblurb"), mysql_result($result, 0, "user_viewemail"), mysql_result($result, 0, "open_num_games"), mysql_result($result, 0, "open_num_pills"), mysql_result($result, 0, "open_num_bases"), mysql_result($result, 0, "open_num_tanks"), mysql_result($result, 0, "tourn_num_games"), mysql_result($result, 0, "tourn_wins"), mysql_result($result, 0, "tourn_loses"), mysql_result($result, 0, "tourn_score"), mysql_result($result, 0, "tourn_num_pills"), mysql_result($result, 0, "tourn_num_bases"), mysql_result($result, 0, "tourn_num_tanks"), $tournRank, mysql_result($result, 0, "strict_num_games"), mysql_result($result, 0, "strict_score"), mysql_result($result, 0, "strict_wins"), mysql_result($result, 0, "strict_loses"), mysql_result($result, 0, "strict_num_pills"), mysql_result($result, 0, "strict_num_bases"), mysql_result($result, 0, "strict_num_tanks"), $strictRank, mysql_result($result, 0, "user_icq"), mysql_result($result, 0, "user_aim"), mysql_result($result, 0, "user_yim"), mysql_result($result, 0, "user_website"), mysql_result($result, 0, "user_occ"), mysql_result($result, 0, "user_msnm"),  mysql_result($result, 0, "user_regdate"), mysql_result($result, 0, "user_sig"), mcHasTopLevelCollection($pid), mysql_result($result, 0, "user_emailforward"), mysql_result($result, 0, "user_showforward"));

        $pc->setOnIrc(mysql_result($result, 0, "user_onirc"));
	// Get number of posts
	$query = "SELECT count(*) AS total FROM posts where poster_id = " . $pid;
	$result = mysql_query($query);
	$pc->setNumPosts(mysql_result($result, 0, "total"));
	if ($pc->getNumPosts() > 0) {
		// Get Posting statistics
		$result = mysql_query("SELECT count(*) AS total FROM posts");
		$pc->setPercentPosts($pc->getNumPosts() / mysql_result($result, 0,"total") * 100);

		if ( ((time()-strtotime($pc->getRegisteredDate())) /(24*60*60)) < 1) {
			$pc->setPostsPerDay($pc->getNumPosts());
		} else {
			$pc->setPostsPerDay($pc->getNumPosts()/((time()-strtotime($pc->getRegisteredDate()))/(24*60*60)));
		}
		mysql_free_result($result);
	}
	$teams = array();
	$query = "select t.name from stats_team t, team_members m where t.tid = m.tid and m.pid = '$pid' order by t.name";
	$result = mysql_query($query);
        $count = 0;
	while ($count <mysql_numrows($result)) {
		$teams[$count] = mysql_result($result, $count, "name");
		$count++;
	}
	$pc->setTeams($teams);
	// Get ranks
	$pc->setTournRank(getRank("stats_player", "tourn_score", $pc->getTournScore(), $pid, "pid", "tourn"));
        $pc->setStrictRank(getRank("stats_player", "strict_score", $pc->getStrictScore(), $pid, "pid", "strict"));

	$pc->setPageViews(getPageViews("view_player", $pid));
	# Get Active Games
	$activeGames = array();
	$activeSql = "SELECT g.gs_ip, g.gs_port, g.gs_key from game_server g, active_player p where p.ap_left = 0 and p.ap_pid = '$pid' and g.gs_key = p.ap_serverkey";
	$activeResult = mysql_query($activeSql);
	$count = 0;
	while ($count < mysql_numrows($activeResult)) {
		$activeGames[$count] =  mysql_result($activeResult, $count , "gs_ip");
		$activeGames[$count+1] =  mysql_result($activeResult, $count , "gs_port");
		$activeGames[$count+2] =  mysql_result($activeResult, $count , "gs_key");
		$count += 3;
	}
	$pc->setActiveGames($activeGames);
	return $pc;
}

#############################################################################
# FUNCTION:     getLatestPosts
# AUTHOR:       John Morrison
# DATE:         03/09/18
# RETURNS:      The latest posts on the forum in an array of postClass
# PURPOSE:
# Gets the latest post
#
# PARAMETERS:
# numItems - number of posts to retrieve 
#############################################################################
function getLatestPosts($numItems) {
	$URLPath = "/forums";  //- Not Not Include Closing \ Mark!
	$posts = array();
	$query = "select t.topic_id, t.topic_title, u.username, p.post_id, p.post_time, pt.bbcode_uid, pt.post_text FROM users u, topics t, posts p, posts_text pt where t.topic_id = p.topic_id and pt.post_id = p.post_id and u.user_id = p.poster_id and t.forum_id <> 11 and t.forum_id <> 7 and t.forum_id <> 14 order by p.post_time desc limit $numItems";
	$result = mysql_query($query);
	$count = 0;
	$numRows = mysql_numrows($result);
	while ($count < $numRows) {
		$ni = new postClass;
		$postId = mysql_result($result, $count, "post_id");
		$url = "http://www.winbolo.net/forums/viewtopic.php?p=" . $postId . "#" . $postId;
		$text = mysql_result($result, $count, "post_text");
		#$text = ereg_replace ("\n", "<br>", mysql_result($result, $count, "post_text"));
		$text = bbencode_second_pass($text, mysql_result($result, $count, "bbcode_uid"));

		$ni->setup($url, mysql_result($result, $count, "topic_title"), mysql_result($result, $count, "username"), mysql_result($result, $count, "post_time"), $text);
		$posts[$count] = $ni;
		$count++;
	}
	return $posts;
}

#############################################################################
# FUNCTION:     doLatestPosts
# AUTHOR:       John Morrison
# DATE:         01/09/09
# RETURNS:      Shows the latest posts on the forum
# PURPOSE:
# Prints out the latest posts on the forum
#
# PARAMETERS:
# PostNumber - number of posts to retrieve 
#############################################################################
function doLatestPosts($PostNumber) {
	$URLPath = "/forums";  //- Not Not Include Closing \ Mark!
	
	$res = mysql_query("select count(*) from posts");
	$num = mysql_result($res, 0, 0) - 10;
	mysql_free_result($res);
	$sql = "SELECT p.post_id, t.topic_title, t.topic_id, u.username, f.forum_id FROM topics t, forums f, posts p, users u WHERE t.topic_id = p.topic_id AND f.forum_id = t.forum_id AND u.user_id = p.poster_id and t.forum_id != 11  and t.forum_id != 7 and t.forum_id != 14 and p.post_id > $num order by post_time DESC LIMIT $PostNumber";
	# original
#	 $sql = "SELECT p.post_id, t.topic_title, t.topic_id, u.username, f.forum_id FROM topics t, forums f, posts p, users u WHERE t.topic_id = p.topic_id AND f.forum_id = t.forum_id AND u.user_id = p.poster_id and t.forum_id != 11 order by post_id DESC LIMIT $PostNumber";
	if ($r = mysql_query($sql)) {
		while($m = mysql_fetch_array($r)) {
			$j = stripslashes($m[topic_title]);
			$k = substr($j, 0, 13) . "...";
			$l = $m[username];
			if (strlen($l) > 10) {
				$l = substr($m[username],0,10) . "...";
			}
			if ($l == "EIvis") { $l = "Eivis"; }
			echo '<tr><td class="bodytext"> <div align="center">';
			echo "<a href=\"$URLPath/viewtopic.php?p=$m[post_id]#$m[post_id]\">$k</a>";
			echo '</div></td><td class="bodytext"> <div align="center"><a href="playerinfo.php?name=' .  urlencode($m[username]) . '">' . $l . '</a></div></td></tr>';
		}
	}
}

#############################################################################
# FUNCTION:     doRandomRankTable
# AUTHOR:       John Morrison
# DATE:         01/09/09
# RETURNS:      Generate a random rank table for the margins.
# PURPOSE:
# Displays it
#
# PARAMETERS:
# 
#############################################################################
function doRandomRankTable() {
$limit = 10;
$title = "Score";
$rnd = mt_rand(1, 10);
$line1 = "";
switch ($rnd) {
	case 1:
		$heading = "Top Players";
		$line1 = "Tournament Games";
		$link = "playerinfo.php";
		$title = "Score";
		$sql = "SELECT u.username as name, p.tourn_score as score from users u, stats_player p where p.pid = u.user_id order by p.tourn_score  desc limit " . $limit;
		break;
	case 2:
		$heading = "Top Players";
		$line1 = "Strict Games";
		$link = "playerinfo.php";
		$title = "Score";
		$sql = "SELECT u.username as name, p.strict_score as score from users u, stats_player p where p.pid = u.user_id order by p.strict_score  desc limit " . $limit;
		break;
	case 3:
		$heading = "Top Teams";
		$line1 = "Tournament Games";
		$link = "teaminfo.php";
		$title = "Score";
		$sql = "SELECT name, tourn_score as score from stats_team order by tourn_score desc limit " . $limit;
		break;
	case 4:
		$heading = "Top Teams";
		$line1 = "Strict Games";
		$link = "teaminfo.php";
		$title = "Score";
		$sql = "SELECT name, strict_score as score from stats_team order by strict_score desc limit " . $limit;
		break;
	case 5:
		$heading = "Most Pill takes (Players)";
		$link = "playerinfo.php";
		$title = "Pills";
		$sql = "SELECT u.username as name, (tourn_num_pills + strict_num_pills + open_num_pills) as score from users u, stats_player p where p.pid = u.user_id order by score  desc limit " . $limit;
		break;
	case 6:
		$heading = "Most Bases (Players)";
		$link = "playerinfo.php";
		$title = "Bases";
		$sql = "SELECT u.username as name, (tourn_num_bases + strict_num_bases + open_num_bases) as score from users u, stats_player p where p.pid = u.user_id order by score  desc limit " . $limit;
		break;
	case 7:
		$heading = "Most Tank kills (Players)";
		$link = "playerinfo.php";
		$title = "Tank Kills";
		$sql = "SELECT u.username as name, (tourn_num_tanks + strict_num_tanks + open_num_tanks) as score from users u, stats_player p where p.pid = u.user_id order by score  desc limit " . $limit;
		break;
	case 8:
		$heading = "Most Pill takes (Teams)";
		$link = "teaminfo.php";
		$title = "Pills";
		$sql = "SELECT name, (tourn_num_pills + strict_num_pills + open_num_pills) as score from stats_team order by score desc limit " . $limit;
		break;
	case 9:
		$heading = "Most Bases (Teams)";
		$link = "teaminfo.php";
		$title = "Bases";
		$sql = "SELECT name, (tourn_num_bases + strict_num_bases + open_num_bases) as score from stats_team order by score desc limit " . $limit;
		break;
	case 10:
		$heading = "Most Tank kills (Teams)";
		$link = "teaminfo.php";
		$title = "Tank Kills";
		$sql = "SELECT name, (tourn_num_tanks + strict_num_tanks + open_num_tanks) as score from stats_team order by score desc limit " . $limit;
		break;

	}


	echo '<table width="194" border="0" align="right" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF"><tr> <td> <table width="194" border="0" cellspacing="0" cellpadding="0"><tr><td width="20"><img src="images/stleft.png" width="20" height="20"></td><td class="topborder" bgcolor="#3366CC"> <div class="heading">' . $heading;

	echo '</div></td><td width="20"><img src="images/stright.png" width="20" height="20"></td></tr></table></td></tr><tr>';
	echo '<td> <table width="194" border="0" cellspacing="0" cellpadding="0"><tr><td width="1" class="leftborder">&nbsp;</td><td width="282"><div align="center" class="bodytext">';
	if ($line1 != "") {
		echo "<b>$line1</b>";
	}
	echo '<table width="100%" cellspacing="2" cellpadding="0">';

	doRankTable($sql, $title, $limit, $link);
	echo '</table><!-- End of pill takes tabular data table --></td><td width="1" class="rightborder">&nbsp;</td></tr></table></td></tr><tr><td> <table width="194" border="0" cellspacing="0" cellpadding="0"><tr> <td width="20" valign="bottom"><img src="images/sbleft.gif" width="20" height="30"></td><td class="bottomborder">&nbsp;</td><td width="20" valign="bottom"><img src="images/sbright.gif" width="20" height="30"></td></tr></table></td></tr></table> <!-- End of pill takes table --></td></tr>';

}


function doRankTable($sql, $title, $limit, $link) {
$result = mysql_query($sql);
# array holding all rows found within the table
$num = mysql_numrows($result); 
# the actual number of rows held in the array $result. 
echo "<tr>";
#print header
echo '<tr><td class="subheading"> <div align="center">Name</div></td><td class="subheading"> <div align="center">Rank</div></td><td class="subheading"> <div align="center">' . $title . '</div></td></tr>';


$count = 0; 
$lastScore = 10000000;
$rank = 0;
$reallink = "$link?name=";
while ($count < $limit) { 
	echo "<tr>";
	if ($count < $num) {
		$name = mysql_result($result,$count,"name");
		$score = mysql_result($result,$count,"score");
		if ($score < $lastScore) {
			$rank += 1;
			$lastScore = $score;
		}
		$link = $reallink . urlencode($name);
		echo '<td class="bodytext"> <div align="center"><a href="' . $link . '">' . $name . '</a></div></td>';
		echo '<td class="bodytext"> <div align="center">' . $rank . '</div></td>';
		echo '<td class="bodytext"> <div align="center">' . $score . '</div></td>';
	} else {
		echo "<TD></TD><TD></TD><TD></TD>";
	}
	echo "</TR>";
	$count++; 
}
}


#############################################################################
# FUNCTION: 	doPlayerSignUp
# AUTHOR:	John Morrison
# DATE:		01/06/17
# RETURNS:	The string "OK" on success other wise an error message 
#		to display
# PURPOSE:	
# Adds a new player to the winbolo.net system. Checks parameters 
#
# PARAMETERS:
# username -	The WinBolo.net login
# password -	The password
# password2 -	The password retyped
# fullName - 	Full Name of the person
# email - 	Email Address of the persion
# country -	Country of the person
# platform -	The players platform
# gametype -	The players favourite game type
# blurb	-	Players blurb
# showemail -	Whether to show the players Email Address or not
# showblurb - 	Whether to show the blurb or not 
# ...
# confirm     -   Image confirmation check ID
# confirmCode -   Image confirmation entered code
# confirmVersion - Confirm the version of winbolo
#############################################################################
function doPlayerSignUp($login, $password, $password2, $fullname, $email, $country, $platform, $gametype, $blurb, $showemail, $showblurb, $occ, $sig, $interests, $website, $icq, $aim, $yim, $mimn, $confirm, $confirmCode, $confirmVersion) { 
	global $cfg_email_from_address;
	global $cfg_email_signup_subject;
	global $cfg_winbolodb;
	global $userdata;


	if ($confirmVersion != "1.16") {
		return "You answered the version number question wrong.";
	}


	$login = strip_tags($login);
	$login = trim($login);
	$login = stripslashes($login);
	$login = chop($login);

	if (strlen($login) == 0) {
		return "Missing name.";
	}

	$llogin = strtolower($login);
	if ($llogin == "john" || $llogin == "postmaster" || $llogin == "webmaster" || $llogin == "root" || $llogin == "non winbolo.net particip"  || $llogin == "dns" || $$llogin == "default" || $llogin == "MAILER-DAEMON") {
		return "Sorry, you can't have that name :)";
	}

	if (strstr($llogin, " ") != FALSE) {
		return "Sorry, WinBolo.net player names can no longer contain spaces.";
	}

	$password = stripslashes($password);
	$password2 = stripslashes($password2);
	$fullname = stripslashes($fullname);
	$email = stripslashes($email);
	$country = stripslashes($country);
	$platform = stripslashes($platform);
	$gametype = stripslashes($gametype);
	$blurb = stripslashes($blurb);
	$showemail = stripslashes($showemail);
	$showblurb = stripslashes($showblurb);
	$occ = stripslashes($occ);
	$sig = stripslashes($sig);
	$interests = stripslashes($interests);
	$aim = stripslashes($aim);
	$yim = stripslashes($yim);
	$mimn = stripslashes($mimn);
                                  
	if (trim($login) == "" || trim($email) == "" || trim($fullname) == "" || trim($password) == "") {
		return "Missing required information";
	}

	if ($password != $password2) {
		return "Passwords do not match";
	}

	$checkEmail = strstr($email, "@");
	if ($checkEmail == null) {
	        return "This does not look like a valid email address. Please try again.";
	}

        if ((!ereg(".+\@.+\..+", $email)) || (!ereg("^[a-zA-Z0-9_@.-]+$", $email))) {
		return "Your email address is not valid.";
        }

	#Check lengths of stuff
	if (strlen($login) > 32) {
		return "Your login is too long. Please choose another";
	}
	if (strlen($password) > 32) {
		return "Your password is too long. Please choose another";
	}
	if (strlen($email) > 255) {
		return "Your email address is too long. Do you have another you could use?";
	}

	if (strlen($fullname) > 255) {
		return "Your Full Name is too long. Maybe you could abbreviate it?";
	}

	if (doesPlayerExist($login) == true) {
		return "Login in use. Please select another";
	}

	if (stristr($login, "'") != false || stristr($login, "\"") != false || stristr($login, "\\") != false) {
		return "Sorry, player names can not contain \\ or ' or \" in them.";
	}

	#
	# Test confirmation code
	#
	$confirm = strip_tags($confirm);
	$confirm = htmlspecialchars($confirm);
	$confirm = trim($confirm);
	if (!preg_match('/^[A-Za-z0-9]+$/', $confirm)) {
		$confirm = '';
	}

	if (strlen($confirm) == 0) {
		return "Missing confirmation code";
	}
	$confirm = mysql_escape_string($confirm);
	$sql = "SELECT code FROM confirm WHERE confirm_id = '$confirm' AND session_id = '" . $userdata['session_id'] . "'";
	$r = mysql_query($sql);
	if (!$r) {
		return "Missing confirmation code";
	}
	$confirmCode = trim($confirmCode);
	if (mysql_result($r, 0, "code") != $confirmCode || strlen($confirmCode) == 0) {
		mysql_free_result($r);
		return "Incorrect confirmation code";
	}
	mysql_free_result($r);
	$sql = "DELETE FROM confirm WHERE confirm_id = '$confirm' AND session_id = '" . $userdata['session_id'] . "'";
	mysql_query($sql);

	#
	# Done confirm check... continue on all good
	#

 	$id = storeForumNames($cfg_winbolodb, $login, $password, $email, $sig, $occ, $country, $website, $icq, $aim, $yim, $mimn, "", $showemail, $gametype);
	if ($id < 0) {
		return "Database error";
	}
	$sql = "insert into view_player (id, view_count) values ($id, 0)";
	if (!mysql_query($sql)) {
		return "Database error";
	}

	$sql = "insert into stats_player (pid, tourn_score, strict_score, locked) values ($id, 1600, 1600, 'F')";
	if (!mysql_query($sql)) {
		return "Database error";
	}
	$blurb = trim($blurb);
	$blurb = normalize_whitespace($blurb);
	$blurb = chop($blurb);
	$blurb = mysql_escape_string($blurb);
	$regip = mysql_escape_string(getenv("REMOTE_ADDR"));
	$sql = "update users set user_blurb = '$blurb', user_viewblurb = $showblurb, user_registerip = '$regip' where user_id = $id";
	if (!mysql_query($sql)) {
		return "Database error";
	} else {
		$message = "Hi " . $fullname . ",\n\nWelcome to WinBolo.net!\n\nPlease keep this email for your personal records.\n\nWinbolo.net is a real time Winbolo game tracking and player statistics website. If you have any questions or problems please take a look at the Help pages and then try asking your questions in the forum.\n\nFor help with using Winbolo.net with your Winbolo client, see the howto page at http://www.winbolo.net/howtowbn.php\n\n";
		$message  .= "Your account information is as follows:\n";
		$message .= "--------------------------------------\n";
		$message .= "Username: $login\n";
		$message .= "Password: $password\n\n";
		$message .= "If you lose of forget your password you can request a new password by visiting WinBolo.net and going to the login/signup page.";
		$message .= "\r\n\n\n--------------------\nThanks and Welcome.\nWinbolo.net";
		mail($email, $cfg_email_signup_subject, $message, "From: $cfg_email_from_address");
	}
	
	return "OK";
}


#############################################################################
# FUNCTION:     doModifyTeam
# AUTHOR:       John Morrison
# DATE:         01/06/17
# RETURNS:      The string "OK" on success other wise an error message
#               to display
# PURPOSE:
# Modifies a team 
#
# PARAMETERS:
# teamName -      The team name
# editpassword -  The edit password (blank for no change(
# editpassword2 - The edit password retyped
# joinpassword -  The join password (blank for no change)
# joinpassword2 - The join password retyped
# blurb -         Team blurb   (optional)
# website -       Team website (optional)
# pid           - Pid to make sure we are on the team
#############################################################################
function doModifyTeam($teamName, $editpassword, $editpassword2, $joinpassword, $joinpassword2, $blurb, $website, $pid) {
	$tid = getTid($teamName);
	if ($tid == -1) {
		return "Error with team";
	}
	$editpassword = stripslashes($editpassword);
	$editpassword2 = stripslashes($editpassword2);
	$joinpassword = stripslashes($joinpassword);
	$joinpassword2 = stripslashes($joinpassword2);
	$blurb = stripslashes($blurb);

       $query = "SELECT pid from team_members where pid = '$pid' and tid = '$tid'";
	$result = mysql_query($query);
	if (mysql_numrows($result) != 1) {
		return "Error performing request - Only team members can modify teams";
	}
					       
	$needChangeEdit = false;
	if ($editpassword != "") {
		if (trim($editpassword) == "") {
			return "Error with passwords";
		}
		if ($editpassword != $editpassword2) {
			return "Edit passwords do not match";
		}

		$editpassword = mysql_escape_string($editpassword);
		$needChangeEdit = true;
	}

	$needChangeJoin = false;
	if ($joinpassword != "") {
		if (trim($joinpassword) == "") {
			return "Error with passwords";
		}
		if ($joinpassword != $joinpassword2) {
			return "Join passwords do not match";
		}
		$joinpassword = mysql_escape_string($joinpassword);
		$needChangeJoin = true;
	}

	$website = trim($website);
	if(substr(strtolower($website), 0, 7) != "http://") {
		$website = "http://" . $website;
	}
	if($website == "http://") {
		$website = "";
	}

	$blurb = mysql_escape_string($blurb);
	$website = mysql_escape_string($website);
	$sql = "update stats_team set website = '$website', blurb = '$blurb'";
	if ($needChangeJoin == true) {
		$sql .= ", join_password = '$joinpassword'";
	}
	if ($needChangeEdit == true) {
		$sql .= ", owner_password = '$editpassword'";
	}
	$sql .= " where tid = '$tid'";
	if (!mysql_query($sql)) {
 		return "Database error";
	}

	return "OK";
}

############################################################################
# FUNCTION:     doCreateTeam
# AUTHOR:       John Morrison
# DATE:         01/06/17
# RETURNS:      The string "OK" on success other wise an error message
#               to display
# PURPOSE:
# Creates a new team
#
# PARAMETERS:
# username -      The WinBolo.net login making the account
# teamName -      The team name
# editpassword -  The edit password (blank for no change(
# editpassword2 - The edit password retyped
# joinpassword -  The join password (blank for no change)
# joinpassword2 - The join password retyped
# blurb -         Team blurb   (optional)
# website -       Team website (optional)
#############################################################################
function doCreateTeam($login, $teamName, $editpassword, $editpassword2, $joinpassword, $joinpassword2, $blurb, $website) {

	if (stristr($teamName, "'") != false || stristr($teamName, "\"") != false || stristr($teamName, "\\") != false) {
		return "Sorry, team names can not contain \\ or ' or \" in them.";
	}
	$teamName = strip_tags($teamName);
	$teamName = trim($teamName);
	$teamName = normalize_whitespace($teamName);
	$teamName = chop($teamName);

	$editpassword = stripslashes($editpassword);
	$editpassword2 = stripslashes($editpassword2);
	$joinpassword = stripslashes($joinpassword);
	$joinpassword2 = stripslashes($joinpassword2);
	$blurb = stripslashes($blurb);

	if (trim($teamName) == "" || trim($joinpassword) == "" || trim($editpassword) == "") {
		return "Missing required information";
	}

	if ($joinpassword != $joinpassword2) {
		return "Join passwords do not match";
	}

	if ($editpassword != $editpassword2) {
		return "Edit passwords do not match";
	}
	if (strlen($teamName) > 32) {
		return "Your team name is too long";
	}
	if (doesTeamExist($teamName) == true) {
		return "Team with this name already exists";
	}

	$pid = getPid($login);
	if ($pid == 0) {
		return "Unexpected error";
	}

	$website = trim($website);
	if(substr(strtolower($website), 0, 7) != "http://") {
		$website = "http://" . $website;
	}
	if($website == "http://") {
		$website = "";
	}

	$sqlTeamName = mysql_escape_string($teamName);
	$joinpassword = mysql_escape_string($joinpassword);
	$editpassword = mysql_escape_string($editpassword);
	$blurb = mysql_escape_string($blurb);
	$website = mysql_escape_string($website);
	$regdate = date("M d, Y");

	$sql = "insert into stats_team (name, owner_password, join_password, owner, blurb, website, regdate, tourn_score, strict_score) values ('$sqlTeamName', '$editpassword', '$joinpassword', '$pid', '$blurb', '$website', '$regdate', 1600, 1600)";
	if (!mysql_query($sql)) {
 		return "Database error";
	}

	$tid = getTid($teamName);
	$sql = "insert into team_members values ('$tid', '$pid')";
	if (!mysql_query($sql)) {
		return "Database Error 2";
	}

        $sql = "insert into view_team (id, view_count) values ($tid, 0)";
        if (!mysql_query($sql)) {
                return "Database error";
        }

	return "OK";
}

?>
