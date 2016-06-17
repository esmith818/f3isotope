<?php

/**
 * @file
 * Allows administrators to configure bundles to work with this module.
 */

/**
 * Date repeat entity administration.
 */
function date_repeat_entity_admin_settings() {
  $form = array();

  $form['#prefix'] = '<h2>' . t('Date Repeat Entity Administration') . '</h2>';

  // Get node entity bundles.
  $bundles = node_type_get_names();

  // If variable has never been initialized set all values to zero.
  if (is_null(variable_get('date_repeat_entity_bundles_available'))) {
    $defaults = array_fill(0, count($bundles), 0);
  }
  else {
    $defaults = array_keys($bundles);
  }

  // Add checkboxes for each bundle in use.
  $form['date_repeat_entity_bundles_available'] = array(
    '#title' => t('Bundles available to date repeat entity'),
    '#type' => 'checkboxes',
    '#options' => $bundles,
    '#default_value' => variable_get('date_repeat_entity_bundles_available', $defaults),
    '#description' => t('Select the bundles (content types) that date repeat entity should work with.'),
  );

  // Append custom submit handler.
  $form['#submit'][] = 'date_repeat_entity_admin_settings_submit';

  return system_settings_form($form);
}

/**
 * Submit handler for configuration form.
 *
 * @param array $form
 *   represents the form and its underlying data schema
 *
 * @param array $form_state
 *   represents the current state of the form
 */
function date_repeat_entity_admin_settings_submit($form, &$form_state) {

  // Loop over bundles.
  foreach ($form_state['values']['date_repeat_entity_bundles_available'] as $bundle => $state) {

    $master_uuid_instance = field_info_instance('node', DATE_REPEAT_ENTITY_FIELD_MASTER_UUID, $bundle);
    $clone_state_instance = field_info_instance('node', DATE_REPEAT_ENTITY_FIELD_CLONE_STATE, $bundle);

    // Work on the unchecked bundles.
    if ($state === 0) {

      // Remove instances of uuid and clone state fields from bundle.
      if ($master_uuid_instance) {
        field_delete_instance($master_uuid_instance);
      }
      if ($clone_state_instance) {
        field_delete_instance($clone_state_instance);
      }
    }
    else {

      // Make sure base fields exist.
      _date_repeat_entity_create_base_fields();

      // Add instances of uuid and clone state fields to bundle.
      if (is_null($master_uuid_instance)) {
        $master_uuid_instance_definition = array(
          'field_name' => DATE_REPEAT_ENTITY_FIELD_MASTER_UUID,
          'label' => 'Master UUID (do not change)',
          'entity_type' => 'node',
          'bundle' => $bundle,
          'default_value' => NULL,
          'display' => array(
            'default' => array(
              'label' => 'hidden',
              'settings' => array(),
              'type' => 'hidden',
            ),
          ),
        );
        field_create_instance($master_uuid_instance_definition);
      }

      if (is_null($clone_state_instance)) {
        $clone_state_instance_definition = array(
          'field_name' => DATE_REPEAT_ENTITY_FIELD_CLONE_STATE,
          'label' => 'Clone state (do not change)',
          'entity_type' => 'node',
          'bundle' => $bundle,
          'default_value' => array(
            0 => array(
              'value' => 0,
            ),
          ),
          'widget' => array(
            'active' => 1,
            'module' => 'options',
            'settings' => array(
              'display_label' => 0,
            ),
            'type' => 'options_onoff',
          ),
          'settings' => array(
            'allowed_values' => array(
              0 => '',
              1 => '',
            ),
            'allowed_values_function' => '',
          ),
          'display' => array(
            'default' => array(
              'label' => 'hidden',
              'settings' => array(),
              'type' => 'hidden',
            ),
          ),
        );
        field_create_instance($clone_state_instance_definition);
      }
    }
  }
}
