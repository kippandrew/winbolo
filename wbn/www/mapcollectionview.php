<?
require "../php/functions.php";
require "../php/db.php";
$site = "localhost";
$username = $cfg_winbolodb_user;
$password = $cfg_winbolodb_pass;
$dbName = $cfg_winbolodb;
mysql_connect($site, $username, $password) or die("I sux"); 
mysql_select_db($dbName);

$id = mysql_escape_string($HTTP_GET_VARS['id']);
$sql = "select md_img from map_data where md_id = '$id'";
$result = mysql_query($sql); 
if (mysql_numrows($result) == 0) {
	closeDatabase();
	return;

}
header("content-type: image/png"); 
echo mysql_result($result, 0, 0);
closeDatabase();

?>
