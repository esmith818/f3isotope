<?php
/**
 * @file
 * views_aggregator_plugin_style_table.inc
 *
 * Based on the default Views table style plugin and a similar class in the
 * Views Calc module.
 */

/**
 * Style plugin to render each item as a row in a table.
 *
 * @ingroup views_style_plugins
 */
class views_aggregator_plugin_style_table extends views_plugin_style_table {

  /**
   * Option definition.
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['group_aggregation'] = array(
      'contains' => array(
        'grouping_field_class' => array('default' => ''),
      )
    );
    $options['column_aggregation'] = array(
      'contains' => array(
        'totals_per_page' => array('default' => TRUE),
        'totals_row_position' => array('default' => array(1 => 0, 2 => 2)),
        'totals_row_class' => array('default' => ''),
        'precision' => array('default' => 2),
      )
    );
    return $options;
  }

  /**
   * Create the tabular form with the aggregation options.
   */
  public function options_form(&$form, &$form_state) {

    $handlers = $this->display->handler->get_handlers('field');
    $columns = $this->sanitize_columns($this->options['columns']);

    foreach ($columns as $field => $column) {
      if ($field == $column) {
        // Make all columns potentially sortable, including Math Expressions.
        // Do this before parent::options_form($form, $form_state);
        $handlers[$field]->definition['click sortable'] = TRUE;
      }
    }
    // Note: bulk of form is provided by superclass views_plugin_style_table.
    parent::options_form($form, $form_state);

    // See function views_aggregator_theme().
    $form['#theme'] = 'views_aggregator_plugin_style_table';

    // Views style of grouping (splitting table into many) interferes, so
    // get rid of the form.
    unset($form['grouping']);

    $form['description_markup'] = array(
      '#markup' => '<div class="description form-item">' . t('Column aggregation functions may be enabled independently of group aggregation functions. Every group aggregation function, except <em>Filter rows (by regexp)</em>, requires exactly <strong>one</strong> field to be assigned the <em>Group and compress</em> function. With that done, select any of the other aggregation functions for some or all of the fields. Functions marked with an asterisk take an optional parameter. For the aggregation functions <em>Enumerate, Range</em> and <em>Tally</em> the optional parameter is a delimiter to separate items. <br/>You may combine multiple fields into the same render column. If you do, the separator specified will be used to separate the fields. You can control column order and field labels in the Fields section of the main configuration page. For the column aggregation function <em>Math expression</em> you may use the tokens from that same page as documented in the "Replacement patterns" in the section "Rewrite the output of this field".') . '</div>',
    );

    foreach ($columns as $field => $column) {

      $form['info'][$field]['has_aggr'] = array(
        '#type' => 'checkbox',
        '#title' => t('Apply group function'),
        '#default_value' => isset($this->options['info'][$field]['has_aggr']) ? $this->options['info'][$field]['has_aggr'] : FALSE,
      );

      $group_options = array();
      $column_options = array();
      foreach (views_aggregator_get_aggregation_functions_info() as $function => $display_names) {
        if (!empty($display_names['group'])) {
          $group_options[$function] = $display_names['group'];
        }
        if (!empty($display_names['column'])) {
          $column_options[$function] = $display_names['column'];
        }
      }
      $form['info'][$field]['aggr'] = array(
        '#type' => 'select',
        '#options' => $group_options,
        '#multiple' => TRUE,
        '#default_value' => empty($this->options['info'][$field]['aggr']) ? array('views_aggregator_first') : $this->options['info'][$field]['aggr'],
        '#states' => array(
          'visible' => array(
            'input[name="style_options[info][' . $field . '][has_aggr]"]' => array(
              'checked' => TRUE,
            ),
          ),
        ),
      );
      // Optional parameter for the selected aggregation function.
      $parameter_label = t('Parameter');
      $form['info'][$field]['aggr_par'] = array(
        '#type' => 'textfield',
        '#size' => 23,
        '#title' => $parameter_label,
        '#default_value' => isset($this->options['info'][$field]['aggr_par']) ? $this->options['info'][$field]['aggr_par'] : '',
        '#states' => array(
          'visible' => array(
            'input[name="style_options[info][' . $field . '][has_aggr]"]' => array(
              'checked' => TRUE,
            ),
            //'select[name="style_options[info][' . $field . '][aggr][]"]' => array(
            //  'value' => array('views_aggregator_sum'),
            //),
          ),
        ),
      );

      $form['info'][$field]['has_aggr_column'] = array(
        '#type' => 'checkbox',
        '#title' => t('Apply column function'),
        '#default_value' => isset($this->options['info'][$field]['has_aggr_column']) ? $this->options['info'][$field]['has_aggr_column'] : FALSE,
      );
      $form['info'][$field]['aggr_column'] = array(
        '#type' => 'select',
        '#options' => $column_options,
        '#multiple' => FALSE,
        '#default_value' => empty($this->options['info'][$field]['aggr_column']) ? 'views_aggregator_sum' : $this->options['info'][$field]['aggr_column'],
        '#states' => array(
          'visible' => array(
            'input[name="style_options[info][' . $field . '][has_aggr_column]"]' => array(
              'checked' => TRUE,
            ),
          ),
        ),
      );
      // Optional parameter for the selected column aggregation function.
      $form['info'][$field]['aggr_par_column'] = array(
        '#type' => 'textfield',
        '#size' => 24,
        '#title' => $parameter_label,
        '#default_value' => isset($this->options['info'][$field]['aggr_par_column']) ? $this->options['info'][$field]['aggr_par_column'] : '',
        '#states' => array(
          'visible' => array(
            'input[name="style_options[info][' . $field . '][has_aggr_column]"]' => array(
              'checked' => TRUE,
            ),
          ),
        ),
      );
    }

    $form['group_aggregation'] = array(
      '#type' => 'fieldset',
      '#title' => t('Group aggregation options'),
      '#weight' => -2,
    );
    $form['group_aggregation']['grouping_field_class'] = array(
      '#title' => t('Grouping field cell class'),
      '#type' => 'textfield',
      '#description' => t('The CSS class to provide on each cell of the column belonging to the field that is being <em>Grouped and compressed</em>.'),
      '#default_value' => $this->options['group_aggregation']['grouping_field_class'],
    );

    $form['column_aggregation'] = array(
      '#type' => 'fieldset',
      '#title' => t('Column aggregation options'),
      '#weight' => -1,
    );
    $form['column_aggregation']['totals_row_position'] = array(
      '#title' => t('Column aggregation row position'),
      '#type' => 'checkboxes',
      '#options' => array(
        1 => t('in the table header'),
        2 => t('in the table footer'),
      ),
      '#default_value' => $this->options['column_aggregation']['totals_row_position'],
    );
    $form['column_aggregation']['totals_per_page'] = array(
      '#title' => t('Column aggregation row applies to'),
      '#type' => 'radios',
      '#options' => array(
        1 => t('the page shown, if a pager is enabled'),
        0 => t('the entire result set'),
      ),
      '#description' => t('If your view does not have a pager, then the two options are equivalent.'),
      '#default_value' => $this->options['column_aggregation']['totals_per_page'],
      '#weight' => 1,
    );
    $form['column_aggregation']['precision'] = array(
      '#title' => t('Column aggregation row default numeric precision'),
      '#type' => 'textfield',
      '#size' => 3,
      '#description' => t('The number of decimals to use for column aggregations whose precisions are not defined elsewhere -- for example aggregations on Views PHP numbers.'),
      '#default_value' => $this->options['column_aggregation']['precision'],
      '#weight' => 2,
    );
    $form['column_aggregation']['totals_row_class'] = array(
      '#title' => t('Column aggregation row class'),
      '#type' => 'textfield',
      '#description' => t('The CSS class to provide on the row containing the column aggregations.'),
      '#default_value' => $this->options['column_aggregation']['totals_row_class'],
      '#weight' => 3,
    );
  }

  /**
   * Overrides options_validate().
   */
  public function options_validate(&$form, &$form_state) {
    parent::options_validate($form, $form_state);

    $allowed_tags = array('b', 'br', 'em', 'i', 'p', 'strong', 'u');
    $tag_msg = t('<strong>Parameter</strong> field contains an illegal character or illegal HTML tag. Allowed tags are: %tags', array('%tags' => implode(', ', $allowed_tags)));

    // Count the number of occurrences of the grouping and other aggregation
    // functions.
    $num_grouped = 0;
    $num_aggregation_functions = 0;
    foreach ($form_state['values']['style_options']['info'] as $field_name => $options) {
      if (!empty($options['has_aggr'])) {
        if (in_array('views_aggregator_group_and_compress', $options['aggr'])) {
          $num_grouped++;
        }
        elseif (!in_array('views_aggregator_row_filter', $options['aggr'])) {
          $num_aggregation_functions += count($options['aggr']);
        }
      }
      $filtered = filter_xss($options['aggr_par'], $allowed_tags);
      if ($options['aggr_par'] != $filtered) {
        form_error($form['info'][$field_name]['aggr_par'], $tag_msg);
      }
      $filtered = filter_xss($options['aggr_par_column'], $allowed_tags);
      if ($options['aggr_par_column'] != $filtered) {
        form_error($form['info'][$field_name]['aggr_par_column'], $tag_msg);
      }
    }
    // When we have no aggregation functions, we must have 0 or 1 grouping
    // function. When we have aggregation functions, there must be 1 grouping.
    $ok = ($num_aggregation_functions == 0) ? $num_grouped <= 1 : $num_grouped == 1;
    if (!$ok) {
      $msg = t('When applying group aggregation functions, you must also select <em>"Group and compress"</em> on exactly one field.');
      foreach ($form_state['values']['style_options']['info'] as $field_name => $options) {
        form_error($form['info'][$field_name]['aggr'], $msg);
        $msg = '';
      }
    }
  }

  /*
   * Overrides pre_render().
   *
   * @param array $results
   *   the results returned from the database query
   *
   * Note that this class being a views_plugin, rather than a views_handler,
   * it does not have a post_execute() function.
   *
   * This function applies to the currently visible page only. If paging is
   * enabled for this display view->result may only contain part of the entire
   * result set.
   */
  public function pre_render($results) {
    if (isset($this->view->is_temp_views_aggregator)) {
      return;
    }
    parent::pre_render($results);

    if (empty($this->view->result)) {
      return;
    }
    $functions = $this->collect_aggregation_functions();

    $show_global_totals_with_pager = empty($this->options['column_aggregation']['totals_per_page']) && !empty($this->view->total_rows);

    if ($show_global_totals_with_pager) {
      $view_without_pager = $this->execute_view_without_pager($this->view, $this->view->current_display);

      // First apply the row filters (if any), then aggregate the columns.
      $view_without_pager->style_plugin->apply_row_filters();
      // Only interested in column aggregation, so only 'column' group needed.
      $column_group = array('column' => array());
      foreach ($view_without_pager->result as $num => $row) {
        $column_group['column'][$num] = $row;
      }
      $totals = $view_without_pager->style_plugin->execute_aggregation_functions($column_group, $functions);
      $this->view->totals = $this->set_totals_row($totals);
    }
    // Because we are going to need the View results AFTER token replacement,
    // we render the result set here. This is NOT duplication of CPU time,
    // because render_fields(), if called for a second time, will do nothing
    // when $this->rendered_fields has been populated already.
    // render_fields() will puts currency signs in front of moneys, embeds node
    // and taxonomy term references in hyperlinks etc.
    $this->render_fields($results);

    // Apply the row filters first, then aggregate the groups.
    $this->apply_row_filters();
    $groups = $this->aggregate_groups();
    $values = $this->execute_aggregation_functions($groups, $functions);

    unset($groups['column']);

    // Write group aggregation results into the View results.
    $this->set_aggregated_group_values($groups, $values);
    if (empty($this->view->totals)) {
      // If not already set above, write the column aggregation result row on
      // the View object. This row will be rendered via
      // template_preprocess_views_aggregator_results_table().
      $this->view->totals = $this->set_totals_row($values);
    }

    // With the aggregation functions now complete, destroy rows not part of the
    // aggregation.
    $this->compress_grouped_results($groups);

    // Sort the table based on the selected sort column, i.e. $this->active.
    if (isset($this->active)) {
      // To aid in sorting, add the row's index to each row object.
      foreach ($this->view->result as $num => $row) {
        $this->view->result[$num]->num = $num;
      }
      uasort($this->view->result, array($this, 'compare_result_rows'));
    }
  }

  /**
   * Filters out rows from the table based on a field cell matching a regexp.
   */
  protected function apply_row_filters() {
    $field_handlers = $this->view->field;
    foreach ($this->options['info'] as $field_name => $options) {
      if (!empty($options['has_aggr']) && in_array('views_aggregator_row_filter', $options['aggr'])) {
        views_aggregator_row_filter($this, $field_handlers[$field_name], $options['aggr_par']);
      }
    }
  }

  /**
   * Aggregate and compress the View's rows into groups.
   *
   * @return array
   *   an array of aggregated groups
   */
  protected function aggregate_groups() {
    $field_handlers = $this->view->field;
    // Find the one column to group by and execute the grouping.
    foreach ($this->options['info'] as $field_name => $options) {
      if (!empty($options['has_aggr']) && in_array('views_aggregator_group_and_compress', $options['aggr'], FALSE)) {
        $groups = views_aggregator_group_and_compress($this->view->result, $field_handlers[$field_name], $options['aggr_par']);
        break;
      }
    }
    if (empty($groups)) {
      // If there are no regular groups, create a special group for column
      // aggregation. This group holds all View result rows.
      foreach ($this->view->result as $num => $row) {
        $groups['column'][$num] = $row;
      }
    }
    return $groups;
  }

  /**
   * Collect the aggregation functions from the Views UI.
   *
   * @return array functions
   */
  protected function collect_aggregation_functions() {
    $functions = array();
    foreach ($this->options['info'] as $field_name => $options) {
      // Make a list of the group and column functions to call for this field.
      if (!empty($options['has_aggr'])) {
        foreach ($options['aggr'] as $function) {
          if ($function != 'views_aggregator_row_filter' && $function != 'views_aggregator_group_and_compress') {
            if (empty($functions[$field_name]) || !in_array($function, $functions[$field_name])) {
              $functions[$field_name][] = $function;
            }
          }
        }
      }
      // Column aggregation function, if requested, is last.
      if (!empty($options['has_aggr_column'])) {
        $function = $options['aggr_column'];
        if (empty($functions[$field_name]) || !in_array($function, $functions[$field_name])) {
          $functions[$field_name][] = $function;
        }
      }
    }
    return $functions;
  }

  /**
   * Executes the supplied aggregation functions with the groups as arguments.
   *
   * @param array $groups
   * @param array $functions
   *
   * @return array of function return values
   */
  protected function execute_aggregation_functions($groups, $functions) {
    $field_handlers = $this->view->field;
    $values = array();
    foreach ($functions as $field_name => $field_functions) {
      if (empty($field_handlers[$field_name])) {
        continue;
      }
      $options = $this->options['info'][$field_name];
      foreach ($field_functions as $function) {
        $group_par = (!isset($options['aggr_par']) || $options['aggr_par'] == '') ? NULL : $options['aggr_par'];
        $column_par = (!isset($options['aggr_par_column']) || $options['aggr_par_column'] == '') ? NULL : $options['aggr_par_column'];
        $aggr_values = $function($groups, $field_handlers[$field_name], $group_par, $column_par);
        // $aggr_values is indexed by group value and/or 'column'.
        // 'column' is the last evaluated value for the field.
        if (isset($aggr_values['column'])) {
          $field_handlers[$field_name]->last_render = $aggr_values['column'];
        }
        foreach ($aggr_values as $group => $value) {
          // 'column' function is last so may override earlier value.
          if (!isset($values[$field_name][$group]) || $group == 'column') {
            $values[$field_name][$group] = $value;
          }
        }
      }
    }
    return $values;
  }

  /**
   * Removes no longer needed View result rows from the set.
   *
   * @param type $groups
   */
  protected function compress_grouped_results($groups) {
    foreach ($groups as $rows) {
      $is_first = TRUE;
      foreach ($rows as $num => $row) {
        // The aggregated row is the first of each group. Destroy the others.
        if (!$is_first) {
          unset($this->rendered_fields[$num]);
          unset($this->view->result[$num]);
        }
        $is_first = FALSE;
      }
    }
  }

  /**
   * Returns the raw or rendered result at the intersection of column and row.
   *
   * @param object $field_handler
   *   The handler associated with the result column being requested.
   * @param int $row_num
   *   The result row number.
   * @param bool $render
   *   Whether the rendered or raw value should be returned.
   *
   * @return string
   *  Returns empty string if there are no results for the requested row_num.
   */
  public function get_cell($field_handler, $row_num, $render) {
    $field_name = $field_handler->options['id'];
    if (isset($this->rendered_fields[$row_num][$field_name])) {
      // Bit of a hack for "Webform submission data: Value(...)" and
      // "Global: Math expression" fields... Always pick up the rendered fields,
      // as it seems that's all we can get!
      if ($render || is_a($field_handler, 'webform_handler_field_submission_data') || is_a($field_handler, 'views_php_handler_field')) {
        return $this->rendered_fields[$row_num][$field_name];
      }
      if (is_a($field_handler, 'views_handler_field_math')) {
        // Ignore non-numeric leading characters like currency signs.
        return vap_num($this->rendered_fields[$row_num][$field_name]);
      }
    }
    if (!isset($field_handler->view->result[$row_num])) {
      return '';
    }
    $field_handler->view->row_index = $row_num;
    return $this->get_cell_raw($field_handler, $field_handler->view->result[$row_num], TRUE);
  }

  /**
   * Returns the raw, unrendered result at the intersection of column and row.
   *
   * Should normally not be called, especially not for Math Expr. or PHP fields.
   *
   * @param object $field_handler
   *   The handler associated with the result column being requested.
   * @param object $result_row
   *   The result row.
   * @param bool $compressed
   *   If the result is a (nested) array, return the first primitive value.
   *
   * @return string
   *   the raw contents of the cell
   */
  private function get_cell_raw($field_handler, $result_row, $compressed = TRUE) {
    
    $field_name = 'field_' . $field_handler->options['id'];
    if (isset($result_row->$field_name)) {
      $value = reset($result_row->$field_name);
      $value = isset($value['raw']) ? $value['raw'] : $value;
    }
    elseif (isset($result_row->{$field_handler->field_alias})) {
      // nid, node_title etc.
      $value = $result_row->{$field_handler->field_alias};
    }
    else {
      return '';
    }
    // Deal with multiple subvalues like AddressFields:
    // $value[0]['country'] == 'AU'
    // $value[0]['postal_code'] = '3040' etc.
    //
    if ($compressed && is_array($value)) {
      $value = reset($value);
      if (is_array($value)) {
        $value = reset($value);
      }
    }
    return $value;
  }

  /**
   * Render and set a raw value on the table cell in specified column and row.
   *
   * @param object $field_handler
   *   The field handler associated with the table column being requested.
   * @param int $row_num
   *   The result row number. Must be specified.
   * @param mixed $new_values
   *   A single or array of values to set. This should be the raw value(s),
   *   otherwise sorting may not work properly.
   * @param string $separator
   *   The separator to use, when $new_values is an array
   *
   * @return mixed
   *   The rendered value.
   */
  public function set_cell($field_handler, $row_num, $new_values, $separator) {
    $rendered_value = FALSE;
    $field_name = $field_handler->options['id'];

    // The webform submission id comes in as views_handler_field_numeric, so all
    // we have to detect it is its name, i.e. 'sid'.
    $is_webform_value = ($field_name == 'sid') || is_a($field_handler, 'webform_handler_field_submission_data');

    // Depending on the aggregation function applied, default rendering may be
    // inappropriate. For instance "Trains (4)" cannot be rendered numerically.
    if ($is_renderable = $this->is_renderable($field_name, FALSE)) {

      if ($is_webform_value) {
        $rendered_value = $this->render_new_webform_value($field_handler, $row_num, $new_values, $separator);
      }
      elseif (is_a($field_handler, 'views_php_handler_field')) {
        // This prevents Views PHP from re-rendering the code snippet and makes
        // it pick up the value from $result_row.
        $field_handler->options['php_output'] = FALSE;
      }
      else {
        $rendered_value = $this->render_new_value($field_handler, $row_num, $new_values, $separator);
      }
    }
    elseif ($is_webform_value) {
      $rendered_value = $new_values;
    }
    if ($rendered_value === FALSE && !$is_webform_value) {
      $rendered_value = is_array($new_values) ? implode($separator, $new_values) : $new_values;
    }
    return $this->rendered_fields[$row_num][$field_name] = $rendered_value;
  }

  /**
   * Returns the rendered value for a new (raw) value of a table cell.
   *
   * @param object $field_handler
   *   The handler associated with the field/table-column being requested.
   * @param int $row_num
   *   The result row number.
   * @param mixed $new_values
   *   The raw value or array of raw values to render.
   * @param string $separator
   *   Separator to use between rendered values, when $new_values is an array.
   *
   * @return mixed
   *   The rendered new value or FALSE if the value could not be rendered.
   */
  protected function render_new_value($field_handler, $row_num, $new_values, $separator) {
    $new_values = is_array($new_values) ? $new_values : array($new_values);
    // If the field_handler belongs to an entity Field (as in the field module),
    // then we call render_from_raw(), which uses the attached parent entity to
    // render the field, which at some point will involve a call to
    // field_view_field($entity...).
    // Other field_handlers (e.g. Math Expressions) don't have the same data
    // structures attached --they are Views fields, but not core Fields-- so
    // require a different approach using format_numeric().
    $rendered_values = array();
    foreach ($new_values as $new_value) {
      if ($this->is_standard_field($field_handler)) {
        $rendered_values[] = $this->render_from_raw($field_handler, $row_num, $new_value);
      }
      elseif ($this->is_commerce_currency_amount($field_handler)) {
        $rendered_values[] = $this->render_from_raw_scalar($field_handler, $row_num, $new_value);
      }
      else {
        // If $new_value is not a number, this tends to return it verbatim.
        $rendered_values[] = $this->format_numeric($field_handler, $new_value);
      }
    }
    $rendered_value = implode(empty($separator) ? ' - ' : $separator, $rendered_values);
    return is_array($rendered_value) ? drupal_render($rendered_value) : $rendered_value;
  }

  /**
   * Returns whether the supplied field is a standard Views field.
   *
   * @param object $field_handler
   *   The views_handler_field_field object belonging to the View result field
   *
   * @return bool
   */
  protected function is_standard_field($field_handler) {
    return is_a($field_handler, 'views_handler_field_field');
  }
  
  protected function is_commerce_currency_amount($field_handler) {
    return !empty($field_handler->aliases['currency_code']);
  }

  /**
   * Render a Commerce amount passed in cents, formatted with currency.
   *
   * The field will be rendered with appropriate CSS classes, without label.
   *
   * @param object $field_handler
   *   The views_handler_field_field object belonging to the View result field
   * @param int $row_num
   *   The view result row number to change; use NULL if you do not wish to
   *   affect the view but just render the raw_value.
   * @param int $raw_value
   *   Amount in cents
   *   If NULL the row value of the field is re-rendered using its current
   *   (raw) value.
   */
  protected function render_from_raw_scalar($field_handler, $row_num, $raw_value) {
    $affect_view = isset($row_num);
    $row_num = (int) $row_num;
    $row = &$field_handler->view->result[$row_num];
    if (isset($raw_value)) {
      $field_alias = $field_handler->field_alias;
      $orig_value = $row->$field_alias;
      $row->$field_alias = $raw_value;
    }
    $rendered_value = $field_handler->render($row);
    if (!$affect_view && isset($orig_value)) {
      $row->$field_alias = $orig_value;
    }
    return $rendered_value;
  }

  /**
   * Returns the rendered representation for a new webform value.
   *
   * @param object $field_handler
   *   The webform handler associated with the field/table-column being requested.
   * @param int $row_num
   *   The result row number.
   * @param array $new_values
   *   The raw value(s) to render using the webform's rounding, prefix, suffix.
   * @param string $separator
   *   Separator to use between rendered values, when $new_values is an array.
   *
   * @return string
   *   The rendered value.
   */
  protected function render_new_webform_value($field_handler, $row_num, $new_values, $separator) {
    $result_row = $field_handler->view->result[$row_num];
    $nid = $field_handler->options['webform_nid'];
    $cid = $field_handler->options['webform_cid'];
    // Need to overwrite the submitted value on the _webform_submissions array
    // before rendering it, adding rounding, prefix, suffix.
    $submission = $field_handler->view->_webform_submissions[$nid][$result_row->sid];
    $rendered_values = array();
    $new_values = is_array($new_values) ? $new_values : array($new_values);
    foreach ($new_values as $new_value) {
      $submission->data[$cid][$row_num] = $new_value;
      $rendered = trim($field_handler->advanced_render($result_row));
      $rendered_values[] = empty($rendered) ? $new_value : $rendered;
    }
    $rendered_value = implode(empty($separator) ? ' - ' : $separator, $rendered_values);
    return is_array($rendered_value) ? drupal_render($rendered_value) : $rendered_value;
  }

  /**
   * Format a raw numeric value according to the supplied handler settings.
   *
   * @param object $field_handler
   * @param double $raw_value
   *
   * @return string, number formatted according to Views handler settings
   *
   * Note: this was taken in part from views_handler_field_math::render($values)
   */
  protected function format_numeric($field_handler, $raw_value) {
    if (!empty($field_handler->options['set_precision'])) {
      $value = number_format($raw_value, $field_handler->options['precision'], $field_handler->options['decimal'], $field_handler->options['separator']);
    }
    elseif (isset($field_handler->options['separator'])) {
      $remainder = abs($raw_value) - intval(abs($raw_value));
      $value = $raw_value > 0 ? floor($raw_value) : ceil($raw_value);
      $value = number_format($value, 0, '', $field_handler->options['separator']);
      if ($remainder && isset($field_handler->options['decimal'])) {
        // Note: substr may not be locale safe.
        $value .= $field_handler->options['decimal'] . substr($remainder, 2);
      }
    }
    elseif (is_float($raw_value)) {
      $precision = isset($this->options['column_aggregation']['precision'])
        ? (int) $this->options['column_aggregation']['precision']
        : (int) variable_get('views_aggregator_def_precision', 2);
      $decimal = variable_get('views_aggregator_def_decimal'); // '.'
      $separator = variable_get('views_aggregator_def_separator'); // ','
      $value = number_format($raw_value, $precision, $decimal, $separator);
    }
    else {
      $value = $raw_value;
    }
    // Check to see if hiding should happen.
    if ($field_handler->options['hide_empty'] && empty($value) && ($value !== 0 || $field_handler->options['empty_zero'])) {
      return '';
    }
    // Should we format as a plural?
    if (!empty($field_handler->options['format_plural']) && ($value != 0 || !$field_handler->options['empty_zero'])) {
      $value = format_plural($value, $field_handler->options['format_plural_singular'], $field_handler->options['format_plural_plural']);
    }
    if (empty($value)) {
      return '';
    }
    $prefix = isset($field_handler->options['prefix']) ? $field_handler->options['prefix'] : '';
    $suffix = isset($field_handler->options['suffix']) ? $field_handler->options['suffix'] : '';
    return $field_handler->sanitize_value($prefix . $value . $suffix, 'xss_admin');
  }

  /**
   * Render a field.module field from a raw value.
   *
   * The field will be rendered with appropriate CSS classes, without label.
   *
   * @param object $field_handler
   *   The views_handler_field_field object belonging to the View result field
   * @param int $row_num
   *   The view result row number to change. Pass NULL to simply render
   *   $raw_value outside the context of a View, without affecting any rows.
   * @param mixed $raw_value
   *   Compound or simple value. 
   *   If NULL the row value of the field is re-rendered using its current
   *   (raw) value.
   *
   * @return string
   *   The rendered value or FALSE, if the type of field is not supported.
   *
   * NB: This is messy code. The lengths we have to go through for this are
   * ridiculous. Patches welcome!
   *
   * The way it currently works is to set the desired $raw_value on the
   * associated entity and then render it via
   *   set_items(), when a row_num is provided to write the value to
   *   field_view_field(), otherwise
   * set_items() internally also calls field_view_field()
   */
  protected function render_from_raw($field_handler, $row_num = NULL, $raw_value = NULL) {
    $field_name = $real_field_name = $field_handler->options['id'];
    $field_alias = $field_handler->field_alias;
    $row = isset($row_num) ? $field_handler->view->result[$row_num] : reset($field_handler->view->result);
    $affect_view = isset($row_num);
    // _field_data[] contains the entities we'll be rendering from/to
    if (!$row || empty($row->_field_data[$field_alias])) {
      // This happens for ViewsPHP fields and for Math Expressions.
      return !$row || $affect_view ? FALSE : (isset($raw_value) ? $raw_value : '?');
    }
    // Note that when a 2nd copy of a field is used in the View, e.g.
    // field_price_1, we refer back to the base field name, i.e. field_price
    $last_underscore = strrpos($real_field_name, '_');
    if ((int)drupal_substr($real_field_name, $last_underscore + 1)) {
      $field_name = drupal_substr($real_field_name, 0, $last_underscore);
    }
    $_field_data = $row->_field_data[$field_alias];
    if ($this->has_no_suitable_renderer($_field_data, $field_name)) {
      // E.g. when the $field_handler refers to a node property (rather than
      // a field) that does not have a renderer.
      return $affect_view ? ($_field_data['entity']->{$field_name} = $raw_value) : $raw_value;
    }
    // Clone entity if we don't want to affect the current View results or if we
    // have multiple displays.
    $entity = $affect_view ? $_field_data['entity'] : clone $_field_data['entity'];
    $entity_type = $_field_data['entity_type'];

    $lang = is_a($field_handler, 'views_handler_field_field') ? $field_handler->field_language($entity_type, $entity) : $entity->language;
    if (isset($raw_value)) {
      // Only supporting values of 1 item, at index 0.
      if (is_array($raw_value)) {
        $entity->{$field_name}[$lang][0] = $raw_value;
      }
      elseif (isset($entity->{$field_name})) {
        if (empty($entity->{$field_name})) {
          $current_value = NULL;
          $key = 'value';
        }
        else {
          $current_value = reset($entity->{$field_name}[$lang][0]);
          $key = key($entity->{$field_name}[$lang][0]);
          // Cannot override 'tid' with non-numeric value. But like 'value',
          // 'tid' may be set in case of min, max, most frequent etc.
          // 'amount' is to allow the setting of Drupal Commerce prices.
          if ($affect_view && $key != 'value' && $key != 'amount' && !($key == 'tid' && is_numeric($raw_value))) {
            return FALSE;
          }
        }
        $entity->{$field_name}[$lang][0][$key] = $raw_value;
      }
      if ($affect_view) {
        // Next employ set_items() to re-render the $entity updated above.
        // set_items() calls field_view_field() to render the value, applying
        // rounding etc.
        // It returns an array with raw and rendered components.
        $raw_plus_rendered = $field_handler->set_items($row, $row_num);
        // Now set the current value back on the entity in case we have multiple
        // displays, all drawing from the same entity.
        if (isset($key)) {
          $entity->{$field_name}[$lang][0][$key] = $current_value;
        }
        // The final step is to theme the rendered values. This includes
        // token replacement and template theming.
        // theme() calls $field_handler->advanced_render($row);
        $row->{'field_' . $real_field_name} = $raw_plus_rendered;
        return $field_handler->theme($row);
      }
    }
    // If we can't affect the View result or $raw_value isn't set, we use the
    // Field API. The Field label is not rendered.
    $display = array(
      'type' => $field_handler->options['type'],
      'settings' => $field_handler->options['settings'],
      'label' => 'hidden',
    );
    $render_array = field_view_field($entity_type, $entity, $field_name, $display, $lang);
    return drupal_render($render_array);
  }

  protected function has_no_suitable_renderer($field_data, $field_name) {
    if (!isset($field_data['entity']->{$field_name})) {
      return TRUE;
    }
    return is_scalar($field_data['entity']->{$field_name});
  }

  /**
   * Write the aggregated results back into the View's rendered results.
   *
   * @param array $groups
   *   an array of groups, indexed by group name
   * @param array $values
   *   an array of value arrays, indexed by field name first and group second
   */
  protected function set_aggregated_group_values($groups, $values) {
    $field_handlers = $this->view->field;
    foreach ($this->options['info'] as $field_name => $options) {
      foreach ($groups as $group => $rows) {
        if ($group != 'column' && isset($values[$field_name][$group])) {
          foreach ($rows as $num => $row) {
            $separator = $this->options['info'][$field_name]['aggr_par'];
            $this->set_cell($field_handlers[$field_name], $num, $values[$field_name][$group], $separator);
            // Only need to set on the first member of the group.
            break;
          }
        }
      }
    }
  }

  /**
   * Write the aggregated results back into the View results totals (footer).
   *
   * @param array $values
   *   an array of field value arrays, indexed by field name and 'column'
   */
  protected function set_totals_row($values) {
    $totals = array();
    foreach ($values as $field_name => $group) {
      if (!empty($this->options['info'][$field_name]['has_aggr_column']) && isset($group['column'])) {
        $total = $group['column'];
        if ($this->is_renderable($field_name, TRUE)) {
          $field_handler = $this->view->field[$field_name];
          $is_webform_value = is_a($field_handler, 'webform_handler_field_submission_data');
          // This is to make render_text() work properly.
          $field_handler->original_value = $total;
          $separator = $this->options['info'][$field_name]['aggr_par_column'];
          $totals[$field_name] = $is_webform_value
            ? $this->render_new_webform_value($field_handler, 0, $total, $separator)
            : $this->render_new_value($field_handler, NULL, $total, $separator);
        }
        else {
          $totals[$field_name] = $total;
        }
      }
    }
    return $totals;
  }

  /**
   * Returns if the supplied field is renderable through its native function.
   * 
   * @param string $field_name
   * @param bool $is_column
   *
   * @return bool
   */
  public function is_renderable($field_name, $is_column = FALSE) {
    if (empty($this->options['info'][$field_name][$is_column ? 'has_aggr_column' : 'has_aggr'])) {
      return TRUE;
    }
    $aggr_functions = $this->options['info'][$field_name][$is_column ? 'aggr_column' : 'aggr'];
    $aggr_function = is_array($aggr_functions) ? end($aggr_functions) : $aggr_functions;
    $aggr_function_info = views_aggregator_get_aggregation_functions_info($aggr_function);
    
    // Aggregation functions are considered renderable unless set to FALSE.
    return !isset($aggr_function_info['is_renderable']) || !empty($aggr_function_info['is_renderable']);
  }

  /**
   * Records the "active" field, i.e. the column clicked to be sorted.
   *
   * Also records the sort order ('asc' or 'desc').
   * This is identical to views_plugin_style_table::build_sort_post(), except
   * for the last statement, which has a condition added.
   */
  public function build_sort_post() {
    if (!isset($_GET['order'])) {
      // Check for a 'default' clicksort. If there isn't one, exit gracefully.
      if (empty($this->options['default'])) {
        return;
      }
      $sort = $this->options['default'];
      if (!empty($this->options['info'][$sort]['default_sort_order'])) {
        $this->order = $this->options['info'][$sort]['default_sort_order'];
      }
      else {
        $this->order = !empty($this->options['order']) ? $this->options['order'] : 'asc';
      }
    }
    else {
      $sort = $_GET['order'];
      // Store the $order for later use.
      $this->order = !empty($_GET['sort']) ? strtolower($_GET['sort']) : 'asc';
    }
    // If a sort we don't know about gets through, exit gracefully.
    if (empty($this->view->field[$sort])) {
      return;
    }
    // Ensure $this->order is valid.
    if ($this->order != 'asc' && $this->order != 'desc') {
      $this->order = 'asc';
    }
    // Store the $sort and sortable flag for later use.
    $this->active = $sort;
    //$this->sortable = $this->options['info'][$sort]['sortable'];

    // Tell the field to click-sort, but only if it is not a Math Expression or
    // a field not aggregated, in which cases sorting will be dealt with in
    // $this->pre_render().
    // This is here predominantly to avoid notices from ViewsPHP, but also
    // makes normal column sorting more efficient by not adding any unnecessary
    // WHERE-clauses, if paging is OFF.
    // @todo Refine this logic
    if (!is_a($this->view->field[$sort], 'views_handler_field_math')
      /* && empty($this->options['info'][$sort]['has_aggr'])*/) {
      $this->view->field[$sort]->click_sort($this->order);
    }
  }

  /**
   * Compare function for aggregated groups, for use in sorting functions.
   *
   * @param array $row1
   *   The first aggregated group of result rows.
   * @param array $row2
   *   The second aggregated group of result rows.
   *
   * @return int
   *   The compare code indicating whether $row1 is smaller than (-1), equal
   *   to (0) or greater than (1) $row2.
   */
  protected function compare_result_rows($row1, $row2) {
    // The sorting data may be raw or rendered, while the sorting style may be
    // alphabetical or numeric.
    //
    // Columns that need to be sorted using raw values:
    // o numbers and moneys, so that "$1,000" comes AFTER "$9.99" (ascending)
    // o dates and date ranges (@todo)
    //
    // Columns that need to be sorted using rendered, post-aggregated values:
    // o Views PHP expressions, addresses, taxonomy terms

    $field_handler = $this->view->field[$this->active];
    $field_type = isset($field_handler->field_info['type']) ? $field_handler->field_info['type'] : '';

    // AddressFields, taxonomy terms and Views PHP expressions are compared in
    // rendered format.
    $compare_rendered =
      ($field_type == 'addressfield') ||
      ($field_type == 'taxonomy_term_reference') ||
      is_a($field_handler, 'views_php_handler_field');

    // Get the cells from the raw or rendered fields.
    // Note that raw data may contain HTML tags too, so always strip.
    $cell1 = strip_tags($this->get_cell($field_handler, $row1->num, $compare_rendered));
    $cell2 = strip_tags($this->get_cell($field_handler, $row2->num, $compare_rendered));

    if ((double)$cell1 == (double)$cell2) {
      // If both cells cast to zero, then compare alphabetically.
      $compare = ($cell1 == $cell2) ? 0 : ($cell1 < $cell2 ? -1 : 1);
    }
    else {
      // Compare numerically, i.e. "20 km" comes after "9.5 km".
      // The double cast causes a read up to the first non-number related char.
      $compare = (double)$cell1 < (double)$cell2 ? -1 : 1;
    }
    return ($this->order == 'asc') ? $compare : -$compare;
  }

  /**
   * Strips the pager off an existing View, then executes and renders it.
   *
   * The View is rebuilt from scratch, without the extra pager query. The View
   * passed in as an argument is in no way affected.
   *
   * @param object $view_with_pager
   *   a View object
   *
   * @param object $display_id
   *   the display to execute, for example 'default', 'page', 'block'
   *
   * @return object
   *   the pageless View, including the complete rendered results set on
   *   $view->style_plugin->rendered_fields
   */
  protected function execute_view_without_pager($view_with_pager, $display_id = NULL) {
    // Apply the filters so the results reflect accurately.
    $this->apply_exposed_filters($view_with_pager);

    $clone = $view_with_pager->clone_view();
    $id = empty($clone->display[$display_id]->display_options['pager']) ? 'default' : $display_id;
    $clone->display[$id]->display_options['pager']['type'] = 'none';
    
    $clone->is_temp_views_aggregator = TRUE;
    $clone->execute($display_id);
    return $clone;
/*
    //  Alternative code below is based on view::copy()
    $code = $view_with_pager->export();
    $code_without_pagers = str_replace(
      "display_options['pager']['type'] = ",
      "display_options['pager']['type'] = 'none'; //", $code);

    // This statement creates a View object by the name of $view.
    eval($code_without_pagers);

    // [#2213417], to avoid notice.
    $view->dom_id = $view_with_pager->dom_id;

    // Avoid recursion in views_aggregator_plugin_style_table::pre_render().
    $view->is_temp_views_aggregator = TRUE;

    // As this is a copy, let's keep caching behaviour the same as the original.

    // Execute the display.
    $view->execute($display_id); // may have to be: $view->render($display_id);
    return $view;
*/
  }

  /**
   * Apply the exposed filters to the view so we get the correct result set.
   *
   * @param object $view
   *   A view object.
   */
  protected function apply_exposed_filters(&$view) {
    $filters = $view->display_handler->display->handler->handlers['filter'];
    // Loop through the filters and UNexpose them.
    foreach ($filters as $filter_handler) {
      $filter_handler->options['exposed'] = FALSE;
    }
  }

}
