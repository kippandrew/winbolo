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

<tr><td><div class="subheading">Log / <a href="gamelog.php?mode=comments&key=<?echo $info->getKey()?>">Comments</a> / <a href="gamelog.php?mode=logfile&key=<?echo $info->getKey()?>">Download</a></div></td></tr>
<tr>
<td> <div class="bodytext">

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
</div></td></tr>
</table>

<!-- end content -->
</td>
<td width="1" class="rightborder">&nbsp;</td>
</tr>
</table>

