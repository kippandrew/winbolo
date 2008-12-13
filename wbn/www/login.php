<?
require "../php/header.php";

$username = isset($HTTP_POST_VARS['username']) ? $HTTP_POST_VARS['username'] : '';
$password = isset($HTTP_POST_VARS['password']) ? $HTTP_POST_VARS['password'] : '';
$username = mysql_escape_string(str_replace("\\", "", $username));
$password = str_replace("\\", "", $password);
$sql = "SELECT user_id, username, user_password, user_active, user_level FROM " . USERS_TABLE . " WHERE username = '" .  $username . "'";
$result = $db->sql_query($sql);
if (!$result) {
	echo "db error";
	exit;
} else {
	$row = $db->sql_fetchrow($result);
	if($row) {
		if( md5($password) == $row['user_password'] && $row['user_active'] ) {
			$autologin = ( isset($HTTP_POST_VARS['autologin']) ) ? TRUE : 0;
			$session_id = session_begin($row['user_id'], $user_ip, PAGE_INDEX, FALSE, $autologin);
			header("Location: index.php");
			exit;
		} else {
		  # Password incorrect
		  header("Location: lostpassword.php?login=true");
		  exit;
		}
	}
	header("Location: lostpassword.php?login=true");

	exit;
}
exit;
?>

