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
		
		<table border="0" class="bodytext">
                       <tr> 
                          <td class="normal">Show Email Address on Page</td>
                          <td> 
                           <input type="radio" name="showemail" value="1" <? if ($player->getShowEmail() == 1) {echo "checked";}?>>
                            Yes 
                            <input type="radio" name="showemail" value="0" <? if ($player->getShowEmail() == 0) {echo "checked";}?>>
                            No </td>
                        </tr>
                       <tr> 
                          <td class="normal">Enable Email Forward</td>
                          <td> 
                           <? 
if ($player->getEmailForward() == 1) {
	echo "Enabled. &nbsp;&nbsp;(<a href='processemailsettings.php?option=disable'>Click to Disable</a>)";
} else {
	echo "Disabled. &nbsp;&nbsp;(<a href='processemailsettings.php?option=enable'>Click to Enable</a>)";
}
?>
                            </td>
                        </tr>
                       <tr> 
                          <td class="normal">Show Email Forward</td>
                          <td> 
                           <input type="radio" name="showforward" value="1" <? if ($player->getShowForward() == 1) {echo "checked";}?>>
                            Yes 
                            <input type="radio" name="showforward" value="0" <? if ($player->getShowForward() == 0) {echo "checked";}?>>
                            No </td>
                        </tr>
		</table>
