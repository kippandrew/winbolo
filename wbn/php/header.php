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

$jmtime = microtime();
$jmtime = explode(" ",$jmtime);
$jmtime = $jmtime[1] + $jmtime[0];
$jmstarttime = $jmtime;
ob_start("ob_gzhandler");
require "/home/winbolo/php/db.php";

#Header stuff
header ("Cache-Control: no-cache, must-revalidate");  // HTTP/1.1
header ("Pragma: no-cache");                          // HTTP/1.0
require "$BASE_FILES/functions.php";
require "$WWW_ROOT/wbn/wbn-gameend.php";
openDatabase(true);

#Check session stuff
define('IN_PHPBB', true);
$phpbb_root_path = "$WWW_ROOT/forums/";
include($phpbb_root_path . 'extension.inc');
include($phpbb_root_path . 'common.'.$phpEx);
//
// Set page ID for session management
//
$userdata = session_pagestart($user_ip, PAGE_LOGIN);
init_userprefs($userdata);
//
// End session management
//


define('IN_PHPBB', false);
?>
