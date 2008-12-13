<?
$html_title="WinBolo.net: Log Viewer Game Information";
require "../php/header.php";
include "$BASE_FILES/inc_top.php";
$key = $HTTP_GET_VARS['key'];
$info = getLogViewerGameInfo($key);
if ($info == 0) {
	$error_body="Sorry this log does not exist.";
	include "$BASE_FILES/inc_error.php";
} else {
	$log = getArchivedGameLog($info->getKey());
	include "$BASE_FILES/inc_archivedetail.php";
}
include "$BASE_FILES/inc_bottom.php";
?>
