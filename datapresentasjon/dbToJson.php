<?php
mysql_connect("j3nsen.com", "hessdalen", "hessdalen123") or die(mysql_error());
mysql_select_db("hessdalen_weather") or die(mysql_error());
if ($_GET["type"] == "getRange") {
    $data = mysql_query('SELECT min(datetime) AS mindate, max(datetime) as maxdate FROM logId;')
    or die(mysql_error());
    $row = mysql_fetch_array($data);
    $weatherData = array(
        'mindate' => $row[0],
        'maxdate' => $row[1]
    );
} else {
    $data = mysql_query('SELECT * FROM logId LEFT JOIN ' . $_GET["type"] . ' ON logId.id=' . $_GET["type"] . '.logId_id WHERE datetime < "'.$_GET["to"].'" AND datetime > "'.$_GET["from"].'";')
    or die(mysql_error());
    $weatherData = array();
    while ($row = mysql_fetch_array($data)) {
        if ($_GET["type"] == "wind") {
            $a = array(
                'logId_id' => $row[0],
                'time' => $row[1],
                'station' => $row[2],
                'valueAvg' => $row[4],
                'valueNow' => $row[5],
                'valueMax' => $row[6],
                'dirMax' => $row[7],
                'timeMax' => $row[8],
                'valueMin' => $row[9],
                'timeMin' => $row[10]
            );
        } else {
            $a = array(
                'logId_id' => $row[0],
                'time' => $row[1],
                'station' => $row[2],
                'valueAvg' => $row[4],
                'valueNow' => $row[5],
                'valueMax' => $row[6],
                'timeMax' => $row[7],
                'valueMin' => $row[8],
                'timeMin' => $row[9]
            );
        }
        array_push($weatherData, $a);
    }
}
echo json_encode($weatherData);
?>