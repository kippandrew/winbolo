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
Player Details</div></td>
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
<b>Player Name: </b><span class="text">: 
<?
	echo $player->getName();
	$activeGames = $player->getActiveGames();
	if (sizeof($activeGames) > 0) {
		echo "<font size=\"-4\"> - Currently in game: ";
		echo ": ";
		$count = 0;
		while ($count < sizeof($activeGames)) {
			$ip = $activeGames[$count];
			$uIP = urlencode($ip);
			$port = $activeGames[$count+1];
			$key = $activeGames[$count+2];
			echo "$ip:$port ";
			echo "(<a href='gameinfo.php?ip=$uIP&port=$port&key=$key'>Info</a>, <a href='winbolo://$ip:$port'>Join</a>";
			$count += 3;
		}
		echo ")</font>";
	}
	if ($player->getOnIrc() == 1) {
		echo "<font size=\"-4\"> - Currently on IRC</font>";
	}
?><br>
<b>Teams</b>: 
<?
$teams = $player->getTeams();
if (sizeof($teams) == 0) {
	echo "Not a member of any teams";
} else {
	$count = 0;
	while ($count < sizeof($teams)) {
		echo "<A href=\"/teaminfo.php?name=" . urlencode($teams[$count]) . "\">" . $teams[$count] . "</a>";
		if ($count != sizeof($teams) -1) {
			echo ", ";
		}
		
		$count++;
	}
}
?>
<br><br>
</div></td></tr>
<tr><td><div class="subheading">Statistics</div></td></tr>
<tr><td><div class="bodytext">These count for 1x1 games not games involving teams or any alliances.
            <table width="95%" border="1">
              <tr> 
                <td>&nbsp;</td>
                <td class="bodytext">Rank</td>
                <td class="bodytext">Score</td>
                <td class="bodytext">Games</td>
                <td class="bodytext">Wins</td>
                <td class="bodytext">Loses</td>
                <td class="bodytext"><img src="images/pill.gif" width="16" height="16"> Pill Captures</td>
                <td width="17%" class="bodytext"><img src="images/base.gif" width="16" height="16"> Base Captures</td>
                <td width="20%" class="bodytext"><img src="images/tank.gif" width="16" height="16">Tank Kills</td>
              </tr>
              <tr> 
                <td class="bodytext">Open</td>
                <td class="bodytext">N/A</td>
                <td class="bodytext">N/A</td>
                <td class="bodytext" id="o.g"><? echo $player->getOpenNumGames(); ?></td>
                <td class="bodytext">N/A</td>
                <td class="bodytext">N/A</td>
                <td class="bodytext" id="o.p"><? echo $player->getOpenNumPills(); ?></td>
                <td class="bodytext" id="o.b"><? echo $player->getOpenNumBases(); ?></td>
                <td class="bodytext" id="o.t"><? echo $player->getOpenNumTanks(); ?></td>
              </tr>
              <tr> 
                <td class="bodytext">Tournament</td>
                <td class="bodytext" id="t.r"><? echo $player->getTournRank(); ?></td>
                <td class="bodytext" id="t.s"><? echo $player->getTournScore(); ?></td>
                <td class="bodytext" id="t.g"><? echo $player->getTournNumGames(); ?></td>
                <td class="bodytext" id="t.w"><? echo $player->getTournNumWins(); ?></td>
                <td class="bodytext" id="t.l"><? echo $player->getTournNumLoses(); ?></td>
                <td class="bodytext" id="t.p"><? echo $player->getTournNumPills(); ?></td>
                <td class="bodytext" id="t.b"><? echo $player->getTournNumBases(); ?></td>
                <td class="bodytext" id="t.t"><? echo $player->getTournNumTanks(); ?></td>
              </tr>
              <tr> 
                <td class="bodytext">Strict</td>
                <td class="bodytext" id="s.r"><? echo $player->getStrictRank(); ?></td>
                <td class="bodytext" id="s.s"><? echo $player->getStrictScore(); ?></td>
                <td class="bodytext" id="s.g"><? echo $player->getStrictNumGames(); ?></td>
                <td class="bodytext" id="s.w"><? echo $player->getStrictNumWins(); ?></td>
                <td class="bodytext" id="s.l"><? echo $player->getStrictNumLoses(); ?></td>
                <td class="bodytext" id="s.p"><? echo $player->getStrictNumPills(); ?></td>
                <td class="bodytext" id="s.b"><? echo $player->getStrictNumBases(); ?></td>
                <td class="bodytext" id="s.t"><? echo $player->getStrictNumTanks(); ?></td>
              </tr>
              <tr> 
                <td class="bodytext"><b>Totals</b></td>
                <td class="bodytext"><b>N/A</b></td>
                <td class="bodytext"><b>N/A</b></td>
                <td class="bodytext" id="a.g"><b><? echo ($player->getOpenNumGames() + $player->getTournNumGames() + $player->getStrictNumGames()) ?></b></td>
                <td class="bodytext" id="a.w"><b><? echo ($player->getTournNumWins() + $player->getStrictNumWins()) ?></b></td>
                <td class="bodytext" id="a.l"><b><? echo ($player->getTournNumLoses() + $player->getStrictNumLoses()) ?></b></td>
                <td class="bodytext" id="a.p"><b><img src="images/pill.gif" width="16" height="16"><? echo ($player->getOpenNumPills() + $player->getTournNumPills() + $player->getStrictNumPills()) ?></b></td>
                <td class="bodytext" id="a.b"><b><img src="images/base.gif" width="16" height="16"><? echo ($player->getOpenNumBases() + $player->getTournNumBases() + $player->getStrictNumBases()) ?></b></td>
                <td class="bodytext" id="a.t"><b><img src="images/tank.gif" width="16" height="16"><? echo ($player->getOpenNumTanks() + $player->getTournNumTanks() + $player->getStrictNumTanks()) ?></b></td>
              </tr>
            </table>
<br><br>
</div></td></tr>
<tr><td><div class="subheading">Details</div></td></tr>
<tr><td><div class="bodytext"><b>Email Address</b> :
<?
if ($player->getShowEmail() == 1) {
	echo $player->getEmail();
} else {
	echo "Player has requested their email address not be shown.";
}
?>
<p>
<b>Email Forward:</b> :
<?
if ($player->getEmailForward() == 1 || $player->getShowForward() == 1) {
	echo "Yes: Send email to Player name @ winbolo.net";
} else {
	echo "Not Enabled or viewable.";
}
?>
<p>
<b>Blurb</b>: 
<?
if ($player->getShowBlurb() == 1) {
	echo $player->getBlurb();
} else {
	echo "No blurb.";
}
?><p>
<b>Registered</b>: <?echo $player->getRegisteredDate();?> (<? printf("%.2f", $player->getPostsPerDay()); ?>  Posts per day) 
<p><b>Number of Posts</b>: <?echo $player->getNumPosts();?> (<?printf("%.2f", $player->getPercentPosts());?>% of total) <?if ($player->getNumPosts() > 0) {echo '<font size=1>[<a href="/forums/search.php?mode=results&term=&addterms=any&forum=all&search_author='.$player->getName().'&sortby=p.post_time&searchboth=both&submit=Search">View posts</a>]</font></a>';}?><p>
	<?if ($player->getWebsite() != "") {echo '<b>Website</b>: <a href="'.$player->getWebsite().'" target="_new">  '.$player->getWebsite().'</a><p>';}?>
            <?if ($player->getIcq() != "") {echo '<img src="images/icq.gif"> <b>ICQ</b>: '.$player->getIcq().'<font size=1>[<a href="http://wwp.icq.com/scripts/search.dll?to='.$player->getIcq().'">Add me</a>] [<a href="http://wwp.mirabilis.com/'.$player->getIcq().'">Page me</a>]<p>';} ?>
            <? if ($player->getAim() != "") { echo '<img src="images/aim.gif"> <b>AOL Instant Messager</b>: '.$player->getAim().' <font size=1>[<a href="aim:goim?screenname='.$player->getAim().'&message=Hi+there.+Are+you+there?">Page me</a>]<p>';}?>
            <? if ($player->getYahoo() != "") {echo '<img src="images/yahoo.gif"> <b>Yahoo Instant Messager</b>: '.$player->getYahoo().' <font size=1>[<a href="http://edit.yahoo.com/config/send_webmesg?.target='.$player->getYahoo().'&.src=pg">Page me</a>]<p>';}?>
            <? if ($player->getMsn() != "") {echo '<img src="images/msn.gif"> <b>MSN Instant Messager</b>: ' . $player->getMsn() .'<p>';}?>
		<p><a class="link" href="playerdiary.php?player=<? echo urlencode($player->getName());?>">View Player Diary</a>
		<p>&nbsp;</p>
<? if ($map == true) {
	include "$BASE_FILES/inc_mapdetail.php";
} ?>
</div></td></tr>
<tr><td><div class="subheading">Map Collection</div></td></tr>
<tr><td><div class="bodytext">
<?
if ($player->getHasMapCollection() == false) {
	echo "Sorry this player doesn't have a map collection yet.";
} else {
	$id = mcGetTopLevelCollectionId($player->getId());
	echo "<a href='mapcollection.php?id=$id'>View players map collection</a>";
}
?>
<BR>
<BR>
<BR>
<BR>This page has been viewed: <? echo $player->getPageViews(); ?> times.
<p>&nbsp;</p>
</div></td></tr>
</table>
<!-- end content -->
</td>
<td width="1" class="rightborder">&nbsp;</td>
</tr>
</table>
