<html>
	<head>
		<title> NICK GAME LEADERBOARD </title>
		<link rel="icon" href="favicon.ico">
		<style>			
			.banner {
				padding: 0;
			}
			
			#nickBanner {
				width:25%;
				display:block;
				margin-left:auto;
				margin-right:auto;
			}
			
			#nickBannerBottom {
				width:25%;
				display:block;
				margin-left:auto;
				margin-right:auto;
			}
			
			#allScores {
				float:left;
				text-align:left;
			}
			
			#nickScores {
				float:right;
				text-align:left;
			}
			
			.scoreList {
				padding: 0px 10px 0px 10px;
				font-size:20;
				line-height:1.8;
			}
		</style>
	</head>
	<body style="background-color:#141414;">
		<header>
			<a href="http://hamel111.myweb.cs.uwindsor.ca/sm64games/wanted/fuckyousimon.php">
				<img  class="banner" id="nickBanner" src="nicktoberWebBannerSmall.png">
			</a>
		</header>

		<div style="background-color:#141414;">
			<h1 style="color:#EBEBEB;text-align:center;">THE MANY FACES OF NICKTOBER</h1>
			<p style="color:#EBEBEB;font-size:20px;text-align:center;"></p>
			<div>
				<h2 style="width:50%;float:left;color:#EBEBEB;font-size:20px;text-align:center;">ALL SCORES</h2>
				<h2 style="width:50%;float:right;color:#EBEBEB;font-size:20px;text-align:center;">NICKTOBER SCORES</h2>
			</div>
		</div>
		<div style="width:100%;">
			<article style="padding: 20px;width:45%;background-color:#EBEBEB;color:#141414;border-style:inset;border-width:thick;" id="allScores">
			<p class="scoreList">
				<?php
				$scoreboard = json_decode(file_get_contents('scoreboard.json'), true);
				
				// sort scoreboard
				asort($scoreboard, SORT_DESC);
				$scoreboard = array_reverse($scoreboard);
				
				// print as sorted
				$x = 1;
				$flag = TRUE;
				$last_score = -1;
				foreach($scoreboard as $name => $value) {
					if($last_score != -1 && $value == $last_score)
						echo nl2br(($x - 1).". ".$name." (".$value.")\n");
					else
						echo nl2br($x++.". ".$name." (".$value.")\n");
					$last_score = $value;
					if ($x == 6 && $flag)
					{
						echo "<br/>";
						$flag = FALSE;
					}
				}
				
				?> 
			</p>
			</article>
			
			<article style="padding: 20px;width:45%;background-color:#EBEBEB;color:#141414;border-style:inset;border-width:thick;" id="nickScores">
			<p class="scoreList">
				<?php
				$scoreboard = json_decode(file_get_contents('scoreboard.json'), true);
				
				// sort scoreboard
				asort($scoreboard, SORT_DESC);
				$scoreboard = array_reverse($scoreboard);
				
				// get nicktober names
				$names = str_getcsv(file_get_contents('nicktober_names.csv'));
				
				// print as sorted
				$x = 1;
				$flag = TRUE;
				$last_score = -1;
				foreach($scoreboard as $name => $value) {
					if (in_array($name, $names))
						if($last_score != -1 && $value == $last_score)
							echo nl2br(($x - 1).". ".$name." (".$value.")\n");
						else
							echo nl2br($x++.". ".$name." (".$value.")\n");
					$last_score = $value;
					if ($x == 6 && $flag)
					{
						echo "<br/>";
						$flag = FALSE;
					}
				}
				?>
			</p>
			</article>
		</div>
		
	</body>
</html>