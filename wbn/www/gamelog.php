<?
$html_title="WinBolo.net: Archived Game Information";
require "../php/header.php";
if (!isset($HTTP_GET_VARS['key'])) {
	$error_body = "Missing Parameters";
	include "$BASE_FILES/inc_error.php";
	include "$BASE_FILES/inc_bottom.php";
}
if ($HTTP_GET_VARS['mode'] == "comments") {
	$mode = "comments";
} else if ($HTTP_GET_VARS['mode'] == "logfile") {
	$mode = "logfile";
# unused XML mode
#} else if (strtolower($HTTP_GET_VARS['mode']) == "xml") {
#	$mode = "xml";
} else {
	$mode = "events";
}

$key = $HTTP_GET_VARS['key'];
$info = getArchivedGameInfo($key);

if ($info != null && $mode == "xml") {
	$log = getArchivedGameLog($info->getKey());
#	include "$BASE_FILES/inc_gamedetailxml.php";
	exit;
}

include "$BASE_FILES/inc_top.php";
if ($info == null) {
	$error_body="Sorry this log does not exist.";
	include "$BASE_FILES/inc_error.php";
} else {
	include "$BASE_FILES/inc_archivedetail.php";
	if ($mode == "events") {
		$log = getArchivedGameLog($info->getKey());
		include "$BASE_FILES/inc_archivedetail_events.php";
	} else if ($mode == "comments") {
		$comments = getGameComments($info->getKey());
		include "$BASE_FILES/inc_archivedetail_comments.php";
	} else {
		# log file
		include "$BASE_FILES/inc_archivedetail_logfile.php";
	}
}
include "$BASE_FILES/inc_bottom.php";
?>
