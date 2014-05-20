<html>
	<head>
		<meta http-equiv="content-type" content="text/html;charset=utf-8" />
		<title>bo14-g15</title>
		<link rel="icon" type="image/ico" href="img/favicon.ico">
		<link rel="stylesheet" type="text/css" href="style/style.css" />
	</head>
	<body>
		<?php 
			include "ele/header.php";
			
			echo '<div class="mainWindow">';
			
			//Henter inn innhold til hovedvinduet.
			$i = $_GET["i"];
			if(isset($i)) {
				include "ele/" . $i . ".php";
			} else {
				include "ele/home.php";
			}
			
			echo "</div>";
			
			include "ele/footer.php"; 
		?>
	</body>
</html>
