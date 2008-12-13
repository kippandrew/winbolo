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
<? $name = getPlayerName($mc->getPid());?>
This map collection is created by: <a href="playerinfo.php?name=<? echo urlencode($name);?>"><?echo $name;?></a><br>
Creation date: <? echo gmdate("D M d, Y g:i a", $mc->getTime()); ?><br>
<?
if (strlen($mc->getComment()) > 0) {
	echo "Comment: <br><pre>" . htmlspecialchars($mc->getComment()) . "</pre><br>";
}
