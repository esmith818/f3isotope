$slack_webhook_url = "https://hooks.slack.com/services/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";
$command = $_POST['command'];
$message = $_POST['text'];
$token = $_POST['token'];
// Anonymous bot only allowed in #shieldlock
$channel_id = "CG0LBLMS7";
$data = array(
    "username" => "Anonymous",
    "channel" => $channel_id,
    "text" => $message,
    "mrkdwn" => true,
);
$json_string = json_encode($data);

$slack_call = curl_init($slack_webhook_url);
curl_setopt($slack_call, CURLOPT_CUSTOMREQUEST, "POST");
curl_setopt($slack_call, CURLOPT_POSTFIELDS, $json_string);
curl_setopt($slack_call, CURLOPT_CRLF, true);
curl_setopt($slack_call, CURLOPT_RETURNTRANSFER, true);
curl_setopt($slack_call, CURLOPT_HTTPHEADER, array(
    "Content-Type: application/json",
    "Content-Length: " . strlen($json_string))
);

$result = curl_exec($slack_call);
curl_close($slack_call);
