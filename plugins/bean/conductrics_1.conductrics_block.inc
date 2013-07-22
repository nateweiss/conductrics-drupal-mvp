<?php
/**
* @file
* Tweet block bean plugin.
*/
class ConductricsBlockBean extends BeanPlugin {
	/**
	* Declares default block settings.
	*/
	public function values() {
		$values = array(
			'settings' => array(
				'agent_code' => '',
				'selection_mode' => 'block-vis',
			),
			'experiences' => array(),
		);
		return array_merge(parent::values(), $values);
	}

	/**
	* Builds extra settings for the block edit form.
	*/
	public function form($bean, $form, &$form_state) {
		$form = array();
		$num_blocks = 4; // TODO - make this a form element or provide other user gesture for adding/removing "experiences"

		$form['selection_mode'] = array(
			'#type' => 'radios',
			'#default_value' => $bean->settings['selection_mode'],
			'#options' => array(
				'block-vis' => t('&nbsp;<strong>Block Visibility</strong> - Show or hide one block'),
				'block-swap' => t('&nbsp;<strong>Block Selection</strong> - Show one of several blocks'),
			),
			'#title' => t('Type of Change'),
		);

		// Container fieldset for the "block visibility" selection use case
		$form['block_vis'] = array(
			'#type' => 'fieldset',
			'#tree' => 1,
			'#title' => t('Block to Show or Hide'),
			'#states' => array(
				'visible' => array(
					':input[name="selection_mode"]' => array('value' => 'block-vis'),
				),
			),
		);

		$default_value = isset($bean->experiences['block-visible']['blocks'][0]) ? conductrics_1_block_data_to_str($bean->experiences['block-visible']['blocks'][0]) : '';
		$form['block_vis']['block'] = array(
			'#type' => 'textfield',
			'#title' => t("Block"),
			'#states' => array(
				'required' => array(
					':input[name="selection_mode"]' => array('value' => 'block-vis'),
				),
			),
			'#size' => 30,
			'#autocomplete_path' => 'autocomplete/blocks',
			'#default_value' => $default_value,
		);

		// Container fieldset for the "block swap" selection use case
		$form['block_swap'] = array(
			'#type' => 'fieldset',
			'#tree' => 1,
			'#title' => t('Blocks'),
			'#states' => array(
				'visible' => array(
					':input[name="selection_mode"]' => array('value' => 'block-swap'),
				),
			),
		);

		for ($num = 1; $num <= $num_blocks; $num++) {
			$is_required = ($num <= 2);
			$default_value = isset($bean->experiences["block-$num"]['blocks'][0]) ? conductrics_1_block_data_to_str($bean->experiences["block-$num"]['blocks'][0]) : '';
			$form['block_swap']["block_$num"] = array(
				'#type' => 'textfield',
				'#title' => t("Block $num"),
				'#states' => array(
					'required' => array(
						':input[name="selection_mode"]' => $is_required ? array('value' => 'block-swap') : FALSE,
					),
				),
				'#size' => 30,
				'#autocomplete_path' => 'autocomplete/blocks',
				'#default_value' => $default_value,
			);
		}

		$form['agent_code'] = array(
			'#type' => 'textfield',
			'#title' => t('Conductrics Agent Code'),
			'#size' => 30,
			'#default_value' => $bean->settings['agent_code'],
			'#description' => 'unique code/name (letters, numbers, dashes, and underscores allowed - no spaces)',
			'#required' => TRUE,
		);


		$goal_options = array();
		$goal_items = conductrics_1_goals_get();
		$goals_selected = array();
		foreach ($goal_items as $goal_code => $goal_item) {
			if (!isset($goal_item['agents'])) {
				continue;
			};
			$goal_options[$goal_code] = t("&nbsp;<strong>$goal_code</strong> (value " . $goal_item['value'] . ')' ); // TODO - does drupal dogma call for theme function here?
			if ( in_array($bean->settings['agent_code'], $goal_item['agents']) ) {
				array_push($goals_selected, $goal_code);
			}
		}

		$form['goals_fieldset'] = array(
			'#type' => 'fieldset',
			'#title' => 'Goals',
			'#tree' => FALSE,
		);
		$form['goals_fieldset']['goals'] = array(
			'#type' => 'checkboxes',
			'#title' => t('Reward the Agent for these goals:'),
			'#required' => TRUE, // TODO - what if there aren't any goals to select?
			'#options' => $goal_options,
			'#default_value' => $goals_selected,
		);

		return $form;
	}

	public function validate($values, &$form_state) {
		$num_blocks = 4; // TODO - make this a form element or provide other user gesture for adding/removing "experiences"

		if ( !in_array($values['selection_mode'], array('block-vis', 'block-swap')) ) {
			form_set_error('selection_mode', t("Unknown selection_mode."));
		}
		if ( $values['selection_mode'] == 'block-vis' ) {
			$val = $values['block_vis']['block'];
			if ( !conductrics_1_block_str_valid($val) ) {
				form_set_error($values['block_vis']['block'], t("You need to select a Block to show or hide."));
			}
		}
		if ( $values['selection_mode'] == 'block-swap' ) {
			for ($num = 1; $num <= $num_blocks; $num++) {
				$val = $values['block_swap']["block_$num"];
				$is_required = $num <= 2;
				if ($is_required && empty($val)) {
					form_set_error("block_swap][block_$num", t('You must provide at least two blocks to select from.'));
					continue;
				}
				if ( !empty($val) && !conductrics_1_block_str_valid($val) ) {
					form_set_error("block_swap][block_$num", t("Invalid Block $num provided - please choose from one of the available blocks." . $val));
				}
			}
		}
		if ( !conductrics_1_code_valid($values['agent_code']) ) {
			form_set_error('agent_code', t('Invalid Agent Code - you can use letters, numbers, dashes, and underscores, but no spaces or other special characters.'));
		}
	}

	/**
	* Displays the bean.
	*/
	public function view($bean, $content, $view_mode = 'default', $langcode = NULL) {
		// Set markup index as empty.
		$content['#markup'] = '';
		// Loop through experiences and extract beans for display.
		if (isset($bean->experiences)) {
			foreach ($bean->experiences as $key => $experience) {
				if (isset($experience['blocks'])) {
					$content['#markup'] .= theme('agent_experience',
						array(
							'blocks' => $experience['blocks'],
							'view_mode' => $view_mode, # needed?
							'agent_code' => $bean->settings['agent_code'],
							'choice_code' => $key,
						)
					);
				}
			}
		}
		return $content;
	}

}
