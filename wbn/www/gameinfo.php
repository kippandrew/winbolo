<?
$html_title="WinBolo.net: Game Information";
require "../php/header.php";
include "$BASE_FILES/inc_top.php";
$ip = $HTTP_GET_VARS['ip'];
$port = $HTTP_GET_VARS['port'];
$key = $HTTP_GET_VARS['key'];
$info = getGameInfo($ip, $port, $key);
if ($info == 0) {
	$error_body="Sorry this game is no longer in progress.<p>You may like to try the <a href='/archivedgames.php'>Archived Game Logs</a>";
	include "$BASE_FILES/inc_error.php";
} else {
	$log = getGameLog($info->getKey());
	include "$BASE_FILES/inc_gamedetail.php";
}
include "$BASE_FILES/inc_bottom.php";
?>
