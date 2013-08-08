<?php
/**
* @file
* Helper functions for getting/setting data about agents, goals, and other data for the Conductrics module
*/

function conductrics_mvp_goals_get() {
  $goals = variable_get('conductrics_goals');
  if (!isset($goals) || count($goals) == 0) {
    $goals = array(
      'goal-1' => array(
        'agents' => array(),
        'goal_mode' => 'page-load',
        'urls' => array('*'),
        'value' => 1,
      ),
    );
  }
  return $goals;
}
function conductrics_mvp_goals_set($goals) {
  variable_set('conductrics_goals', $goals);
}
function conductrics_mvp_goal_update($goal_code, $data) {
  $goals = conductrics_mvp_goals_get();
  if ( !isset($goals[$goal_code]) ) {
    $goals[$goal_code] = array(
      'agents' => array(), // maintained via conductrics_mvp_goals_associate
    );
  }
  foreach ($data as $key => $value) {
    if ($key != 'agents') {
      $goals[$goal_code][$key] = $data[$key];
    }
  }
  conductrics_mvp_goals_set($goals);
}
function conductrics_mvp_goal_remove($goal_code) {
  $goals = conductrics_mvp_goals_get();
  unset($goals[$goal_code]);
  conductrics_mvp_goals_set($goals);
}
function conductrics_mvp_goals_associate($agent_code, $goal_selections) {
  $goals = conductrics_mvp_goals_get();
  foreach ($goals as $goal_code => &$goal_item) {
    if ( isset($goal_selections[$goal_code]) && $goal_selections[$goal_code] ) { // user wants an association
      if ( !in_array($agent_code, $goal_item['agents']) ) { // and there is no association now
        array_push($goal_item['agents'], $agent_code);
      }
    } else { // user doesn't want an association
      if ( in_array($agent_code, $goal_item['agents']) ) { // and there is an association now
        $offset = array_search($agent_code, $goal_item['agents']);
        array_splice($goal_item['agents'], $offset, 1);
      }
    }
  }
  conductrics_mvp_goals_set($goals);
}

function conductrics_mvp_get_goals_relevant_here($path) {
    $goals = conductrics_mvp_goals_get();
    $relevant_here = array();
    foreach ($goals as $goal_code => $goal_item) {
      if ($goal_item['goal_mode'] == 'page-load') {
        foreach ($goal_item['urls'] as $url) {
          if ( fnmatch($url, $path) ) {
            $relevant_here[$goal_code] = $goal_item;
          }
        }
      }
    }
    return $relevant_here;
}

function conductrics_mvp_get_feature_values_for_visitor() {
  // Which features, if any, should be specified in 'data-conductrics-features' attribute of the page's body element?
  $features = array(); // will be an array of feature codes to pass to conductrics
  $account_info = variable_get('conductrics_account_info');
  if (isset($account_info['tokens'])) {
    foreach ($account_info['tokens'] as $token_code => $token_item) { // for each token that has been configured as passable to Conductrics
      // assume $token_code is a string in normal Drupal-style token format, for example: '[current-user:field-sport]'
      $token_value = token_replace($token_code); // will now be the value of the token, or the value of $token_code if no replacement took place (unknown token)
      if (!empty($token_value) && $token_value != $token_code) { // the token as a value
        $prefix_with = empty($token_item['prefix']) ? '' : ($token_item['prefix'] . '-'); // The 'prefix' at this level is a cheap way to namespace the values that get returned by the tokens
        foreach ( explode(',', $token_value) as $part) { // we might have a comma-separated list of values, as in "Football, Baseball, Soccer"
          $feature_value = $prefix_with . trim($part);
          if (conductrics_mvp_code_valid($feature_value)) {
            array_push($features, $feature_value);
          }
        }
      }
    }
  }
  return $features;
}

function conductrics_mvp_blocks_matching($criteria) {
  $theme = 'demonstratie'; // TODO - pass in somehow
  $results = array();
  $blocks = _block_rehash($theme);
  foreach ($blocks as $block) {
    $module = $block['module'];
    $delta = $block['delta'];
    $identifier = "$module:$delta";
    if ($criteria == '' or stristr($module, $criteria) or stristr($delta, $criteria) ) {
      $results[$identifier] = "Block '$delta' from module '$module'";
    }
  }
  return $results;
}
function conductrics_mvp_str_to_block_data($str) {
  $parts = explode(':', $str);
  $block_data = array(
    'module' => $parts[0],
    'delta' => $parts[1],
  );
  return $block_data;
}
function conductrics_mvp_block_data_to_str($block_data) {
  if (isset($block_data['module']) && isset($block_data['delta'])) {
    return $block_data['module'] . ':' . $block_data['delta'];
  }
  return FALSE;
}
function conductrics_mvp_block_str_valid($str) {
  return preg_match('/^[0-9A-Za-z_-]+\:[0-9A-Za-z_-]+$/', $str);
}
function conductrics_mvp_code_valid($str) {
  return preg_match('/^[0-9A-Za-z_-]+$/', $str);
}
