(function( $ ) {

	var collectAgentsFromDataAttributes = function(selector) {
		var agents = {};
		$(selector).each(function() {
			var item = $(this);
			var agent = item.attr('data-agent');
			var choice = item.attr('data-choice');
			if (!agents[agent]) {
				agents[agent] = {choices:[]}
			}
			agents[agent].choices.push(choice);
		});
		return agents;
	}

	$(document).ready(function() {
		// initialize Conductrics plugin for jQuery
		$.conductrics(Drupal.settings['conductrics-jquery']);
		// get the elements that contain 'experiences' to select amongst
		var agents = collectAgentsFromDataAttributes('.conductrics-experience');

		$('.conductrics-experience').hide(); // TODO - do via css instead

		for (var agent in agents) {
			(function(agentCode) {
				$.conductrics('get-decision', {agent:agentCode, choices:agents[agentCode].choices}, function(selection) {
					$('.conductrics-experience[data-agent="'+agentCode+'"][data-choice="' +selection.code+ '"]').show();
				});
			})(agent);
		}
	});

})( jQuery );
