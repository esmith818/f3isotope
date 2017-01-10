<?php

/**
 * @file
 * This file contains all of the functions that deal with the database
 * for Date Reminder. The database apis are majorly different between
 * Drupal 6 and Drupal 7, so isolating the database calls in one file
 * keeps most of the module common between the two releases.
 *
 * This version of the file is for Drupal 6.
 */

module_load_include('inc', 'datereminder', 'includes/defines');

/**
 * Clean up any references to a given node or nodes.
 *
 * @param array $nids
 *   nid (or array of nids) to clean up.
 * @param int $en
 *   If not NULL, also remove this nid from {datereminder_enable}.
 *   Defaults to NULL.
 *
 * Note, we'll clean reminders even if this type didn't acutally have
 * reminders enabled. It's always possible that something messed up and
 * left some reminders lying around.
 */
function _datereminder_clean_node_reminders($nids, $en = NULL) {
  if (!is_array($nids)) {
    $nids = array($nids);
  }
  db_delete('datereminder')
    ->condition('nid', $nids, 'IN')
    ->execute();

  if (isset($en)) {
    db_delete('datereminder_enable')
      ->condition('nid', $nids, 'IN')
      ->execute();
  }
}

/**
 * Clean out reminders for everything associated with this type.
 *
 * @todo
 *   Note that this is for when reminders are disabled for a type.
 */
function _datereminder_clean_type_reminders($type) {
  if (_datereminder_type_enabled($type) != DATEREMINDER_TYPE_DISABLED) {

    // There must be a way to do this using a join, but I can't figure it out.
    $q = db_select('node', 'n')
            ->fields('n', 'nid')
            ->condition('type', $type)
            ->execute();
    $nids = array();
    while ($n = $q->fetchObject()) {
      $nids[] = $n->nid;
    }
    if (count($nids) > 0) {
      _datereminder_clean_node_reminders($nids);
    }
  }
}

/**
 * Enable/disable reminders for a node.
 *
 * Note that this should only be used for node types with reminders
 * enabled. (Or at least in RETAIN state.) If a type is set to DISASBLED,
 * all node-specific information should be cleaned out by calling
 * _datereminder_clean_type_reminders().
 *
 * @param node $node
 *   The node
 * @param int $enabled
 *   Value to set enable flag to for this node. If NULL, remove all
 *   record of this node in the datereminder tables.
 */
function _datereminder_set_node_enabled($node, $enabled = NULL) {
  _datereminder_set_nid_enabled($node->nid, $enabled);
}

/**
 * Version of _datereminder_set_node_enabled() that takes nid
 * instead of node structure.
 * @param type $nid
 * @param type $enabled
 */
function _datereminder_set_nid_enabled($nid, $enabled = NULL) {
  switch ($enabled) {
    case NULL:
    case DATEREMINDER_TYPE_DISABLED:
      _datereminder_clean_node_reminders($nid, $enabled);
      break;

    default:
      $d = db_merge('datereminder_enable')
        ->key(array('nid' => $nid))
        ->fields(array('enabled' => $enabled));

      $d->execute();
      break;
  }
}


/**
 * Load node enable status from the database.
 *
 * @param int $nid
 *   nid of the node.
 *
 * @return int
 *   Returns the DATEREMINDER_TYPE_* value for this node from
 *   {datereminder_enable}. If NULL, this nid isn't in the database.
 */
function _datereminder_get_node_enabled($nid) {

  $q = db_select('datereminder_enable', 'd')
      ->condition('d.nid', $nid)
      ->fields('d', array('enabled'));

  $result = $q->execute();

  $e = $result->fetchObject();
  if ($e) {
    return $e->enabled;
  }

  return NULL;
}

/**
 * Load reminders filtering one one or more keys.
 *
 * @param array $selectors
 *   a key:value array specifying what to load from
 *   the reminders table. Anything in the reminder table is fair game, but
 *   generally the keys are 'nid', 'rid',  or 'uid'.
 * @param node $node
 *   Optional parameter. If set, $r->node is set for any loaded reminder
 *   with the right nid.
 * @param string $sortkey
 *   Optional parameter. If set, returned nodes are sorted in this order.
 *
 * @return array
 *   an array of reminder objects as selected from the database, keyed by rid.
 *   Each object also includes the node title and user name. Orer is as
 *   specified by sortkey.
 *
 * @example: $reminders = _datereminder_load_reminders(array('uid' => $uid));
 */
function _datereminder_load_reminders(
        $selectors,
        $node = NULL,
        $sortkey = NULL) {

  $ret = array();

  $q = db_select('datereminder', 'r');
  $q->join('node', 'n', 'r.nid = n.nid');
  $q->join('users', 'u', 'r.uid = u.uid');
  $q->fields('r');
  $q->fields('u', array('name'));
  $q->fields('n', array('title'));

  foreach ($selectors as $k => $v) {
    $q->condition("r.$k", $v);
  }

  if ($sortkey != NULL) {
    $q->orderBy("r.$sortkey");
  }

  $rems = $q->execute()->fetchAll();

  // Return is array keyed by rid.
  $ret = array();
  $user = $GLOBALS['user'];
  foreach ($rems as $r) {
    if ($node && ($node->nid == $r->nid)) {
      $r->node = $node;
    }
    if ($user && ($user->uid == $r->uid)) {
      $r->user = $user;
    }
    $ret[$r->rid] = $r;
  }

  return $ret;
}

/**
 * Write back user reminder information.
 */
function _datereminder_set_reminders(&$reminders) {
  foreach (array_keys($reminders) as $k) {
    $r = $reminders[$k];
    // Should we save this reminder?
    if (($r->leadtime == 0) || !isset($r->next_due)) {
      // No reminder, so delete any existing entry.
      if ($r->rid > 0) {
        db_delete('datereminder')
          ->condition('rid', $r->rid)
          ->execute();
      }
      unset($reminders[$k]);
    }
    else {
      if (!isset($r->expired)) {
        $r->expired = 0;
      }
      $fvals = array(
        'nid' => $r->nid,
        'uid' => $r->uid,
        'leadtime' => $r->leadtime,
        'next_due' => $r->next_due,
        'email' => $r->email,
        'expired' => $r->expired,
      );

      if ($r->rid > 0) {
        $q = db_merge('datereminder')
          ->key(array('rid' => $r->rid))
          ->fields($fvals)
          ->execute();
      }
      else {
        // This is a new entry.
        $q = db_insert('datereminder')
          ->fields($fvals);
        $rid = $q->execute();
        if ($rid > 0) {
          $r->rid = $rid;
          // Update worked, so adjust array indix to reflect new rid.
          unset($reminders[$k]);
          $reminders[$rid] = $r;
        }
      }
    }
  }
}

/**
 * Delete a list of reminder ids.
 *
 * @param array $rids
 *   List of rids to delete.
 */
function _datereminder_delete_rids($rids) {
  db_delete('datereminder')
    ->condition('rid', $rids, 'IN')
    ->execute();
}


/**
 * Return array of reminders with next_due older than $timestamp.
 *
 * @param int $dt
 *   Timestamp for due reminders.
 *
 * @return array
 *   Array of past-due reminders, indexed by rid.
 */
function _datereminder_get_due_reminders($dt) {

  $q = db_select('datereminder', 'r')
          ->fields('r')
          ->condition('next_due', intval($dt), '<=');

  $rems = $q->execute()->fetchAll();

  // Return is array keyed by rid.
  $ret = array();
  foreach ($rems as $r) {
    $ret[$r->rid] = $r;
  }

  return $ret;
}

/**
 * Set enabled for all nodes of the given type.
 *
 * @param type $type
 *   Node type.
 */
function _datereminder_enable_all_nodes($type) {
  $q = db_select('node', 'n');
  $q->leftjoin('datereminder_enable', 'e', 'n.nid = e.nid');
  $q->condition('n.type', $type)
          ->fields('n', array('nid'))
          ->fields('e', array('enabled'));

  $result = $q->execute();
  foreach ($result as $rec) {
    if (!isset($rec->enabled) || ($rec->enabled != DATEREMINDER_TYPE_ON)) {
      _datereminder_set_nid_enabled($rec->nid, DATEREMINDER_TYPE_ON);
    }

  }

}
