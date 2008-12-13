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
Game Information</div></td>
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

<?

	$players = $info->getPlayers();
?>
	<B>Server IP:</B> <a href="winbolo://<?echo $info->getIp() . ":".$info->getPort();?>"><?echo $info->getIp();?>:<?echo $info->getPort() . "</a> " . getLockedHTML($info); ?><BR>
	<B>Map Name:</B> <?echo mcPrepareSearch($info->getMapName());?><BR>
	<B>Version:</B> <?echo $info->getVersion();?><BR>
	<B><img src="/images/tank.gif"> Players:</B> 
	<?
	# Do Playesr part
	echo $info->getNumPlayers();
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
		if (sizeof($players) != $info->getNumPlayers() && ($info->getNumPlayers() - sizeof($players) > 0)) {
			echo " + " . ($info->getNumPlayers() - sizeof($players)) . " non WinBolo.net participants";
		}
		echo ")";
	} else if ($info->getNumPlayers() > 0) {
		echo " (" . $info->getNumPlayers()  . " non WinBolo.net participants)";
	}	
	?>
	<BR>
	<B><img src="/images/pill.gif"> Number of Pills:</B> <? echo $info->getNumPills();?> <i> - <? echo $info->getNumFreePills();?> Free</i><BR>
	<B><img src="/images/base.gif"> Number of Bases:</B> <? echo $info->getNumBases();?><i> - <? echo $info->getNumFreeBases();?> Free</i><BR>
	<B>Game Type:</B> <? echo $info->getGameType();?><BR>
	<B>Mines:</B> <? echo $info->getHiddenMines();?><BR>
	<B>Brains:</B> <? echo $info->getAi();?><BR>
	<B>Pass worded:</B> <? echo $info->getPassword();?><BR>
	<B>Start Time:</B> <? echo date("D M j G:i:s T Y", $info->getStartTime());?><BR>
	<B>Last Heard:</B> <? echo date("D M j G:i:s T Y", $info->getLastHeard());?><BR>
	<P>

</div></td></tr>
<tr><td><div class="subheading">Log</div></td></tr>
<tr><td><div class="bodytext">

	<?
	$count =0;
	$rows = sizeof($log);
	while ($count < $rows) {
		$event = $log[$count];
		echo date("G:i:s", $event->getTimestamp());
		$event->writeEntry();
		echo "<br>";
		$count++;

	}
	?>
<p>&nbsp;</p>
</div></td></tr>
</table>

<!-- end content -->
</td>
<td width="1" class="rightborder">&nbsp;</td>
</tr>
</table>


