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

<table width=100%><tr><td><div class="subheading">Map Details</div></td></tr>
<tr><td><div class="bodytext">
<P><table class="bodytext" border=0>
<tr><td>
<a href="mapcollectiondownload.php?id=<? echo $md->getId();?>" >
<img border=0 src="mapcollectionview.php?id=<? echo $md->getId();?>" ></a>
</td>
<td valign = top class="bodytext">
<b>Name: </b><a href="mapcollectiondownload.php?id=<? echo $md->getId();?>" ><? echo $md->getName();?></a><br>
<b>Upload Date: </b> <? echo gmdate("D M d, Y g:i a", $md->getTime()); ?><br>
<b>Author: </b> <? echo "<a href='playerinfo.php?name=" . urlencode($md->getOwnerName()) . "'>" . $md->getOwnerName() . "</a>"; ?><br>
<b>Path: </b><?
$parents = $md->getParents();
$count = sizeof($parents) - 1;
while ($count >= 0) {
	$current = $parents[$count];
	$id = $current->getId();
	$name = htmlspecialchars($current->getName());
	echo "<a href='mapcollection.php?id=$id'>$name</a>";
	if ($count > 0) {
		echo " / ";
	}
	$count--;
}
echo "<br><b>Rating: </b>";
if ($md->getNumRatings() > 0) {
	echo mcPrepareRatingStars($md->getRating()) . "(" . $md->getRating() .  "/10 from " . $md->getNumRatings() . " ratings.)";
} else {
	echo "No ratings yet";
}
?>
<br>
<b>Bases: </b><? echo $md->getNumBases();?><br>
<b>Pills: </b><? echo $md->getNumPills();?><br>
<b>Starts: </b><? echo $md->getNumStarts();?><br>
<b>Downloads: </b><? echo $md->getDownloads();?><br>
<b>Comment: </b><br><? echo htmlspecialchars($md->getComment());?>
</td>
</tr>
</table>
</table>
