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
				
				<!-- Begging of Login Table -->
                  <table width="320" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
                    <tr> 
                      <td> <table width="320" border="0" cellspacing="0" cellpadding="0">
                          <tr> 
                            <td width="20"><img src="images/stleft.png" width="20" height="20"></td>
                            <td class="topborder" bgcolor="#FF9900"> <div class="heading">Login</div></td>
                            <td width="20"><img src="images/stright.png" width="20" height="20"></td>
                          </tr>
                        </table></td>
                    </tr>
                    <tr> 
                      <td> <table width="320" border="0" cellspacing="0" cellpadding="0">
                          <tr> 
                            <td width="1" class="leftborder">&nbsp;</td>
                            <td width="282"><table width="100%" border="0" cellpadding="0" cellspacing="0">
                                <tr>
                                  <td width="60%">
<div align="center" class="bodytext"><b>Options</b><br>
                                      <a href="playermodify.php">Modify account</a> | 
                                      <a href="createteam.php">Create Team</a>
                                      <a href="logout.php">Logout</a> 
                                    </div></td>
                                  <td width="60%" class="leftborderdashed">
<div align="center" class="loggedin"><? echo $userdata['username'];?><br>
                                      is logged in.</div></td>
                                </tr>
                              </table> </td>
                            <td width="1" class="rightborder">&nbsp;</td>
                          </tr>
                        </table></td>
                    </tr>
                    <tr> 
                      <td> <table width="320" border="0" cellspacing="0" cellpadding="0">
                          <tr> 
                            <td width="20" valign="bottom"><img src="images/sbleft.gif" width="20" height="30"></td>
                            <td class="bottomborder">&nbsp;</td>
                            <td width="20" valign="bottom"><img src="images/sbright.gif" width="20" height="30"></td>
                          </tr>
                        </table></td>
                    </tr>
                  </table><!-- End of Login Table -->
