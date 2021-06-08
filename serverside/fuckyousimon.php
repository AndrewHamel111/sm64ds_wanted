<!doctype html>
<html>
<head>
<meta charset="utf-8">
<title>fuck you simon</title>
<link rel="icon" href="simonPogSmall.png">
	<style>
		#mainDiv {
			text-align: center;
			background-color: aqua;
			border-style: outset;
			
			border-width: thick;
			border-color: aquamarine;
			border-bottom-color: blue;
		}
		#secondaryDiv {
  			margin: auto;
			background-color: cadetblue;
			text-align:  center;
			border-style: inset;
  			width: 50%;
  
			border-width: thick;
			border-color: ##5d9399;
			border-bottom-color: #629988;
		}
		#foot {
  			margin-bottom: 10%;
			background-color: cadetblue;
			text-align:  center;
		}
	</style>
</head>

<body style="background-color: pink">
<div id="mainDiv"><h1>FUCK YOU SIMON</h1>
<iframe width="560" height="315" src="https://www.youtube.com/embed/Qx0BBuFZpoc" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
<br/><br/><br/></div>
<br/><br/><br/><br/><br/>
<div id="secondaryDiv">
 
 <?php 
	// increments each time the page is visited.
    $n = file_get_contents('simon.data') + 0;
	$n++;
	// update file
	file_put_contents('simon.data', $n);
	
	// fuck you simon
	echo 'This page has been visited '.$n.' times. ';
?> 
</div>
<br/><br/>
<br/><br/><br/><br/><br/><br/><br/><br/><br/>
<footer id="foot">Sorry Simon we love you<br/><a href=https://twitter.com/SPannitto>twitter</a></footer>
</body>
</html>
