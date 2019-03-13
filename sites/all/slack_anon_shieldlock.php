<?php
	$slack_webhook_url = "https://hooks.slack.com/services/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";
	$command = $_POST['command'];
	$message = $_POST['text'];
	$token = $_POST['token'];
	$trigger_id = $_POST['trigger_id'];
	$channel_id = $_POST['channel_id'];
	$user_id = $_POST['user_id'];

	// Check which channel post came from.
	// Command can only be used in #shieldlock (CG0LBLMS7).
	// Return warning in same channel if not #shieldlock.
	if ($channel_id != "CG0LBLMS7") {
		$data = array(
			"username" => "Slackbot",
			"icon_emoji" => ":slack:",
		    "channel" => $channel_id,
		    "response_type" => "ephemeral",
		    "replace_original" => false,
		    "text" => "Anonymous posting is only allowed in <#CG0LBLMS7|shieldlock>",
		);
	}
	else {
		$data = array(
		    "username" => "Anonymous",
		    "channel" => $channel_id,
		    "response_type" => "in_channel",
		    "text" => $message,
		    //"mrkdwn" => true,
		);
	}

	$json_string = json_encode($data);

	$slack_call = curl_init($slack_webhook_url);
	curl_setopt($slack_call, CURLOPT_CUSTOMREQUEST, "POST");
	curl_setopt($slack_call, CURLOPT_POSTFIELDS, $json_string);
	curl_setopt($slack_call, CURLOPT_CRLF, true);
	curl_setopt($slack_call, CURLOPT_RETURNTRANSFER, true);
	curl_setopt($slack_call, CURLOPT_HTTPHEADER, array(
	    "Content-Type: application/json",
	    "Authorization: Bearer " . $token,
	    "Content-Length: " . strlen($json_string))
	);

	curl_exec($slack_call);
	curl_close($slack_call);
?>
