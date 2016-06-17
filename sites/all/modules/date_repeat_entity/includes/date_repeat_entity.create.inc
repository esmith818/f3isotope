<?php

/**
 * @file
 * Contains functions that support creation of entities with repeating
 * date fields.
 */

/**
 * Clones new entities that are part of a repeating date series.
 *
 * @param object $entity
 *   The entity being created
 *
 * @param string $entity_type
 *   the entity type to load
 */
function date_repeat_entity_create_dates($entity, $entity_type) {

  // Make sure utility functions are available.
  module_load_include('inc', 'date_repeat_entity', 'includes/date_repeat_entity.utility');

  // Get entity wrapper.
  $wrapper = entity_metadata_wrapper($entity_type, $entity);

  // Get bundle type.
  $bundle = $wrapper->getBundle();

  // Get value of clone state - a new date entity will have the default state
  // of FALSE while a cloned entity will have a state of TRUE.
  $field_clone_state_value = $wrapper->{DATE_REPEAT_ENTITY_FIELD_CLONE_STATE}->value();

  // Clone the new entity and its attached fields using Replicate API
  // module.
  if ($field_clone_state_value === FALSE) {

    // If the current entity contains a repeating date field then
    // we need to process the entity.
    $repeating_date_field = date_repeat_entity_get_repeating_date_field($entity_type, $bundle);

    if ($repeating_date_field != NULL) {
      // Create a clone of the entity for each date instance in the series
      // except the first, which will be saved though the submit handler
      // on the entity edit form itself.
      //
      // Get the UUID from the master (original) date.
      $master_uuid_value = $wrapper->uuid->value();

      // Assign the UUID to the master UUID field.
      // Give the original entity a reference to its own UUID.
      // This will be needed for any updates of the recurring date series.
      $wrapper->{DATE_REPEAT_ENTITY_FIELD_MASTER_UUID} = $master_uuid_value;

      // Get an array of field data for the current entity.
      $field_name = $repeating_date_field['field_name'];
      $field_language = field_language($entity_type, $entity, $field_name);
      $field_data = field_get_items($entity_type, $entity, $field_name);

      // Create a new set of dates by removing the first item in
      // the array - since it is the original (master) entity.
      array_shift($field_data);

      // Clone a new entity for each date in series
      // (except original entity).
      foreach ($field_data as $delta => $datum) {

        // Clone entity.
        $clone_entity = replicate_clone_entity($entity_type, $entity);

        // Get entity wrapper for the cloned entity.
        $clone_wrapper = entity_metadata_wrapper($entity_type, $clone_entity);

        // Flag the entity as cloned.
        $clone_wrapper->{DATE_REPEAT_ENTITY_FIELD_CLONE_STATE} = TRUE;

        // Give the cloned entity a reference to the master UUID.
        $clone_wrapper->{DATE_REPEAT_ENTITY_FIELD_MASTER_UUID} = $master_uuid_value;

        // Reset the UUID, version UUID and version log string
        // of the cloned entity to differentiate from the
        // original entity.
        $clone_entity->uuid = NULL;
        $clone_entity->vuuid = NULL;
        $clone_entity->log = "cloned from uuid : " . $master_uuid_value;

        // Replace field array with the row that matches the
        // current delta.  Need to add an offset of 1 to delta
        // because delta values are shifted when we applied
        // array_shift to field_data.
        $clone_entity->{$field_name}[$field_language] = array_slice($clone_entity->{$field_name}[$field_language], $delta + 1, 1);

        // Save the cloned entity.
        entity_save($entity_type, $clone_entity);
      }
      // Remove all items from the master entity's field array
      // except the first.
      array_splice($entity->{$field_name}[$field_language], 1);
    }
  }

  // Reset the flag for clone state, just before saving the entity.
  $wrapper->{DATE_REPEAT_ENTITY_FIELD_CLONE_STATE} = FALSE;
}
