<?
require "../php/db.php";
require "$BASE_FILES/functions.php";
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
$sql = "select img from map where pid = '$pid'";
$result = mysql_query($sql); 
if (mysql_numrows($result) == 0) {
	closeDatabase();
	return;

}
header("content-type: image/png"); 
echo mysql_result($result, 0, "img");
closeDatabase();

?>
