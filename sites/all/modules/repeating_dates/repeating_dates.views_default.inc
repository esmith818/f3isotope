<?php
/**
 * @file
 * repeating_dates.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function repeating_dates_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'calendar';
  $view->description = '';
  $view->tag = 'Calendar';
  $view->base_table = 'node';
  $view->human_name = 'Calendar';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['link_display'] = 'page_1';
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'date_views_pager';
  $handler->display->display_options['pager']['options']['date_id'] = 'month';
  $handler->display->display_options['style_plugin'] = 'calendar_style';
  $handler->display->display_options['style_options']['name_size'] = '3';
  $handler->display->display_options['style_options']['with_weekno'] = '0';
  $handler->display->display_options['style_options']['multiday_theme'] = '1';
  $handler->display->display_options['style_options']['theme_style'] = '1';
  $handler->display->display_options['style_options']['max_items'] = '0';
  $handler->display->display_options['row_plugin'] = 'calendar_entity';
  /* Field: Content: Q */
  $handler->display->display_options['fields']['field_q_tax']['id'] = 'field_q_tax';
  $handler->display->display_options['fields']['field_q_tax']['table'] = 'field_data_field_q_tax';
  $handler->display->display_options['fields']['field_q_tax']['field'] = 'field_q_tax';
  $handler->display->display_options['fields']['field_q_tax']['label'] = '';
  $handler->display->display_options['fields']['field_q_tax']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_q_tax']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['field_q_tax']['alter']['text'] = '&nbsp;';
  $handler->display->display_options['fields']['field_q_tax']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_q_tax']['empty'] = ':OPEN';
  $handler->display->display_options['fields']['field_q_tax']['hide_alter_empty'] = FALSE;
  $handler->display->display_options['fields']['field_q_tax']['type'] = 'taxonomy_term_reference_plain';
  $handler->display->display_options['fields']['field_q_tax']['delta_limit'] = 'all';
  $handler->display->display_options['fields']['field_q_tax']['delta_offset'] = '0';
  /* Field: Content: Date */
  $handler->display->display_options['fields']['field_f_new_event_date']['id'] = 'field_f_new_event_date';
  $handler->display->display_options['fields']['field_f_new_event_date']['table'] = 'field_data_field_f_new_event_date';
  $handler->display->display_options['fields']['field_f_new_event_date']['field'] = 'field_f_new_event_date';
  $handler->display->display_options['fields']['field_f_new_event_date']['label'] = '';
  $handler->display->display_options['fields']['field_f_new_event_date']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_f_new_event_date']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_f_new_event_date']['settings'] = array(
    'format_type' => 'short',
    'fromto' => 'both',
    'multiple_number' => '',
    'multiple_from' => '',
    'multiple_to' => '',
    'show_remaining_days' => 0,
    'show_repeat_rule' => 'show',
  );
  $handler->display->display_options['fields']['field_f_new_event_date']['delta_offset'] = '0';
  /* Field: Content: Event Title */
  $handler->display->display_options['fields']['field_f_event_title']['id'] = 'field_f_event_title';
  $handler->display->display_options['fields']['field_f_event_title']['table'] = 'field_data_field_f_event_title';
  $handler->display->display_options['fields']['field_f_event_title']['field'] = 'field_f_event_title';
  $handler->display->display_options['fields']['field_f_event_title']['label'] = '';
  $handler->display->display_options['fields']['field_f_event_title']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_f_event_title']['alter']['text'] = '[field_f_event_title][field__event_q]';
  $handler->display->display_options['fields']['field_f_event_title']['element_label_colon'] = FALSE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['title']['alter']['text'] = '[field_f_event_title][field_q_tax]';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_class'] = 'cal-event-[field_q_tax]';
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['hide_alter_empty'] = FALSE;
  /* Sort criterion: Content: Date -  start date (field_f_new_event_date) */
  $handler->display->display_options['sorts']['field_f_new_event_date_value']['id'] = 'field_f_new_event_date_value';
  $handler->display->display_options['sorts']['field_f_new_event_date_value']['table'] = 'field_data_field_f_new_event_date';
  $handler->display->display_options['sorts']['field_f_new_event_date_value']['field'] = 'field_f_new_event_date_value';
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Subregion (field_subregion) */
  $handler->display->display_options['filters']['field_subregion_tid']['id'] = 'field_subregion_tid';
  $handler->display->display_options['filters']['field_subregion_tid']['table'] = 'field_data_field_subregion';
  $handler->display->display_options['filters']['field_subregion_tid']['field'] = 'field_subregion_tid';
  $handler->display->display_options['filters']['field_subregion_tid']['group'] = 1;
  $handler->display->display_options['filters']['field_subregion_tid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_subregion_tid']['expose']['operator_id'] = 'field_subregion_tid_op';
  $handler->display->display_options['filters']['field_subregion_tid']['expose']['label'] = 'Subregion';
  $handler->display->display_options['filters']['field_subregion_tid']['expose']['operator'] = 'field_subregion_tid_op';
  $handler->display->display_options['filters']['field_subregion_tid']['expose']['identifier'] = 'field_subregion_tid';
  $handler->display->display_options['filters']['field_subregion_tid']['expose']['remember_roles'] = array(
    2 => '2',
    3 => 0,
    8 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
    7 => 0,
    1 => 0,
  );
  $handler->display->display_options['filters']['field_subregion_tid']['type'] = 'select';
  $handler->display->display_options['filters']['field_subregion_tid']['vocabulary'] = 'subregions';
  $handler->display->display_options['filters']['field_subregion_tid']['hierarchy'] = 1;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'f1_event' => 'f1_event',
    'f2_event' => 'f2_event',
    'f3_event' => 'f3_event',
  );
  $handler->display->display_options['filters']['type']['group'] = 1;
  $handler->display->display_options['filters']['type']['exposed'] = TRUE;
  $handler->display->display_options['filters']['type']['expose']['operator_id'] = 'type_op';
  $handler->display->display_options['filters']['type']['expose']['label'] = 'Event Type';
  $handler->display->display_options['filters']['type']['expose']['operator'] = 'type_op';
  $handler->display->display_options['filters']['type']['expose']['identifier'] = 'type';
  $handler->display->display_options['filters']['type']['expose']['remember_roles'] = array(
    2 => '2',
    3 => 0,
    8 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
    7 => 0,
    1 => 0,
  );
  $handler->display->display_options['filters']['type']['expose']['reduce'] = TRUE;
  /* Filter criterion: Global: Dependent filter */
  $handler->display->display_options['filters']['views_dependent_filter']['id'] = 'views_dependent_filter';
  $handler->display->display_options['filters']['views_dependent_filter']['table'] = 'views';
  $handler->display->display_options['filters']['views_dependent_filter']['field'] = 'views_dependent_filter';
  $handler->display->display_options['filters']['views_dependent_filter']['group'] = 1;
  $handler->display->display_options['filters']['views_dependent_filter']['controller_filter'] = 'type';
  $handler->display->display_options['filters']['views_dependent_filter']['controller_values'] = array(
    'f1_event' => 'f1_event',
    'article' => 0,
    'back_blast' => 0,
    'page' => 0,
    'f2_event' => 0,
    'f3_event' => 0,
    'preblast' => 0,
  );
  $handler->display->display_options['filters']['views_dependent_filter']['dependent_filters'] = array(
    'field_event_style_value' => 'field_event_style_value',
  );
  $handler->display->display_options['filters']['views_dependent_filter']['soft_disabling'] = TRUE;
  /* Filter criterion: Content: Style (field_event_style) */
  $handler->display->display_options['filters']['field_event_style_value']['id'] = 'field_event_style_value';
  $handler->display->display_options['filters']['field_event_style_value']['table'] = 'field_data_field_event_style';
  $handler->display->display_options['filters']['field_event_style_value']['field'] = 'field_event_style_value';
  $handler->display->display_options['filters']['field_event_style_value']['group'] = 1;
  $handler->display->display_options['filters']['field_event_style_value']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_event_style_value']['expose']['operator_id'] = 'field_event_style_value_op';
  $handler->display->display_options['filters']['field_event_style_value']['expose']['label'] = 'Workout Style';
  $handler->display->display_options['filters']['field_event_style_value']['expose']['operator'] = 'field_event_style_value_op';
  $handler->display->display_options['filters']['field_event_style_value']['expose']['identifier'] = 'field_event_style_value';
  $handler->display->display_options['filters']['field_event_style_value']['expose']['remember_roles'] = array(
    2 => '2',
    3 => 0,
    8 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
    7 => 0,
    1 => 0,
  );

  /* Display: Month */
  $handler = $view->new_display('page', 'Month', 'page_1');
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'date_views_pager';
  $handler->display->display_options['pager']['options']['date_id'] = 'month';
  $handler->display->display_options['pager']['options']['link_format'] = 'clean';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'calendar_style';
  $handler->display->display_options['style_options']['name_size'] = '3';
  $handler->display->display_options['style_options']['mini'] = '0';
  $handler->display->display_options['style_options']['with_weekno'] = '0';
  $handler->display->display_options['style_options']['multiday_theme'] = '1';
  $handler->display->display_options['style_options']['theme_style'] = '1';
  $handler->display->display_options['style_options']['max_items'] = '0';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'calendar_entity';
  $handler->display->display_options['row_options']['colors']['legend'] = 'type';
  $handler->display->display_options['row_options']['colors']['calendar_colors_type'] = array(
    'f1_event' => '#cc3333',
    'f2_event' => '#3380cc',
    'f3_event' => '#33cc80',
    'article' => '#ffffff',
    'back_blast' => '#ffffff',
    'page' => '#ffffff',
    'preblast' => '#ffffff',
  );
  $handler->display->display_options['row_options']['colors']['taxonomy_field'] = 'field_q_tax';
  $handler->display->display_options['row_options']['colors']['calendar_colors_vocabulary'] = array(
    'field_q_tax' => '4',
  );
  $handler->display->display_options['row_options']['colors']['calendar_colors_taxonomy'] = array(
    105 => '#ffffff',
    53 => '#ffffff',
    69 => '#ffffff',
    91 => '#ffffff',
    111 => '#ffffff',
    42 => '#ffffff',
    117 => '#ffffff',
    95 => '#ffffff',
    49 => '#ffffff',
    60 => '#ffffff',
    106 => '#ffffff',
    120 => '#ffffff',
    115 => '#ffffff',
    123 => '#ffffff',
    66 => '#ffffff',
    76 => '#ffffff',
    99 => '#ffffff',
    59 => '#ffffff',
    94 => '#ffffff',
    110 => '#ffffff',
    63 => '#ffffff',
    50 => '#ffffff',
    96 => '#ffffff',
    61 => '#ffffff',
    73 => '#ffffff',
    122 => '#ffffff',
    55 => '#ffffff',
    112 => '#ffffff',
    119 => '#ffffff',
    114 => '#ffffff',
    118 => '#ffffff',
    75 => '#ffffff',
    48 => '#ffffff',
    93 => '#ffffff',
    107 => '#ffffff',
    64 => '#ffffff',
    121 => '#ffffff',
    72 => '#ffffff',
    92 => '#ffffff',
    68 => '#ffffff',
    70 => '#ffffff',
    62 => '#ffffff',
    67 => '#ffffff',
    77 => '#ffffff',
    56 => '#ffffff',
    51 => '#ffffff',
    113 => '#ffffff',
    65 => '#ffffff',
    109 => '#ffffff',
    116 => '#ffffff',
    108 => '#ffffff',
    43 => '#ffffff',
  );
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Date: Date (node) */
  $handler->display->display_options['arguments']['date_argument']['id'] = 'date_argument';
  $handler->display->display_options['arguments']['date_argument']['table'] = 'node';
  $handler->display->display_options['arguments']['date_argument']['field'] = 'date_argument';
  $handler->display->display_options['arguments']['date_argument']['default_action'] = 'default';
  $handler->display->display_options['arguments']['date_argument']['default_argument_type'] = 'date';
  $handler->display->display_options['arguments']['date_argument']['granularity_reset'] = 0;
  $handler->display->display_options['arguments']['date_argument']['add_delta'] = 'yes';
  $handler->display->display_options['arguments']['date_argument']['date_fields'] = array(
    'field_data_field_f_new_event_date.field_f_new_event_date_value' => 'field_data_field_f_new_event_date.field_f_new_event_date_value',
  );
  $handler->display->display_options['path'] = 'calendar/month';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Month';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['tab_options']['type'] = 'normal';
  $handler->display->display_options['tab_options']['title'] = 'Calendar';
  $handler->display->display_options['tab_options']['weight'] = '0';

  /* Display: Week */
  $handler = $view->new_display('page', 'Week', 'page_2');
  $handler->display->display_options['enabled'] = FALSE;
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'date_views_pager';
  $handler->display->display_options['pager']['options']['date_id'] = 'week';
  $handler->display->display_options['pager']['options']['link_format'] = 'clean';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'calendar_style';
  $handler->display->display_options['style_options']['calendar_type'] = 'week';
  $handler->display->display_options['style_options']['name_size'] = '3';
  $handler->display->display_options['style_options']['mini'] = '0';
  $handler->display->display_options['style_options']['with_weekno'] = '0';
  $handler->display->display_options['style_options']['multiday_theme'] = '1';
  $handler->display->display_options['style_options']['theme_style'] = '1';
  $handler->display->display_options['style_options']['max_items'] = '0';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'calendar_entity';
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Date: Date (node) */
  $handler->display->display_options['arguments']['date_argument']['id'] = 'date_argument';
  $handler->display->display_options['arguments']['date_argument']['table'] = 'node';
  $handler->display->display_options['arguments']['date_argument']['field'] = 'date_argument';
  $handler->display->display_options['arguments']['date_argument']['default_action'] = 'default';
  $handler->display->display_options['arguments']['date_argument']['default_argument_type'] = 'date';
  $handler->display->display_options['arguments']['date_argument']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['date_argument']['granularity'] = 'week';
  $handler->display->display_options['arguments']['date_argument']['add_delta'] = 'yes';
  $handler->display->display_options['arguments']['date_argument']['date_fields'] = array(
    'field_data_field_f_event_date.field_f_event_date_value' => 'field_data_field_f_event_date.field_f_event_date_value',
  );
  $handler->display->display_options['path'] = 'calendar-node-field-f-event-date/week';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Week';
  $handler->display->display_options['menu']['weight'] = '2';
  $handler->display->display_options['menu']['context'] = 0;

  /* Display: Day */
  $handler = $view->new_display('page', 'Day', 'page_3');
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'date_views_pager';
  $handler->display->display_options['pager']['options']['date_id'] = 'day';
  $handler->display->display_options['pager']['options']['link_format'] = 'clean';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'calendar_style';
  $handler->display->display_options['style_options']['calendar_type'] = 'day';
  $handler->display->display_options['style_options']['name_size'] = '3';
  $handler->display->display_options['style_options']['mini'] = '0';
  $handler->display->display_options['style_options']['with_weekno'] = '0';
  $handler->display->display_options['style_options']['multiday_theme'] = '1';
  $handler->display->display_options['style_options']['theme_style'] = '0';
  $handler->display->display_options['style_options']['max_items'] = '0';
  $handler->display->display_options['style_options']['groupby_times'] = '';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'calendar_entity';
  $handler->display->display_options['row_options']['colors']['legend'] = 'type';
  $handler->display->display_options['row_options']['colors']['calendar_colors_type'] = array(
    'f1_event' => '#cc3333',
    'f2_event' => '#3380cc',
    'f3_event' => '#33cc80',
    'article' => '#ffffff',
    'back_blast' => '#ffffff',
    'page' => '#ffffff',
    'preblast' => '#ffffff',
  );
  $handler->display->display_options['row_options']['colors']['taxonomy_field'] = 'field_q_tax';
  $handler->display->display_options['row_options']['colors']['calendar_colors_vocabulary'] = array(
    'field_q_tax' => '4',
  );
  $handler->display->display_options['row_options']['colors']['calendar_colors_taxonomy'] = array(
    105 => '#ffffff',
    53 => '#ffffff',
    69 => '#ffffff',
    91 => '#ffffff',
    111 => '#ffffff',
    42 => '#ffffff',
    117 => '#ffffff',
    95 => '#ffffff',
    49 => '#ffffff',
    60 => '#ffffff',
    106 => '#ffffff',
    120 => '#ffffff',
    115 => '#ffffff',
    123 => '#ffffff',
    66 => '#ffffff',
    76 => '#ffffff',
    99 => '#ffffff',
    59 => '#ffffff',
    94 => '#ffffff',
    110 => '#ffffff',
    63 => '#ffffff',
    50 => '#ffffff',
    96 => '#ffffff',
    61 => '#ffffff',
    73 => '#ffffff',
    122 => '#ffffff',
    55 => '#ffffff',
    112 => '#ffffff',
    119 => '#ffffff',
    114 => '#ffffff',
    118 => '#ffffff',
    75 => '#ffffff',
    48 => '#ffffff',
    93 => '#ffffff',
    107 => '#ffffff',
    64 => '#ffffff',
    121 => '#ffffff',
    72 => '#ffffff',
    92 => '#ffffff',
    68 => '#ffffff',
    70 => '#ffffff',
    62 => '#ffffff',
    67 => '#ffffff',
    77 => '#ffffff',
    56 => '#ffffff',
    51 => '#ffffff',
    113 => '#ffffff',
    65 => '#ffffff',
    109 => '#ffffff',
    116 => '#ffffff',
    108 => '#ffffff',
    43 => '#ffffff',
  );
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Date: Date (node) */
  $handler->display->display_options['arguments']['date_argument']['id'] = 'date_argument';
  $handler->display->display_options['arguments']['date_argument']['table'] = 'node';
  $handler->display->display_options['arguments']['date_argument']['field'] = 'date_argument';
  $handler->display->display_options['arguments']['date_argument']['default_action'] = 'default';
  $handler->display->display_options['arguments']['date_argument']['default_argument_type'] = 'date';
  $handler->display->display_options['arguments']['date_argument']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['date_argument']['granularity'] = 'day';
  $handler->display->display_options['arguments']['date_argument']['date_fields'] = array(
    'field_data_field_f_event_date.field_f_event_date_value' => 'field_data_field_f_event_date.field_f_event_date_value',
  );
  $handler->display->display_options['path'] = 'calendar/day';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Day';
  $handler->display->display_options['menu']['weight'] = '3';
  $handler->display->display_options['menu']['context'] = 0;

  /* Display: Year */
  $handler = $view->new_display('page', 'Year', 'page');
  $handler->display->display_options['enabled'] = FALSE;
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'date_views_pager';
  $handler->display->display_options['pager']['options']['date_id'] = 'year';
  $handler->display->display_options['pager']['options']['link_format'] = 'clean';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'calendar_style';
  $handler->display->display_options['style_options']['calendar_type'] = 'year';
  $handler->display->display_options['style_options']['name_size'] = '1';
  $handler->display->display_options['style_options']['mini'] = '0';
  $handler->display->display_options['style_options']['with_weekno'] = '0';
  $handler->display->display_options['style_options']['multiday_theme'] = '1';
  $handler->display->display_options['style_options']['theme_style'] = '1';
  $handler->display->display_options['style_options']['max_items'] = '0';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'calendar_entity';
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['hide_alter_empty'] = FALSE;
  /* Field: Broken/missing handler */
  $handler->display->display_options['fields']['field_f_event_date']['id'] = 'field_f_event_date';
  $handler->display->display_options['fields']['field_f_event_date']['table'] = 'field_data_field_f_event_date';
  $handler->display->display_options['fields']['field_f_event_date']['field'] = 'field_f_event_date';
  $handler->display->display_options['fields']['field_f_event_date']['label'] = '';
  $handler->display->display_options['fields']['field_f_event_date']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_f_event_date']['hide_alter_empty'] = FALSE;
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Date: Date (node) */
  $handler->display->display_options['arguments']['date_argument']['id'] = 'date_argument';
  $handler->display->display_options['arguments']['date_argument']['table'] = 'node';
  $handler->display->display_options['arguments']['date_argument']['field'] = 'date_argument';
  $handler->display->display_options['arguments']['date_argument']['default_action'] = 'default';
  $handler->display->display_options['arguments']['date_argument']['default_argument_type'] = 'date';
  $handler->display->display_options['arguments']['date_argument']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['date_argument']['granularity'] = 'year';
  $handler->display->display_options['arguments']['date_argument']['add_delta'] = 'yes';
  $handler->display->display_options['arguments']['date_argument']['date_fields'] = array(
    'field_data_field_f_event_date.field_f_event_date_value' => 'field_data_field_f_event_date.field_f_event_date_value',
  );
  $handler->display->display_options['path'] = 'calendar-node-field-f-event-date/year';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Year';
  $handler->display->display_options['menu']['weight'] = '4';
  $handler->display->display_options['menu']['context'] = 0;

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block_1');
  $handler->display->display_options['enabled'] = FALSE;
  $handler->display->display_options['defaults']['use_ajax'] = FALSE;
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'date_views_pager';
  $handler->display->display_options['pager']['options']['date_id'] = 'mini';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'calendar_style';
  $handler->display->display_options['style_options']['calendar_type'] = 'day';
  $handler->display->display_options['style_options']['name_size'] = '1';
  $handler->display->display_options['style_options']['mini'] = '1';
  $handler->display->display_options['style_options']['with_weekno'] = '0';
  $handler->display->display_options['style_options']['multiday_theme'] = '1';
  $handler->display->display_options['style_options']['theme_style'] = '1';
  $handler->display->display_options['style_options']['max_items'] = '0';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'calendar_entity';
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Date: Date (node) */
  $handler->display->display_options['arguments']['date_argument']['id'] = 'date_argument';
  $handler->display->display_options['arguments']['date_argument']['table'] = 'node';
  $handler->display->display_options['arguments']['date_argument']['field'] = 'date_argument';
  $handler->display->display_options['arguments']['date_argument']['default_action'] = 'default';
  $handler->display->display_options['arguments']['date_argument']['default_argument_type'] = 'date';
  $handler->display->display_options['arguments']['date_argument']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['date_argument']['add_delta'] = 'yes';
  $handler->display->display_options['arguments']['date_argument']['date_fields'] = array(
    'field_data_field_f_event_date.field_f_event_date_value' => 'field_data_field_f_event_date.field_f_event_date_value',
  );

  /* Display: Upcoming */
  $handler = $view->new_display('block', 'Upcoming', 'block_2');
  $handler->display->display_options['enabled'] = FALSE;
  $handler->display->display_options['display_description'] = 'Upcoming events block';
  $handler->display->display_options['defaults']['use_more'] = FALSE;
  $handler->display->display_options['use_more'] = TRUE;
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '5';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Date: Date (node) */
  $handler->display->display_options['filters']['date_filter']['id'] = 'date_filter';
  $handler->display->display_options['filters']['date_filter']['table'] = 'node';
  $handler->display->display_options['filters']['date_filter']['field'] = 'date_filter';
  $handler->display->display_options['filters']['date_filter']['operator'] = '>=';
  $handler->display->display_options['filters']['date_filter']['default_date'] = 'now';
  $handler->display->display_options['filters']['date_filter']['add_delta'] = 'yes';
  $handler->display->display_options['filters']['date_filter']['date_fields'] = array(
    'field_data_field_f_event_date.field_f_event_date_value' => 'field_data_field_f_event_date.field_f_event_date_value',
  );

  /* Display: Block 3 */
  $handler = $view->new_display('block', 'Block 3', 'block_3');
  $handler->display->display_options['enabled'] = FALSE;
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'calendar_style';
  $handler->display->display_options['style_options']['calendar_type'] = 'day';
  $handler->display->display_options['style_options']['name_size'] = '3';
  $handler->display->display_options['style_options']['mini'] = '0';
  $handler->display->display_options['style_options']['with_weekno'] = '0';
  $handler->display->display_options['style_options']['multiday_theme'] = '1';
  $handler->display->display_options['style_options']['theme_style'] = '0';
  $handler->display->display_options['style_options']['max_items'] = '0';
  $handler->display->display_options['style_options']['groupby_times'] = '';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'calendar_entity';
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Date: Date (node) */
  $handler->display->display_options['arguments']['date_argument']['id'] = 'date_argument';
  $handler->display->display_options['arguments']['date_argument']['table'] = 'node';
  $handler->display->display_options['arguments']['date_argument']['field'] = 'date_argument';
  $handler->display->display_options['arguments']['date_argument']['default_action'] = 'default';
  $handler->display->display_options['arguments']['date_argument']['default_argument_type'] = 'date';
  $handler->display->display_options['arguments']['date_argument']['granularity'] = 'day';
  $handler->display->display_options['arguments']['date_argument']['granularity_reset'] = 0;
  $handler->display->display_options['arguments']['date_argument']['date_fields'] = array(
    'field_data_field_f_event_date.field_f_event_date_value' => 'field_data_field_f_event_date.field_f_event_date_value',
  );
  $handler->display->display_options['block_description'] = 'Block-DayCalendar';
  $export['calendar'] = $view;

  return $export;
}
