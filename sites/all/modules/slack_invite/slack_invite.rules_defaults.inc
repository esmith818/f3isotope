<?php

/**
 * @file
 * Slack invite module rules default configuration.
 */

/**
 * Implements hook_rules_default_rules_configuration().
 */
function slack_invite_default_rules_configuration() {
  $send_invite_rule_export = '{ "rules_slack_invite_send_invite_one_time" : {
    "LABEL" : "Send Slack Invite one-time",
    "PLUGIN" : "reaction rule",
    "OWNER" : "rules",
    "REQUIRES" : [ "rules", "slack_invite" ],
    "ON" : { "user_login" : [] },
    "IF" : [ { "data_is_empty" : { "data" : [ "account:last-access" ] } } ],
    "DO" : [ { "slack_send_invite" : { "account" : [ "account" ] } } ]
  }
}';
  $configs['slack_invite_send_invite_one_time'] = rules_import($send_invite_rule_export);
  return $configs;
}
