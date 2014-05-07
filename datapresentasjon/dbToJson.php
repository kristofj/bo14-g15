<?php
mysql_connect("xxx", "xxx", "xxx") or die(mysql_error());
mysql_select_db("xxx") or die(mysql_error());
if ($_GET["type"] == "getRange") {
    $data = mysql_query('SELECT min(datetime) AS mindate, max(datetime) as maxdate FROM logId;')
    or die(mysql_error());
    $row = mysql_fetch_array($data);
    $weatherData = array(
        'mindate' => $row[0],
        'maxdate' => $row[1]
    );
} else {
    $intervalDate=new DateTime($_GET["from"]);
    $interval = ($intervalDate->diff((new DateTime($_GET["to"]))));
    $interval = intval($interval->format('%R%a'));

    if ($_GET["hoursOverride"] == "true") {
        $limiter = '';
        if ($_GET["hours00"] == "true") {
            $limiter = '(HOUR(datetime) =00 ';
        }
        if ($_GET["hours06"] == "true") {
            if ($limiter==''){
                $limiter = '(HOUR(datetime) =06 ';
            }
            else{
                $limiter .= 'OR HOUR(datetime) =06 ';
            }
        }
        if ($_GET["hours12"] == "true") {
            if ($limiter==''){
                $limiter = '(HOUR(datetime) =12 ';
            }
            else{
                $limiter .= 'OR HOUR(datetime) =12 ';
            }
        }
        if ($_GET["hours18"] == "true") {
            if ($limiter==''){
                $limiter = '(HOUR(datetime) =18 ';
            }
            else{
                $limiter .= 'OR HOUR(datetime) =18 ';
            }
        }
        $limiter.=')AND';

    } else {
        //hvis det er mer enn 10 dager mellom datoene, hent ut måling fra kl 12 hver dag
        if ($interval >= 10 & $_GET["allData"] != "true") {
            $limiter = 'HOUR(datetime) =12 AND';
        } //hvis det er mer enn 3 dager mellom datoene, hent kun ut målinger for kl 6 og 12 hver dag.
        elseif ($interval >= 3 & $_GET["allData"] != "true") {
            $limiter = '(HOUR(datetime) =6 OR HOUR(datetime) =12) AND';
        } //hent ut alle målinger
        else {
            $limiter = '';
        }
    }
    $datatype=mysql_real_escape_string($_GET["type"]);
    $toDate=mysql_real_escape_string($_GET["to"]);
    $fromDate=mysql_real_escape_string($_GET["from"]);
    $sql='SELECT * FROM logId LEFT JOIN ' . $datatype . ' ON logId.id=' . $datatype . '.logId_id WHERE ' . $limiter . ' datetime <= "' . $toDate . '" AND datetime >= "' . $fromDate . '";';

    $data = mysql_query($sql)
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