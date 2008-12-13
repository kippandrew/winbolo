<?
require "../php/header.php";
$id = stripslashes($HTTP_POST_VARS['id']);
$comment = stripslashes($HTTP_POST_VARS['comment']);
$rating = stripslashes($HTTP_POST_VARS['rating']);
if (isset($HTTP_POST_VARS['id']) && isset($HTTP_POST_VARS['comment']) && $userdata['session_logged_in'] == true && $id != "") {
	$comment = trim($comment);
	$len = strlen($comment);
	if (mcMapExist($id) == true && $len > 0) {
	        $pid = $userdata['user_id'];
		$comment = mysql_escape_string($comment);
		$commented = mcHasCommented($pid);
		$md = mcGetMapDetails($id);
		if ($commented == true || $rating < 1 || $rating > 10) {
			$sqlRating = 0;
		} else {
			$sqlRating = "'" . mysql_escape_string($rating) . "'";
		}
		$sqlId = mysql_escape_string($id);
		$sql = "insert into map_comment (mc_mapid, mc_pid, mc_rating, mc_comment) values ('$sqlId', '$pid', $sqlRating, '$comment')";
		mysql_query($sql);
		mcCalculateRating($id);
	} else {
		if ($len == 0) {
			$error = "You must enter a comment";
		} else {
			$error = "Map does not exist any more";
		}
	}
} else {
	$error = "Sorry you must be logged in.";
}
if ($error == "") {
	header("Location: mapcollectioninfo.php?id=$id");
} else {
$html_title="WinBolo.net: Operation Failed";
require "$BASE_FILES/inc_top.php";
$error_body = $error;
include "$BASE_FILES/inc_error.php";
require "$BASE_FILES/inc_bottom.php";
}
?>
