<?
require "../php/header.php";
$id = stripslashes($HTTP_GET_VARS['id']);
if (isset($HTTP_GET_VARS['id']) && $userdata['session_logged_in'] == true) {
        $pid = $userdata['user_id'];
	$mc = mcGetLevel($id);
#	echo "<pre>";
#	print_r($mc);
#	echo "</pre>";
	$parents = $mc->getParents();
	$parent = $parents[0]->getId();
	#echo "size = " . sizeof($parents) . "<br>";
	if (sizeof($parents) <= 1) {
		$error = "Sorry you can't delete a top level folder.";
	} else if (sizeof($mc->getSubFolders()) > 0 || sizeof($mc->getMaps()) > 0) {
		$error = "Sorry, you can't delete a folder with items in it";
	} else if ($mc->getPid() == $pid) {
		#OK to delete
		$id = mysql_escape_string($id);
		$sql = "delete from map_structure where ms_id = '$id'";
		#echo $sql;
		mysql_query($sql);
		$error = "";
	} else {
		$error = "Sorry you can't modify folders you don't own.";
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
