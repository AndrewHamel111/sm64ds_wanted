<html>
<head>
<title>get out</title>
</head>
<body>
<?php
	// get an associative array representation of the json
	$board = json_decode(file_get_contents("scoreboard.json"), true);
	// only update scoreboard value if this is a new high score.
	if($board[$_POST["name"]] < $_POST["score"])
		$board[$_POST["name"]] = $_POST["score"];
	
	file_put_contents("scoreboard.json", json_encode($board));
	// create a backup for the file (lol)
?>
</body>
</html>