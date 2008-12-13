<?
#Downloading a logfile
require "../php/header.php";
$error = true;
$testKey = $HTTP_GET_VARS['key'];

# Key is mysqlescaped
function getPlayerString($key) {
	$rv = "";
	$sql = "select distinct u.username from users u, archived_game_event e where e.age_serverkey = '$key' and e.age_eventtype = 9 and e.age_playera = u.user_id order by u.username";
	$r = mysql_query($sql);
	$count = 0;
	while ($count < mysql_num_rows($r)) {
		if ($count != 0) {
			$rv .= " ";
		}
		$rv .= mysql_result($r, $count, 0);
		$count++;
	}
	return $rv;
}

if (strstr($testKey, "..") == true || strstr($testKey, "\\") == true || strstr($testKey, "/") == true) {
	$error_body = "An error occured processing request.";
} else if (isset($HTTP_GET_VARS['key'])) {
	$key = mysql_escape_string($HTTP_GET_VARS['key']);
	$sql = "select ags_mapname, ags_firstheard from archived_game_server where ags_key = '$key' and ags_log_available = 'Y'";
	$result = mysql_query($sql);
	if ($result) {
		if (mysql_num_rows($result) > 0) {
			$id = $key; 
			$file = $cfg_upload_log_path . $id;
			if (file_exists($file) == false) {
				// File no present
				// Delete the log file
				$error_body = "Sorry this log no longer available for download";
				$sql = "update archived_game_server set ags_log_available = 'N' where ags_key = '$key'";
				mysql_query($sql);
			} else {
				$size = filesize($file);
				if ($size != false) {
					$filename = date("YmdHms",mysql_result($result, 0, "ags_firstheard"));
					$filename .= " - " . mysql_result($result, 0, "ags_mapname");
					$players = getPlayerString($key);
					if (strlen($players) > 0) {
						$filename .= " - " . $players;
					}
					$filename .= ".wbv";
					
					$sql = "update archived_game_server set ags_log_downloads = ags_log_downloads + 1 where ags_key = '$key'";
					mysql_query($sql);
					header("Content-Length: ".$size. "bytes");
					header("Content-Description: WinBolo Log File");
					header("Content-Type: application/winbolologfile");
					header("Content-Disposition: attachment; filename=\"$filename\"");
					readfile($file);
					$error = false;
				}
			}
		}
	} else {
		$error_body = "Log does not exist";
	}
}

if ($error == true) {
	$html_title="WinBolo.net: Operation Failed";
	require "$BASE_FILES/inc_top.php";
	if (!isset($error_body)) {
		$error_body = "Error occurred downloading log file.";
	}
	require "$BASE_FILES/inc_error.php";
	require "$BASE_FILES/inc_bottom.php";
}
?>

