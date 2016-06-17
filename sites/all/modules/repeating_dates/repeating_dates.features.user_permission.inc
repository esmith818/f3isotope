<?php
/**
 * @file
 * repeating_dates.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function repeating_dates_user_default_permissions() {
  $permissions = array();

  // Exported permission: 'view date repeats'.
  $permissions['view date repeats'] = array(
    'name' => 'view date repeats',
    'roles' => array(
      'administrator' => 'administrator',
    ),
    'module' => 'date_repeat_field',
  );

  return $permissions;
}
