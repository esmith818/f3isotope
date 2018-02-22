<?php

/**
 * @file
 * Provides hook documentation for the Voting Like module.
 */

/**
 * Save a vote in the database.
 *
 * @param $vote
 *   See votinglike_vote() for the structure of this array, with the
 */
function hook_votinglike_vote(&$vote) {
  //TODO
}

/**
 * Remove a vote in the database.
 *
 * @param $type, $eid, $uid, $tag
 *   See votinglike_unvote() for the structure of this param, with the
 */
function hook_votinglike_unvote($type, $eid, $uid, $tag) {
  //TODO
}

