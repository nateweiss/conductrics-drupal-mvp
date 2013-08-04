<?php
/**
* @file
* Helper functions for getting/setting data about agents, goals, and other data for the Conductrics module
*/

function conductrics_1_goals_get() {
  $goals = variable_get('conductrics_goals');
  if (!isset($goals) || count($goals) == 0) {
    $goals = array(
      'any-page' => array(
        'agents' => array(),
        'goal_mode' => 'page-load',
        'urls' => array('*'),
        'value' => 1,
      ),
    );
  }
  return $goals;
}
function conductrics_1_goals_set($goals) {
  variable_set('conductrics_goals', $goals);
}
function conductrics_1_goal_update($goal_code, $data) {
  $goals = conductrics_1_goals_get();
  if ( !isset($goals[$goal_code]) ) {
    $goals[$goal_code] = array(
      'agents' => array(), // maintained via conductrics_1_goals_associate
    );
  }
  foreach ($data as $key => $value) {
    if ($key != 'agents') {
      $goals[$goal_code][$key] = $data[$key];
    }
  }
  conductrics_1_goals_set($goals);
}
function conductrics_1_goal_remove($goal_code) {
  $goals = conductrics_1_goals_get();
  unset($goals[$goal_code]);
  conductrics_1_goals_set($goals);
}
function conductrics_1_goals_associate($agent_code, $goal_selections) {
  $goals = conductrics_1_goals_get();
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
  conductrics_1_goals_set($goals);
}

function conductrics_1_blocks_matching($criteria) {
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
function conductrics_1_str_to_block_data($str) {
  $parts = explode(':', $str);
  $block_data = array(
    'module' => $parts[0],
    'delta' => $parts[1],
  );
  return $block_data;
}
function conductrics_1_block_data_to_str($block_data) {
  if (isset($block_data['module']) && isset($block_data['delta'])) {
    return $block_data['module'] . ':' . $block_data['delta'];
  }
  return FALSE;
}
function conductrics_1_block_str_valid($str) {
  return preg_match('/^[0-9A-Za-z_-]+\:[0-9A-Za-z_-]+$/', $str);
}
function conductrics_1_code_valid($str) {
  return preg_match('/^[0-9A-Za-z_-]+$/', $str);
}
