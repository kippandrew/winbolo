<?
require "../php/db.php";
$site = "localhost";
$username = $cfg_winbolodb_user;
$password = $cfg_winbolodb_pass;
$dbName = $cfg_winbolodb;
mysql_connect($site, $username, $password) or die("I sux");
mysql_select_db($dbName);

$id = mysql_escape_string($HTTP_GET_VARS['id']);
$sql = "select m.mm_name, d.md_map from map_meta m, map_data d where m.mm_id = '$id' && m.mm_id = d.md_id";
$result = mysql_query($sql); 
if (mysql_numrows($result) == 0) {
	closeDatabase();
	return;
}
$sql = "update map_meta set mm_downloads = mm_downloads + 1 where mm_id = '$id'";
mysql_query($sql);

$name = "Content-Disposition: filename=\"" . mysql_result($result, 0, 0) . ".map\"";
header($name); 
header( "Content-Description: WinBolo Map" );
header("content-type: application/winbolomap"); 
echo mysql_result($result, 0, 1);
?>
