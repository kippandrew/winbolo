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

<table width=100%><tr><td><div class="subheading">Add Review/Comment</div></td></tr>
<tr><td><div class="bodytext">
<form action="mapcollectionaddcomment.php" method="post">
Please enter your comment below<br>
<table>
<tr><td class="bodytext" valign="top">Rating:</td><td class="bodytext">
<? if (mcHasCommented($md->getId(), $userdata['user_id']) == true) {
	echo "Sorry you can only add one rating.";
} else {
	echo "<select name='rating'>";
	$count = 1;
	while ($count < 11) {
		echo "<option value='$count'>$count</option>";
		$count++;
	}
	echo "</select>";
}
?>
</td></tr>
<tr><td class="bodytext" valign="top">Comments:</td><td><textarea name='comment' rows=5 cols=40></textarea></td></tr>
</table><br>
<input type="submit" value="Submit review"><input type="hidden" name="id" value="<? echo $md->getId();?>">
</form>
<P>
</td>
</tr>
</table>
