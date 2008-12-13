<?
require "../php/header.php";
$html_title="WinBolo.net: Map Collections";
require "$BASE_FILES/inc_mapcollection_top.php";
$name = $HTTP_POST_VARS['name'];
$inOrder = $HTTP_POST_VARS['order'];
if (isset($HTTP_POST_VARS['name']) && strlen($name) > 0) {
	#name search
	$rating = false;
	$useName = true;
	$useDate = false;
	if ($inOrder == "newest") {
		$order = false;
	} else {
		$order = true;
	}
} 

if (isset($HTTP_POST_VARS['search']) && $HTTP_POST_VARS['search'] == "rating") {
	$rating = true;
	$name = '';
	$useName = false;
	$useDate = false;
	if ($inOrder == "newest") {
		$order = true;
	} else {
		$order = false;
	}


} else if (isset($HTTP_POST_VARS['search']) && $HTTP_POST_VARS['search'] == "date") {
	$rating = false;
	$name = '';
	$useName = false;
	$useDate = true;
	if ($inOrder == "newest") {
		$order = true;
	} else {
		$order = false;
	}

}

$results = mcSearchResults($name, $useName, $rating, $useDate, $order);
require "$BASE_FILES/inc_mapcollection_searchresults.php";
require "$BASE_FILES/inc_mapcollection_bottom.php";
?>

