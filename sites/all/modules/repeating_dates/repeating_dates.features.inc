<?php
/**
 * @file
 * repeating_dates.features.inc
 */

/**
 * Implements hook_views_api().
 */
function repeating_dates_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function repeating_dates_node_info() {
  $items = array(
    'f1_event' => array(
      'name' => t('1st F Event'),
      'base' => 'node_content',
      'description' => t('template for workouts'),
      'has_title' => '1',
      'title_label' => t('Workout Name'),
      'help' => '',
    ),
    'f2_event' => array(
      'name' => t('2nd F Event'),
      'base' => 'node_content',
      'description' => t('template for social events'),
      'has_title' => '1',
      'title_label' => t('Event Name'),
      'help' => '',
    ),
    'f3_event' => array(
      'name' => t('3rd F Event'),
      'base' => 'node_content',
      'description' => t('template for faith meetups'),
      'has_title' => '1',
      'title_label' => t('Meetup Name'),
      'help' => '',
    ),
  );
  drupal_alter('node_info', $items);
  return $items;
}
