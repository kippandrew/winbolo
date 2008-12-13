<?
require "../php/header.php";
$id = stripslashes($HTTP_POST_VARS['id']);
$name = stripslashes($HTTP_POST_VARS['name']);
$comment = stripslashes($HTTP_POST_VARS['comment']);

if ($userdata['session_logged_in'] == true && isset($HTTP_POST_VARS['id']) && isset($HTTP_POST_VARS['name'])) {
	$name = trim($name);
	$comment = trim($comment);
	$owner = mcGetOwnerFromLevelId($id);
	$pid = $userdata['user_id'];
	if (strlen($name) > 0 && $owner != -1 && $owner == $pid && mcFolderNameExists($id, $name) == false) {
		$name = mysql_escape_string($name);
		$sqlId = mysql_escape_string($id);
		$comment = mysql_escape_string($comment);
		$sql = "insert into map_structure (ms_pid, ms_name, ms_parent, ms_comment) values ('$pid', '$name', '$sqlId', '$comment')";
		mysql_query($sql);
	}
}
header("Location: mapcollection.php?id=$id");

?>
