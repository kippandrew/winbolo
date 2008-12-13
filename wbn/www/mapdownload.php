<?
require "../php/db.php";
require "../php/functions.php";
$site = "localhost";
$username = $cfg_winbolodb_user;
$password = $cfg_winbolodb_pass;
$dbName = $cfg_winbolodb;
mysql_connect($site, $username, $password) or die("I sux");
mysql_select_db($dbName);

$name = $HTTP_GET_VARS['name'];
$pid = getPid($name);
if ($pid == 0) {
	return;
}
$sql = "select name, map from map where pid = '$pid'";
$result = mysql_query($sql); 
if (mysql_numrows($result) == 0) {
	closeDatabase();
	return;
}
$name = "Content-Disposition: filename=\"" . mysql_result($result, 0, "name") . ".map\"";
header($name); 
header( "Content-Description: WinBolo Map" );
header("content-type: application/winbolomap"); 
echo mysql_result($result, 0, "map");
closeDatabase();

?>
