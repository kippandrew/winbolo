<?
/*
 * $Id$
 *
 * Copyright (c) 1998-2008 John Morrison.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */	


#Everything to do with the map collections!

#############################################################################
# FUNCTION:     mcFolderNameExist
# AUTHOR:       John Morrison
# DATE:         03/11/02
# RETURNS:      
# PURPOSE:      
# Returns if a folder name exists at level id 
#
# PARAMETERS:
# id - level ID to check 
# name - Name to check
#############################################################################
function mcFolderNameExists($pid) {
	$id = mysql_escape_string($id);
	$name = mysql_escape_string($name);
	$sql = "select ms_id from map_structure where ms_id = '$id' && ms_name = '$name'";
	$results = mysql_query($sql);
	if (mysql_num_rows($results) != 0) {
		return true;
	}
	return false;
}

function mcPrepareSearch($name) {
	return "<a href=mapcollectionsearchcurrent.php?name=" . urlencode($name) . ">$name</a>";
}


function mcGetNumberRatings($id) {
	$id = mysql_escape_string($id);
	$sql = "select count(*) from map_comment where mc_mapid = '$id' and mc_rating > 0";
	$result = mysql_query($sql);
	if (mysql_num_rows($result) == 1) {
		return mysql_result($result, 0, 0);
	}
	return 0;
}

function mcCalculateRating($id) {
	$id = mysql_escape_string($id);
	$sql = "select avg(mc_rating) from map_comment where mc_mapid = '$id' and mc_rating > 0";
	$result = mysql_query($sql);
	$val = mysql_result($result, 0, 0);
	$sql = "update map_meta set mm_rating = '$val', mm_timestame = mm_timestame where mm_id = '$id'";
	mysql_query($sql);
}

function mcPrepareRatingStars($rating) {
	$ret = "";
	$count = 1;
	while ($count <= 10) {
		if ($rating >= $count) {
			$ret .= "<img src='images/star-full.gif'>";
		} else if ($rating+1  > $count) {
			# fractional
			$ret .= "<img src='images/star-half.gif'>";
		} else {
			$ret .= "<img src='images/star-empty.gif'>";
		}
		$count++;
	}
	return $ret;
}

function mcMapExist($id) {
	$id = mysql_escape_string($id);
	$sql = "select mm_id from map_meta where mm_id = '$id'";
	$result = mysql_query($sql);
	if (mysql_num_rows($result) == 1) {
		return true;
	}
	return false;
}

#############################################################################
# FUNCTION:     mcHasCommented
# AUTHOR:       John Morrison
# DATE:         03/11/02
# RETURNS:      
# PURPOSE:      
# Returns if a player has placed a comment (and there for a rating) on a map 
#
# PARAMETERS:
# id - Map id to check
# pid - Player ID to check 
#############################################################################
function mcHasCommented($id, $pid) {
	$id = mysql_escape_string($id);
	$pid = mysql_escape_string($pid);
	$sql = "select mc_id from map_comment where mc_pid = '$pid' and mc_mapid = '$id'";
	$result = mysql_query($sql);
	if (mysql_num_rows($result) > 0) {
		return true;
	}
	return false;
}

#############################################################################
# FUNCTION:     mcHasTopLevelCollection   
# AUTHOR:       John Morrison
# DATE:         03/11/02
# RETURNS:      
# PURPOSE:      
# Returns if a top level collection. Returns true/false
#
# PARAMETERS:
# pid - Player ID to check 
#############################################################################
function mcHasTopLevelCollection($pid) {
	$pid = mysql_escape_string($pid);
	$sql = "select ms_id from map_structure where ms_pid = '$pid'";
	$results = mysql_query($sql);
	if (mysql_num_rows($results) != 0) {
		return true;
	}
	return false;
}

#############################################################################
# FUNCTION:     mcGetOwnerFromLevelId
# AUTHOR:       John Morrison
# DATE:         03/11/02
# RETURNS:      
# PURPOSE:      
# Returns the pid that owns a level id. Returns -1 on error
#
# PARAMETERS:
# id - Player ID to get  
#############################################################################
function mcGetOwnerFromLevelId($id) {
	$id = mysql_escape_string($id);
	$sql = "select ms_pid from map_structure where ms_id = '$id'";
	$results = mysql_query($sql);
	if (mysql_num_rows($results) != 0) {
		return mysql_result($results, 0, 0);
	}
	return -1;
}


#############################################################################
# FUNCTION:     mcGetParent
# AUTHOR:       John Morrison
# DATE:         03/11/02
# RETURNS:      
# PURPOSE:      
# Returns the parent id for a level id. 
#
# PARAMETERS:
# id - id to get  
#############################################################################
function mcGetParent($id) {
	$id = mysql_escape_string($id);
	$sql = "select ms_parent from map_structure where ms_id = '$id'";
	$results = mysql_query($sql);
	if (mysql_num_rows($results) != 0) {
		return mysql_result($results, 0, 0);
	}
	return '';
}

#############################################################################
# FUNCTION:     mcGetTopLevelCollectionId
# AUTHOR:       John Morrison
# DATE:         03/11/02
# RETURNS:      
# PURPOSE:      
# Returns the top level collection id. 
#
# PARAMETERS:
# pid - Player ID to get  
#############################################################################
function mcGetTopLevelCollectionId($pid) {
	$pid = mysql_escape_string($pid);
	$sql = "select ms_id from map_structure where ms_pid = '$pid'";
	$results = mysql_query($sql);
	if (mysql_num_rows($results) != 0) {
		return mysql_result($results, 0, 0);
	}
	return '';
}

#############################################################################
# FUNCTION:     mcCreateTopLevelCollection   
# AUTHOR:       John Morrison
# DATE:         03/11/02
# RETURNS:      
# PURPOSE:      
# Creates a top level collection. Returns OK on success, Errormessage on
# fail.
#
# PARAMETERS:
# pid - Player ID who owns this collection
#############################################################################
function mcCreateTopLevelCollection($pid) {
	$playerName = getPlayerName($pid);
	if ($playerName == null) {
		return "Player does not exist";
	}
	if (mcHasTopLevelCollection($pid) == true) {
		return "Player already has a collection";
	}
	$pid = mysql_escape_string($pid);
	$name = mysql_escape_string($playerName . "'s Maps");
	$sql = "insert into map_structure (ms_pid, ms_name) values ('$pid', '$name')";
	$result = mysql_query($sql);
	if (mysql_affected_rows() != 1) {
		return "Database error";
	}
	return "OK";
}

function mcSearchResults($name, $useName, $rating, $newest, $orderDesc) {
 	$res = array();

	if ($useName == true && strlen($name) > 0) {
		# Name search
		$name = mysql_escape_string(strtolower($name));
		$sql = "select mm_id from map_meta where lower(mm_name) like '$name%'";
		$sqlOrderVal = "mm_name";
	} else if ($rating == true) {
		#rating search
		$sql = "select mm_id from map_meta where mm_rating > 0";
		$sqlOrderVal = "mm_rating";
	} else {
		#Newest search
		$sql = "select mm_id from map_meta";
		$sqlOrderVal = "mm_timestame";
	}

	$sql = $sql . " order by " . $sqlOrderVal;
	if ($orderDesc == true) {
		$sql = $sql . " desc";
	}

	$sql = $sql . " limit 50";
	$results = mysql_query($sql);
	$count = 0;
	$total = mysql_num_rows($results);
	while ($count < $total) {
		$add = mcGetMapDetails(mysql_result($results, $count, 0), false);
		$res[$count] = $add;
		$count++;
	}
	return $res;
}

#############################################################################
# FUNCTION:     mcGetMapDetails
# AUTHOR:       John Morrison
# DATE:         03/11/02
# RETURNS:      
# PURPOSE:      
# Gets enough to create the map details. Returns null on error 
#
# PARAMETERS:
# id - The map ID from map meta 
# wantComments - True if we want to retrieve the comments for this map
#############################################################################
function mcGetMapDetails($id, $wantComments) {
	$md = new mapCollectionMapInfoClass;
	$md->setId($id);
	if ($wantComments == true) {
		$md->setComments(mcGetComments($id));
	}
	
	$id = mysql_escape_string($id);
	$sql = "select mm_id, mm_level, mm_pid, mm_name, mm_bases, mm_pills, mm_starts, mm_rating, unix_timestamp(mm_timestame) as mm_timestame, mm_comment, mm_downloads from map_meta where mm_id = '$id'";
	$result = mysql_query($sql);
	if (mysql_num_rows($result) != 1) {
		return null;
	}

	$md->setup(mysql_result($result, 0, "mm_name"), mysql_result($result, 0, "mm_bases"), mysql_result($result, 0, "mm_pills"), mysql_result($result, 0, "mm_starts"), mysql_result($result, 0, "mm_comment"));
	$md->setTime(mysql_result($result, 0, "mm_timestame"));
	$md->setPid(mysql_result($result, 0, "mm_pid"));
	$md->setOwnerName(getPlayerName($md->getPid()));
	$md->setRating(mysql_result($result, 0, "mm_rating"));
	$md->setDownloads(mysql_result($result, 0, "mm_downloads"));
	# Get parents
	$parents = mcGetParents(mysql_result($result, 0, "mm_level"));
	$level = mysql_result($result, 0, "mm_level");
	$sql = "select ms_name from map_structure where ms_id = '$level'";
	$result = mysql_query($sql);
	$add = new mapCollectionItemClass;
	$add->setup($level, mysql_result($result, 0 , 0));
	$parents = array_reverse($parents);
	$parents[sizeof($parents)] = $add;
	$parents = array_reverse($parents);
	$md->setParents($parents);
	$md->setNumRatings(mcGetNumberRatings($md->getId()));

	return $md;
}	

#############################################################################
# FUNCTION:     mcGetComments
# AUTHOR:       John Morrison
# DATE:         03/11/08
# RETURNS:      
# PURPOSE:      
# Gets the array of comments for this map
#
# PARAMETERS:
# id - The map id to get comments for
#############################################################################
function mcGetComments($id) {
	$comments = array();
	$id = mysql_escape_string($id);
	$sql = "select mc_pid, mc_rating, unix_timestamp(mc_timestamp) as mc_timestamp, mc_comment from map_comment where mc_mapid = '$id' order by mc_timestamp";
	$result = mysql_query($sql);
	if ($result) {
		$total = mysql_num_rows($result);
		$count = 0;
		while ($count < $total) {
			$add = new mapCollectionCommentClass;
			$add->setup(mysql_result($result, $count, "mc_timestamp"), mysql_result($result, $count, "mc_comment"), mysql_result($result, $count, "mc_pid"), mysql_result($result, $count, "mc_rating"));
			$add->setName(getPlayerName($add->getPid()));
			$comments[$count] = $add;
			$count++;
		}
	}
	return $comments;
}

#############################################################################
# FUNCTION:     mcGetLevel
# AUTHOR:       John Morrison
# DATE:         03/11/02
# RETURNS:      
# PURPOSE:      
# Gets enough to create a map collection level. Returns null on error
#
# PARAMETERS:
# id - The level ID from map structure 
#############################################################################
function mcGetLevel($id) {
	$mcl = new mapLevelClass;
	$mcl->setSubFolders(mcGetSubFolders($id));	
	$mcl->setMaps(mcGetMaps($id));
	$mcl->setParents(mcGetParents($id));
	
	$id = mysql_escape_string($id);
	$sql = "select ms_id, ms_pid, ms_name, ms_parent, unix_timestamp(ms_timestamp) as ms_timestamp, ms_comment from map_structure where ms_id = '$id'";
	$result = mysql_query($sql);
	if (mysql_num_rows($result) != 1) {
		return $mcl;
	}

	$mcl->setup(mysql_result($result, 0, 'ms_id'),  mysql_result($result, 0, 'ms_name'), mysql_result($result, 0, 'ms_comment'), mysql_result($result, 0, 'ms_timestamp'), mysql_result($result, 0, 'ms_pid'));
	return $mcl;
}	

function mcGetFolderItemCount($id) {
	$ret = 0;
	$id = mysql_escape_string($id);
	$sql = "select count(*) from map_structure where ms_parent='$id'";
	$result = mysql_query($sql);
	$ret = $ret + mysql_result($result, 0, 0);
	mysql_free_result($result);
	$sql = "select count(*) from map_meta where mm_level='$id'";
	$result = mysql_query($sql);
	$ret = $ret + mysql_result($result, 0, 0);
	mysql_free_result($result);
	return $ret;
}

#############################################################################
# FUNCTION:     mcGetSubFolders
# AUTHOR:       John Morrison
# DATE:         03/11/02
# RETURNS:      
# PURPOSE:      
# Gets the list of subfolders in this map collection. Returns empty array if
# none there
#
# PARAMETERS:
# id - The level ID from map structure 
#############################################################################
function mcGetSubFolders($id) {
	$count = 0;
	$ret = array();
	$id = mysql_escape_string($id);
	if ($id == '') {
		$id = "is NULL";
	} else {
		$id = " = " . $id;
	}
	$sql = "select ms_id, ms_name from map_structure where ms_parent $id order by ms_name";
	$result = mysql_query($sql);
	$total = mysql_num_rows($result);
	while ($count < $total) {
		$add = new mapCollectionItemClass;
		$add->setup(mysql_result($result, $count, 0), mysql_result($result, $count, 1)); 
		$add->setSubItems(mcGetFolderItemCount($add->getId()));
		$ret[$count] = $add;
		$count++;
	}
	return $ret;
}

#############################################################################
# FUNCTION:     mcGetMaps
# AUTHOR:       John Morrison
# DATE:         03/11/02
# RETURNS:      
# PURPOSE:      
# Gets the list of subfolders in this map collection. Returns empty array if
# none there
#
# PARAMETERS:
# id - The level ID from map structure 
#############################################################################
function mcGetMaps($id) {
	$count = 0;
	$ret = array();
	$id = mysql_escape_string($id);
	if ($id == '') {
		$id = "is NULL";
	} else {
		$id = " = " . $id;
	}

	$sql = "select mm_id, mm_name, mm_rating from map_meta where mm_level $id order by mm_name";
	$result = mysql_query($sql);
	$total = mysql_num_rows($result);
	while ($count < $total) {
		$add = new mapCollectionItemClass;
		$add->setup(mysql_result($result, $count, 0), mysql_result($result, $count, 1)); 
		$add->setRating(mysql_result($result, $count, 2));
		$ret[$count] = $add;
		$count++;
	}
	return $ret;
}

#############################################################################
# FUNCTION:     mcGetParents
# AUTHOR:       John Morrison
# DATE:         03/11/02
# RETURNS:      
# PURPOSE:      
# Gets the list of folders above us in this map collection. Returns empty 
# array if none there
#
# PARAMETERS:
# id - The level ID from map structure 
#############################################################################
function mcGetParents($id) {
	$ret = array();
	$finish = false;
	if ($id == "") {
		$add = new mapCollectionItemClass;
		$add->setup("", "Collections"); 
		$ret[0] = $add;
		return $ret;
	}

	$count = 0;
	$parent = mcGetParent($id);
	while ($finish == false) {
		$parent = mysql_escape_string($parent);
		if ($parent == '') {
			$total = 0;
		} else {
			$sql = "select ms_id, ms_name, ms_parent from map_structure where ms_id = $parent";
			$result = mysql_query($sql);
			$total = mysql_num_rows($result);
		}
		if ($total == 0) {
			$add = new mapCollectionItemClass;
			$add->setup("", "Collections"); 
			$ret[$count] = $add;
			$finish = true;
		} else {
			$add = new mapCollectionItemClass;
			$add->setup(mysql_result($result, 0, 0), mysql_result($result, 0, 1)); 
			$ret[$count] = $add;
			$parent = mysql_result($result, 0, 2);
			$count++;
		}
	}
	return $ret;
}

?>
