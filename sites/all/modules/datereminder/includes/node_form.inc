<?php
/**
 * @file
 * Functions that support node type edit forms.
 */

/**
 * Implements hool_alter_node_form().
 *
 * Add reminder information when a node with reminders is displayed.
 *
 * @param array &$form
 *   The form to be altered.
 * @param array $form_state
 *   Current form state.
 * @param string $form_id
 *   Form id, in case there are more than one.
 *
 * No return value. The form is updated.
 */
function datereminder_alter_node_form(&$form, &$form_state, $form_id) {
  module_load_include('inc', 'datereminder', 'includes/defines');
  // Get type of node that we're editing.
  $type = $form['type']['#value'];
  $enabled = _datereminder_type_enabled($type);
  switch ($enabled) {
    case DATEREMINDER_TYPE_ON:
      $dflt = DATEREMINDER_TYPE_ON;
      break;

    case DATEREMINDER_TYPE_ALLOWED:
      $dflt = DATEREMINDER_TYPE_RETAIN;
      break;

    default:
      return;
  }

  // Node is not saved but previewed (nid is empty).
  if (isset($form['#node']->build_mode)
          && $form['#node']->build_mode == NODE_BUILD_PREVIEW) {
    $node = $form['#node'];
  }
  // Load the node if it already exists.
  elseif (!empty($form['nid']['#value'])) {
    $node = node_load($form['nid']['#value']);
  }
  else {
    $node = NULL;
  }
  if ($node != NULL && isset($node->datereminder_enabled)) {
    $dflt = $node->datereminder_enabled;
  }

  $form['reminder'] = array(
    '#type' => 'fieldset',
    '#title' => t('Reminder settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
    '#weight' => 40,
  );

  $radios[DATEREMINDER_TYPE_DISABLED] = t('Disable and discard existing reminders');
  $radios[DATEREMINDER_TYPE_RETAIN] = t('Disable, but keep existing reminders');
  $radios[DATEREMINDER_TYPE_ON] = t('Enable');

  $form['reminder']['datereminder_enabled'] = array(
    '#type' => 'radios',
    '#options' => $radios,
    '#default_value' => $dflt,
    '#description' => t('If enabled, users can sign up to get reminders before this event'),
  );
  $form['#submit'][] = '_datereminder_form_submit_node';
  $form['#validate'][] = '_datereminder_form_validate_node';
  form_load_include($form_state, 'inc', 'datereminder', 'includes/node_form');
}

/**
 * Implements hook_alter_node_type_form().
 * @todo
 *   To be consistent with D7 format, how do we get this to be a separate
 *   tab on the side? See hook_form_node_form_alter().
 */
function datereminder_alter_node_type_form(&$form, &$form_state, $nodetype) {
  module_load_include('inc', 'datereminder', 'includes/defines');
  module_load_include('inc', 'datereminder', 'includes/date');

  $fields = field_info_fields();
  $datetypes = _datereminder_supported_date_field_types();

  $datefields = array();
  foreach ($fields as $name => $field) {
    if (!isset($field['module'])
            || ($field['module'] != 'date')
            || !isset($field['bundles'])
            || !is_array($field['bundles'])
            || !isset($field['bundles']['node'])
            || !is_array($field['bundles']['node'])) {
      continue;
    }
    if (array_key_exists($field['type'], $datetypes) &&
        in_array($nodetype, $field['bundles']['node'])) {
      // This node uses this field type.
      $datefields[$name] = $name;
      $lastkey = $name;
    }
  }

  if (count($datefields) == 0) {
    $form['datereminder_enabled'] = array(
      '#type' => 'hidden',
      '#value' => DATEREMINDER_TYPE_DISABLED,
    );
  }
  else {
    // What is current setting?
    $enabled = _datereminder_type_enabled($nodetype);
    $currentfield = variable_get("datereminder_datefield_$nodetype", $lastkey);
    
    /*
     * One would certainly not expect the following to be necessary.
     * It means that the variable is set to something that isn't an
     * existing date key. One user reported that this happened. I suspect
     * some kind of operator error, but this should clean it up
     * if that happens.
     */
    if (!in_array($currentfield, $datefields)) {
      $currentfield = $lastkey;
    }
    
    $fs = array(
      '#type' => 'fieldset',
      '#title' => t('Reminder settings'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#weight' => 20,
      '#submit' => '_datereminder_node_form_submit',
      '#group' => 'additional_settings',
    );
    $radios[DATEREMINDER_TYPE_DISABLED] = t('Disabled');
    $radios[DATEREMINDER_TYPE_RETAIN]
      = t('Disabled, but retain any existing reminders');
    $radios[DATEREMINDER_TYPE_ALLOWED] = t('Allowed, off unless explicitly enabled');
    $radios[DATEREMINDER_TYPE_ON] = t('Allowed, automatically enabled for new nodes');
    $fs['datereminder_enabled'] = array(
      '#type' => 'radios',
      '#options' => $radios,
      '#default_value' => $enabled,
    );
    $fs['datereminder_all_on'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable reminders for existing nodes'),
      '#description' => t('Check if you want to enable reminders for existing nodes.'),
      '#default_value' => FALSE,
    );

    if (count($datefields) > 1) {
      $fs['datereminder_datefield'] = array(
        '#type' => 'select',
        '#options' => $datefields,
        '#default_value' => $currentfield,
        '#description' => t('Select which of several date fields to use for reminders'),
      );
      $lbl = t('Enable allowing users to request reminders for an event');
    }
    else {
      $fs['datereminder_datefield'] = array(
        '#type' => 'hidden',
        '#value' => $currentfield,
      );
      $lbl
        = t('Enable allowing users to request reminders for an event, based on field %date',
              array('%date' => $datefields[$lastkey]));
    }
    $fs['datereminder_enabled']['#title'] = $lbl;

    $form['reminder'] = $fs;
    $form['#validate'][] = '_datereminder_form_validate_node_type';
    $form['#submit'][] = '_datereminder_form_submit_node_type';
    form_load_include($form_state, 'inc', 'datereminder', 'includes/node_form');
  }
}

/**
 * Implements hook_node_type_update().
 *
 * If reminders are completely removed for this type, delete any reminders.
 */
function datereminder_node_type_update($info) {
  module_load_include('inc', 'datereminder', 'includes/defines');
  $type = $info->type;
  $en = _datereminder_type_enabled($info->type);
  if ($en == DATEREMINDER_TYPE_DISABLED) {
    module_load_include('inc', 'datereminder', DATEREMINDER_DB);
    // If reminders are disabled for this node type, clean out old reminders.
    _datereminder_clean_type_reminders($type);
  }
}

/**
 * Implements hook_node_type_delete().
 *
 * Delete any reminders for this type.
 *
 * Note: In theory it would make sense to clean out any reminders for nodes
 * at this type, but Drupal allows nodes to be kept around even if the type
 * is deleted. Though that's not advised.
 *
 * Any leftover reminders will be removed when the individual node is removed,
 * just like any other node-specific content.
 */
function datereminder_node_type_delete($info) {
  $type = $info->type;
  variable_delete("datereminder_enabled_$type");
}

/**
 * Validate node type form submission.
 * (1) Don't allow "all_on" to be set if reminders are disabled.
 * (2) Move the "all_on" value out of $form_state['values'] so that
 *     core's node form handler doesn't create a variable. We'll handle
 *     it in our own submit function.
 * @param type $form
 * @param type $form_state
 */
function _datereminder_form_validate_node_type(&$form, &$form_state) {
  $allon = $form_state['values']['datereminder_all_on'];
  if (isset($allon)) {
    $enabled = $form_state['values']['datereminder_enabled'];
    if ($allon && ($enabled <= DATEREMINDER_TYPE_RETAIN)) {
      form_set_error('datereminder_all_on',
              t('Don\'t ask to enable for all if reminders aren\'t enabled'));
      return;
    }
    $form_state['customvalues']['datereminder_all_on'] = $allon;
    unset($form_state['values']['datereminder_all_on']);
  }
}

/**
 * Processing on a node type submit. This does the enable of reminders
 * in existing nodes, if requested.
 *
 * @param type $form
 * @param type $form_state
 */
function _datereminder_form_submit_node_type(&$form, &$form_state) {
  $allon = $form_state['customvalues']['datereminder_all_on'];
  if ($allon) {
    $type = $form_state['values']['type'];
    module_load_include('inc', 'datereminder', DATEREMINDER_DB);
    _datereminder_enable_all_nodes($type);
  }
}
