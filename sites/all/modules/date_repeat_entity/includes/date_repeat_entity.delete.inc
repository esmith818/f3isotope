<?php

/**
 * @file
 * Contains functions that support deletion of entities with
 * repeating date fields.
 */

/**
 * Delete dates associated with a repeating date series.
 *
 * @param EntityDrupalWrapper $entity_wrapper
 *   A property wrapper for an entity.
 * @param string $entity_type
 *   An entity type e.g. node.
 * @param string $bundle
 *   A bundle type e.g. event.
 * @param string $scope
 *   Defines the extent to which date series should be searched.
 *   Can be one of:
 *     current (for the current date instance only),
 *     future (for the current and all future date instances),
 *     all (all instances of a date series).
 * @param bool $exclude_current_entity
 *   Specifies whether a query that searches a date series should exclude
 *   the entity that represents the current date instance.
 *
 * @return bool
 *   the success of this function is represented by this value.
 */
function date_repeat_entity_delete_dates($entity_wrapper, $entity_type, $bundle, $scope = 'current', $exclude_current_entity = FALSE) {

  // Instantiate boolean variable to track success of operation.
  $repeating_date_operation_successful = FALSE;

  // Make sure utility functions are available.
  module_load_include('inc', 'date_repeat_entity', 'includes/date_repeat_entity.utility');

  // Get dates.
  $result = date_repeat_entity_get_date_entities($entity_wrapper, $entity_type, $bundle, $scope, $exclude_current_entity);

  // If query successful, update retrieved entities.
  if (isset($result[$entity_type])) {

    // Build an array of entity ids for for the entity type.
    $date_entity_ids = array_keys($result[$entity_type]);

    // Delete an array of entities. Returns failure as FALSE, success as NULL.
    // Note: referenced entities are also deleted if the option is checked on
    // the entityreference field configuration form.
    $success = entity_delete_multiple($entity_type, $date_entity_ids);
    if ($success !== FALSE) {
      $repeating_date_operation_successful = TRUE;
    }
  }

  // Need to update RRULE on remaining entities to reflect the fact that some
  // future dates were truncated and so the effective end date of the series
  // should be brought forward.
  // $rrule = date_api_ical_build_rrule($element['#value']);
  return $repeating_date_operation_successful;
}
