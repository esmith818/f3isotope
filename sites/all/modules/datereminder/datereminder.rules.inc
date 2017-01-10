<?php
/**
  * @file
 * 
 * DateReminder rules.
 *
 */

function datereminder_rules_action_info() {
  $actions = array(
      'datereminder_rules_action_set_reminder' => array(
          'label' => t('Set a reminder'),
          'group' => t('Date Reminder'),
          'parameter' => array(
              'node' => array(
                  'type' => 'node',
                  'label' => t('Node'),
                  'save' => true,
              ),
              'user' => array(
                  'type' => 'user',
                  'label' => t('User'),
              ),
              'leadtime' => array(
                  'type' => 'integer',
                  'label' => t('Reminder lead time'),
                  'options list' => 'datereminder_rules_type_leadtime_list',
              ),
          )
      ),
      'datereminder_rules_action_set_enable_reminder' => array(
          'label' => t('Set reminder enable on node'),
          'group' => t('Date Reminder'),
          'parameter' => array(
              'node' => array(
                  'type' => 'node',
                  'label' => t('Node'),
                  'save' => true,
              ),
              'enabled' => array(
                  'type' => 'boolean',
                  'label' => t('Enable setting'),
              ),
          )

      )

  );
  return $actions;
}

function datereminder_rules_condition_info() {
  $conditions = array(
      'datereminder_rules_condition_reminder_enabled' => array(
          'label' => t('Node allows reminders'),
          'group' => t('Date Reminder'),
          'parameter' => array(
             'node' => array(
                  'type' => 'node',
                  'label' => t('Node'),
                  'save' => false,
              ),
          ),
      ),
      'datereminder_rules_condition_user_may_set' => array(
          'label' => t('User may set reminder on node'),
          'group' => t('Date Reminder'),
          'parameter' => array(
             'node' => array(
                  'type' => 'node',
                  'label' => t('Node'),
                  'save' => false,
              ),
              'user' => array(
                  'type' => 'user',
                  'label' => t('User'),
                  'save' => false,
              ),
          ),
      ),
  );

  return $conditions;
}

/**
 * Return set of allowed reminder values for the set reminder rule.
 * @return type
 */
function datereminder_rules_type_leadtime_list() {
  module_load_include('inc', 'datereminder', 'includes/datereminder_form');
  return _datereminder_leadtime_list();

}


/**
 * Called as a rule action to set a reminder on the given node.
 * @param type $node
 * @param type $user
 * @param type $lead
 */
function datereminder_rules_action_set_reminder($node, $user, $lead) {
  if (!datereminder_rules_condition_user_may_set($node, $user)) {
    return;
  }

  module_load_include('inc', 'datereminder', 'includes/db7');

  // Get existing reminders so we can check for redundancy.
  $selector = array(
      'nid' => $node->nid,
      'uid' => $user->uid,
  );
  $reminders = _datereminder_load_reminders($selector, $node);

  foreach ($reminders as $r) {
    if ($r->leadtime == $lead) {
      // Reminder is already set.
      return;
    }
  }

  // Create new reminder.
  $r = (object)array();
  $r->rid = -1;
  $r->leadtime = $lead;
  $r->nid = $node->nid;
  $r->uid = $user->uid;
  $r->email = NULL;
  $r->next_due = NULL;

  module_load_include('inc', 'datereminder', 'includes/node');
  _datereminder_get_next_reminder($r);
  $reminders[] = $r;
  _datereminder_set_reminders($reminders);

}

/**
 * Check if reminders are possible for node.
 * @param type $node
 */
function datereminder_rules_condition_reminder_enabled($node) {
  module_load_include('inc', 'datereminder', 'includes/defines');
  if (!isset($node) || ($node->datereminder_enabled != DATEREMINDER_TYPE_ON)) {
    return false;
  }

  // Does node have any future dates?
  module_load_include('inc', 'datereminder', 'includes/date');
  $datefield = _datereminder_get_node_datefield($node);
  $rtime = _datereminder_get_next_occurance($datefield);
  if (!isset($rtime)) {
    // Date has passed.
    return false;
  }
  return true;
}

/**
 * Can the given user set reminders on this node?
 * @param type $node
 * @param type $user
 * @return boolean
 */
function datereminder_rules_condition_user_may_set($node, $user) {
  if (!isset($user) || ($user->uid == 0)) {
    return false;
  }

  if (!isset($user)) {
    return false;
  }

  if (!datereminder_rules_condition_reminder_enabled($node)) {
    return false;
  }
  
  if (!user_access(DATEREMINDER_REQUEST_REMINDER, $user)) {
    return false;
  }

  return true;
}

function datereminder_rules_action_set_enable_reminder($node, $enable) {
  if (!isset($node)) {
    return;
  }
  $type_enabled = _datereminder_type_enabled($node->type);
  if ($type_enabled >= DATEREMINDER_TYPE_ALLOWED) {
    $node->datereminder_enabled =
            $enable ? DATEREMINDER_TYPE_ON : DATEREMINDER_TYPE_RETAIN;
  }
}