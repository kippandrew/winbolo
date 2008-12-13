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

#require "/home/winbolo/www/wbn/errors.php";

#############################################################################
# FUNCTION:     processRemoveStaleServers
# AUTHOR:       John Morrison
# DATE:         02/04/06
# RETURNS:
# PURPOSE:
# Removes stale servers if they haven't updated in 5 minutes
#
# PARAMETERS:
#
#############################################################################
function processRemoveStaleServers() {
	global $link;

	$now = time() - (5 * 60);
	$sql = "select gs_key from game_server where gs_lastheard < $now";
	$result = mysql_query($sql);
	$count = 0;
	$len = mysql_num_rows($result);
	while ($count < $len) {
		processServerQuit(mysql_result($result, $count, 0));
		$count++;
	}
}

#############################################################################
# FUNCTION:     getScoreFrom
# AUTHOR:       John Morrison
# DATE:         03/02/16
# RETURNS:
# PURPOSE:
# Returns a score from a PID/TID
#
# PARAMETERS:
# id      - Player ID
# gameType - Game type prefix for the field name
# isTeam   - True if to come from the team table (false = player table)
#############################################################################
function getScore($id, $gameType, $isTeam) {
	$gameType = getGameType($gameType);
	$gameType = $gameType . "_score";
	if ($isTeam == true) {
		$sql = "select $gameType from stats_team where tid = '$id'";
	} else {
		$sql = "select $gameType from stats_player where pid = '$id'";
	}
	$result = mysql_query($sql);
	if (!$result) {
		return 0;
	}
	if (mysql_numrows($result) == 0) {
		return 0;
	}
	return mysql_result($result, 0, 0);
}

#############################################################################
# FUNCTION:     updateWinsLoses
# AUTHOR:       John Morrison
# DATE:         03/02/16
# RETURNS:
# PURPOSE:
# Updates a players wins/loses field by adding 1 to it for a PID/TID
#
# PARAMETERS:
# id       - PID/TID
# gameType - Game type prefix for the field name
# win      - True to increment win, falst to increment loss.
# isTeam   - True if a team game
#############################################################################
function updateWinsLoses($id, $gameType, $wins, $isTeam) {
	$gameType = getGameType($gameType);
	if ($wins == true) {
		$gameType = $gameType . "_wins";
	} else {
		$gameType = $gameType . "_loses";
	}
	if ($isTeam == true) {
		$sql = "update stats_team set $gameType = $gameType + 1 where tid = '$id'";
	} else {
		$sql = "update stats_player set $gameType = $gameType + 1 where pid = '$id'";
	}
	mysql_query($sql);
}

#############################################################################
# FUNCTION:     updateScore
# AUTHOR:       John Morrison
# DATE:         03/02/16
# RETURNS:
# PURPOSE:
# Updates a score for a PID/TID
#
# PARAMETERS:
# id       - PID/TID
# gameType - Game type prefix for the field name
# score    - The new score to set to 
# isTeam   - True if we are a team
#############################################################################
function setScore($id, $gameType, $newScore, $isTeam) {
	$gameType = getGameType($gameType);
	$gameType = $gameType . "_score";
	if ($isTeam == true) {
		$sql = "update stats_team set $gameType = $newScore where tid = '$id'";
	} else {
		$sql = "update stats_player set $gameType = $newScore where pid = '$id'";
	}
	$result = mysql_query($sql);
}

#############################################################################
# FUNCTION:     calculateELO
# AUTHOR:       John Morrison
# DATE:         03/02/16
# RETURNS:
# PURPOSE:
# Calculates the ELO score for two score.
#
# From: http://www.lgm.com/bolo/guides/math.html
#
#
#                    1
#r1 + K (w - (-----------------))
#	             (r2-r1)/400
#                1 + 10
#
#You may find it easier to understand in smaller pieces:
#p = win expectancy = 1/(1+10^[(r2-r1)/400])
#w = did you win? (1 if you win, 0.5 if you draw, 0 if you lose)
#K = your K-factor (maximum points you can win/lose per game)
#d = delta factor or rating change = K * (w - p)
#if abs(d) < 1, then d = 2w - 1
#
#K is the largest change your rating can experience as a result of the game.
#The value K=32 is used in the chess world, but we bumped it up by quite a
#bit (to about 100) to encourage more dynamic changes in ratings and thus IBL
#match-ups. To prevent top players from getting too many points, K is decreased
#to 0.75 K for players with ratings between 2100 and 2399, or 0.5 K for 
#players above 2400 points.
#
# PARAMETERS:
# r1 - First score
# r2 - Second score
# didWin - TRUE if R1 beat R2
#############################################################################
function calculateElo($r1, $r2, $didWin) {
	if ($didWin == true) {
		$w = 1;
	} else {
		$w = 0;
	}
	$k = 100;
	$p = 1 / (1 + pow(10, ($r2-$r1)/400));
	if ($r1 >= 2400) {
		$k = 50;
	} else if ($r1 >= 2100) {
		$k = 75;
	}
	$d = $k * ($w - $p);
	if (abs($d) < 1) {
		$d = (2 * $w) - 1;
	}
	return $r1 + $d;
}

#############################################################################
# FUNCTION:     calculateRanksTeams
# AUTHOR:       John Morrison
# DATE:         03/02/21
# RETURNS:
# PURPOSE:
# Calculates the new scores for team games. Updates the players scores, 
# number of wins and losses
#
# PARAMETERS:
# sqlServerKey - Server key of the game
# winnerKey    - Key of the player that won
# gameType     - Game type prefix for the field name
#############################################################################
function calculateRanksTeams($sqlServerKey, $winnerKey, $gameType) {
	# Check that there are only two teams
	$sql = "select at_team_number, count(*) as total from active_team where at_serverkey = '$sqlServerKey' group by at_team_number";
	$result = mysql_query($sql);
	if (!$result) {
		return;
	}
	if (mysql_num_rows($result) != 2) {
		return;
	}
	#Check each team as 2 players or more
	$count = 0;
	while ($count < 2) {
		if (mysql_result($result, $count, "total") < 2) {
			return;
		}
		$count++;
	}
	# Get Teams ID's
	$sql = "select at_team_number, at_pid from active_team where at_serverkey = '$sqlServerKey'";
	$result = mysql_query($sql);
	if (!$result) {
		return;
	}
	$teamCount1 = 0;
	$teamCount2 = 0;
	$count = 0;
	$total = mysql_num_rows($result);
	$sql1 = " where ";
	$sql2 = " where ";
	while ($count < $total) {
		if (mysql_result($result, $count, "at_team_number") == 1) {
			$teamCount1++;
			if ($teamCount1 != 1) {
				$sql1 .= " and ";
			}
			$sql1 .= "t" . $teamCount1 . ".pid = " . mysql_result($result, $count, "at_pid");
		} else {
			$teamCount2++;
			if ($teamCount2 != 1) {
				$sql2 .= " and ";
			}
			$sql2 .= "t" . $teamCount2 . ".pid = " . mysql_result($result, $count, "at_pid");
		}
		$count++;
	}
	#Get the first team ID
	$sql = "select t1.tid from ";
	$endSql = "";
	$count = 1;
	while ($count <= $teamCount1) {
		$endSql .= " and ";
		if ($count != 1) {
			$sql .= ",";
		}
		$sql .= " team_members t$count";
		$endSql .= " t1.tid = t$count.tid";
		$count++;
	}
	$sql .= " " . $sql1 . $endSql;

	$result = mysql_query($sql);
	if (!$result) {
		return;
	}
	$count = 0;
	$total = mysql_num_rows($result);
	if ($total < 1) {
		return;
	}
	$found = false;
	while ($found == false && $count < $total) {
		$tid1 = mysql_result($result, $count, "tid");
		$sql = "select count(*) as total from team_members where tid = $tid1";
		$result2 = mysql_query($sql);
		if (mysql_result($result2, 0, 0) == $teamCount1) {
			$found = true;
		}
		$count++;
	}
	if ($found == false) {
		return;
	}
	#Get the first second ID
	$sql = "select t1.tid from ";
	$endSql = "";
	$count = 1;
	while ($count <= $teamCount2) {
		$endSql .= " and ";
		if ($count != 1) {
			$sql .= ",";
		}
		$sql .= " team_members t$count";
		$endSql .= " t1.tid = t$count.tid";
		$count++;
	}
	$sql .= " " . $sql2 . $endSql;
	$result = mysql_query($sql);
	if (!$result) {
		return;
	}
	$count = 0;
	$total = mysql_num_rows($result);
	if ($total < 1) {
		return;
	}
	$found = false;
	while ($found == false && $count < $total) {
		$tid2 = mysql_result($result, $count, "tid");
		$sql = "select count(*) as total from team_members where tid = $tid1";
		$result2 = mysql_query($sql);
		if (mysql_result($result2, 0, 0) == $teamCount2) {
			$found = true;
		} 
		$count++;
	}
	if ($found == false) {
		return;
	}
	# Figure out which is the winner and which is the loser...
	$winnerKey = mysql_escape_string($winnerKey);
	$sql = "select at_pid from active_team where at_key = '$winnerKey' and at_serverkey = '$sqlServerKey'";
	$result = mysql_query($sql);
	if (!$result) {
		return;
	}

	if (mysql_num_rows($result) != 1) {
		$return;
	}
	$winningPid = mysql_result($result, 0, 0);
	$sql = "select tid from team_members where (tid = '$tid1' or tid = '$tid2') and pid = $winningPid";
	$result = mysql_query($sql);
	if (!$result) {
		return;
	}
	if (mysql_num_rows($result) != 1) {
		return;
	}
	if (mysql_result($result, 0, 0) == $tid1) {
		$winnerTid = $tid1;
		$loserTid = $tid2;
	} else {
		$winnerTid = $tid2;
		$loserTid = $tid1;
	}

	# We have the two team ids Rank them already...
	$winnerTid = $tid1;
	$loserTid = $tid2;
	# Get player Ranks
	$winnerScore = getScore($winnerTid, $gameType, true);
	$loserScore = getScore($loserTid, $gameType, true);
	
	# Calclulate New Scores 
	$newWinnerScore = calculateElo($winnerScore, $loserScore, true);
	$newLoserScore = calculateElo($loserScore, $winnerScore, false);
	# Update ranks
	setScore($winnerTid, $gameType, $newWinnerScore, true);
	setScore($loserTid, $gameType, $newLoserScore, true);
	updateWinsLoses($winnerTid, $gameType, true, true);
	updateWinsLoses($loserTid, $gameType, false, true);
	addTeamGames($gameType, $winnerTid);
	addTeamGames($gameType, $loserTid);

	# Lastly update number of bases/pills etc
	$gameType = getGameType($gameType);
	$sql = "select at.at_team_number, ge.ge_eventtype, count(*) as total from game_event ge, active_team at where at.at_serverkey='$sqlServerKey' and ge.ge_serverkey=at.at_serverkey and at.at_key = ge.ge_playera group by at.at_team_number, ge.ge_eventtype;";
	$playersResult = mysql_query($sql);
	$rows = mysql_num_rows($playersResult);
	$count = 0;
	while ($count < $rows) {
		$type = mysql_result($playersResult, $count, "ge_eventtype");
		$value = mysql_result($playersResult, $count, "total");
		$id = mysql_result($playersResult, $count, "at_team_number");
		if ($id == 1) {
			$tid = $tid1;
		} else {
			$tid = $tid2;
		}
		$proceed = false;
                if ($type == WINBOLO_NET_EVENT_BASE_CAPTURE || $type == WINBOLO_NET_EVENT_BASE_STEAL) {
                        $event = $gameType . "_num_bases";
                        $proceed = true;
                } else if ($type == WINBOLO_NET_EVENT_PILL_CAPTURE || $type == WINBOLO_NET_EVENT_PILL_STEAL) {
                        $event = $gameType . "_num_pills";
                        $proceed = true;
                } else if ($type == WINBOLO_NET_EVENT_TANK_KILL) {
                        $event = $gameType . "_num_tanks";
                        $proceed = true;
                }
                if ($proceed == true) {
                        $sql = "UPDATE stats_team set $event = $event + $value where tid = $tid";
                        $result = mysql_query($sql);
                }

		$count++;
	}
}

function getPidFromKey($key, $sqlServerKey) {
	$key = mysql_escape_string($key);
	$sql = "select ap_pid from active_player where ap_key = '$key' and ap_serverkey = '$sqlServerKey'";
	$result = mysql_query($sql);
	if (!$result) {
		return 0;
	}
	if (mysql_num_rows($result) != 1) {
		return 0;
	}
	return mysql_result($result, 0, 0);
}

#############################################################################
# FUNCTION:     calculateRanksPlayers
# AUTHOR:       John Morrison
# DATE:         03/02/16
# RETURNS:
# PURPOSE:
# Calculates the new scores for 1x1 games. Updates the players scores, 
# number of wins and losses
#
# PARAMETERS:
# sqlServerKey - Server key of the game
# winnerKey    - Key of the player that won
# gameType     - Game type prefix for the field name
#############################################################################
function calculateRanksPlayers($sqlServerKey, $winnerKey, $gameType) {
	$sql = "select distinct ge_playera from game_event where ge_serverkey='$sqlServerKey' and ge_playera is not null and ge_playera <> '$winnerKey' and (ge_eventtype = " . WINBOLO_NET_EVENT_BASE_CAPTURE . " or ge_eventtype = " . WINBOLO_NET_EVENT_PILL_CAPTURE . " or ge_eventtype = " . WINBOLO_NET_EVENT_BASE_STEAL . " or ge_eventtype = " . WINBOLO_NET_EVENT_PILL_STEAL . ")";

	$result = mysql_query($sql);
	$loserKey = mysql_result($result, 0, 0);
	$winnerPid = getPidFromKey($winnerKey, $sqlServerKey);
	$loserPid = getPidFromKey($loserKey, $sqlServerKey);
	if ($winnerPid == 0 || $loserPid == 0) {
		return;
	}
	
	# Get player Ranks
	$winnerScore = getScore($winnerPid, $gameType, false);
	$loserScore = getScore($loserPid, $gameType, false);
	# Calclulate New Scores 
	$newWinnerScore = calculateElo($winnerScore, $loserScore, true);
	$newLoserScore = calculateElo($loserScore, $winnerScore, false);
	# Update ranks
	setScore($winnerPid, $gameType, $newWinnerScore, false);
	setScore($loserPid, $gameType, $newLoserScore, false);
	updateWinsLoses($winnerPid, $gameType, true, false);
	updateWinsLoses($loserPid, $gameType, false, false);
}


function addTeamGames($gametype, $tid) {
	$gametype = getGameType($gametype);
if ($gametype != "tourn") {
	$gametype = $gametype . "_";
}
	$sql = "update stats_team set " . $gametype . "num_games = " . $gametype . "num_games + 1 where tid = $tid";
	mysql_query($sql);
}

#############################################################################
# FUNCTION:     calculateRanks
# AUTHOR:       John Morrison
# DATE:         03/02/16
# RETURNS:
# PURPOSE:
# Calculates the new scores at the end of a game.
#
# PARAMETERS:
# sqlServerKey - Server key of the game
#############################################################################
function calculateRanks($sqlServerKey) {
        ############################################################
	#Step 1 - Determine if we can rank a game.                 #
	#It must have a WINBOLO_NET_EVENT_WIN event to be rankable #
	############################################################
	$sql = "select ge_playera from game_event where ge_serverkey='$sqlServerKey' and ge_eventtype = " . WINBOLO_NET_EVENT_WIN;
	$result = mysql_query($sql);
	if (!$result) {
		return;
	}
	if (mysql_num_rows($result) != 1) {
		return;
	}
	$winnerKey = mysql_result($result, 0, 0);
	###############################################
	#Step 2 - Filter out games that we can't rank #
	###############################################
	
	#If a game has any pills or bases taken by non-ranked players
	$sql = "select count(*) as total from game_event where ge_playera is null and ge_serverkey='$sqlServerKey' and (ge_eventtype = " . WINBOLO_NET_EVENT_BASE_CAPTURE . " or ge_eventtype = " . WINBOLO_NET_EVENT_PILL_CAPTURE . " or ge_eventtype = " . WINBOLO_NET_EVENT_BASE_STEAL . " or ge_eventtype = " . WINBOLO_NET_EVENT_PILL_STEAL . ")";
	$result = mysql_query($sql);
	if (!$result) {
		return;
	}
	if (mysql_result($result, 0, 0) > 0) {
		return;
	}

	$sql = "select gs_gametype from game_server where gs_key='$sqlServerKey'";
	$result = mysql_query($sql);
	if (!$result) {
		return;
	}
	$gameType = mysql_result($result, 0, 0);
	if ($gameType == 1) {
		# Open game - Don't rank
		return;
	}
	#####################################################################
	#Step 4 - Determine the number of players in the game and calculate #
	#####################################################################
	$sql = "select distinct (ge_playera) as total from game_event where ge_serverkey='$sqlServerKey' and (ge_eventtype = " . WINBOLO_NET_EVENT_BASE_CAPTURE . " or ge_eventtype = " . WINBOLO_NET_EVENT_PILL_CAPTURE . " or ge_eventtype = " . WINBOLO_NET_EVENT_BASE_STEAL . " or ge_eventtype = " . WINBOLO_NET_EVENT_PILL_STEAL . ")";
	$result = mysql_query($sql);
	if (!$result) {
		return;
	}
	$noPlayers = mysql_num_rows($result);
	if ($noPlayers == 1) {
		# Single player game
		
	} else if ($noPlayers == 2) {
		# Two player game
		calculateRanksPlayers($sqlServerKey, $winnerKey, $gameType);
	} else {
		# Its a team game (maybe)
		calculateRanksTeams($sqlServerKey, $winnerKey, $gameType);
	}
	
	#If a game has more then 2 players but no alliances then it can't be ranked

}

#############################################################################
# FUNCTION:     processServerQuit
# AUTHOR:       John Morrison
# DATE:         02/04/06
# RETURNS:
# PURPOSE:
# Removes a server. Updates all player database values, calculate ranks etc
#
# PARAMETERS:
# key - Server key
#############################################################################
function processServerQuit($key) {
	global $link;
	
	$ip = getenv ("REMOTE_ADDR");
	if (strlen($key) != 32) {
		writeError("Invalid data length");
	}

	$sqlServerKey = mysql_escape_string($key);
	openDatabase(true);

	#Copy into the archived_game_server table
	$sql = "insert into archived_game_server (ags_key, ags_ip, ags_port, ags_mapname, ags_password, ags_numbases, ags_numpills, ags_mines, ags_freebases, ags_freepills, ags_numtanks, ags_starttime, ags_firstheard, ags_lastheard, ags_ai , ags_gametype, ags_version) select gs_key, gs_ip, gs_port, gs_mapname, gs_password, gs_numbases, gs_numpills, gs_mines, gs_freebases, gs_freepills, gs_numtanks, gs_starttime, gs_firstheard, gs_lastheard, gs_ai , gs_gametype, gs_version from game_server where gs_key='$sqlServerKey'";
	$result = mysql_query($sql);
	if (mysql_affected_rows() != 1) {
		writeError("<br>Database error");
	}

	$sql = "SELECT gs_gametype from game_server where gs_key = '$sqlServerKey'";
	$result = mysql_query($sql);
	$gameType = getGameType(mysql_result($result, 0, "gs_gametype"));

	#Copy game events into archived_game_events table

	# Player A and Player B is null
	$sql = "insert into archived_game_event (age_serverkey, age_eventtype, age_timestamp) select g.ge_serverkey, g.ge_eventtype, g.ge_timestamp from game_event g where g.ge_playera is null and g.ge_playerb is null and g.ge_serverkey = '$sqlServerKey' ";
	$result = mysql_query($sql);
	
	# Player A is not null and Player B is null
	$sql = "insert into archived_game_event (age_serverkey, age_eventtype, age_playera, age_timestamp) select g.ge_serverkey, g.ge_eventtype, a.ap_pid, g.ge_timestamp from game_event g, active_player a where g.ge_playera = a.ap_key and g.ge_playerb is null and g.ge_serverkey = '$sqlServerKey'";
	$result = mysql_query($sql);

	# Player A is null and Player B is not null
	$sql = "insert into archived_game_event (age_serverkey, age_eventtype, age_playerb, age_timestamp) select g.ge_serverkey, g.ge_eventtype, a.ap_pid, g.ge_timestamp from game_event g, active_player a where g.ge_playerb = a.ap_key and g.ge_playera is null and g.ge_serverkey = '$sqlServerKey'";
	$result = mysql_query($sql);

	# Player A and Player B are not null
	$sql = "insert into archived_game_event (age_serverkey, age_eventtype, age_playera, age_playerb, age_timestamp) select g.ge_serverkey, g.ge_eventtype, a.ap_pid, a2.ap_pid, g.ge_timestamp from game_event g, active_player a, active_player a2 where g.ge_playera = a.ap_key and g.ge_playerb = a2.ap_key and g.ge_playerb is not null and g.ge_serverkey = '$sqlServerKey'";
	$result = mysql_query($sql);

	# Do stats run
	# Update number of pill/base/tank takes
	$sql = "SELECT a.ap_pid, g.ge_eventtype, count(*) as total from game_event g, active_player a where g.ge_serverkey = '$sqlServerKey' and g.ge_playera = a.ap_key group by a.ap_pid, g.ge_eventtype";
	$playersResult = mysql_query($sql);
	$rows = mysql_num_rows($playersResult);
	$count = 0;
	while ($count < $rows) {
		$type = mysql_result($playersResult, $count, "ge_eventtype");
		$value = mysql_result($playersResult, $count, "total");
		$pid = mysql_result($playersResult, $count, "ap_pid");
		$proceed = false;
		if ($type == WINBOLO_NET_EVENT_BASE_CAPTURE || $type == WINBOLO_NET_EVENT_BASE_STEAL) {
			$event = $gameType . "_num_bases";
			$proceed = true;
		} else if ($type == WINBOLO_NET_EVENT_PILL_CAPTURE || $type == WINBOLO_NET_EVENT_PILL_STEAL) {
			$event = $gameType . "_num_pills";
			$proceed = true;
		} else if ($type == WINBOLO_NET_EVENT_TANK_KILL) {
			$event = $gameType . "_num_tanks";
			$proceed = true;
		}
		if ($proceed == true) {
			$sql = "UPDATE stats_player set $event = $event + $value where pid = $pid";
			$result = mysql_query($sql);
		}
		$count++;
	}

	#update number of games played
	$sql = "SELECT distinct ap_pid from active_player where ap_serverkey = '$sqlServerKey'";
	$playersResult = mysql_query($sql);
	$rows = mysql_num_rows($playersResult);
	$count = 0;
	while ($count < $rows) {
		$pid = mysql_result($playersResult, $count, "ap_pid");
		$fieldName = $gameType . "_num_games";
		$sql = "UPDATE stats_player set $fieldName = $fieldName + 1 where pid = $pid";
		$result = mysql_query($sql);
		$count++;
	}

	#Calculate Ranks
	calculateRanks($sqlServerKey);

	# Insert into the player journal table
	$sql = "insert into journal (j_key, j_pid) select distinct g.ge_serverkey, a.ap_pid from game_event g, active_player a where g.ge_serverkey = '$sqlServerKey' and g.ge_playera = a.ap_key and g.ge_playera is not null;";
	$result = mysql_query($sql);

	# Lastly delete it from the active games list
	$sql = "delete from game_event where ge_serverkey = '$sqlServerKey'";
	$result = mysql_query($sql);
	$sql = "delete from active_player where ap_serverkey = '$sqlServerKey'";
	$result = mysql_query($sql);
	$sql = "delete from game_server where gs_key = '$sqlServerKey'";
	$result = mysql_query($sql);
	$sql = "delete from active_team where at_serverkey = '$sqlServerKey'";
	$result = mysql_query($sql);
}

#############################################################################
# FUNCTION:     getGameType   
# AUTHOR:       John Morrison
# DATE:         02/04/06
# RETURNS:
# PURPOSE:
# Returns the game database prefix type (ie "tourn", "open", "strict) for
# the internal game type number
#
# PARAMETERS:
# gt - Internal gametype
#############################################################################
function getGameType($gt) {
        switch ($gt) {
		case 1:
			return "open";
			break;
		case 2:
			return "tourn";
			break;
		default:
			return "strict";
			break;
	}
	return "strict";
}

?>
