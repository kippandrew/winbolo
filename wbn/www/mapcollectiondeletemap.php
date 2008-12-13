<?
require "../php/header.php";
$id = stripslashes($HTTP_GET_VARS['id']);
if (isset($HTTP_GET_VARS['id']) && $userdata['session_logged_in'] == true && $id != "") {
        $pid = $userdata['user_id'];
	$md = mcGetMapDetails($id);
	if ($md != null) {
		$parents = $md->getParents();
		$parent = $parents[0]->getId();
		#echo "size = " . sizeof($parents) . "<br>";
		if ($md->getPid() == $pid) {
			#OK to delete
			$id = mysql_escape_string($id);
			$sql = "delete from map_meta where mm_id = '$id'";
			mysql_query($sql);
			$sql = "delete from map_comment where mc_mapid = '$id'";
			mysql_query($sql);
			$sql = "delete from map_data where md_id = '$id'";
			mysql_query($sql);
			$error = "";
		} else {
			$error = "Sorry you can't delete maps you don't own.";
		}
	} else {
		$error = "Sorry, map not found to delete!";
	}
} else {
	$error = "Sorry you must be logged in.";
}
if ($error == "") {
	header("Location: mapcollection.php?id=$parent$error");
} else {
$html_title="WinBolo.net: Operation Failed";
require "$BASE_FILES/inc_top.php";
$error_body = $error;
include "$BASE_FILES/inc_error.php";
require "$BASE_FILES/inc_bottom.php";
}
?>
