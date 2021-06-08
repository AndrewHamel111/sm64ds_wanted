<html>
 <head>
  <title>PHP Test</title>
 </head>
 <body>
 <?php
    $scoreboard = json_decode(file_get_contents('scoreboard.json'), true);
    if($scoreboard[$_POST['name']] < $_POST['score'])
		$scoreboard[$_POST['name']] = $_POST['score'];
    file_put_contents('scoreboard.json', json_encode($scoreboard));
	
	$f = 'sb_bkp/'.date('Y_m_d').'_wanted.json';
	if(!file_exists($f))
		file_put_contents($f, json_encode($scoreboard));
?> 
 </body>
</html>