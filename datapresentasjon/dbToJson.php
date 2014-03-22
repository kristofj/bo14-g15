<?php
mysql_connect("j3nsen.com", "hessdalen", "hessdalen123") or die(mysql_error());
mysql_select_db("hessdalen_weather") or die(mysql_error());
if($_GET["type"]=="temp"){
    $data = mysql_query("SELECT * FROM logId LEFT JOIN temperature ON logId.id=temperature.logId_id;")
    or die(mysql_error());
}
elseif($_GET["type"]=="humidity"){
    $data = mysql_query("SELECT * FROM logId LEFT JOIN humidity ON logId.id=humidity.logId_id;")
    or die(mysql_error());
}
elseif($_GET["type"]=="pressure"){
    $data = mysql_query("SELECT * FROM logId LEFT JOIN pressure ON logId.id=pressure.logId_id;")
    or die(mysql_error());
}
elseif($_GET["type"]=="wind"){
    $data = mysql_query("SELECT * FROM logId LEFT JOIN wind ON logId.id=wind.logId_id;")
    or die(mysql_error());
}
else{
    die("Invalid type");
}
$weatherData=array();
while($row = mysql_fetch_array( $data ))
{
    if($_GET["type"]=="wind"){
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
            'timeMin' => $row[10],
        );
    }
    else{
        $a = array(
            'logId_id' => $row[0],
            'time' => $row[1],
            'station' => $row[2],
            'valueAvg' => $row[4],
            'valueNow' => $row[5],
            'valueMax' => $row[6],
            'timeMax' => $row[7],
            'valueMin' => $row[8],
            'timeMin' => $row[9],
        );
    }
    array_push($weatherData,$a);
}
echo json_encode($weatherData);
?>