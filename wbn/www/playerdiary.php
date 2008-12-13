<?
require "../php/header.php";
$html_title="WinBolo.net: Player Diary";
include "$BASE_FILES/inc_top.php";

$start = $HTTP_GET_VARS['start'];
$username= $HTTP_GET_VARS['player'];
if (!isset($HTTP_GET_VARS['player'])) {
	$error_body = "Missing Parameters";
	include "$BASE_FILES/inc_error.php";
	include "$BASE_FILES/inc_bottom.php";
	exit();
}

if (!isset($start) || $start == null || $start < 0) {
  $start = 0;
}

$partialServerList = getJournalEntries($username, $start);
if ($partialServerList == null) {
	$error_body = "Player does not exist";
	include "$BASE_FILES/inc_error.php";
	include "$BASE_FILES/inc_bottom.php";
	exit();
}

$servers = $partialServerList->getServers();
$numServers = $partialServerList->getEntireSize();

if (sizeof($servers) > 0) {
	require "$BASE_FILES/inc_diarylist.php";
} else {
	$error_heading="Player Game Diary";
	$error_body="Sorry this player has not played any games";
	include "$BASE_FILES/inc_error.php";
}

include "$BASE_FILES/inc_bottom.php";
?>

