<?php
/**
 * @file
 * Bean theme templates for Conductrics "Agent Experience" children.
 */

/**
 * Define theme templates.
 */
function theme_agent_experience(&$variables) {
	$content = '<div class="conductrics-experience" data-conductrics-agent="'. $variables['agent_code'] .'" data-conductrics-choice="' . $variables['choice_code'] . '">';
	foreach ( $variables['entities'] as $display_bean) {
		$content .= theme('agent_experience_child',
			array(
				'entity' => $display_bean,
				'view_mode' => $variables['view_mode'],
			)
		);
	}
	$content .= '</div>';
	return $content;
}

function theme_agent_experience_child(&$variables) {
	$child_bean = bean_view($variables['entity'], $variables['view_mode']);
	return render($child_bean);
}

function theme_preprocess_page(&$vars, $hook) {
	drupal_add_js(drupal_get_path('theme', 'mytheme') . '/mytheme.js');
}
