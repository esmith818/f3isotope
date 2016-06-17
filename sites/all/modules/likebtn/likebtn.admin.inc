<?php

/**
 * @file
 * Administration forms for the LikeBtn module.
 */

/**
 * Menu callback; Displays the administration settings for LikeBtn.
 */
function likebtn_general_settings() {

  $public_url = _likebtn_public_url();

  drupal_add_js(drupal_get_path('module', 'likebtn') . "/assets/js/admin.js", 'file');

  $form = array();

  // Get all available content types.
  $types = node_type_get_types();
  $options = array();
  foreach ($types as $type) {
    $options[$type->type] = $type->name;
  }

  // Get all available entities view modes.
  $view_modes = entity_get_info('node');
  foreach ($view_modes['view modes'] as $view_mode_id => $view_mode_info) {
    $view_modes_options[$view_mode_id] = $view_mode_info['label'];
  }

  $form['likebtn_plan'] = array(
    '#type'          => 'select',
    '#title'         => t('Website tariff plan'),
    '#description'   => t('Specify your website <a href="http://likebtn.com/en/#plans_pricing">plan</a>. The plan specified determines available settings.'),
    '#default_value' => variable_get('likebtn_plan', LIKEBTN_PLAN_TRIAL),
    '#options'       => array(
      LIKEBTN_PLAN_TRIAL => 'TRIAL',
      LIKEBTN_PLAN_FREE  => 'FREE',
      LIKEBTN_PLAN_PLUS  => 'PLUS',
      LIKEBTN_PLAN_PRO   => 'PRO',
      LIKEBTN_PLAN_VIP  => 'VIP',
      LIKEBTN_PLAN_ULTRA  => 'ULTRA',
    ),
  );

  $form['likebtn_general_display_options'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('General display options'),
    '#collapsible' => FALSE,
    '#collapsed'   => FALSE,
  );

  $form['likebtn_general_display_options']['likebtn_hint'] = array(
    '#type'          => 'item',
    '#description'   => t('You can set up the Like Button globally on this page, or per content type as a field in <a href="@link-manage_fields">Structure » Content types » Manage fields</a>.') . '<br/>' . t('Keep in mind that only websites upgraded to <a href="http://likebtn.com/en/#plans_pricing" target="_blank">PLUS</a> plan or higher are allowed to display more then 10 like buttons per page.',
      array(
        '@link-manage_fields'   => url('admin/structure/types'),
      )
    ),
  );

  $form['likebtn_general_display_options']['likebtn_nodetypes'] = array(
    '#type'          => 'checkboxes',
    '#title'         => t('Enable for the following content types'),
    '#description'   => t('Select the content types for which you want to activate like button.'),
    '#default_value' => variable_get('likebtn_nodetypes', array(
        'article' => 'article',
        'page' => 'page',
    )),
    '#options'       => $options,
  );

  $form['likebtn_general_display_options']['likebtn_comments_nodetypes'] = array(
    '#type'          => 'checkboxes',
    '#title'         => t('Enable for comments to the following content types'),
    '#description'   => t('Select the content types for comments to which you want to activate like button.'),
    '#default_value' => variable_get('likebtn_comments_nodetypes', array()),
    '#options'       => $options,
    '#disabled'      => !module_exists('comment'),
  );

  $form['likebtn_general_display_options']['likebtn_view_modes'] = array(
    '#type'          => 'checkboxes',
    '#title'         => t('Entities view modes'),
    '#description'   => t('When will the like button be displayed?'),
    '#default_value' => variable_get('likebtn_view_modes', array(
        'full' => 'full',
        'teaser' => 'teaser',
    )),
    '#options'       => $view_modes_options,
  );

  $form['likebtn_general_display_options']['likebtn_weight'] = array(
    '#type'          => 'select',
    '#title'         => t('Position'),
    '#description'   => t('The more the weight, the lower like button position in the entity.'),
    '#default_value' => variable_get('likebtn_weight', 10),
    '#options'       => array(
      -100 => '-100',
      -50  => '-50',
      -20  => '-20',
      -10  => '-10',
      -5   => '-5',
      5    => '5',
      10   => '10',
      20   => '20',
      50   => '50',
      100  => '100',
    ),
  );

  $form['likebtn_general_display_options']['likebtn_user_logged_in'] = array(
    '#type'          => 'select',
    '#title'         => t('User authorization'),
    '#description'   => t('Show the Like Button when user is logged in, not logged in or show for all.'),
    '#default_value' => variable_get('likebtn_user_logged_in', 'all'),
    '#options'       => array(
      'all' => t('For all'),
      'logged_in' => t('Logged in'),
      'not_logged_in' => t('Not logged in'),
    ),
  );

  $form['likebtn_account_data'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Automatic syncing of likes into local database') . ' (PRO, VIP, ULTRA)',
    '#collapsible' => TRUE,
    '#collapsed'   => TRUE,
  );
  $form['likebtn_account_data']['likebtn_hint_account_data'] = array(
    '#type'          => 'item',
    '#description'   => t('Fill in these fields if you want information on likes to be periodically fetched from LikeBtn.com system into your database. It would allow to sort content in views by vote results using Voting API or LikeBtn field.') . '<br/><br/>' . t('Requirements:') . '<ul><li>' . t('Your website must be upgraded to <a href="http://likebtn.com/en/#plans_pricing" target="_blank">PRO</a> or higher on <a href="http://likebtn.com/en/#plans_pricing" target="_blank">LikeBtn.com</a>.') . '</li><li>' . t('PHP curl extension must be enabled.') . '</li></ul>',
  );
  $form['likebtn_account_data']['likebtn_account_data_email'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Email'),
    '#default_value' => variable_get('likebtn_account_data_email', ''),
    '#description'   => t('Your LikeBtn.com account email (can be found on <a href="http://likebtn.com/en/customer.php/profile/edit" target="_blank">Profile page</a>)'),
    '#states' => array(
      // Disable field.
      'disabled' => array(
        array(':input[name="likebtn_plan"]' => array('value' => LIKEBTN_PLAN_FREE)),
        array(':input[name="likebtn_plan"]' => array('value' => LIKEBTN_PLAN_PLUS)),
      ),
    ),
  );
  $form['likebtn_account_data']['likebtn_account_data_api_key'] = array(
    '#type'          => 'textfield',
    '#title'         => t('API key'),
    '#maxlength'     => 32,
    '#default_value' => variable_get('likebtn_account_data_api_key', ''),
    '#description'   => t('Your website API key on LikeBtn.com (can be requested on <a href="http://likebtn.com/en/customer.php/websites" target="_blank">Websites page</a>)'),
    '#states' => array(
      // Disable field.
      'disabled' => array(
        array(':input[name="likebtn_plan"]' => array('value' => LIKEBTN_PLAN_FREE)),
        array(':input[name="likebtn_plan"]' => array('value' => LIKEBTN_PLAN_PLUS)),
      ),
    ),
  );
  $form['likebtn_account_data']['likebtn_sync_inerval'] = array(
    '#type'          => 'select',
    '#title'         => t('Synchronization interval'),
    '#description'   => t('Time interval in minutes in which fetching of likes from LikeBtn.com into your database is being launched. The less the interval the heavier your database load (60 minutes interval is recommended)'),
    '#default_value' => variable_get('likebtn_sync_inerval', 60),
    '#options'       => array(
      5 => '5',
      15 => '15',
      30 => '30',
      60 => '60',
      90 => '90',
      120 => '120',
      120 => '120',
    ),
    '#states' => array(
      // Disable field.
      'disabled' => array(
        array(':input[name="likebtn_plan"]' => array('value' => LIKEBTN_PLAN_FREE)),
        array(':input[name="likebtn_plan"]' => array('value' => LIKEBTN_PLAN_PLUS)),
      ),
    ),
  );
  $form['likebtn_account_data']['likebtn_test_sync'] = array(
    '#type' => 'markup',
    '#markup' => '<button onclick="return testSync(\'' . _likebtn_subdirectory() . 'admin/config/services/likebtn/testsync\')" >' . t('Test synchronization') . '</button> &nbsp;<strong class="likebtn_test_sync_container"><img src="' . $public_url . '/assets/img/ajax_loader.gif" style="display:none"/><span class="likebtn_test_sync_message"></span></strong>',
  );

  $form['likebtn_domain_subdirectory'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Domain and subdirectory'),
    '#collapsible' => TRUE,
    '#collapsed'   => TRUE,
  );

  $form['likebtn_domain_subdirectory']['likebtn_settings_local_domain'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Local domain'),
    '#description'   => t('Example:') . ' localdomain!50f358d30acf358d30ac000001. ' . t('Specify it if your website is located on a local server and is available from your local network only and NOT available from the Internet. You can find the domain on your <a href="http://likebtn.com/en/customer.php/websites" target="_blank">Websites</a> page after adding your local website to the panel. See <a href="http://likebtn.com/en/faq#local_domain" target="_blank">FAQ</a> for more details.'),
    '#default_value' => variable_get('likebtn_settings_local_domain', ''),
  );

  $form['likebtn_domain_subdirectory']['likebtn_settings_subdirectory'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Website subdirectory'),
    '#description'   => t('If your website is one of websites located in different subdirectories of one domain and you want to have a statistics separate from other websites on this domain, enter subdirectory (for example /subdirectory/).'),
    '#default_value' => variable_get('likebtn_settings_subdirectory', ''),
  );

  //$form = array_merge($form, _likebtn_settings_form());

  return system_settings_form($form);
}

/**
 * Menu callback; Displays the administration settings for LikeBtn.
 */
function likebtn_likebtn_settings()
{
    return system_settings_form(_likebtn_settings_form());
}