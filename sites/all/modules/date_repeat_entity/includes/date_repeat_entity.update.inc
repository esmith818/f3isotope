<?php

/**
 * @file
 * Contains functions that support update of entities with repeating
 * date fields.
 */

/**
 * Update or replaces date entities based on a scope determined by user.
 *
 * @param object $entity
 *   The entity being updated.
 *
 * @param string $entity_type
 *   the entity type to load
 */
function date_repeat_entity_update_dates($entity, $entity_type) {

  // Get entity wrapper.
  $entity_wrapper = entity_metadata_wrapper($entity_type, $entity);
  $bundle = $entity_wrapper->getBundle();

  // Get value of clone state - a new entity will have the default state
  // of FALSE while a cloned entity will have a state of TRUE.
  $field_clone_state_value = $entity_wrapper->{DATE_REPEAT_ENTITY_FIELD_CLONE_STATE}->value();

  // Only act on entities that are not clones or equivalent.
  if ($field_clone_state_value === FALSE) {

    $scope = $entity->date_repeat_entity['scope'];

    // Determine if there is has been a material change between the
    // original and updated entity.
    $repeating_date_has_changed = module_invoke_all('repeating_date_has_changed', $entity->original, $entity, $entity_type);

    // If the repeating date pattern has changed then replace the entities.
    if (in_array(TRUE, $repeating_date_has_changed)) {

      // Replace dates and update entity with new master uuid and related
      // properties.
      $entity = _date_repeat_entity_replace_dates($entity, $entity_type, $bundle, $scope);
    }
    else {
      // Update dates, keeping same master uuid.
      _date_repeat_entity_update_dates($entity_wrapper, $entity, $entity_type, $bundle, $scope);
    }
  }

  // Find out which field we are operating on.
  $repeating_date_field = date_repeat_entity_get_repeating_date_field($entity_type, $bundle);
  $repeating_date_field_name = $repeating_date_field['field_name'];
  $repeating_date_field_language = field_language($entity_type, $entity, $repeating_date_field_name);

  // Remove any date field delta values that were created during the save
  // operation - any existing entities should not have these deltas.
  array_splice($entity->{$repeating_date_field_name}[$repeating_date_field_language], 1);

  // Make sure entity clone state is set to FALSE - this should be normal state.
  $entity_wrapper->{DATE_REPEAT_ENTITY_FIELD_CLONE_STATE} = FALSE;
}

/**
 * Replace dates associated with a repeating date series.
 *
 * @param object $entity
 *   The entity being updated.
 * @param string $entity_type
 *   The entity type to load, e.g. node.
 * @param string $bundle
 *   The bundle type, e.g. event.
 * @param string $scope
 *   Defines the extent to which date series should be searched.
 *   Can be one of:
 *     current (for the current date instance only),
 *     future (for the current and all future date instances),
 *     all (all instances of a date series).
 *
 * @return object
 *   Represents the entity that has been updated with a new uuid.
 */
function _date_repeat_entity_replace_dates($entity, $entity_type, $bundle, $scope = 'current') {

  // Clone the entity before the UUID gets changed.
  // Notes:
  // - use the clone method rather than replicate_clone_entity which resets
  //   the entity_id.
  // - cannot clone the original wrapper because it still points to the original
  //   entity.
  // - clone the entity's original state as stored in entity->original.  This
  //   is because we need to use the original start date when the scope is
  //   'future'.
  $original_entity = clone($entity->original);
  $original_entity_wrapper = entity_metadata_wrapper($entity_type, $original_entity);

  // Generate a new master UUID.
  $master_uuid_new = uuid_generate();

  // Change the entity uuid and set its status to new.
  $entity->uuid = $master_uuid_new;
  $entity->vuuid = $master_uuid_new;

  // Create new dates based on the current entity and RRULE defined with it.
  date_repeat_entity_create_dates($entity, $entity_type);

  // Make sure repeating date delete functions are available.
  module_load_include('inc', 'date_repeat_entity', 'includes/date_repeat_entity.delete');

  // Delete original date entities that were replaced
  // - excluding the current entity date because it will be updated by the save
  // action that called this function.
  $exclude_current_entity = TRUE;
  date_repeat_entity_delete_dates($original_entity_wrapper, $entity_type, $bundle, $scope, $exclude_current_entity);

  return $entity;
}

/**
 * Update dates associated with a repeating date series.
 *
 * @param EntityDrupalWrapper $entity_wrapper
 *   A property wrapper for an entity.
 * @param object $entity
 *   The entity being updated.
 * @param string $entity_type
 *   The entity type to load, e.g. node.
 * @param string $bundle
 *   The bundle type, e.g. event.
 * @param string $scope
 *   Defines the extent to which date series should be searched.
 *   Can be one of:
 *     current (for the current date instance only),
 *     future (for the current and all future date instances),
 *     all (all instances of a date series).
 */
function _date_repeat_entity_update_dates($entity_wrapper, $entity, $entity_type, $bundle, $scope = 'current') {

  // Get entities in date series
  // - excluding the current entity date because it will be updated by the save
  // action that called this function.
  $exclude_current_entity = TRUE;
  $result = date_repeat_entity_get_date_entities($entity_wrapper, $entity_type, $bundle, $scope, $exclude_current_entity);

  // If query successful, update retrieved entities.
  if (isset($result[$entity_type])) {

    // Build an array of entity ids for for the entity type.
    $date_entity_ids = array_keys($result[$entity_type]);

    // Retrieve entities based on entity ids.
    $date_entities = entity_load($entity_type, $date_entity_ids);

    // Update each date entity as required.
    foreach ($date_entities as $date_entity) {

      // Let other modules update the date entity.
      module_invoke_all('repeating_date_update', $date_entity, $entity, $entity_type);

      // Get a wrapper for the entity.
      $date_entity_wrapper = entity_metadata_wrapper($entity_type, $date_entity);

      // Set clone state to TRUE so the cloning procedures are skipped
      // when hook_entity_presave is triggered.
      $date_entity_wrapper->{DATE_REPEAT_ENTITY_FIELD_CLONE_STATE} = TRUE;

      // Save the entity.
      entity_save($entity_type, $date_entity);
    }
  }
}

/**
 * Implements hook_repeating_date_has_changed().
 *
 * see @date_repeat_entity.api.php
 *
 * Determines if an entity with a repeating date has changed.  Compares the
 * start date, end date and RRULE of new and old entity.
 */
function date_repeat_entity_repeating_date_has_changed($original_entity, $updated_entity, $entity_type = 'node') {

  $repeating_date_has_changed = FALSE;

  // Check that entity aleady exists - we are not validating new entities.
  if (!is_null($original_entity) && !is_null($updated_entity)) {

    // Create two wrappers.
    $original_wrapper = entity_metadata_wrapper($entity_type, $original_entity);
    $updated_wrapper = entity_metadata_wrapper($entity_type, $updated_entity);

    // Get bundle type from original entity.
    $bundle = $original_wrapper->getBundle();

    // Make sure utility functions are available.
    module_load_include('inc', 'date_repeat_entity', 'includes/date_repeat_entity.utility');
    $repeating_date_field = date_repeat_entity_get_repeating_date_field($entity_type, $bundle);

    // Check that the entity form has a repeating date field.
    if ($repeating_date_field != NULL) {

      // Get the name of the repeating field.
      $repeating_date_field_name = $repeating_date_field['field_name'];

      // Get original date field properties.
      $original_date = $original_wrapper->{$repeating_date_field_name}[0]->value();
      $original_date_start_value = $original_date['value'];
      $original_date_end_value = $original_date['value2'];
      $original_rrule = $original_date['rrule'];

      // Get updated date field properties.
      $updated_date = $updated_wrapper->{$repeating_date_field_name}[0]->value();
      $updated_date_start_value = $updated_date['value'];
      $updated_date_end_value = $updated_date['value2'];
      $updated_rrule = $updated_date['rrule'];

      // Check if the entity date has changed to the extent that
      // the repeating date series has changed and therefore dependent data
      // like date exceptions and referencing entities will need to be reset.
      $updated_date_data = array(
        $updated_date_start_value,
        $updated_date_end_value,
        $updated_rrule,
      );

      $original_date_data = array(
        $original_date_start_value,
        $original_date_end_value,
        $original_rrule,
      );

      if ($updated_date_data !== $original_date_data) {
        $repeating_date_has_changed = TRUE;
      }
    }
  }
  return $repeating_date_has_changed;
}

/**
 * Implements hook_repeating_date_update().
 *
 * see @date_repeat_entity.api.php
 */
function date_repeat_entity_repeating_date_update($date_entity, $updated_entity, $entity_type = 'node') {

  $date_entity_wrapper = entity_metadata_wrapper($entity_type, $date_entity);
  $updated_entity_wrapper = entity_metadata_wrapper($entity_type, $updated_entity);

  // Update date entity title from updated entity.
  $date_entity_wrapper->title = $updated_entity_wrapper->label();
}

/**
 * Get new date series if an entity with a repeating date has changed.
 *
 * Note: this function is not currently used but could be useful if we decide
 * to allow all dates in a series to be shifted based on the revised RRULE,
 * start and end dates of the instance that changed.
 *
 * @param array $form_state
 *   represents the current state of the form
 * @param int $entity_id
 *   entity identifier
 * @param string $entity_type
 *   The entity type to load, e.g. node.
 * @param string $bundle
 *   The bundle type, e.g. event.
 *
 * @return array
 *   NULL if the repeating date has not changed.
 */
function _date_repeat_entity_get_updated_dates($form_state, $entity_id, $entity_type, $bundle) {

  $new_dates = array();

  // Make sure utility functions are available.
  module_load_include('inc', 'date_repeat_entity', 'includes/date_repeat_entity.utility');

  // Check that the entity form has a repeating date field.
  $repeating_date_field = date_repeat_entity_get_repeating_date_field($entity_type, $bundle);

  if ($repeating_date_field != NULL) {

    // Get the name of the repeating field.
    $repeating_date_field_name = $repeating_date_field['field_name'];
    $language = $form_state['values']['language'];

    $new_date_start_value = $form_state['values'][$repeating_date_field_name][$language][0]['value'];
    $new_date_end_value = $form_state['values'][$repeating_date_field_name][$language][0]['value2'];
    $new_rrule = $form_state['values'][$repeating_date_field_name][$language][0]['rrule'];

    $old_entity = entity_load_unchanged($entity_type, $entity_id);

    $old_date_start_value = $old_entity->{$repeating_date_field_name}[$old_entity->language][0]['value'];
    $old_date_end_value = $old_entity->{$repeating_date_field_name}[$old_entity->language][0]['value2'];

    // Get timezone associated with the field instance.
    $timezone = date_get_timezone($repeating_date_field['settings']['tz_handling']);

    // Create DateObjects (extensions of PHP DateTime objects)
    // from the new start date and old start date.
    $new_date_start = new DateObject($new_date_start_value, $timezone, date_type_format($repeating_date_field['type']));
    $new_date_end = new DateObject($new_date_end_value, $timezone, date_type_format($repeating_date_field['type']));
    $old_date_start = new DateObject($old_date_start_value, $timezone, date_type_format($repeating_date_field['type']));
    $old_date_end = new DateObject($old_date_end_value, $timezone, date_type_format($repeating_date_field['type']));

    // Return the time difference (seconds) between:
    // a) the new start date and old start date and
    // b) the new start date and new end date.
    $seconds_between_new_and_old_start_date = $new_date_start->difference($old_date_start);
    $seconds_between_new_start_and_new_end_date = $new_date_end->difference($new_date_start);

    // Define PHP Interval objects.
    $interval_between_new_and_old_start_date = new DateInterval('PT' . $seconds_between_new_and_old_start_date . 'S');
    $interval_between_new_start_and_new_end_date = new DateInterval('PT' . $seconds_between_new_start_and_new_end_date . 'S');

    // Get the series master entity uuid from the current entity.
    $master_uuid_value = $old_entity->{DATE_REPEAT_ENTITY_FIELD_MASTER_UUID}[$old_entity->language][0]['value'];

    // Get the master entity using the master entity uuid.
    $master_entity = date_repeat_entity_get_master_entity($entity_type, $bundle, $master_uuid_value);

    // Get a start date value for the new date series.
    if (!empty($master_entity)) {
      // If the master entity exists, then use it as the start date value
      // for calculating new date series.
      $master_date_start_value = $master_entity->{$repeating_date_field_name}[$master_entity->language][0]['value'];
    }
    else {
      // Otherwise, use the value in the value of the
      // repeat_date_start field.
      $master_date_start_value = $new_date_start_value;
    }
    // Convert master entity start date to a DateObject.
    $master_date_start = new DateObject($master_date_start_value, $timezone, date_type_format($repeating_date_field['type']));

    // Apply the calculated interval between the new and old start
    // dates of the current entity to the master entity start date.
    if ($new_date_start > $old_date_start) {
      $new_master_date_start = $master_date_start->add($interval_between_new_and_old_start_date);
    }
    else {
      $new_master_date_start = $master_date_start->sub($interval_between_new_and_old_start_date);
    }

    // Create a new master entity end date based on the adjusted
    // master start date.
    $new_master_date_start_clone = clone $new_master_date_start;

    // Apply the calculated interval between the new start and
    // new end date of the current entity to the new master entity
    // end date. This will yield a new master end date.
    if ($new_date_end > $old_date_end) {
      $new_master_date_end = $new_master_date_start_clone->add($interval_between_new_start_and_new_end_date);
    }
    else {
      $new_master_date_end = $new_master_date_start_clone->sub($interval_between_new_start_and_new_end_date);
    }

    // Get formatted strings representing new master start date and
    // end date.
    $new_master_date_start_value = date_format($new_master_date_start, date_type_format($repeating_date_field['type']));
    $new_master_date_end_value = date_format($new_master_date_end, date_type_format($repeating_date_field['type']));

    // Create an item array to simulate the field instance structure
    // used in Date module's Form API functions.
    $item = array(
      'value' => $new_master_date_start_value,
      'value2' => $new_master_date_end_value,
      'timezone' => $timezone);

    // Get new date series based on the new master start and end dates.
    $new_dates = date_repeat_build_dates($new_rrule, NULL, $repeating_date_field, $item);
  }
  return $new_dates;
}
