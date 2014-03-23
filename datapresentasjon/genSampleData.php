<?php
ini_set('display_startup_errors',1);
ini_set('display_errors',1);
error_reporting(-1);
$mysqli = new mysqli("j3nsen.com","hessdalen","hessdalen123","hessdalen_weather");
// Check connection
if (mysqli_connect_errno())
{
    echo "Failed to connect to MySQL: " . mysqli_connect_error();
}

function gr() {
    return rand(-30, 30);
}
function gw() {
    return rand(0, 359);
}
function gd($d,$h) {
    return 'NOW()- INTERVAL '.$d.' DAY+INTERVAL '.$h.' HOUR-INTERVAL '.rand(1,58).' MINUTE';
}

$day=90;

for ($hour = 1; $hour <= (($day-1)*24); $hour++) {
    $thisdate='NOW()- INTERVAL 50 DAY+INTERVAL '.$hour.' HOUR';
    for($i=1; $i <=2; $i++){
        $mysqli->query('INSERT INTO logId (datetime,stationId) VALUES ('.$thisdate.','.$i.')');
        $id=$mysqli->insert_id;
        echo $id.":";
        $mysqli->query('INSERT INTO temperature (logId_id, temperatureAvg, temperatureNow, temperatureMax, timeMax, temperatureMin, timeMin) VALUES ('.$id.', '.gr().', '.gr().', '.gr().', '.gd($day,$hour).', '.gr().', '.gd($day,$hour).')');
        $mysqli->query('INSERT INTO humidity (logId_id, humidityAvg, humidityNow, humidityMax, timeMax, humidityMin, timeMin) VALUES ('.$id.', '.gr().', '.gr().', '.gr().', '.gd($day,$hour).', '.gr().', '.gd($day,$hour).')');
        $mysqli->query('INSERT INTO pressure (logId_id, pressureAvg, pressureNow, pressureMax, timeMax, pressureMin, timeMin) VALUES ('.$id.', '.gr().', '.gr().', '.gr().', '.gd($day,$hour).', '.gr().', '.gd($day,$hour).')');
        $mysqli->query('INSERT INTO wind (logId_id, windspeedAvg, windspeedNow, windspeedMax, winddirectionMax, timeMax, windspeedMin, timeMin) VALUES ('.$id.', '.gr().', '.gr().', '.gr().', '.gw().', '.gd($day,$hour).', '.gr().', '.gd($day,$hour).')');
    }
}

$mysqli->close();
?>