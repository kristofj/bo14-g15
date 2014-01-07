<html>
	<head>
		<meta charset="UTF-8"/>
		<title>energi.hiof.no</title>
		<link rel="icon" type="image/ico" href="img/favicon.ico">
		<link rel="stylesheet" type="text/css" href="style/style.css" />
		<script type="text/javascript" src="src/functions.js"></script>
		<script type="text/javascript" src="../../lib/Chart.js/Chart.js"></script>
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
