<?php
/**
 * @file
 * Forms for administrative settings.
 */

/**
 * Implements hook_menu().
 *
 * Redirected from datereminder.module.
 */
function _datereminder_menu() {
  module_load_include('inc', 'datereminder', 'includes/defines');

  $astab = variable_get('datereminder_display_loc', DATEREMINDER_IN_NODE)
          == DATEREMINDER_AS_TAB;
  $cfgbase = 'admin/config/people/datereminder';

  $items = array();

  $items[$cfgbase] = array(
    'title' => 'Date reminders',
    'description' => 'Configure reminders for calendar events',
    'page callback' => 'drupal_get_form',
    'access arguments' => array(DATEREMINDER_ADMINISTER_REMINDERS),
    'page arguments' => array('datereminder_settings_form'),
    'file' => 'datereminder.admin.inc',
    'weight' => 0,
  );
  $items["$cfgbase/settings"] = array(
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'title' => 'Configure Settings',
  );
  $items["$cfgbase/mail"] = array(
    'type' => MENU_LOCAL_TASK,
    'title' => 'Configure email',
    'description' => 'Set up reminder email content',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('datereminder_email_form'),
    'access arguments' => array(DATEREMINDER_ADMINISTER_REMINDERS),
    'file' => 'datereminder.admin.inc',
    'weight' => 5,
  );

  $items["$cfgbase/view"] = array(
    'type' => MENU_LOCAL_TASK,
    'title' => 'View all',
    'description' => 'View all reminders',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('datereminder_form_summary', NULL, 'all', 'm'),
    'access arguments' => array(DATEREMINDER_ADMINISTER_REMINDERS),
    'weight' => 10,
  );

  $items['user/%user/datereminder'] = array(
    'type' => MENU_LOCAL_TASK,
    'title' => 'Reminders',
    'description' => 'Outstanding reminders for this user',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('datereminder_form_summary', 1, 'all_user', 'm'),
    'access callback' => 'datereminder_allowed_access_user',
    'access arguments' => array(1),
  );

  /* Different menus installed depending on where reminders are displayed */
  if ($astab) {
    $items['node/%node/datereminder'] = array(
      'type' => MENU_LOCAL_TASK,
      'title' => 'Reminders',
      'description' => 'Your reminders for this event',
      'access callback' => 'datereminder_allowed_access_node',
      'access arguments' => array(1, 'own', DATEREMINDER_AS_TAB),
      'page callback' => 'drupal_get_form',
      'page arguments' => array('datereminder_form', 1, 'm'),
    );
    $items['node/%node/datereminder/mine'] = array(
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'title' => 'Your Reminders',
      'weight' => -10,
    );
    $items['node/%node/datereminder/all'] = array(
      'type' => MENU_LOCAL_TASK,
      'title' => 'All For This Event',
      'description' => 'All reminders for this event',
      'access callback' => 'datereminder_allowed_access_node',
      'access arguments' => array(1, 'all', DATEREMINDER_AS_TAB),
      'page callback' => 'drupal_get_form',
      'page arguments' => array('datereminder_form_summary', 1, 'all_node', 'm'),
    );
  }
  else {
    $items['node/%node/datereminder'] = array(
      'type' => MENU_LOCAL_TASK,
      'title' => 'All Reminders',
      'description' => 'All reminders for this event',
      'access callback' => 'datereminder_allowed_access_node',
      'access arguments' => array(1, 'all', DATEREMINDER_IN_NODE),
      'page callback' => 'drupal_get_form',
      'page arguments' => array('datereminder_form_summary', 1, 'all_node', 'm'),
    );
  }

  return $items;
}


/**
 * Build form to set datereminder adminstrative settings.
 *
 * @return array
 *   The form for settings.
 */
function datereminder_settings_form() {
  module_load_include('inc', 'datereminder', 'includes/defines');
  $form = array();

  $form['datereminder_cron_frequency'] = array(
    '#type' => 'textfield',
    '#title' => t('Cron frequency in minutes'),
    '#size' => 5,
    '#default_value' => variable_get('datereminder_cron_frequency',
        DATEREMINDER_CRON_FREQUENCY),
    '#description' => t('How often does cron run, in minutes? This affects when reminders are sent. Reminders will be sent for anything due before the next expected cron run.'),
  );
  $form['datereminder_max_reminders'] = array(
    '#type' => 'textfield',
    '#title' => t('Max reminders'),
    '#size' => 3,
    '#default_value' => variable_get('datereminder_max_reminders',
        DATEREMINDER_MAX_REMINDERS),
    '#description' => t('Maximum number of reminders a user can request for one node'),
  );
  $form['datereminder_min_add'] = array(
    '#type' => 'textfield',
    '#title' => t('Add this many at once'),
    '#size' => 2,
    '#default_value' => variable_get('datereminder_min_add', 1),
    '#description' => t('Allow user to add this many reminders at once'),
  );
  $form['datereminder_retain_expired'] = array(
    '#type' => 'textfield',
    '#title' => t('Days to retain reminder'),
    '#description' => t('How long after last occurence to keep record of reminders'),
    '#size' => 3,
    '#default_value' => variable_get('datereminder_retain_expired', 14),
  );
  $form['datereminder_display_loc'] = array(
    '#type' => 'radios',
    '#description' => t('Where should reminders appear when viewing a node?'),
    '#default_value' => variable_get('datereminder_display_loc',
            DATEREMINDER_IN_NODE),
    '#title' => t('Reminder display'),
    '#options' => array(
      DATEREMINDER_IN_NODE => t('Fieldset within the node'),
      DATEREMINDER_AS_TAB => t('As a tab'),
      DATEREMINDER_AS_BLOCK =>
        t('Fieldset in a separate %block block.',
                array('%block' => t('Reminder request'))),
    ),
  );
  $form['datereminder_anonymous_link'] = array(
      '#type' => 'checkbox',
      '#description' =>
          t('If displayed in node or block, give anonymous user a prompt to log in'),
      '#default_value' => variable_get('datereminder_anonymous_link', false),
      '#title' => t('Prompt anonymous user to log in.'),
  );
  module_load_include('inc', 'datereminder', 'includes/datereminder_form');
  $leads = _datereminder_leadtime_list();
  $boxes = array();
  foreach ($leads as $l => $lbl) {
    $boxes["_l_$l"] = array(
      '#type' => 'checkbox',
      '#title' => check_plain($lbl),
      '#default_value' => TRUE,
    );
  }
  $form['datereminder_leadtime_list'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Specify allowed lead times'),
    '#description' => t('Check to retain'),
  );
  $form['datereminder_leadtime_list'] += $boxes;

  $nval = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#title' => t('Add new lead time'),
    '#description' => t('Enter new lead time to add to the list'),
  );
  $nval['_added'] = array(
    '#type' => 'textfield',
    '#size' => 5,
  );
  $nval['_units'] = array(
    '#type' => 'select',
    '#description' => t('Select units'),
    '#options' => array(
      1 => t('Seconds'),
      60 => t('Minutes'),
      3600 => t('Hours'),
      86400 => t('Days'),
      604800 => t('Weeks'),
    ),
    '#default_value' => 3600,
  );
  $form['datereminder_leadtime_list'][] = $nval;

  $form = system_settings_form($form);

  $form['#submit'][] = 'datereminder_settings_form_submit';
  return $form;
}

/**
 * Check if we need to rebuild menus after changing datereminder configuration.
 * 
 * @param array $form
 *   The form itself
 * @param array $form_state
 *   Current form state
 */
function datereminder_settings_form_submit($form, &$form_state) {
  // If value of datereminder_display_loc changed, remember that it's
  // necessary to rebuild menus.
  if ($form_state['values']['datereminder_display_loc'] !=
          $form['datereminder_display_loc']['#default_value']) {
    variable_set('menu_rebuild_needed', TRUE);
  }
}

/**
 * Validate the administrative settings.
 *
 * @param array $form
 *   Form just submitted
 * @param array $form_state
 *   And state thereof
 */
function datereminder_settings_form_validate($form, &$form_state) {
  module_load_include('inc', 'datereminder', 'includes/defines');
  $vals = $form_state['values'];
  $cron_rate = $vals['datereminder_cron_frequency'];
  if (!is_numeric($cron_rate) || $cron_rate < 5) {
    form_set_error('datereminder_cron_frequency',
            t('Cron frequency needs to be a number and at least 5'));
  }

  $maxrem = $vals['datereminder_max_reminders'];
  if (!is_numeric($maxrem) || $maxrem < 1) {
    form_set_error('datereminder_max_reminders',
            t('Must be a positive number'));
  }

  $minadd = $vals['datereminder_min_add'];
  if (!is_numeric($minadd) || $minadd < 1) {
    form_set_error('datereminder_min_add',
            t('Must be a positive number'));
  }

  $retdays = $vals['datereminder_retain_expired'];
  if (!is_numeric($retdays) || (intval($retdays) < 0)) {
    form_set_error('datereminder_retain_expired',
            t('Retention days must be non-negative numeric'));
  }

  $leads = array();
  $nv = $vals['_added'];
  if (isset($nv) && $nv != '' && $nv > 0) {
    if (!is_numeric($nv)) {
      form_set_error('_added',
              t('New lead time must be numeric'));
      unset($nv);
    }
    else {
      $nv *= $vals['_units'];
    }
  }
  else {
    unset($nv);
  }

  foreach ($vals as $l => $v) {
    if (drupal_substr($l, 0, 3) == '_l_') {
      // This is one of our lead times.
      unset($form_state['values'][$l]);
      if ($v) {
        $leads[] = drupal_substr($l, 3);
      }
    }
  }
  if (isset($nv)) {
    $leads[] = $nv;
  }
  rsort($leads, SORT_NUMERIC);
  $form_state['values']['datereminder_leadtime_list'] = implode(',', $leads);

  unset($form_state['values']['_added']);
  unset($form_state['values']['_units']);
}

/**
 * Build form to configure email message to send.
 *
 * @returns array
 *   The email form.
 */
function datereminder_email_form() {
  module_load_include('inc', 'datereminder', 'includes/defines');
  module_load_include('inc', 'datereminder', 'includes/mailer');
  $form = array();

  $form['datereminder_mail_from'] = array(
    '#type' => 'textfield',
    '#title' => t('Mail sender'),
    '#size' => 30,
    '#default_value' => _datereminder_email_from(),
    '#description' => t('Address reminders should be from. Leave blank to use system default.'),
  );

  $form['datereminder_mail_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Reminder subject'),
    '#size' => 60,
    '#default_value' => _datereminder_email_subject(),
    '#description' => t('Email subject. Use tokens.'),
  );

  $form['datereminder_mail_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Reminder body'),
    '#default_value' => _datereminder_email_body(),
    '#description' => t('Body of reminder email. Use tokens.'),
  );

  $form['datereminder_token_tree'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Available tokens'),
    '#theme' => 'token_tree',
    '#token_types' => array('datereminder','node','user'),
  );

  $form = system_settings_form($form);
  return $form;
}
