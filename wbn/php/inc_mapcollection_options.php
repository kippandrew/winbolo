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

<? $id = $mc->getId(); ?>
<table border="0" width=100%>
<tr><td colspan=2><div class="subheading"><b>Options</b></div></td></tr><tr>
<td colspan=2>
<div class="bodytext">Use the following forms to add maps and folders to this map collection.
</td></tr>
<tr>
<td width="50%">
<div class="bodytext" cellpadding=15><b>Add Map</b>
<p><form ENCTYPE="multipart/form-data"  action="mapcollectionupload.php" method="post">
If you wish to upload a map to WinBolo.net map collection so other people can download it you can by the following form.</p>
<p><input NAME="mapfile" TYPE="file" SIZE="30"><br>
<input TYPE="hidden" name="MAX_FILE_SIZE" value="50">
<input TYPE="hidden" name="id" value="<?echo "$id";?>">
<br>Map Comment<br><textarea name="blurb" cols="50" rows="6"></textarea><p>
<input TYPE="submit" VALUE="Upload Map">
</form>
</td>

<td>
<div class="bodytext" cellpadding=15><b>Add Folder</b>
<p><form ENCTYPE="multipart/form-data"  action="mapcollectionaddfolder.php" method="post">
If you wish to add a folder to your WinBolo.net map collection you can by the following form.</p>
<p>Name: <input NAME="name" TYPE="text" SIZE="30" maxlength="64"><br>
<br>Comment<br><textarea name="comment" cols="50" rows="6"></textarea><p>
<input TYPE="hidden" name="id" value="<?echo "$id";?>">
<input TYPE="submit" VALUE="Create Folder">
</form>
</td></tr>
<?
if (sizeof($mc->getParents()) != 1 && sizeof($mc->getSubFolders()) == 0 && sizeof($mc->getMaps()) == 0) {
	echo "<tr><td colspan='2'><center><a href='mapcollectiondelete.php?id=$id'>Delete this folder</a></center></td></tr>";
}
?>
</table>
<P>
