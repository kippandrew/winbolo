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

</div></td></tr>
<tr><td><div class="subheading">Favourite Map</div></td></tr>
<tr><td><div class="bodytext">
<P><table class="bodytext" border=0>
<tr><td>
<a href="mapdownload.php?name=<? echo $player->getName();?>" >
<img border=0 src="mapview.php?name=<? echo urlencode($player->getName());?>" ></a>
</td>
<td valign = top class="bodytext">
<b>Name: </b><a href="mapdownload.php?name=<? echo urlencode($player->getName());?>" ><? echo $map->getName();?></a><br>
<b>Bases: </b><? echo $map->getNumBases();?><br>
<b>Pills: </b><? echo $map->getNumPills();?><br>
<b>Starts: </b><? echo $map->getNumStarts();?><br>
<b>Comment: </b><br><? echo $map->getComment();?>
</td>
</tr>
</table>
