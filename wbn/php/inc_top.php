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

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title><? echo $html_title;?></title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<link rel="stylesheet" type="text/css" href="global.css">
</head>

<body vlink="#3366CC" alink="#3366CC">
<table width="700" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#3366CC">
  <tr>
    <td><table width="700" border="0" cellspacing="0" cellpadding="0">
        <tr> 
          <td width="30"><img src="images/tleft.gif" width="30" height="30"></td>
          <td width="640" class="topborder2">&nbsp;</td>
          <td width="30"><img src="images/tright.gif" width="30" height="30"></td>
        </tr>
      </table></td>
  </tr>
  <tr> 
    <td>
<table width="700" border="0" cellspacing="0" cellpadding="0">
        <tr> 
          <td width="1" class="leftborder">&nbsp;</td>
          <td width="698">
<table width="690" border="0" cellspacing="0" cellpadding="0">
              <tr>
                <td width="345"><a href="index.php"><img src="images/titleorig.gif" border=0></a></td>
                <td width="345" valign="top">
		<? if ($userdata['session_logged_in'] == true) {
		  include "$BASE_FILES/inc_signedin.php";
		} else {
		  include "$BASE_FILES/inc_signin.php";
		}
		?>
		</td>
              </tr>
            </table>
            <br>
            <table width="690" border="0" align="center" cellpadding="0" cellspacing="0">
              <tr>
                <td width="200" valign="top"><table width="190" border="0" align="right" cellpadding="0" cellspacing="0">
                    <tr> 
                      <td><!-- Start of navigation menu table --><table width="194" border="0" align="right" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
                          <tr> 
                            <td> <table width="194" border="0" cellspacing="0" cellpadding="0">
                                <tr> 
                                  <td width="20"><img src="images/stleft.png" width="20" height="20"></td>
                                  <td class="topborder" bgcolor="#3366CC"> <div class="heading">Navigation</div></td>
                                  <td width="20"><img src="images/stright.png" width="20" height="20"></td>
                                </tr>
                              </table></td>
                          </tr>
                          <tr> 
                            <td> <table width="194" border="0" cellspacing="0" cellpadding="0">
                                <tr> 
                                  <td width="1" class="leftborder">&nbsp;</td>
                                  <td width="282"> <div class="subheading">New Players?</div>
                                    <div align="right"><a href="help.php">Help</a><br>
                                    <a href="forums/">Forums</a><br>
                                    <a href="addons.php">Add-ons</a><br>
                                    <a href="mapcollection.php">Map Collections</A><BR>
                                    <a href="links.php">Links and Downloads</A><br>
									<a href="about.php">About</a><br>
									<a href="forumpolicy.php">Forums Policy</a></div>
                                    <div class="subheading">Games</div>
                                    <div align="right"><a href="activegames.php">Active Games</a><br>
                                    <a href="archivedgames.php">Archived Games</a></div>
				    <div class="subheading">Statistics</div>
                                    <div align="right"><a href="showstats.php?game_type=strict&stats_type=player">Top Strict Tournament Players</a><br>
                                    <a href="showstats.php?game_type=strict&stats_type=teams">Top Strict Tournament Teams</a><br>
                                    <a href="stats.php">More Statistics</a><br>
<a href="whatif.php">What if? Calculator</a></div><br>
                                    <div class="subheading">Find a Player</div>
                                    <div align="center"><FORM action=searchplayer.php method=get><INPUT 
                                size=15 name=name> 
                              </FORM></div>
                                    <div class="subheading">Find a Team</div>
                                    <div align="center"><FORM action=searchteam.php method=get><INPUT 
                                size=15 name=name> 
                              </FORM></div>
                                    <DIV class=subheading>Find a Map</DIV>
                                    <DIV align=center>
                                    <form action="mapcollectionsearchresults.php" method="POST">
                                    <input type=text name="name" size=15>
                                    <input type="hidden" name="search" value="name" checked></FORM></DIV>
                                    <p><div class="subheading">Support WinBolo.net</div>
			      <div align="center"><form action="https://www.paypal.com/cgi-bin/webscr" method="post">
			      <input type="hidden" name="cmd" value="_xclick">
			      <input type="hidden" name="business" value="john@winbolo.net">
			      <input type="hidden" name="item_name" value="Donate to WinBolo.net">
			      <input type="hidden" name="item_number" value="1">
			      <input type="hidden" name="no_shipping" value="1">
			      <input type="hidden" name="cn" value="Your WinBolo.net login:">
			      <input type="hidden" name="currency_code" value="USD">
			      <input type="hidden" name="tax" value="0">
			      <input type="image" src="https://www.paypal.com/en_US/i/btn/x-click-but04.gif" border="0" name="submit" alt="Make payments with PayPal - it's fast, free and secure!">
			      </form></div>
			      </td>
                                  <td width="1" class="rightborder">&nbsp;</td>
                                </tr>
                              </table></td>
                          </tr>
                          <tr> 
                            <td> <table width="194" border="0" cellspacing="0" cellpadding="0">
                                <tr> 
                                  <td width="20" valign="bottom"><img src="images/sbleft.gif" width="20" height="30"></td>
                                  <td class="bottomborder">&nbsp;</td>
                                  <td width="20" valign="bottom"><img src="images/sbright.gif" width="20" height="30"></td>
                                </tr>
                              </table></td>
                          </tr>
                        </table><!-- End of navigation menu table -->
</td>
                    </tr>
                    <tr> 
                      <td><br><!-- Start of Recent forum posts table -->
                        <table width="194" border="0" align="right" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
                          <tr> 
                            <td> <table width="194" border="0" cellspacing="0" cellpadding="0">
                                <tr> 
                                  <td width="20"><img src="images/stleft.png" width="20" height="20"></td>
                                  <td class="topborder" bgcolor="#3366CC"> <div class="heading">Recent 
                                      Forum Posts</div></td>
                                  <td width="20"><img src="images/stright.png" width="20" height="20"></td>
                                </tr>
                              </table></td>
                          </tr>
                          <tr> 
                            <td> <table width="194" border="0" cellspacing="0" cellpadding="0">
                                <tr> 
                                  <td width="1" class="leftborder">&nbsp;</td>
                                  <td width="282"> <!-- Start of Recent forum posts tabular data table --><table width="100%" cellspacing="2" cellpadding="0">
                                      <tr> 
                                        <td class="subheading"> <div align="center">Subject</div></td>
                                        <td class="subheading"> <div align="center">Posted 
                                            by</div></td>
                                      </tr>
<? doLatestPosts(10);?>

</table><!-- End of Recent forum posts tabular data table --></td>
                                  <td width="1" class="rightborder">&nbsp;</td>
                                </tr>
                              </table></td>

                          </tr>
                          <tr> 
                            <td> <table width="194" border="0" cellspacing="0" cellpadding="0">
                                <tr> 
                                  <td width="20" valign="bottom"><img src="images/sbleft.gif" width="20" height="30"></td>
                                  <td class="bottomborder">&nbsp;</td>
                                  <td width="20" valign="bottom"><img src="images/sbright.gif" width="20" height="30"></td>
                                </tr>
                              </table></td>
                          </tr>
                        </table><!-- End of Recent forum posts table -->
                        <p>&nbsp;</p> </td>
                    </tr>
                    <tr>
                      <td><br>
<? doRandomRankTable();?>
                  </table> 
                </td>
                <td width="490" valign="top">

