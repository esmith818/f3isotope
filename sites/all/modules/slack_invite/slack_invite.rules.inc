<?php

/**
 * @file
 * Slack integration module rules functions.
 */

/**
 * Implements hook_rules_action_info().
 */
function slack_invite_rules_action_info() {
  $actions = array();
  $actions['slack_send_invite'] = array(
    'base' => 'slack_rules_send_invite_action',
    'label' => t('Slack send invite'),
    'group' => t('Slack Invite'),
    'parameter' => array(
      'account' => array(
        'type' => 'user',
        'label' => t('User'),
        'description' => t("Slack invite will be sent to user's email address."),
        'optional' => FALSE,
      ),
    ),
  );
  return $actions;
}

/**
 * Rules action for sending an invite to Slack.
 */
function slack_rules_send_invite_action($account) {
  global $user;
  if (!$account) {
    $account = $user;
  }
  // Need to figure out what I should do with response.
  $response = _slack_invite_send_invite($account->mail);
}
