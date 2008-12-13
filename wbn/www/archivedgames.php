<?
require "../php/header.php";
$html_title="WinBolo.net: Archived Games";
include "$BASE_FILES/inc_top.php";

$start = $HTTP_GET_VARS['start'];
if (!isset($start) || $start == null || $start < 0) {
	$start = 0;
}
$partialServerList = getArchivedGames($start);
if ($partialServerList == null) {
	$servers = array();
} else {
	$servers = $partialServerList->getServers();
	$numServers = $partialServerList->getEntireSize();
}

if (sizeof($servers) > 0) {
	require "$BASE_FILES/inc_archive.php";
} else {
	$error_heading="Archived Games";
	$error_body="Sorry there have been no registered games in the past 24 hours";
	include "$BASE_FILES/inc_error.php";
}

include "$BASE_FILES/inc_bottom.php";
?>
