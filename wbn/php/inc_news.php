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
<td class="topborder" bgcolor="#3366CC"> <div class="heading">News</div></td>
<td width="20"><img src="images/stright.png" width="20" height="20"></td>
</tr>
</table></td>
</tr>
<tr>
<td> <table width="480" border="0" cellspacing="0" cellpadding="0">
<tr>
<td width="1" class="leftborder">&nbsp;</td>
<td>
<?


$events = getGameLogWithServer(5);
$size = sizeof($events);
$count = 0;
if ($size > 0) {
	echo '<div class="bodytext">';
	echo "In news just to hand...<p>";
	while ($count < $size) {
		$event = $events[$count];
                $event->writeEntry();
		echo "(<a href=\"gameinfo.php?ip=" . urlencode($event->getIp()) . "&port=" . $event->getPort() . "&key=" . $event->getKey() . "\">info</a> - <a href=\"winbolo://" . urlencode($event->getIp()) . ":" . $event->getPort() . "\">join</a> - " . $event->getGameType() . " " . getLockedHTML($event)  . ")";
		echo "<br>";
		$count++;
	}
	echo '<br></div>';
}
$nis = getNewsItems(6);
$count = 0;
$size = sizeof($nis);
while ($count < $size) {
	$item = $nis[$count];
?>
<!-- Beginning of news item table --><table width="470" border="0" align="center" cellpadding="0" cellspacing="0">
<tr>
<td> <div class="subheading"><? echo $item->getSubject();?></div></td>
</tr>
<tr>
<td> <div class="bodytext">
<p align="right">
Posted on: <? echo $item->getTime();?> by <a href="playerinfo.php?name=<?echo $item->getPosterUrl() . "\">" . $item->getPoster(); ?></a></p>
<p align="left">
<?echo $item->getText();?>
</p>
<p align="right">
<a href="/forums/viewtopic.php?t=<? echo $item->getPostId();?>">Comments &gt;&gt;</a>
</div></td>
</tr>
</table>
<br>
<?
	$count++;
}
?>
<center><a href="/forums/viewforum.php?f=5"><b>Older News </b></a></center>
</td>
<td width="1" class="rightborder">&nbsp;</td>
</tr>
</table>

