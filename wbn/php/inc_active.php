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
<td class="topborder" bgcolor="#3366CC"> <div class="heading">Active Games</div></td>
<td width="20"><img src="images/stright.png" width="20" height="20"></td>
</tr>
</table></td>
</tr>
<tr>
<td>

<table width="480" border="0" cellspacing="0" cellpadding="0">
<tr>
<td width="1" class="leftborder">&nbsp;</td>
<td width="282">

<!-- Begin Filter -->

<form action="activegames.php" method="get">

  <br>
  <table border="0" cellpadding="3" cellspacing="0" align="center">
    <tr>
      <td class="bodytext"><b>Game Type</b></td>
      <td>
        <select name="type" class="bodytext">
          <option value="any">Any</option>
          <option <? if ($type=="open") echo "selected" ?> value="open">Open</option>
          <option <? if ($type=="tourny") echo "selected" ?> value="tourny">Tournament</option>
        </select>
      </td>
      <td class="bodytext"><b>Password?</b></td>
      <td>
        <select name="pw" class="bodytext">
          <option value="any">Any</option>
          <option <? if ($pw=="yes") echo "selected" ?> value="yes">Passworded</option>
          <option <? if ($pw=="no") echo "selected" ?> value="no">Non-Passworded</option>
        </select>
      </td>
    </tr>
    <tr>
      <td class="bodytext"><b>Locked?</b></td>
      <td>
        <select name="lock" class="bodytext">
          <option value="any">Any</option>
          <option <? if ($lock=="yes") echo "selected" ?> value="yes">Locked</option>
          <option <? if ($lock=="no") echo "selected" ?> value="no">Unlocked</option>
        </select>
      </td>
      <td class="bodytext"><b>Started?</b></td>
      <td>
        <select name="start" class="bodytext">
          <option value="any">Any</option>
          <option <? if ($start=="no") echo "selected" ?> value="no">All Bases/Pills Available</option>
        </select>
      </td>
    </tr>
    <tr>
      <td colspan="4" align="center">
        <input type="submit" value="Refresh Game List" class="bodytext">
          &nbsp;
        <input type="button" value="Show All Games" class="bodytext"
            onClick="window.location='activegames.php?type=any&pw=any&lock=any&start=any'">
      </td>
    </tr>
  </table>

</form>

<!-- End Filter -->

<table width="470" border="0" align="center" cellpadding="0" cellspacing="0">
<tr> 
</tr>
<tr>
<td> <div class="bodytext">
<?
	$count=0;
        $valid_count=0;
	while ($count < sizeof($servers)) {

		$cs = $servers[$count];
		$players = $cs->getPlayers();
		$fullAddress = urlencode($cs->getIp()) . ":" . $cs->getPort();

                $bSkip = false;

                if ($type == "open" && $cs->getGameType() != "Open")
                  $bSkip = true;
                elseif ($type == "tourny" && $cs->getGameType() == "Open")
                  $bSkip = true;

                if ($pw == "yes" && $cs->getPassword() != "Yes")
                  $bSkip = true;
                elseif ($pw == "no" && $cs->getPassword() != "No")
                  $bSkip = true;

                if ($lock == "yes" && !$cs->locked)
                  $bSkip = true;
                elseif ($lock == "no" && $cs->locked)
                  $bSkip = true;

                if ($start == "no" && ($cs->numBases != $cs->freeBases || $cs->numPills != $cs->freePills))
                  $bSkip = true;

                if (!$bSkip) {
?>
		<B>Server IP:</B> <? echo $fullAddress ?> (<a href="gameinfo.php?ip=<?echo urlencode($cs->getIp());?>&port=<?echo urlencode($cs->getPort());?>&key=<?echo urlencode($cs->getKey());?>">Info</a> - <a href="winbolo://<? echo $fullAddress;?>">Join</a> <? echo getLockedHTML($cs);?>)<BR>
		<B>Map Name:</B> <?echo mcPrepareSearch($cs->getMapName());?><BR>
		<B>Version:</B> <?echo $cs->getVersion();?><BR>
		<B><img src="images/tank.gif"> Players:</B> 
		<?
		# Do Playesr part
		echo $cs->getNumPlayers();
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
			if (sizeof($players) != $cs->getNumPlayers() && ($cs->getNumPlayers() - sizeof($players) > 0) ) {
				echo " + " . ($cs->getNumPlayers() - sizeof($players)) . " non WinBolo.net participants";
			}
			echo ")";
		} else if ($cs->getNumPlayers() > 0) {
			echo " (" . $cs->getNumPlayers()  . " non WinBolo.net participants)";
		}	
		?>
		<BR>
		<B><img src="images/pill.gif"> Number of Pills:</B> <? echo $cs->getNumPills();?> <i> - <? echo $cs->getNumFreePills();?> Free</i><BR>
		<B><img src="images/base.gif"> Number of Bases:</B> <? echo $cs->getNumBases();?><i> - <? echo $cs->getNumFreeBases();?> Free</i><BR>
		<B>Game Type:</B> <? echo $cs->getGameType();?><BR>
		<B>Mines:</B> <? echo $cs->getHiddenMines();?><BR>
		<B>Brains:</B> <? echo $cs->getAi();?><BR>
		<B>Pass worded:</B> <? echo $cs->getPassword();?><BR>
		<B>Start Time:</B> <? echo date("D M j G:i:s T Y", $cs->getStartTime());?><BR>
		<B>Last Heard:</B> <? echo date("D M j G:i:s T Y", $cs->getLastHeard());?><BR>
		<P>
<p>
<?              
                   $valid_count++;
                }
		$count++;
	}
?>

<? if ($valid_count == 0) { ?>
  <div align="center">
    Sorry there are no active games that match your search criteria.
  </div>
<? } ?>

<p>&nbsp;</p>

</div>

</td></tr>
</table>

<!-- end content -->
</td>
<td width="1" class="rightborder">&nbsp;</td>
</tr>
</table>
