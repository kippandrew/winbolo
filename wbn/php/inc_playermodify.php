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

<td class="topborder" bgcolor="#3366CC"> <div class="heading">Modify Account Details</div></td>

<td width="20"><img src="images/stright.png" width="20" height="20"></td>

</tr>

</table></td>

</tr>

<tr>

<td> <table width="480" border="0" cellspacing="0" cellpadding="0">

<tr>

<td width="1" class="leftborder">&nbsp;</td>



<td width="282">

<table width="470" border="0" align="center" cellpadding="0" cellspacing="0">

<tr>

</tr>

<tr>

<td> <div class="bodytext">

<p>Update your player information: Email address is required.

            <form method="post" action="processmodifyplayer.php">

	    	Player Name:<b> <? echo $player->getName(); ?></b><p>

		Email Address: <input type="text" size="30"  name="email" value="<? echo $player->getEmail(); ?>">

<P><div class="subheading">Password</div>

		<table border="0" class="bodytext">

		<tr><td>New Password:</td><td><input type="password" name="password"></td></tr>

		<tr><td>Retype New Password:</td><td><input type="password" name="password2"></td></tr>

		</table>

		Leave blank if you don't want to change your password.<br><br>

<P><div class="subheading">E-mail Settings</div>

<?

include "$BASE_FILES/inc_playeremail.php";

?>

<P><div class="subheading">Details</div>

                <table border="0" class="bodytext">

                    <tr> 

                      <td>Favourite Bolo Game Type 

                       </td>

                       <td> 

		       <select>

                           <option value="0" <?if ($player->getGameType() == 0) {echo "selected";}?>>No Preference</option>

                           <option value="1" <?if ($player->getGameType() == 1) {echo "selected";}?>>Open</option>

                           <option value="2" <?if ($player->getGameType() == 2) {echo "selected";}?>>Tournament</option>

                           <option value="3"<?if ($player->getGameType() == 3) {echo "selected";}?>>Strict</option>

                         </select>

                       </td>

                    </tr>

                        <tr> 

                          <td>Blurb</td>

                          <td> 

                            <textarea name="blurb" cols="30" rows="6"><?echo $player->getBlurb();?></textarea>

                          </td>

                        </tr>

                        <tr> 

                          <td>Show blurb on page</td>

                          <td> 

                            <input type="radio" name="showblurb" value="1" <? if ($player->getShowBlurb() == 1) {echo "checked";}?>>

                            Yes

                            <input type="radio" name="showblurb" value="0" <? if ($player->getShowBlurb() == 0) {echo "checked";}?>>

                            No </td>

                        </tr>

                        <tr> 

<tr><td height="63">Website Address</td>

<td height="63"><input type="text" name="website" value="<?echo $player->getWebsite();?>" size=30></td></tr>

<tr><td height="63">Occupation</td>

<td height="63"><input type="text" name="occupation" value="<?echo $player->getOccupation();?>" size=30></td></tr>

<tr><td height="63">Signature<br><font size=-1>Text that appears at the end of your posts. Max 255 letters.</font></td>

<td height="63"><TEXTAREA NAME="sig" ROWS="4" COLS="30"><?echo $player->getSig();?></textarea></td>



			</tr>
		    </table>


<P><div class="subheading">Instant Messages</div>

		<table border="0" class="bodytext">
<td><img src="images/icq.gif"> ICQ Number</td>

<td><input type="text" name="icq" value="<?echo $player->getIcq();?>"size=30></td></tr>

<tr><td height="30"><img src="images/aim.gif"> AIM</td>

<td height="30"><input type="text" name="aim" value="<?echo $player->getAim();?>"size=30></td></tr>

<tr><td height="30"><img src="images/yahoo.gif"> Yahoo Messenger</td>

<td height="30"><input type="text" name="yahoo" value="<?echo $player->getYahoo();?>"size=30></td></tr>

<tr><td height="30"><img src="images/msn.gif"> MSN</td>

<td height="30"><input type="text" name="msn" value="<?echo $player->getMsn();?>"size=30></td></tr>

		</table>

	                      <p align="center"> 

                        <input type="submit" name="Submit2" value="Update">

                      </p>

 	<p>

                    </form>

		    </div></td></tr>

<tr><td><div class="subheading">Map</div></td></tr>

<tr><td><div class="bodytext">

<P>		    	<? $map = getPlayerMap($player->getId());

		  	if ($map == false) {

				

				include "$BASE_FILES/inc_playermapupload.php";

			} else {

				include "$BASE_FILES/inc_mapmodify.php";

			}

			?>

</div></td></tr>

</div></td></tr>

<tr><td><div class="subheading">Map Collection</div></td></tr>

<tr><td><div class="bodytext">

<?

if ($player->getHasMapCollection() == false) {

        	echo "<a href='createmapcollection.php'>Create map collection</a>";

	} else {

		$id = mcGetTopLevelCollectionId($player->getId());

	        echo "<a href='mapcollection.php?id=$id'>View players map collection</a>";

	}

?>



</table>



<!-- end content -->

</td>

<td width="1" class="rightborder">&nbsp;</td>

</tr>

</table>



