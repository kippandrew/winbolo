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

<div class="subheading"><b>Maps</b></div>
<div class="bodytext">The following maps are available.<p>
<center>
<table width="100%" border=0><tr>
<?
$maps = $mc->getMaps();
$count = 0;
$total = sizeof($maps);
while ($count < $total) {
	$current = $maps[$count];
	$id = $current->getId();
	$name = htmlspecialchars($current->getName());
	echo "<td align='center' class='bodytext'><a href='mapcollectiondownload.php?id=$id'><img src='mapcollectionview.php?id=$id'><br>$name</a> - <a href='mapcollectioninfo.php?id=$id'>Info</a>";
	if ($current->getRatings() > 0) {
		echo " - " .  mcPrepareRatingStars($current->getRatings());
	}
	echo "</td>";
	$count++;
	if ($count % 2 == 0) {
		echo "</tr><tr>";
	}
}
?>
</tr></table>
</center>
<P>


