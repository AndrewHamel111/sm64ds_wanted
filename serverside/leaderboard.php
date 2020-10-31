<html>
<head>
	<title>NICK GAME LEADERBOARD</title>
	<style>
		div#leftDiv {
			padding: 0px 3% 30px 3%;
			width:43.5%;
			float:left;
			background-color:#EAEAEA;
		}
		div#rightDiv {
			padding: 0px 3% 30px 3%;
			width:43.5%;
			position:relative;
			float:right;
			background-color:#EAEAEA;
		}
		p.scores {
			font-size:25;
			line-height:1.6;
			padding-left:10px;
			padding-top:10px;
			color:#141414;
		}
	</style>
</head>
<body style="background-color:#141414">
<div style="margin:10px 0px 20px 0px;">
	<!-- header div-->
	<p style="text-align:center;"><a href="https://github.com/AndrewHamel111/sm64ds_wanted"><img src="wanted_BANNER.png" width="640" height="320"/></a></p>
</div>
<div>
	<!-- root div-->
	<div id="leftDiv">
		<!-- left div-->
		<h1 style="text-align:center;" >ALL SCORES</h1>
		<div>
			<p class="scores">
			<?php
				// get an associative array representation of the json
				$board = json_decode(file_get_contents("scoreboard.json"), true);
				asort($board);
				$board = array_reverse($board);
				
				$x = 1;
				$last_score = -1;
				foreach($board as $name => $score)
				{
					if($score == $last_score)
					{
						echo nl2br(($x - 1).". ".$name." (".$score."pts)\n");
					}
					else
					{
						echo nl2br($x++.". ".$name." (".$score."pts)\n");
					}
					
					if ($x == 6) echo "<br>";
				}
			?>
			</p>
		</div>
	</div>
	<div id="rightDiv">
		<!-- right div-->
		<h1 style="text-align:center;" >NICKTOBER SCORES</h1>
		<div>
			<p class="scores">
			<?php
				// get an associative array representation of the json
				$board = json_decode(file_get_contents("scoreboard.json"), true);
				asort($board);
				$board = array_reverse($board);
				
				// get nicktober_names.csv
				$names = str_getcsv(file_get_contents("nicktober_names.csv"));
				
				$x = 1;
				$last_score = -1;
				foreach($board as $name => $score)
				{
					if (in_array($name, $names))
					{
						if($score == $last_score)
						{
							echo nl2br(($x - 1).". ".$name." (".$score."pts)\n");
						}
						else
						{
							echo nl2br($x++.". ".$name." (".$score."pts)\n");
						}
						
						if ($x == 6) echo "<br>";
					}
				}
			?>
			</p>
		</div>
	</div>
</div>
</body>
</html>