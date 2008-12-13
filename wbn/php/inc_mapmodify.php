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

<table border=0>
<tr><td>
<a href="mapdownload.php?name=<? echo $player->getName();?>" >
<img border=0 src="mapview.php?name=<? echo $player->getName();?>" ></a>
</td>
<td valign = top>
<span class="normal"><b>Name: </b><a href="mapdownload.php?name=<? echo $player->getName();?>" ><? echo $map->getName();?></a><br>
<b>Bases: </b><? echo $map->getNumBases();?><br>
<b>Pills: </b><? echo $map->getNumPills();?><br>
<b>Starts: </b><? echo $map->getNumStarts();?><br>
<b>Comment: </b><br><? echo $map->getComment();?><p>
<form action="mapdelete.php">
<input type=submit value='Delete Map'>
</form>
</td>
</tr>
</table>
