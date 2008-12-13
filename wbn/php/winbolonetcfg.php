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

# Config file
if (!isset($BASE_FILES)) {
	$BASE_FILES="/home/winbolo/php";
}
require "$BASE_FILES/db.php";

$cfg_email_from_address = "webmaster@winbolo.net";
$cfg_email_signup_subject = "Welcome to WinBolo.net";

$cfg_email_lostpassword_subject = "Winbolo.net password";
$cfg_email_lostpassword_address = "webmaster@winbolo.net";
#NOTE the cfg_email must be stored in the code because we want to embed user name and full name n' stuff in it.


#Winbolo Server Stuff

#Versiuon Headers
$cfg_wbnversion_major = 1;
$cfg_wbnversion_minor = 0;
$cfg_wbnversion_revision = 0;
$cfg_wbn_motd = "Welcome to WinBolo.net. (http://www.winbolo.net/)";

$cfg_ServersPerPage = 10; # added by Andrew
$cfg_MaxServersSelect = 500; # assume there should never be more than this many servers in 24 hours

define("WINBOLO_NET_EVENT_ALLY_JOIN", 0); 	/* Player B has joined Alliance A */
define("WINBOLO_NET_EVENT_ALLY_LEAVE", 1); 	/* Player B has left Alliance A */
define("WINBOLO_NET_EVENT_BASE_CAPTURE", 2); 	/* Player A has captured a base */
define("WINBOLO_NET_EVENT_PILL_CAPTURE", 3); 	/* Player A has captured a pill */
define("WINBOLO_NET_EVENT_TANK_KILL", 4); 	/* Player A has killed a tank */
define("WINBOLO_NET_EVENT_LGM_KILL", 5); 	/* Player A has killed a lgm */
define("WINBOLO_NET_EVENT_LGM_LOST", 6); 	/* Player A has lost their lgm */
define("WINBOLO_NET_EVENT_BASE_STEAL", 7); 	/* Player A has captured a base */
define("WINBOLO_NET_EVENT_PILL_STEAL", 8); 	/* Player A has captured a pill */
define("WINBOLO_NET_EVENT_PLAYER_JOIN", 9); 	/* Player A has joined the game */
define("WINBOLO_NET_EVENT_PLAYER_LEAVE", 10); 	/* Player A has left the game */
define("WINBOLO_NET_EVENT_WIN", 11); 		/* Player A has won the game */
define("WINBOLO_NET_EVENT_REJOIN", 12); 		/* Player A has won the game */
define("WINBOLO_NET_EVENT_QUITTING", 13); 		/* Player A is quitting the game */

?>
