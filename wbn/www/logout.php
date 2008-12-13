<?
require "../php/header.php";

$userdata = session_pagestart($user_ip, PAGE_LOGIN);

if ($userdata['session_logged_in']) {
	session_end($userdata['session_id'], $userdata['user_id']);
}
header("Location: index.php");

?>
