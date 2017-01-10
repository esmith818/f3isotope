<?php

/**
 * @file
 * Support for node operations for Date Reminder. These are all administrative
 * operations (insert, update, delete). The common view operations are in
 * datereminder.module.
 *
 * These are D7-format functions. They'll be called directly from core
 * in D7. They're called from datereminder_node_api() in D6.
 */

/**
 * Implements hook_node_insert().
 *
 * Set default enabled for the node. Note that "enable" status for
 * this node would have been set on original submit. Since this is a new
 * node, we know it isn't already in teh database.
 */
function _datereminder_node_insert($node) {
  if (isset($node->datereminder_enabled)) {
    module_load_include('inc', 'datereminder', 'includes/defines');

    $en = $node->datereminder_enabled;
    if ($en != DATEREMINDER_TYPE_DISABLED) {
      module_load_include('inc', 'datereminder', DATEREMINDER_DB);
      _datereminder_set_node_enabled($node, $en);
    }
  }
}

/**
 * Implements hook_node_update().
 *
 * Update any reminders that might have changed when the node
 * (its date, in particular) changed.
 */
function _datereminder_node_update($node) {
  module_load_include('inc', 'datereminder', 'includes/defines');
  $enabled = _datereminder_type_enabled($node->type);

  if ($enabled >= DATEREMINDER_TYPE_ALLOWED) {
    module_load_include('inc', 'datereminder', DATEREMINDER_DB);
    _datereminder_set_node_enabled($node, $node->datereminder_enabled);
    _datereminder_update_node_reminders($node);
  }
}


/**
 * Update all reminders for a node if the node is updated.
 *
 * @param node $node
 *   Node that's being written back.
 */
function _datereminder_update_node_reminders($node) {
  module_load_include('inc', 'datereminder', 'includes/defines');
  module_load_include('inc', 'datereminder', DATEREMINDER_DB);
  $rems = _datereminder_load_reminders(array('nid' => $node->nid), $node);
  _datereminder_update_reminder_nexts($rems);
}

/**
 * Recompute "next" time and write back a group of reminders.
 *
 * This might be reminders associated with a node when the node changes,
 * or a set of reminders whose time has expired and we need to move on
 * to the next occurance.
 *
 * Note that reminders that are past last occurance of this node
 * will be deleted.
 *
 * @param array $rems
 *   list of reminders
 */
function _datereminder_update_reminder_nexts($rems) {
  module_load_include('inc', 'datereminder', 'includes/date');
  module_load_include('inc', 'datereminder', 'includes/defines');
  module_load_include('inc', 'datereminder', DATEREMINDER_DB);
  foreach ($rems as $r) {
    _datereminder_complete_reminder($r);
    _datereminder_get_next_reminder($r);
  }
  _datereminder_set_reminders($rems);
}

