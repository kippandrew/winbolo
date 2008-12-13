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
?>

<table width="480" border="0" align="right" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
<tr>
<td> <table width="480" border="0" cellspacing="0" cellpadding="0">
<tr>
<td width="20"><img src="images/stleft.png" width="20" height="20"></td>
<td class="topborder" bgcolor="#3366CC"> <div class="heading">Team Details</div></td>
<td width="20"><img src="images/stright.png" width="20" height="20"></td>
</tr>
</table></td>
</tr>
<tr>
<td> <table width="480" border="0" cellspacing="0" cellpadding="0">
<tr>
<td width="1" class="leftborder">&nbsp;</td>
<td width="282">
<table width="470" border="0" align="center" cellpadding="0" cellspacing="0">
<tr>
</tr>
<tr>
<td> <div class="bodytext">
<span class="normal"><b>Team Name</b>: <?echo $team->getName();?><br>
<? #!--	      <font size="-4">(Currently in game <a href="winbolo://server.winbolo.com:2345/">server.winbolo.com:2345</a>)</font></span></span></p> --> ?>
<p><b>Created by</b>: <?echo $team->getOwner();?>
<p><b>Team members</b>: 
<?
$players = $team->getPlayers();
$count = 0;
while ($count < sizeof($players)) {
	echo "<A href=\"/playerinfo.php?name=" . $players[$count] . "\">" . $players[$count] . "</a>";
	if ($count != sizeof($players) -1) {
		echo ", ";
	}

	$count++;
}
?>
<br>
<br>
</div></td></tr>
<tr><td><div class="subheading">Statistics</div></td></tr>
<tr><td><div class="bodytext">
            <p>These count for games where all players are allied for the duration of the game.
            <table width="95%" border="1">
              <tr> 
                <td class="bodytext">&nbsp;</td>
                <td class="bodytext">Rank</td>
                <td class="bodytext">Score</td>
                <td class="bodytext">Games</td>
                <td class="bodytext">Wins</td>
                <td class="bodytext">Loses</td>
                <td class="bodytext"><img src="images/pill.gif" width="16" height="16"> Pill Captures</td>
                <td class="bodytext"><img src="images/base.gif" width="16" height="16"> Base Captures</td>
                <td class="bodytext"><img src="images/tank.gif" width="16" height="16">Tank Kills</td>
              </tr>
              <tr> 
                <td class="bodytext">Open</td>
                <td class="bodytext">N/A</td>
                <td class="bodytext">N/A</td>
                <td class="bodytext"><? echo $team->getOpenNumGames(); ?></td>
                <td class="bodytext">N/A</td>
                <td class="bodytext">N/A</td>
                <td class="bodytext"><? echo $team->getOpenNumPills(); ?></td>
                <td class="bodytext"><? echo $team->getOpenNumBases(); ?></td>
                <td class="bodytext"><? echo $team->getOpenNumTanks(); ?></td>
              </tr>
              <tr> 
                <td class="bodytext">Tournament</td>
                <td class="bodytext"><? echo $team->getTournRank(); ?></td>
                <td class="bodytext"><? echo $team->getTournScore(); ?></td>
                <td class="bodytext"><? echo $team->getTournNumGames(); ?></td>
                <td class="bodytext"><? echo $team->getTournNumWins(); ?></td>
                <td class="bodytext"><? echo $team->getTournNumLoses(); ?></td>
                <td class="bodytext"><? echo $team->getTournNumPills(); ?></td>
                <td class="bodytext"><? echo $team->getTournNumBases(); ?></td>
                <td class="bodytext"><? echo $team->getTournNumTanks(); ?></td>
              </tr>
              <tr> 
                <td class="bodytext">Strict</td>
                <td class="bodytext"><? echo $team->getStrictRank(); ?></td>
                <td class="bodytext"><? echo $team->getStrictScore(); ?></td>
                <td class="bodytext"><? echo $team->getStrictNumGames(); ?></td>
                <td class="bodytext"><? echo $team->getStrictNumWins(); ?></td>
                <td class="bodytext"><? echo $team->getStrictNumLoses(); ?></td>
                <td class="bodytext"><? echo $team->getStrictNumPills(); ?></td>
                <td class="bodytext"><? echo $team->getStrictNumBases(); ?></td>
                <td class="bodytext"><? echo $team->getStrictNumTanks(); ?></td>
              </tr>
              <tr> 
                <td class="bodytext"><b>Totals</b></td>
                <td class="bodytext"><b>N/A</b></td>
                <td class="bodytext"><b>N/A</b></td>
                <td class="bodytext"><b><? echo ($team->getOpenNumGames() + $team->getTournNumGames() + $team->getStrictNumGames()) ?></b></td>
                <td class="bodytext"><b><? echo ($team->getOpenNumGames() + $team->getTournNumWins() + $team->getStrictNumWins()) ?></b></td>
                <td class="bodytext"><b><? echo ($team->getTournNumLoses() + $team->getStrictNumLoses()) ?></b></td>
                <td class="bodytext"><b><img src="images/pill.gif" width="16" height="16"><? echo ($team->getOpenNumPills() + $team->getTournNumPills() + $team->getStrictNumPills()) ?></b></td>
                <td class="bodytext"><b><img src="images/base.gif" width="16" height="16"><? echo ($team->getOpenNumBases() + $team->getTournNumBases() + $team->getStrictNumBases()) ?></b></td>
                <td class="bodytext"><b><img src="images/tank.gif" width="16" height="16"><? echo ($team->getOpenNumTanks() + $team->getTournNumTanks() + $team->getStrictNumTanks()) ?></b></td>
              </tr>
            </table>
            <p>
</div></td></tr>
<tr><td><div class="subheading">Team Details</div></td></tr>
<tr><td><div class="bodytext">
<br>
<b>Blurb</b>: 

<?
if ($team->getBlurb() !== "") {
	echo str_replace("\n", "<BR>", $team->getBlurb());
} else {
	echo "No blurb.";
}
?>
<p><b>Registered</b>: <?echo $team->getRegisteredDate();?>
<?if ($team->getWebsite() != "") {echo '<p><b>Website</b>: <a href="'.$team->getWebsite().'" target="_new">  '.$team->getWebsite().'</a>';}?>
<br><br>
<tr><td><div class="subheading">Options</div></td></tr>
<tr><td><div class="bodytext">
<p>
<?
if ($userdata['session_logged_in'] == FALSE) {
	echo "Sorry, you must be logged in to join or edit this teams details";
} else {
	include "$BASE_FILES/inc_teamdetailsoptions.php";
}


?>
<BR><BR>This page has been viewed: <? echo $team->getPageViews(); ?> times.
</div></td></tr>
</table>

<!-- end content -->
</td>
<td width="1" class="rightborder">&nbsp;</td>
</tr>
</table>

