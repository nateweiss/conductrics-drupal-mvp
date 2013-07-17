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
			'agent_code' => '',
			'selection_mode' => 'block-vis',
			'experiences' => array(),
		);
		return array_merge(parent::values(), $values);
	}

	/**
	* Builds extra settings for the block edit form.
	*/
	public function form($bean, $form, &$form_state) {
		$form = array();

		$form['selection_mode'] = array(
			'#type' => 'radios',
			'#default_value' => 'block-vis',
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
			'#title' => t('Block to Change'),
			'#states' => array(
				'visible' => array(
					':input[name="selection_mode"]' => array('value' => 'block-vis'),
				),
			),
		);

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

		$num_blocks = 4; // TODO - make this a form element or provide other user gesture for adding/removing "experiences"
		for ($num = 1; $num <= $num_blocks; $num++) {
			$is_required = ($num <= 2);
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
			);
		}

		$form['agent_code'] = array(
			'#type' => 'textfield',
			'#weight' => 2,
			'#title' => t('Conductrics Agent Code'),
			'#size' => 30,
			'#default_value' => $bean->agent_code,
			'#description' => 'unique code/name',
			'#required' => TRUE,
		);

		return $form;
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
							'agent_code' => $bean->agent_code,
							'choice_code' => $key,
						)
					);
				}
			}
		}
		return $content;
	}

}
