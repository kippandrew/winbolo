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

<tr><td><div class="subheading"><a href="gamelog.php?key=<?echo $info->getKey()?>">Log</a> / Comments / <a href="gamelog.php?mode=logfile&key=<?echo $info->getKey()?>">Download</a></div></td></tr>
<tr>
<td> <div class="bodytext">
	<?
	$total = sizeof($comments);
	if ($total == 0) {
		echo "Sorry, there are no comments on this game.";
	} else {
		$count = 0;
		while ($count < $total) {
			$current = $comments[$count];
			echo "<i><a href='playerinfo.php?name=" . urlencode($current->getName()) . "'>" . $current->getName() . "</a> at " . gmdate("D M d, Y g:i a", $current->getTime()) . "</i> ";
			if ($current->getRating() > 0) {
				echo mcPrepareRatingStars($current->getRating());
			}
			echo "<P>" . htmlspecialchars($current->getComment()) . "<br>";
			echo "<hr>";
			$count++;
		}
	}

	echo "<P><P>";

	if ($userdata['session_logged_in'] == true) {
        	include "$BASE_FILES/inc_archivedetail_addcomment.php";
	} else {
	        echo "<center>If you were logged in you could comment on this log.<BR></center>";
	}
	
	?>
</div></td></tr>
</table>

<!-- end content -->
</td>
<td width="1" class="rightborder">&nbsp;</td>
</tr>
</table>

