<?
require "../php/header.php";
$html_title="WinBolo.net: Map Collections - Map Information";
$id = $HTTP_GET_VARS['id'];
$done = false;
if (isset($id)) {
	$md = mcGetMapDetails($id, true);
	if ($md != null) {
		$done=true;
		require "$BASE_FILES/inc_mapcollection_top.php";
		require "$BASE_FILES/inc_mapcollection_mapdetails.php";
		require "$BASE_FILES/inc_mapcollection_bottom.php";
	}
}

if ($done == false) {
	#error
	$error_mbody = "Map does not exist. Maybe the owner deleted it.";
	require "$BASE_FILES/inc_top.php";
	require "$BASE_FILES/inc_error.php";
	require "$BASE_FILES/inc_bottom.php";
}
?>

