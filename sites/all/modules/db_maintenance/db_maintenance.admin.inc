<?php

/**
 * @file
 * Module db_maintenance admin section.
 */

use Drupal\db_maintenance\Module\Hook\AdminHookHandler;

/**
 * Administration settings.
 *
 * Options: log each optimization, multi-select list of tables to optimize.
 *
 * @return array
 *   An array containing form items to place on the module settings page.
 */
function db_maintenance_admin_settings() {
  return AdminHookHandler::hookAdminSettings();
}

/**
 * Validation for the administrative settings form.
 *
 * @param object $form
 *   An associative array containing the structure of the form.
 * @param object $form_state
 *   A keyed array containing the current state of the form.
 */
function db_maintenance_admin_settings_validate($form, &$form_state) {
  AdminHookHandler::hookAdminSettingsValidate($form, $form_state);
}
