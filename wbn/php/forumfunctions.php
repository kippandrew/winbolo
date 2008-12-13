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

/** Largely based upon PHP BB 2 Code **/

/**
 * Translates any sequence of whitespace (\t, \r, \n, or space) in the given
 * string into a single space character.
 * Returns the result.
 */
function normalize_whitespace($str) {
	$output = "";
	$tok = preg_split("/[ \t\r\n]+/", $str);
	$tok_count = sizeof($tok);
	for ($i = 0; $i < ($tok_count - 1); $i++) {
		$output .= $tok[$i] . " ";
	}
	$output .= $tok[$tok_count - 1];
	return $output;
}


function storeForumNames($db, $username, $password, $email, $sig, $occ, $from, $website, $icq, $aim, $yim, $msnm, $interests, $viewemail, $gametype) {
	$username = strip_tags($username);
	$username = trim($username);
	$username = normalize_whitespace($username);
	$username = mysql_escape_string($username);

	$sig = chop($sig); // Strip all trailing whitespace.
	$sig = str_replace("\n", "<BR>", $sig);
	$sig = mysql_escape_string($sig);
	$occ = mysql_escape_string($occ);
	$intrest = mysql_escape_string($intrest);
	$from = mysql_escape_string($from);
	$password = str_replace("\\", "",$password);
	$passwd = md5($password);
	$email = mysql_escape_string($email);
	$regdate = time();

	// Ensure the website URL starts with "http://".
	$website = trim($website);
	if(substr(strtolower($website), 0, 7) != "http://") {
		$website = "http://" . $website;
	}

	if($website == "http://") {
		$website = "";
	}

	$website = mysql_escape_string($website);
	// Check if the ICQ number only contains digits
	$icq = (ereg("^[0-9]+$", $icq)) ? $icq : '';
	$aim = mysql_escape_string($aim);
	$yim = mysql_escape_string($yim);
	$msnm = mysql_escape_string($msnm);

	if($viewemail == "1") {
		$sqlviewemail = "1";
	} else {
		$sqlviewemail = "0";
	}
	$sql = "SELECT max(user_id) AS total FROM users";
	if(!$r = mysql($db, $sql)) {
		return -1;
	}
	list($total) = mysql_fetch_array($r);
	$total += 1;
	$userDateFormat = $board_config['default_dateformat'];
	$userTimeZone = $board_config['board_timezone'];
	$sql = "INSERT INTO users (user_id, username, user_dateformat, user_timezone, user_regdate, user_email, user_icq, user_password, user_occ, user_interests, user_from, user_website, user_sig, user_aim, user_viewemail, user_yim, user_msnm, user_game_type) VALUES ('$total', '$username', '$userDateFormat', '$userTimeZone', '$regdate', '$email', '$icq', '$passwd', '$occ', '$intrest', '$from', '$website', '$sig', '$aim', '$sqlviewemail', '$yim', '$msnm', '$gametype')";

	if(!$result = mysql($db, $sql)) {
		return -1;
	}
	return $total;
}


function doForumsLostPassword($userid, $username, $email) {

global $cfg_email_lostpassword_subject;
global $cfg_email_lostpassword_address;

	$chars = array("a","A","b","B","c","C","d","D","e","E","f","F","g","G","h","H","i","I","j","J","k","K","l","L","m","M","n","N","o","O","p","P","q","Q","r","R","s","S","t","T","u","U","v","V","w","W","x","X","y","Y","z","Z","1","2","3","4","5","6","7","8","9","0");

	$max_elements = count($chars) - 1;
	srand((double)microtime()*1000000);
	$newpw = $chars[rand(0,$max_elements)];
	$newpw .= $chars[rand(0,$max_elements)];
	$newpw .= $chars[rand(0,$max_elements)];
	$newpw .= $chars[rand(0,$max_elements)];
	$newpw .= $chars[rand(0,$max_elements)];
	$newpw .= $chars[rand(0,$max_elements)];
	$newpw .= $chars[rand(0,$max_elements)];
	$newpw .= $chars[rand(0,$max_elements)];
	$newpw_enc = md5($newpw);

	// Don't ask...
	$key = md5(md5(md5($newpw_enc)));
	$sql = "UPDATE users SET user_actkey = '$key', user_newpasswd = '$newpw_enc' WHERE user_id = '$userid'";
	mysql_query($sql);
	$message = "Dear " . $username;
	$message .= "\nYou are receiving this email because you (or someone pretending to be you)\nhas requested a password change on Winbolo.net. If you believe you have\nreceived this message in error simply delete it and your password will remain\nthe same.\n\nYour new password as generated is: " . $newpw;
	$message .= "\n\nIn order for this change to take effect you must visit this page:\n\nhttp://www.winbolo.net/activatepassword.php?actkey=" . $key;
	$message .= "\n\n   Once you have visited the page your password will be changed in our database,\nand you may login to the profile section and change it as desired.\n\n";
	mail($email, $cfg_email_lostpassword_subject, $message, "From: $cfg_email_lostpassword_address\r\nX-Mailer: phpBB/$phpbbversion");
}

# Returns an ID or "Error" if too many attempts
function generateRegisterID() {
	global $userdata;

	#Generate and delete old confirm ID's from the confirm table based
	#upon inactive sessions
	$sql = 'SELECT session_id FROM sessions';
	$result = mysql_query($sql);
	if (!$result) {
		return "Error";
	}

	$confirm_sql = '';
	$count = 0;
	while ($count < mysql_num_rows($result)) {
		$confirm_sql .= (($confirm_sql != '') ? ', ' : '') . "'" . mysql_result($result, $count, 0) . "'";
		$count++;
	}
	mysql_free_result($result);
	$sql = "DELETE FROM confirm WHERE session_id NOT IN ($confirm_sql)";
	$result = mysql_query($sql);
	if (!$result) {
		return "Error";
	}

	#
	# Check number of create requests
	#
	$sql = "SELECT COUNT(session_id) AS attempts from confirm WHERE session_id = '" . $userdata['session_id'] . "'";
	$result = mysql_query($sql);
	if (!$result) {
		return "Error";
	}
	if (mysql_result($result, 0, 0) > 3) {
		return "Error";
	}
	mysql_free_result($result);
	// Generate the required confirmation code
	// NB 0 (zero) could get confused with O (the letter) so we make change it
	$code = dss_rand();
	$code = substr(str_replace('0', 'Z', strtoupper(base_convert($code, 16, 35))), 2, 6);
	$confirm_id = md5(uniqid($user_ip));
	$sql = "INSERT INTO confirm (confirm_id, session_id, code) VALUES ('$confirm_id', '". $userdata['session_id'] . "', '$code')";
	$result = mysql_query($sql);
	if (!$result) {
		return "Error";
	}
	unset($code);
	return $confirm_id;
}



?>