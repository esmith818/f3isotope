<?php
/**
 * @file
 * LikeBtn like button.
 */

define('LIKEBTN_LAST_SUCCESSFULL_SYNC_TIME_OFFSET', 57600);
define('LIKEBTN_LOCALES_SYNC_INTERVAL', 57600);
define('LIKEBTN_STYLES_SYNC_INTERVAL', 57600);
define('LIKEBTN_API_URL', 'http://api.likebtn.com/api/');

class LikeBtn {

  protected static $synchronized = FALSE;
  // Cached API request URL.
  protected static $apiurl = '';

  /**
   * Constructor.
   */
  public function __construct() {
    // Do nothing.
  }

  /**
   * Running votes synchronization.
   */
  public function runSyncVotes() {
    if (!self::$synchronized && variable_get('likebtn_account_data_email') && variable_get('likebtn_account_data_api_key') && $this->timeToSyncVotes(variable_get('likebtn_sync_inerval', 60) * 60) && function_exists('curl_init')) {
      $this->syncVotes(variable_get('likebtn_account_data_email'), variable_get('likebtn_account_data_api_key'));
    }
  }

  /**
   * Check if it is time to sync votes.
   */
  public function timeToSyncVotes($sync_period) {

    $last_sync_time = variable_get('likebtn_last_sync_time', 0);

    $now = time();
    if (!$last_sync_time) {
      variable_set('likebtn_last_sync_time', $now);
      self::$synchronized = TRUE;
      return TRUE;
    }
    else {
      if ($last_sync_time + $sync_period > $now) {
        return FALSE;
      }
      else {
        variable_set('likebtn_last_sync_time', $now);
        self::$synchronized = TRUE;
        return TRUE;
      }
    }
  }

  /**
   * Retrieve data.
   */
  public function curl($url) {

    $path = drupal_get_path('module', 'likebtn') . '/likebtn.info';
    $info = drupal_parse_info_file($path);
    $drupal_version = VERSION;
    $likebtn_version = $info["core"];
    $php_version = phpversion();
    $useragent = "Drupal $drupal_version; likebtn module $likebtn_version; PHP $php_version";

    try {
      $ch = curl_init();
      curl_setopt($ch, CURLOPT_USERAGENT, $useragent);
      curl_setopt($ch, CURLOPT_URL, $url);
      curl_setopt($ch, CURLOPT_TIMEOUT, 60);
      curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
      curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
      $result = curl_exec($ch);
      curl_close($ch);
    }
    catch(Exception $e) {

    }

    return $result;
  }

  /**
   * Comment sync function.
   */
  public function syncVotes($account_api_key, $site_api_key) {
    $sync_result = TRUE;

    $last_sync_time = number_format(variable_get('likebtn_last_sync_time', 0), 0, '', '');

    $updated_after = '';

    if (variable_get('likebtn_last_successfull_sync_time', 0)) {
      $updated_after = variable_get('likebtn_last_successfull_sync_time') - LIKEBTN_LAST_SUCCESSFULL_SYNC_TIME_OFFSET;
    }

    $url = "output=json&last_sync_time=" . $last_sync_time;
    if ($updated_after) {
      $url .= '&updated_after=' . $updated_after;
    }

    // Retrieve first page.
    $response = $this->apiRequest('stat', $url);
    if (!$this->updateVotes($response)) {
      $sync_result = FALSE;
    }

    // Retrieve all pages.
    if (isset($response['response']['total']) && isset($response['response']['page_size'])) {
      $total_pages = ceil((int) $response['response']['total'] / (int) $response['response']['page_size']);

      for ($page = 2; $page <= $total_pages; $page++) {
        $response = $this->apiRequest('stat', $url . '&page=' . $page);

        if (!$this->updateVotes($response)) {
          $sync_result = FALSE;
        }
      }
    }

    if ($sync_result) {
      variable_set('likebtn_last_successfull_sync_time', $last_sync_time);
    }
  }

  /**
   * Test synchronization.
   */
  public function testSync($email, $api_key) {
    $email = trim($email);
    $api_key = trim($api_key);

    $response = $this->apiRequest('stat', 'output=json&page_size=1', $email, $api_key);

    return $response;
  }

  /**
   * Decode JSON.
   */
  public function jsonDecode($jsong_string) {
    return json_decode($jsong_string, TRUE);
  }

  /**
   * Update votes in database from API response.
   */
  public function updateVotes($response) {
    $votes = array();

    if (!empty($response['response']['items'])) {
      foreach ($response['response']['items'] as $item) {

        $entity_type = '';
        $entity_id = '';
        $field_id = '';
        $field_index = 0;

        // Parse identifier.
        if (strstr($item['identifier'], '_field_')) {
          // Item is a field.
          preg_match("/^(.*)_(\d+)_field_(\d+)(?:_index_(\d+))?$/", $item['identifier'], $identifier_parts);

          if (!empty($identifier_parts[1])) {
            $entity_type = $identifier_parts[1];
          }
          else {
            continue;
          }

          if (!empty($identifier_parts[2])) {
            $entity_id = $identifier_parts[2];
          }
          else {
            continue;
          }

          if (!empty($identifier_parts[3])) {
            $field_id = $identifier_parts[3];
          }
          else {
            continue;
          }

          if (!empty($identifier_parts[4])) {
            $field_index = $identifier_parts[4];
          }
        }
        else {
          // Item is an entity.
          preg_match("/^(.*)_(\d+)$/", $item['identifier'], $identifier_parts);

          if (!empty($identifier_parts[1])) {
            $entity_type = $identifier_parts[1];
          }
          else {
            continue;
          }
          if (!empty($identifier_parts[2])) {
            $entity_id = $identifier_parts[2];
          }
          else {
            continue;
          }
        }

        $vote_source = LIKEBTN_VOTING_VOTE_SOURCE;
        if ($field_id) {
          $vote_source = 'field_' . $field_id . '_index_' . $field_index;
        }
        $likes = 0;
        if (!empty($item['likes'])) {
          $likes = $item['likes'];
        }
        $dislikes = 0;
        if (!empty($item['dislikes'])) {
          $dislikes = $item['dislikes'];
        }

        // If vote for this entity/field has been already stored - continue.
        foreach ($votes as $vote) {
          if ($vote['entity_type'] == $entity_type && $vote['entity_id'] == $entity_id && $vote['vote_source'] == $vote_source) {
            continue 2;
          }
        }

        // Get entity info.
        try {
          $entity_type_info = entity_get_info($entity_type);
          if (empty($entity_type_info['controller class'])) {
            continue;
          }
        }
        catch(Exception $e) {
          continue;
        }

        // Likes and Disliked stored in Voting API.
        $votes[] = array(
          'entity_type' => $entity_type,
          'entity_id'   => $entity_id,
          'value_type'  => 'points',
          'value'       => $likes,
          'tag'         => LIKEBTN_VOTING_TAG,
          'uid'         => 0,
          'vote_source' => $vote_source,
        );
        $votes[] = array(
          'entity_type' => $entity_type,
          'entity_id'   => $entity_id,
          'value_type'  => 'points',
          'value'       => $dislikes * (-1),
          'tag'         => LIKEBTN_VOTING_TAG,
          'uid'         => 0,
          'vote_source' => $vote_source,
        );

        // Remove (backup) votes cast on this entity by other modules.
        /*$remove_old_votes_fields = array(
        'entity_type' => $entity_type . '_backup',
        );
        try {
        db_update('votingapi_vote')
        ->fields($remove_old_votes_fields)
        ->condition('entity_type', $entity_type)
        ->condition('vote_source', array('like', 'dislike'), 'NOT IN')
        ->execute();
        }
        catch (Exception $e) {}
        */

        // Remove votes cast on this entity by the previous version
        // of the plugin, when vote_source was ''
        /*try {
        db_delete('votingapi_vote')
        ->condition('entity_type', $entity_type)
        ->condition('entity_id', $entity_id)
        ->condition('tag', 'vote')
        ->condition('vote_source', '')
        ->execute();
        }
        catch (Exception $e) {

        }
        */

        // Update LikeBtn fields.
        if ($vote_source) {
          $entities = entity_load($entity_type, array($entity_id));
          if (empty($entities[$entity_id])) {
            continue;
          }
          $entity   = $entities[$entity_id];
          list($tmp_entity_id, $entity_revision_id, $bundle) = entity_extract_ids($entity_type, $entity);

          // Get entity LikeBtn fields.
          $entity_fields = field_info_instances($entity_type, $bundle);

          // Set field value.
          $likes_minus_dislikes = $likes - $dislikes;

          foreach ($entity_fields as $field_name => $field_info) {
            if ($field_info['widget']['module'] != 'likebtn') {
              continue;
            }

            $field_fields_data = array(
              'entity_type'         => $entity_type,
              'bundle'              => $bundle,
              'entity_id'           => $entity_id,
              'revision_id'         => $entity_id,
              'delta'               => $field_index,
              'language'            => isset($entity->language) ? $entity->language : LANGUAGE_NONE,
            );
            $field_fields_data[$field_name . '_likebtn_likes']        = $likes;
            $field_fields_data[$field_name . '_likebtn_dislikes']     = $dislikes;
            $field_fields_data[$field_name . '_likebtn_likes_minus_dislikes'] = $likes_minus_dislikes;

            try {
              // Insert value.
              db_insert('field_data_' . $field_name)
                ->fields($field_fields_data)
                ->execute();
            }
            catch(Exception $e) {
              // Update value.
              try {
                db_update('field_data_' . $field_name)
                  ->fields($field_fields_data)
                  ->condition('entity_type', $entity_type)
                  ->condition('bundle', $bundle)
                  ->condition('entity_id', $entity_id)
                  ->execute();
              }
              catch(Exception $e) {

              }
            }
          }
        }
      }

      if ($votes) {
        // Prepare criteria for removing previous vote values.
        $criteria = array();
        foreach ($votes as $vote) {
          $criteria[] = array(
            'entity_type' => $vote['entity_type'],
            'entity_id'   => $vote['entity_id'],
            'value_type'  => $vote['value_type'],
            'tag'         => $vote['tag'],
            'uid'         => $vote['uid'],
            'vote_source' => $vote['vote_source'],
          );
        }
        // Votes must be saved altogether.
        votingapi_set_votes($votes, $criteria);
        return TRUE;
      }
      return FALSE;
    }
  }

  /**
   * Run locales synchronization.
   */
  public function runSyncLocales() {
    if ($this->timeToSync(LIKEBTN_LOCALES_SYNC_INTERVAL, 'likebtn_last_locale_sync_time') && function_exists('curl_init')) {
      $this->syncLocales();
    }
  }

  /**
   * Run styles synchronization.
   */
  public function runSyncStyles() {
    if ($this->timeToSync(LIKEBTN_STYLES_SYNC_INTERVAL, 'likebtn_last_style_sync_time') && function_exists('curl_init')) {
      $this->syncStyles();
    }
  }

  /**
   * Check if it is time to sync locales.
   */
  public function timeToSync($sync_period, $sync_variable) {

    $last_sync_time = variable_get($sync_variable, 0);

    $now = time();
    if (!$last_sync_time) {
      variable_set($sync_variable, $now);
      return TRUE;
    }
    else {
      if ($last_sync_time + $sync_period > $now) {
        return FALSE;
      }
      else {
        variable_set($sync_variable, $now);
        return TRUE;
      }
    }
  }

  /**
   * Locales sync function.
   */
  public function syncLocales() {
    $url = LIKEBTN_API_URL . "?action=locale";

    $response_string = $this->curl($url);
    $response = $this->jsonDecode($response_string);

    if (isset($response['result']) && $response['result'] == 'success' && isset($response['response']) && count($response['response'])) {
      variable_set('likebtn_locales', $response['response']);
    }
  }

  /**
   * Styles sync function.
   */
  public function syncStyles() {
    $url = LIKEBTN_API_URL . "?action=style";

    $response_string = $this->curl($url);
    $response = $this->jsonDecode($response_string);

    if (isset($response['result']) && $response['result'] == 'success' && isset($response['response']) && count($response['response'])) {
      variable_set('likebtn_styles', $response['response']);
    }
  }

  /**
   * Request to API.
   */
  public function apiRequest($action, $request, $email = '', $api_key = '') {
    if (!self::$apiurl) {
      if (!$email) {
        $email  = trim(variable_get('likebtn_account_data_email'));
      }
      if (!$api_key) {
        $api_key = trim(variable_get('likebtn_account_data_api_key'));
      }
      $subdirectory = trim(variable_get('likebtn_settings_subdirectory'));
      $local_domain = trim(variable_get('likebtn_settings_local_domain'));
      if ($local_domain) {
        $domain = $local_domain;
      }
      else {
        $parse_url    = parse_url(url(NULL, array('absolute' => TRUE)));
        $domain       = $parse_url['host'] . $subdirectory;
      }

      self::$apiurl = LIKEBTN_API_URL . "?email={$email}&api_key={$api_key}&domain={$domain}&nocache=.php&source=drupal&";
    }
    $url = self::$apiurl . "action={$action}&" . $request;

    $response_string = $this->curl($url);

    $response = $this->jsonDecode($response_string);

    return $response;
  }

}
