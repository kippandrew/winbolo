<?
if (isset($HTTP_GET_VARS['logout']) && $HTTP_GET_VARS['logout'] == "true") {
	header("Location: /logout.php");
} else {
	header("Location: /signup.php");
}
exit;
?>
