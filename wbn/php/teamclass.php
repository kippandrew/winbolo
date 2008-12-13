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

class teamInfoClass {
  var $name;
  var $owner;
  var $blurb;
  var $openNumGames;
  var $openNumPills;
  var $openNumBases;
  var $openNumTanks;
  var $tournNumGames;
  var $tournNumWins;
  var $tournNumLoses;
  var $tournScore;
  var $tournNumPills;
  var $tournNumBases;
  var $tournNumTanks;
  var $tournRank;
  var $strictNumGames;
  var $strictScore;
  var $strictNumWins;
  var $strictNumLoses;
  var $strictNumPills;
  var $strictNumBases;
  var $strictNumTanks;
  var $strictRank;
  var $players;
  var $website;
  var $registerDate;
  var $views;

  function setStuff($name, $owner, $blurb, $openNumGames, $openNumPills, $openNumBases, $openNumTanks, $tournNumGames, $tournNumWins, $tournNumLoses, $tournScore, $tournNumPills, $tournNumBases, $tournNumTanks, $tournRank, $strictNumGames, $strictScore, $strictNumWins, $strictNumLoses, $strictNumPills, $strictNumBases, $strictNumTanks, $strictRank, $website, $regDate) {
  $this->name = $name;
  $this->owner = $owner;
  $this->blurb = $blurb;
  $this->openNumGames = $openNumGames;
  $this->openNumPills = $openNumPills;
  $this->openNumBases = $openNumBases;
  $this->openNumTanks = $openNumTanks;
  $this->tournNumGames = $tournNumGames;
  $this->tournNumWins = $tournNumWins;
  $this->tournNumLoses = $tournNumLoses;
  $this->tournScore = $tournScore;
  $this->tournNumPills = $tournNumPills;
  $this->tournNumBases = $tournNumBases;
  $this->tournNumTanks = $tournNumTanks;
  $this->tournRank = $tournRank;
  $this->strictNumGames = $strictNumGames;
  $this->strictScore = $strictScore;
  $this->strictNumWins = $strictNumWins;
  $this->strictNumLoses = $strictNumLoses;
  $this->strictNumPills = $strictNumPills;
  $this->strictNumBases = $strictNumBases;
  $this->strictNumTanks = $strictNumTanks;
  $this->strictRank = $strictRank;
  $this->website = $website;
  $this->registerDate = $regDate;
}

  function getName() {
    return $this->name;
  }

  function getOwner() {
    return $this->owner;
  }

function setPageViews($views) {
  $this->views = $views;
}

function getPageViews() {
  return $this->views;
}


function getBlurb() {
  return $this->blurb;
}

function getOpenNumGames() {
  return $this->openNumGames;
}
  function getOpenNumPills() {
  return $this->openNumPills;
  }
  function getOpenNumBases() {
  return $this->openNumBases;
  }
  function getOpenNumTanks() {
  return $this->openNumTanks;
  }
  function getTournNumGames() {
  return $this->tournNumGames;
  }
  function getTournNumWins() {
  return $this->tournNumWins;
  }
  function getTournNumLoses() {
  return $this->tournNumLoses;
  }
  function getTournScore() {
  return $this->tournScore;
  }
  function getTournNumPills() {
  
  return $this->tournNumPills;
  }
  function getTournNumBases() {
  return $this->tournNumBases;
  }
  function getTournNumTanks() {
  return $this->tournNumTanks;
  }
  function getTournRank() {
  return $this->tournRank ;
  }
  function getStrictNumGames() {
  return $this->strictNumGames;
  }
  function getStrictScore() {
  return $this->strictScore;
  }
  function getStrictNumWins() {
  return $this->strictNumWins;
  }
  function getStrictNumLoses() {
  return $this->strictNumLoses;
  }
  function getStrictNumPills() {
  return $this->strictNumPills;
  }
  function getStrictNumBases() {
  return $this->strictNumBases;
  }
  function getStrictNumTanks() {
  return $this->strictNumTanks;
  }
  function getStrictRank() {
  return $this->strictRank;
  }

  function getWebsite() {
  return $this->website;
  }

  function setPlayers($players) {
    $this->players = $players;
  }
  function getPlayers() {
    return $this->players;
  }

 function getRegisteredDate() {
  return $this->registerDate;
 }

 function setTournRank($rank) {
  $this->tournRank = $rank;
 }

 function setStrictRank($rank) {
  $this->strictRank = $rank;
 }


}

?>
