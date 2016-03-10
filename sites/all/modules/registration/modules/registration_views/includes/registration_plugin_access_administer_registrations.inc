<?php

/**
 * @file
 * Definition of registration_plugin_access_administer_registrations.
 */

/**
 * Allows restricting access to Registrations views based on access to
 * host entities.
 *
 * @ingroup views_access_plugins
 */
class registration_plugin_access_administer_registrations extends views_plugin_access {
  function access($account) {
    $entity = $this->tokenize_value($this->options['entity'], 0);
    return registration_administer_registrations_access($this->options['entity_type'], (int)$entity, $account);
  }

  function get_access_callback() {
    $entity = $this->tokenize_value($this->options['entity'], 0);
    return array('registration_administer_registrations_access', array($this->options['entity_type'], (int)$entity));
  }

  function summary_title() {
    return t('%entity_type with ID %entity', array('%entity_type' => $this->options['entity_type'], '%entity' => $this->options['entity']));
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['entity_type'] = array('default' => NULL);
    $options['entity'] = array('default' => NULL);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['entity_type'] = array(
      '#type' => 'textfield',
      '#title' => t('Entity type'),
      '#default_value' => $this->options['entity_type'],
      '#required' => TRUE,
      '#description' => t("The type of entity who's permission needs to be checked."),
    );
    $form['entity'] = array(
      '#type' => 'textfield',
      '#title' => t('Entity ID'),
      '#default_value' => $this->options['entity'],
      '#required' => TRUE,
      '#description' => t("The entity ID of the entity who's permission needs to be checked."),
    );

    $count = 0; // This lets us prepare the key as we want it printed.
    foreach ($this->view->display_handler->get_handlers('argument') as $arg => $handler) {
      $options[t('Arguments')]['%' . ++$count] = t('@argument title', array('@argument' => $handler->ui_name()));
      $options[t('Arguments')]['!' . $count] = t('@argument input', array('@argument' => $handler->ui_name()));
    }

    if (!empty($options)) {
      $output = '<p>' . t('The following tokens are available. If you would like to have the characters \'[\' and \']\' please use the html entity codes \'%5B\' or  \'%5D\' or they will get replaced with empty space.' . '</p>');
      foreach (array_keys($options) as $type) {
        if (!empty($options[$type])) {
          $items = array();
          foreach ($options[$type] as $key => $value) {
            $items[] = $key . ' == ' . $value;
          }
          $output .= theme('item_list',
            array(
              'items' => $items,
              'type' => $type
            ));
        }
      }

      $form['token_help'] = array(
        '#type' => 'fieldset',
        '#title' => t('Replacement patterns'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#value' => $output,
        '#id' => 'edit-options-token-help',
        '#dependency' => array(
          'edit-options-tokenize' => array(1),
        ),
        '#prefix' => '<div>',
        '#suffix' => '</div>',
      );
    }
  }

  function options_validate(&$form, &$form_state) {
    // @todo
    if (FALSE && !$form_state['values']['entity_type']) {
      form_error($form['entity_type'], t('You must enter a valid entity type.'));
    }
  }

  /**
   * Get the 'render' tokens to use for advanced rendering.
   *
   * This runs through all of the fields and arguments that
   * are available and gets their values. This will then be
   * used in one giant str_replace().
   */
  function get_render_tokens() {
    $tokens = array();
    if (!empty($this->view->build_info['substitutions'])) {
      $tokens = $this->view->build_info['substitutions'];
    }

    $path = explode('/', $this->view->display_handler->get_option('path'));
    $count = 0;
    foreach ($path as $key => $piece) {
      if (strpos($piece, '%') !== FALSE) {
        $token = '%' . ++$count;
        if (!isset($tokens[$token])) {
          $tokens[$token] = '';
        }

        // Use strip tags as there should never be HTML in the path.
        // However, we need to preserve special characters like " that
        // were removed by check_plain().
        $tokens['!' . $count] = $key;
      }
    }

    return $tokens;
  }

  /**
   * Replace a value with tokens from the last field.
   */
  function tokenize_value($value, $row_index = NULL) {
    if (strpos($value, '[') !== FALSE || strpos($value, '!') !== FALSE || strpos($value, '%') !== FALSE) {
      $tokens = $this->get_render_tokens();

      $value = filter_xss_admin($value);
      $value = strtr($value, $tokens);
      $value = strip_tags($value);
    }

    return $value;
  }
}
