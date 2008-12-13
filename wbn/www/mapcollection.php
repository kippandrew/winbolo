<?
require "../php/header.php";
$html_title="WinBolo.net: Map Collections";
$id = $HTTP_GET_VARS['id'];
$mc = mcGetLevel($id);
$levelExists = true;
include "$BASE_FILES/inc_mapcollection_top.php";
if (isset($HTTP_GET_VARS['id']) && $mc->getPid() == '') {
	$mc = mcGetLevel('');
	$levelExists = false;
}
require "$BASE_FILES/inc_mapcollection_navigation.php";
if (sizeof($mc->getSubFolders()) > 0) {
	require "$BASE_FILES/inc_mapcollection_folders.php";
}
if (sizeof($mc->getMaps()) > 0) {
	require "$BASE_FILES/inc_mapcollection_maps.php";
}


if ($levelExists == true && $userdata['session_logged_in'] == true && $userdata['user_id'] == $mc->getPid()) {
	include "$BASE_FILES/inc_mapcollection_options.php";
}

require "$BASE_FILES/inc_mapcollection_bottom.php";
?>

