<?php

/**
 * @file
 * Contains utility functions that support operations on entities with
 * repeating date fields.
 */

/**
 * Returns an entity object from a master uuid.
 *
 * @param string $entity_type
 *   the entity type to load, e.g. node.
 * @param string $bundle
 *   the bundle type, e.g. event.
 * @param string $master_uuid_value
 *   the alphanumeric master_uuid.
 *
 * @return object
 *   representing an entity.
 */
function date_repeat_entity_get_master_entity($entity_type, $bundle, $master_uuid_value = NULL) {

  // Instantiate array to be returned.
  $master_entity = array();

  // Use EFQ to retrieve master entity.
  $query = new EntityFieldQuery();

  // Define EFQ conditions based on the entity type, bundle and master_uuid.
  $query->entityCondition('entity_type', $entity_type)
      ->entityCondition('bundle', $bundle)
      ->propertyCondition('uuid', $master_uuid_value);

  // Retrieve matching entities.
  $result = $query->execute();

  // If query successful, update retrieved entities.
  if (isset($result[$entity_type])) {

    // Get array of entity ids.
    $entity_ids = array_keys($result[$entity_type]);

    // Retrieve entities based on entity ids.
    $master_entities = entity_load($entity_type, $entity_ids);

    // Get first element in array (there should only be one).
    $master_entity = reset($master_entities);
  }

  return $master_entity;
}

/**
 * Returns the name of a repeating date field, where it exists.
 *
 * Searches database field definition for a repeating date field which is used
 * in the $entity_type (e.g. node) and $bundle (e.g. event) passed as
 * arguments.
 *
 * Note: Function returns once the first match of a repeating date field
 * is found and does not consider the scenario where there may be more than
 * one repeating date field associated with the $entity_type and $bundle.
 *
 * @param string $entity_type
 *   an entity type e.g. node.
 * @param string $bundle
 *   a bundle type e.g. event.
 *
 * @return array
 *   representing a field.
 */
function date_repeat_entity_get_repeating_date_field($entity_type, $bundle) {

  // Set a default value of NULL.
  $repeating_date_field = NULL;

  // Get list of bundles configured at module's admin page.
  // (see /admin/config/date/date_repeat_entity).
  $bundles_available = variable_get('date_repeat_entity_bundles_available', array());

  // Filter any unchecked bundles from list of available.
  $bundles_enabled = array_filter($bundles_available);

  // Check if this bundle supports the date_repeat_entity module.
  if (array_key_exists($bundle, $bundles_enabled) || empty($bundles_enabled)) {

    // Retrieve meta information about all of the fields in the database.
    $fields = field_info_fields();

    // Loop through the fields.
    foreach ($fields as $field) {

      // Check if the field is a repeating date field.
      if (in_array($field['type'], array('date', 'datestamp', 'datetime')) && date_is_repeat_field($field)) {

        // Loop through the entity types associated with this field.
        foreach ($field['bundles'] as $field_entity_type => $bundles) {

          // Loop through the bundles (content types - in the case of node
          // entity types) associated with this entity type.
          foreach ($bundles as $field_bundle) {

            // If the entity type and bundle type passed to this function are
            // the same as the entity type and bundle of the repeating date
            // field then we have a match.
            if ($entity_type == $field_entity_type && $bundle == $field_bundle) {
              $repeating_date_field = $field;
              return $repeating_date_field;
            }
          }
        }
      }
    }
  }
  return $repeating_date_field;
}

/**
 * Returns a field for a given entity type, bundle and field machine name.
 *
 * Searches database field definition for a field which is used
 * in the $entity_type (e.g. node) and $bundle (e.g. event)
 *
 * @param string $entity_type
 *   an entity type e.g. node.
 * @param string $bundle
 *   a bundle type e.g. event.
 * @param string $name
 *   a field machine name.
 *
 * @return array
 *   representing a field.
 */
function date_repeat_entity_get_field($entity_type, $bundle, $name) {

  // Set a default value of NULL.
  $field_match = NULL;

  // Retrieve meta information about all of the fields in the database.
  $fields = field_info_fields();

  // Loop through the fields.
  foreach ($fields as $field_name => $field) {

    // Loop through the entity types associated with this field.
    foreach ($field['bundles'] as $field_entity_type => $bundles) {

      // Loop through the bundles (content types - in the case of node
      // entity types) associated with this entity type.
      foreach ($bundles as $field_bundle) {

        // If the entity type, bundle and field name passed to this function
        // are the same as the entity type and bundle of the field then
        // we have a match.
        if ($entity_type == $field_entity_type &&
            $bundle == $field_bundle &&
            $name == $field_name) {
          $field_match = $field;
        }
      }
    }
  }
  return $field_match;
}

/**
 * Returns an array of entity reference field names associate with a bundle.
 *
 * Searches database field definition for all entity reference fields used
 * in the $entity_type (e.g. node) and $bundle (e.g. event) passed as
 * arguments.
 *
 * @param string $entity_type
 *   an entity type e.g. node.
 * @param string $bundle
 *   a bundle type e.g. event.
 *
 * @return array
 *   an array of entity reference fields.
 */
function date_repeat_entity_get_entity_reference_fields($entity_type, $bundle) {

  // Set a default value of NULL.
  $entity_reference_fields = array();

  // Retrieve meta information about all of the fields in the database.
  $fields = field_info_fields();

  // Loop through the fields.
  foreach ($fields as $field) {

    // Check if the field is an entity reference date field.
    if (in_array($field['type'], array('entityreference'))) {

      // Loop through the entity types associated with this field.
      foreach ($field['bundles'] as $field_entity_type => $bundles) {

        // Loop through the bundles (content types - in the case of node
        // entity types) associated with this entity type.
        foreach ($bundles as $field_bundle) {

          // If the entity type and bundle type passed to this function are the
          // same as the entity type and bundle of the repeating date field then
          // we have a match.
          if ($entity_type == $field_entity_type && $bundle == $field_bundle) {
            $entity_reference_fields[] = $field;
          }
        }
      }
    }
  }
  return $entity_reference_fields;
}

/**
 * Gets an array of entity ids with the same master uuid.
 *
 * @param string $entity_type
 *   the entity type to load, e.g. node.
 * @param string $bundle
 *   the bundle type, e.g. event.
 * @param string $master_uuid_value
 *   the alphanumeric master_uuid.
 *
 * @return array
 *   of entity ids
 */
function date_repeat_entity_get_related_entity_ids($entity_type, $bundle, $master_uuid_value) {

  $date_entity_ids = array();

  // Use EFQ to get all dates with the same master uuid value.
  $query = new EntityFieldQuery();

  // Define EFQ conditions based on the entity type, bundle and master uuid.
  $query->entityCondition('entity_type', $entity_type)
      ->entityCondition('bundle', $bundle)
      ->fieldCondition(DATE_REPEAT_ENTITY_FIELD_MASTER_UUID, 'value', $master_uuid_value);

  // Retrieve matching entities.
  $result = $query->execute();

  // If query successful, update retrieved entities.
  if (isset($result[$entity_type])) {

    // Build an array of entity ids for for the entity type.
    $date_entity_ids = array_keys($result[$entity_type]);
  }

  return $date_entity_ids;
}

/**
 * Constructs a query that searches for entities matching certain conditions.
 *
 * @param EntityDrupalWrapper $entity_wrapper
 *   A property wrapper for an entity.
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
 * @param bool $exclude_current_entity
 *   Specifies whether a query that searches a date series should exclude
 *   the entity that represents the current date instance.
 *
 * @return array
 *   Represents the result of a query.
 */
function date_repeat_entity_get_date_entities($entity_wrapper, $entity_type, $bundle, $scope = 'current', $exclude_current_entity = FALSE) {

  // Get the series master UUID.
  $master_uuid_value = $entity_wrapper->{DATE_REPEAT_ENTITY_FIELD_MASTER_UUID}->value();

  // Get the entity_id from the entity wrapper.
  $entity_id = $entity_wrapper->getIdentifier();

  // Use EFQ to get all dates with the same master uuid value.
  $query = new EntityFieldQuery();

  // Define EFQ conditions based on the entity type, bundle and master_uuid.
  $query->entityCondition('entity_type', $entity_type)
      ->entityCondition('bundle', $bundle)
      // Note: exclude current entity because it will be deleted as part of the
      // hook_entity_delete function that calls this one.
      ->fieldCondition(DATE_REPEAT_ENTITY_FIELD_MASTER_UUID, 'value', $master_uuid_value);

  // If necessary, exclude the subject entity from the query.
  if ($exclude_current_entity == TRUE) {
    $query->propertyCondition('nid', $entity_id, '<>');
  }

  switch ($scope) {
    case 'all':
      // Don't add any more conditions.
      break;

    case 'future':
      // If scope is 'future' and a start date is passed to function,
      // add field condition to exclude earlier entities from the query;
      $repeating_date_field = date_repeat_entity_get_repeating_date_field($entity_type, $bundle);
      $repeating_date_field_name = $repeating_date_field['field_name'];

      // Retrieve start date for current entity since this is being applied
      // to all dates by the user.
      $field_data = $entity_wrapper->{$repeating_date_field_name}[0]->raw();
      $start_date = $field_data['value'];

      if (!is_null($start_date)) {
        $query->fieldCondition($repeating_date_field_name, 'value', $start_date, '>=');
      }
      break;

    default:
      // Default is 'current'.
      $query->propertyCondition('nid', $entity_id, '=');
      break;
  }

  // Retrieve matching entities.
  $result = $query->execute();

  return $result;
}
