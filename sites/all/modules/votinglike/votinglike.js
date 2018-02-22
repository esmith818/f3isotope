// Javascript for flag.js

(function ($) {

Drupal.behaviors.votinglike = {
  	attach: function(context) {
	  function updateButton(element, newHtml) {
		var $newButton = $(newHtml);

		// Reattach the behavior to the new <span> element. This element
		// is either whithin the wrapper or it is the outer element itself.
		var $nucleus = $newButton.is('span.voting-toggle') ? $newButton : $('span.voting-toggle', $newButton);
		$nucleus.addClass('voting-processed').click(votingClick);

		// Find the wrapper of the old link.
		var $wrapper = $(element).parents('.voting-wrapper:first');
		// Replace the old link with the new one.
		$wrapper.after($newButton).remove();
		Drupal.attachBehaviors($newButton.get(0));

		return $newButton.get(0);
	  }

	  /**
	   * A click handler that is attached to all <A class="voting"> elements.
	   */
	  function votingClick(event) {
		event.preventDefault();
		var element = this;

		var $wrapper = $(element).parents('.voting-wrapper:first');
		if ($wrapper.is('.vote-waiting')) {
		  // Guard against double-clicks.
		  return false;
		}
		$wrapper.addClass('vote-waiting');
		
		var parent 		= $wrapper.get(0);
		var pElement	=element.parentElement;
		var post_url 	=parent.getAttribute('data-url');
		
		var vote_count 	=pElement.getAttribute('vote-count');
		var vote_value 	=pElement.getAttribute('vote-value');
		var newNumber 	=Number(vote_count) + Number(vote_value);
		
		var btnVoting 	=$(element).parents('.btnVoting:first');
		btnVoting.children('.btnLike-count').html( newNumber);
		
		
		// Send POST request
		$.ajax({
		  type: 'POST',
		  url: post_url,
		  data: {ajax: true, 
		  	type: 	parent.getAttribute('data-type'), 
		  	tag: 	pElement.getAttribute('data-tag'), 
		  	eid: 	parent.getAttribute('data-eid'), 
		  	val: 	parent.getAttribute('data-val'), 
		  	value_type: 	parent.getAttribute('data-value_type'), 
		  	button_type: 	parent.getAttribute('data-button_type'), 
		  	dislike_show: 	parent.getAttribute('data-dislike_show'), 
		  	label_like: 	parent.getAttribute('data-label_like'),
		  	label_dislike: 	parent.getAttribute('data-label_dislike'),
		  	tips_show: 		parent.getAttribute('data-tips_show')
		  },
		  dataType: 'json',
		  success: function (data) {
		    data.link = $wrapper.get(0);
		    
		    if (!data.preventDefault) { // A handler may cancel updating the link.
		      data.link = updateButton(element, data.newButton);
		    }

		    var $newButton = $(data.newButton);
		    // Finally, update the page.
		    Drupal.attachBehaviors($newButton.parent());
		  },
		  error: function (xmlhttp) {
		    alert('An HTTP error '+ xmlhttp.status +' occurred.\n'+ element.href);
		    $wrapper.removeClass('vote-waiting');
		  }
		});		
	  }

	  $('.voting-wrapper .voting-toggle:not(.voting-processed)', context).addClass('voting-processed').click(votingClick);
	}
};

})(jQuery);
