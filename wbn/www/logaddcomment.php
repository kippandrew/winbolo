<?
require "../php/header.php";
$key = stripslashes($HTTP_POST_VARS['key']);
$comment = stripslashes($HTTP_POST_VARS['comment']);
$rating = stripslashes($HTTP_POST_VARS['rating']);
if (isset($HTTP_POST_VARS['key']) && isset($HTTP_POST_VARS['comment']) && $userdata['session_logged_in'] == true && $key != "") {
	$comment = trim($comment);
	$len = strlen($comment);
	$info = getArchivedGameInfo($key);
	if ($info != null && $len > 0) {
	        $pid = $userdata['user_id'];
		$comment = mysql_escape_string($comment);
		$commented = logHasCommented($key, $pid);
		$key = mysql_escape_string($key);
		if ($commented == true || $rating < 1 || $rating > 10) {
			$sqlRating = 0;
		} else {
			$sqlRating = "'" . mysql_escape_string($rating) . "'";
		}
		$time = time();
		$sql = "insert into log_comment (lc_key, lc_timestamp, lc_pid, lc_rating, lc_comment) values ('$key', NOW(), '$pid', $sqlRating, '$comment')";
		mysql_query($sql);
		logCalculateRating($key);
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
	header("Location: gamelog.php?mode=comments&key=" . $HTTP_POST_VARS['key']);
} else {
$html_title="WinBolo.net: Operation Failed";
require "$BASE_FILES/inc_top.php";
$error_body = $error;
include "$BASE_FILES/inc_error.php";
require "$BASE_FILES/inc_bottom.php";
}
?>
