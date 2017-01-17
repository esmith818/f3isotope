<head>
<!DOCTYPE html>
<title>Back Blast Mad Libs</title>
<link rel="stylesheet" type="text/css" href="//fonts.googleapis.com/css?family=Bitter" />
<link rel="stylesheet" type="text/css" href="//fonts.googleapis.com/css?family=Droid+Sans" />
<style>
	body, label {
		color:#5d5d5d;
		font-family:'Droid Sans';
	}
	h1 {
		color:#a80707;
		font-family:'Bitter';
		font-size:35px;"
	}
	input[type=submit] {
		-webkit-appearance: none;
		background-color: #FFF;
		color: #5d5d5d;
		border: solid 1px;
		font-size: 16px;
		border-radius: 3px;
		padding: 12px 30px;
		cursor: pointer;
	}
	input[type=text], textarea {
		width: 90%;
		margin: 8px 0;
		padding: 6px 12px;
		font-size: 16px;
	}
</style>
</head>

<?php // Setup a form to retrieve custom information for generating a MadLibs back blast
function create_array($str) {
	$array = explode(',', $str);
	$array = array_map('trim', $array);
	return $array;
}

function randomizer($arr, $int = 1) {
	shuffle($arr);
	$random = array_rand(array_flip($arr), $int);
	return $random;
}

$backblast = '';
if (isset($_POST['action'])) {
	$pax = $_POST['pax'];
	$ao = $_POST['ao'];
	$exercises = $_POST['exercises'];
	$locations = $_POST['locations'];

	$exercises_arr = create_array($exercises);
	$pax_arr = create_array($pax);
	$pax_count = count($pax_arr);
	$exercises_arr = create_array($exercises);
	$locations_arr = create_array($locations);
	$conditions_arr = array("warm","chilly","foggy","moist","comfortable","freezing","ridiculously hot","depressingly dark");
	$strength_arr = array("mettle","strength","fitness","fortitude","vigor","vitality","potency","courage");
	$mouth_arr = array("yapper","maw","mouth","pie hole");
	$emotions_arr = array("sleepy","like a tomato","emotional","unsettled","weak in the knees","like a man who ate too many donuts");
	$verbs_arr = array("moseyed","sauntered","sallied forth","rushed","bear crawled","crab walked","lunge walked","skipped","tip-toed");
	$bodypart_arr = array("knee","elbow","coccyx","left nostril","sphincter","big toe","armpit","beer gut","badonkadonk");
	$plants_arr = array("clover","red clay","hard work","asphalt","gravel","ornamental bushes","concrete","whatever might be over there");
	$adjective1_arr = array("fattened","over-stuffed","bulging","bloated","enlarged","puffy");
	$animal_arr = array("blowfish","piglet","oragutan","cat","guinea pig","whale","frog");
	$adjective2_arr = array("enjoyable","spontaneous","made up as I went","mediocre","strenuous","whimsical","plagiarized","better than its competition");

	$backblast = "It was a " . randomizer($conditions_arr) . " morning, but " . $pax_count . " men defied the lure of the fartsack to test their ";
	$backblast .= randomizer($strength_arr) . " at " . $ao . " where YHC had several exercises to make the workout challenging. When we began the ";
	$backblast .= randomizer($exercises_arr) . ", " . randomizer($pax_arr) . " exclaimed BS because he was feeling ";
	$backblast .= randomizer($emotions_arr) . ". We also did " . implode(" and ",randomizer($exercises_arr,2)) . " generating similar responses from guys like ";
	$backblast .= randomizer($pax_arr) . " who always feel the need to open their " . randomizer($mouth_arr) . " instead of doing the work. At one point, we ";
	$backblast .= randomizer($verbs_arr) . " to the " . randomizer($locations_arr) . " where the plan was to do " . randomizer($exercises_arr) . ", but ";
	$backblast .= randomizer($pax_arr) . " immediately pointed out that his " . randomizer($bodypart_arr) . " was sore and he would need to call an audible. ";
	$backblast .= "When we finally got to the " . randomizer($exercises_arr) . ", " . randomizer($pax_arr);
	$backblast .= " inquired as to why we didn't do them earlier. YHC told him that we would have done them at the " . randomizer($locations_arr);
	$backblast .= ", but " . randomizer($pax_arr) . " was allergic to " . randomizer($plants_arr);
	$backblast .= " and might have swole up like a " . randomizer($adjective1_arr) . " " . randomizer($animal_arr);
	$backblast .= " if we had gone there. All in all, the workout was " . randomizer($adjective2_arr) . ". Aside from the hijinks above, the workout went something like this:";
	$backblast .= "\n\n(insert your weinke here)";
}
?>
<body>
<h1>Back Blast Intro Generator</h1>
<p>Are you having a hard time coming up with something interesting to say in your back blast? Do you need a little humor to draw in the reader? With a little
	information, this form will generate the perfect introduction to your back blast. Fill in the following fields and submit your answers to generate a custom opening
	paragraph. If you don't like the first attempt, click the Submit button again for a revised option. When you like the result, copy and paste into your back
	blast and then add the actual (boring) details after. With a witty opening, you'll find the pax waiting impatiently after each of your Qs to see what
	you will say!</p>
<br /><strong>Provide a few details about your workout:</strong>
<hr />
<form action="bbgenerator.php" method="post">
  <p><label for="ao">Enter name of the workout:<br /><input type="text" name="ao" value="<?php print $ao; ?>" />
  <p><label for="pax">Enter names of all pax at workout separated with commas:<br /><input type="text" name="pax" value="<?php print $pax; ?>" />
  <p><label for="exercises">Enter names of a few exercises at workout separated with commas:<br /><input type="text" name="exercises" value="<?php print $exercises; ?>" />
  <p><label for="locations">Enter a few locations (e.g. parking lot; rock pile) separated with commas:<br /><input type="text" name="locations" value="<?php print $locations; ?>" />
  <p><input type="submit" value="Submit" />
  <input type="hidden" name="action" value="backblast" />
</form>
<hr />
<br />
<strong>Result:</strong><br />
<textarea rows="16" cols="80"><?php echo $backblast; ?></textarea>
</body>
