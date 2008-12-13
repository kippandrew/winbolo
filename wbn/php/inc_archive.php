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
<td class="topborder" bgcolor="#3366CC"> <div class="heading">
Archived Games</div></td>
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
<td>  <div class="bodytext">
<P>
You can view the log files for archived games for 24 hour after they finish.<p>
<?
##############################################################################
# FUNCTION:     printPageList
# AUTHOR:       Andrew Roth, John Morrrison
# DATE:         02/03/03
# RETURNS:
# PURPOSE:
# Prints a text list of "page indexes" used to divide long pages up
#
# it will print enough pages to cover all the items, given the number of items
# per page
#
# PARAMETERS:
# $start - Starting index
# $itemsPerPage - the number of items listed in a page
# $numItems - the total number of "items"
# $url - the url to print the links with ?page= to
#############################################################################
function printPageList($start, $numItems, $url) {
	global $cfg_ServersPerPage;

	$page = floor(($start+1)/$cfg_ServersPerPage);
	$str = generate_pagination($url, $numItems, $cfg_ServersPerPage, $page * $cfg_ServersPerPage);
	echo "$str<P>";

}

	echo "<center>";
	printPageList($start, $numServers, "archivedgames.php?");
	echo "</center>";
	if ($numServers == $cfg_MaxServersSelect) {
		echo "<B>warning: reached upper bound on archived servers! ($cfg_MaxServersSelect).  Email ";
		echo "<a href=mailto:canuck@sluggy.net>canuck</A></B><BR><BR>";
	}
	$count=0;
	while ($count < sizeof($servers)) {
		$cs = $servers[$count];
		$players = $cs->getPlayers();
?>
	  <table border=1 width=100%><tr><td><div class="bodytext">
		<B>Server:</B> <?echo $cs->getIp();?>:<?echo $cs->getPort();?> - <a href="gamelog.php?key=<?echo urlencode($cs->getKey());?>">Log</a><BR>
		<B>Map Name:</B> <?echo mcPrepareSearch($cs->getMapName());?><BR>
		<B><img src="images/tank.gif"> WinBolo.net Players Were:</B> 
		<?
		# Do Players part
		$playersCount = 0;
		if (sizeof($players) > 0) {
			echo " (";
			while ($playersCount < sizeof($players)) {
				if ($playersCount != 0) {
					echo ", ";
				}
				echo "<A href=\"playerinfo.php?name=" . urlencode($players[$playersCount]) . "\">" . $players[$playersCount] . "</A>";
				$playersCount++;
			}
			echo ")";
		} else if ($cs->getNumPlayers() > 0) {
			echo " (" . $cs->getNumPlayers()  . " non WinBolo.net participants)";
		}	
		?>
		<BR>
		<B>Game Type:</B> <? echo $cs->getGameType();?>&nbsp;&nbsp; <B>Version:</B> <?echo $cs->getVersion();?><BR>
                <B>Ratings:</B> <?
		if ($cs->getLogNumRatings() > 0) {
			echo mcPrepareRatingStars($cs->getLogRating()) . "(" . $cs->getLogRating() .  "/10 from " . $cs->getLogNumRatings() . " ratings.)";
		} else {
			echo "No ratings yet.";
		}
?>
		<BR><B>Start Time:</B> <? echo date("D M j G:i:s T Y", $cs->getStartTime());?><BR>
		<B>Last Heard:</B> <? echo date("D M j G:i:s T Y", $cs->getLastHeard());?><BR>
		</td></tr></table>
<?		$count++;
	}
?>
<p>&nbsp;</p>
</div>
<div align="center" class="bodytext"><? 
	printPageList($start, $numServers, "archivedgames.php?");
?>
</div></td></tr>
</table>

<!-- end content -->
</td>
<td width="1" class="rightborder">&nbsp;</td>
</tr>
</table>

