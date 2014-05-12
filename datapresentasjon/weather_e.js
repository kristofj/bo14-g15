//bruker ajax i jQuery for å parse JSON fra .php-backend
function getJson(url, type) {
    $.ajax({
        url: url,
        type: "post",
        dataType: "json",
        timeout:5000,
        //fyrer callback-funksjon for å sende behandlet data videre.
        success: function (response) {
            document.getElementById("timeout").style.visibility = "hidden";
            if (type == "graph") {
                populateArrays(response);
                fillChart();
            }
            else if (type == "formFiller") {
                fillForms(response);
            }
            else if (type == "table") {
                populateArrays(response);
                makeTable();
            }
            else {
                alert('Invalid request');
            }
        },
        error: function() {
            document.getElementById("timeout").style.visibility = "visible";
            getJson(url, type);
        }
    });
}

//legger til 0 der verdi starter med det. f.eks 3.8.1991 blir til 03.08.1991
function getDateFixed(date, type) {
    if (type == "date") {
        if (date.getDate() < 10)
            return "0" +date.getDate();
        return date.getDate();
    }
    else if (type == "month") {
        var month=date.getMonth()+1;
        if (month < 10)
            return "0" + month;
        return month;
    }
    //egentlig redundant
    else if (type == "year") {
        return date.getFullYear();
    }
    else if (type == "hour") {
        if (date.getHours() < 10)
            return "0" + date.getHours();
        return date.getHours();
    }
    else if (type == "minute") {
        if (date.getMinutes() < 10)
            return "0" + date.getMinutes();
        return date.getMinutes();
    }
}

//fyller forms med info (kanskje bruke en kalender?)
function fillForms(jsonResponse) {
    var fs = jsonResponse.mindate.split(" ");
    var ss = fs[0].split("-");
    var ts = fs[1].split(":");
    minCalendarDate = new Date(ss[0], ss[1] - 1, ss[2], ts[0], ts[1]);
    var fs = jsonResponse.maxdate.split(" ");
    var ss = fs[0].split("-");
    var ts = fs[1].split(":");
    maxCalendarDate = new Date(ss[0], ss[1] - 1, ss[2], ts[0], ts[1]);
    toDate = new Date(maxCalendarDate.getTime());
    fromDate = new Date(maxCalendarDate.getTime());

    var diff = Math.floor(( Date.parse(maxCalendarDate) - Date.parse(minCalendarDate) ) / 86400000);
    if (diff > 7) {
        fromDate.setDate(fromDate.getDate() - 7);
    }
    else {
        fromDate.setDate(fromDate.getDate() - diff - 1);
    }

    $("#from").datepicker({ minDate: minCalendarDate, maxDate: maxCalendarDate });
    $("#from").datepicker("option", "dateFormat", "dd/mm/yy");
    $("#from").datepicker("setDate", fromDate);

    $("#to").datepicker({ minDate: fromDate, maxDate: maxCalendarDate });
    $("#to").datepicker("option", "dateFormat", "dd/mm/yy");
    $("#to").datepicker("setDate", maxCalendarDate);
    weatherFetch('graph');
}

//kalles av onChange på form
function changedDates() {
    fromDate = $("#from").datepicker('getDate');
    toDate = $("#to").datepicker('getDate');
    var diff = Math.floor(( Date.parse(toDate) - Date.parse(fromDate) ) / 86400000);
    var hours00form = $("#hours00");
    var hours06form = $("#hours06");
    var hours12form = $("#hours12");
    var hours18form = $("#hours18");
    var hoursStandardForm = $("#hoursStandard");
    if (diff >= 30) {
        if (hours00form.is(":checkbox")) {
            hours00form.replaceWith('<input name="hours" type="radio" id="' + hours00form.attr('id') + '" />');
            hours06form.replaceWith('<input name="hours" type="radio" id="' + hours06form.attr('id') + '" />');
            hours12form.replaceWith('<input name="hours" type="radio" id="' + hours12form.attr('id') + '" />');
            hours18form.replaceWith('<input name="hours" type="radio" id="' + hours18form.attr('id') + '" />');
            hoursStandardForm.replaceWith('<input name="hours" type="radio" id="' + hoursStandardForm.attr('id') + '"checked/>');
        }
    }
    else {
        if (hours00form.is(":radio")) {
            hours00form.replaceWith('<input name="hours" type="checkbox" id="' + hours00form.attr('id') + '" />');
            hours00form.checked = hours00;
            hours06form.replaceWith('<input name="hours" type="checkbox" id="' + hours06form.attr('id') + '" />');
            hours06form.checked = hours06;
            hours12form.replaceWith('<input name="hours" type="checkbox" id="' + hours12form.attr('id') + '" />');
            hours12form.checked = hours12;
            hours18form.replaceWith('<input name="hours" type="checkbox" id="' + hours18form.attr('id') + '" />');
            hours18form.checked = hours18;
            hoursStandardForm.replaceWith('<input name="hours" type="checkbox" id="' + hoursStandardForm.attr('id') + '" checked />');
        }
    }
    $('#from').datepicker('option', 'maxDate', toDate);
    $('#to').datepicker('option', 'minDate', fromDate);
    changedType();
}

function changedStations() {
    if (((document.getElementById("station1").checked) == true) && ((document.getElementById("station2").checked) == true)) {
        document.getElementById("station1").disabled = false;
        document.getElementById("station2").disabled = false;
        station1 = true;
        station2 = true;
    }
    else if ((document.getElementById("station1").checked) == true) {
        document.getElementById("station1").disabled = true;
        station2 = false;
        station1 = true;
    }
    else {
        document.getElementById("station2").disabled = true;
        station1 = false;
        station2 = true;
    }
    if (stateOfDiv == "graph") {
        drawChart();
    }
    else {
        makeTable();
    }
}

//kalles av onChange på værtype
function changedType() {
    radioSelected = ($('input[name=type]:checked').val());
    weatherFetch(stateOfDiv);
}

//kalles av onChange på timer
function changedHours(standard) {
    if (standard == true) {
        document.getElementById("hoursStandard").checked = true;
        document.getElementById("hours00").checked = false;
        document.getElementById("hours06").checked = false;
        document.getElementById("hours12").checked = false;
        document.getElementById("hours18").checked = false;
        hours00 = false;
        hours06 = false;
        hours12 = false;
        hours18 = false;
        hoursOverride = false;
    }
    else {
        if ((document.getElementById("hours00").checked) == true) {
            hours00 = true;
        }
        else {
            hours00 = false;
        }
        if ((document.getElementById("hours06").checked) == true) {
            hours06 = true;
        }
        else {
            hours06 = false;
        }
        if ((document.getElementById("hours12").checked) == true) {
            hours12 = true;
        }
        else {
            hours12 = false;
        }
        if ((document.getElementById("hours18").checked) == true) {
            hours18 = true;
        }
        else {
            hours18 = false;
        }
        document.getElementById("hoursStandard").checked = false;
        hoursOverride = true;
        if(hours00 == false && hours06 == false && hours12 == false && hours18==false){
            changedHours(true);
        }
    }
    weatherFetch(stateOfDiv);
}

//fyller globalt deklarerte variabler
function genChart() {
    radioSelected = ($('input[name=type]:checked').val());
    chart = new google.visualization.LineChart(document.getElementById('chart_div'));
    document.getElementById("hoursStandard").checked = true;
    //starter prosess for å fylle forms og en standardgraf
    getJson('dbToJson.php?type=getRange', 'formFiller');
}

//laget for å unngå å endre kallargumenter på flere steder
function weatherFetch(type) {
    if(type=="graph"){
        document.getElementById("graphButton").style.backgroundColor = "#648762";
        document.getElementById("tableButton").style.backgroundColor = "#78a576";
        document.getElementById("graphButton").style.borderStyle = "inset";
        document.getElementById("tableButton").style.borderStyle = "outset";
    }
    else{
        document.getElementById("graphButton").style.backgroundColor = "#78a576";
        document.getElementById("tableButton").style.backgroundColor = "#648762";
        document.getElementById("tableButton").style.borderStyle = "inset";
        document.getElementById("graphButton").style.borderStyle = "outset";
    }

    if (stateOfDiv == "graph") {
        document.getElementById("loadingOverlay").style.visibility = "visible";
    }
    else if(stateOfDiv == "table"){
        document.getElementById("dataTable").style.backgroundColor = "rgba(0, 0, 0, 0.1)";
    }
    document.getElementById("loading").style.visibility = "visible";
    clearArrays();
    var fromDateStr = fromDate.getFullYear() + "-" + (fromDate.getMonth() + 1) + "-" + fromDate.getDate() + " 00:00:00";
    var toDateStr = toDate.getFullYear() + "-" + (toDate.getMonth() + 1) + "-" + toDate.getDate() + " 23:59:59";
    if (hoursOverride == true) {
        if (type == 'graph') {
            getJson('dbToJson.php?type=' + radioSelected + '&from=' + fromDateStr + '&to=' + toDateStr + '&hoursOverride=true&hours00=' + hours00 + '&hours06=' + hours06 + '&hours12=' + hours12 + '&hours18=' + hours18, "graph");
        }
        else if (type == 'table') {
            getJson('dbToJson.php?type=' + radioSelected + '&from=' + fromDateStr + '&to=' + toDateStr + '&hoursOverride=true&hours00=' + hours00 + '&hours06=' + hours06 + '&hours12=' + hours12 + '&hours18=' + hours18, "table");
        }
    }
    else {
        if (type == 'graph') {
            getJson('dbToJson.php?type=' + radioSelected + '&from=' + fromDateStr + '&to=' + toDateStr + '&allData=false&hoursOverride=false', "graph");
        }
        else if (type == 'table') {
            getJson('dbToJson.php?type=' + radioSelected + '&from=' + fromDateStr + '&to=' + toDateStr + '&allData=true&hoursOverride=false', "table");
        }
    }
}

function makeTable() {
    document.getElementById("loading").style.visibility = "hidden";
    document.getElementById("loadingOverlay").style.visibility = "hidden";
    stateOfDiv = "table";
    var htmlTable = "<b>" + typeData + "</b><br/>";
    if (station1 == true && station2 == true) {
        htmlTable += '<table id="dataTable"><tr><th>Date</th><th>Time</th><th>Station 1<br/>Average</th><th>Station 2<br/>Average</th><th>Station 1<br/>Max</th><th>Station 2<br/>Max</th><th>Station 1<br/>Min</th><th>Station 2<br/>Min</th></tr>';
        document.getElementById("chart_div").style.width = "750px";
        document.getElementById("chart_div").style.marginRight = "50px";
    }
    else if (station1 == true) {
        htmlTable += '<table id="dataTable"><tr><th>Date</th><th>Time</th><th>Station 1<br/>Average</th><th>Station 1<br/>Max</th><th>Station 1<br/>Min</th></tr>';
        document.getElementById("chart_div").style.width = "460px";
        document.getElementById("chart_div").style.marginRight = "340px";
    }
    else {
        htmlTable += '<table id="dataTable"><tr><th>Date</th><th>Time</th><th>Station 2<br/>Average</th><th>Station 2<br/>Max</th><th>Station 2<br/>Min</th></tr>';
        document.getElementById("chart_div").style.width = "460px";
        document.getElementById("chart_div").style.marginRight = "340px";
    }

    for (var i = time.length-1; i > 0; i--) {
        var dateFixed = getDateFixed(time[i], "date") + "/" + getDateFixed(time[i], "month") + "/" + getDateFixed(time[i], "year");
        var clockFixed = getDateFixed(time[i], "hour") + ":" + getDateFixed(time[i], "minute");

        if (valAvg1[i] == null || valMax1[i] == null || valMin1[i] == null) {
            var Avg1 = errorMsg;
            var Max1 = errorMsg;
            var Min1 = errorMsg;
        }
        else {
            var Avg1 = valAvg1[i] + mTypeData;
            if (radioSelected == "wind") {
                var Max1 = valMax1[i] + mTypeData + "<br/>" + dirMax1[i] + "<br/>" + timeMax1[i];
            }
            else {
                var Max1 = valMax1[i] + mTypeData + " <br/>" + timeMax1[i];
            }
            var Min1 = valMin1[i] + mTypeData + " <br/>" + timeMin1[i];
        }

        if (valAvg2[i] == null || valMax2[i] == null || valMin2[i] == null) {
            var Avg2 = errorMsg;
            var Max2 = errorMsg;
            var Min2 = errorMsg;
        }
        else {
            var Avg2 = valAvg2[i] + mTypeData;
            if (radioSelected == "wind") {
                var Max2 = valMax2[i] + mTypeData + "<br/>" + dirMax2[i] + "<br/>" + timeMax2[i];
            }
            else {
                var Max2 = valMax2[i] + mTypeData + " <br/>" + timeMax2[i];
            }
            var Min2 = valMin2[i] + mTypeData + " <br/>" + timeMin2[i];
        }
        if (station1 == true && station2 == true) {
            htmlTable += "<tr><td>" + dateFixed + "</td><td>" + clockFixed + "</td><td>" + Avg1 + "</td><td>" + Avg2 + "</td><td>" + Max1 + "</td><td>" + Max2 + "</td><td>" + Min1 + "</td><td>" + Min2 + "</td></tr>";
        }
        else if (station1 == true) {
            htmlTable += "<tr><td>" + dateFixed + "</td><td>" + clockFixed + "</td><td>" + Avg1 + "</td><td>" + Max1 + "</td><td>" + Min1 + "</td></tr>";
        }
        else {
            htmlTable += "<tr><td>" + dateFixed + "</td><td>" + clockFixed + "</td><td>" + Avg2 + "</td><td>" + Max2 + "</td><td>" + Min1 + "</td></tr>";
        }

    }
    htmlTable += "</table>";
    document.getElementById("chart_div").style.overflow = "auto";
    document.getElementById("chart_div").style.marginLeft = "0px";
    document.getElementById("chart_div").innerHTML = htmlTable;
    document.getElementById("dataTable").style.backgroundColor = "white";

    chart = new google.visualization.LineChart(document.getElementById('chart_div'));
}

//fyller data-arrays med værdata for bruk i enten tabell eller graf
function populateArrays(jsonResponse) {
    var divedent = 1;
    if (radioSelected == "temperature") {
        typeData = "Temperature (Celsius)";
        mTypeData = "&deg;C";
    }
    else if (radioSelected == "humidity") {
        typeData = "Humidity (Percent)";
        mTypeData = "&#37;";
    }
    else if (radioSelected == "pressure") {
        typeData = "Pressure (Hectopascals)";
        mTypeData = " hPa";
        divedent = 2.25;
    }
    else if (radioSelected == "wind") {
        typeData = "Wind speed (Meters per second)";
        mTypeData = " m/s";
    }
    else {
        alert('Invalid option.');
    }
    $.each(jsonResponse, function (i, item) {
        //bytter om på stasjon 1 og 2 da vi byttet om stasjonene under montering.
        if (item.station == 2) {
            var fs = item.time.split(" ");
            var ss = fs[0].split("-");
            var ts = fs[1].split(":");
            var d = new Date(ss[0], ss[1] - 1, ss[2], ts[0], ts[1], ts[2]);
            var findIndex = trackerTime.indexOf((ss[0] + ss[1] - 1 + ss[2] + ts[0]));
            //-1 betyr ikke eksisterer
            if (findIndex == -1) {
                time.push(d);
                trackerTime.push((ss[0] + ss[1] - 1 + ss[2] + ts[0]));
                var index = time.length - 1;
            }
            else {
                var index = findIndex;
            }
            valAvg1[index] = parseFloat((item.valueAvg / divedent).toFixed(1)).toString();
            valMin1[index] = parseFloat((item.valueMin / divedent).toFixed(1)).toString();
            valMax1[index] = parseFloat((item.valueMax / divedent).toFixed(1)).toString();
            if (radioSelected == "wind") {
                var deg = parseFloat(item.dirMax).toFixed();
                dirMax1[index] = deg + "&deg;(" + windDirection(deg) + ")";
            }
            else {
                dirMax1[index] = ("");
            }
            var fs = item.timeMin.split(" ");
            var ss = fs[0].split("-");
            var ts = fs[1].split(":");
            var d = new Date(ss[0], ss[1], ss[2], ts[0], ts[1]);
            timeMin1[index] = (getDateFixed(d, "hour") + ':' + getDateFixed(d, "minute"));
            var fs = item.timeMax.split(" ");
            var ss = fs[0].split("-");
            var ts = fs[1].split(":");
            var d = new Date(ss[0], ss[1], ss[2], ts[0], ts[1]);
            timeMax1[index] = (getDateFixed(d, "hour") + ':' + getDateFixed(d, "minute"));
            isData1 = true;
        }
        else {
            var fs = item.time.split(" ");
            var ss = fs[0].split("-");
            var ts = fs[1].split(":");
            var d = new Date(ss[0], ss[1] - 1, ss[2], ts[0], ts[1], ts[2]);
            var findIndex = trackerTime.indexOf((ss[0] + ss[1] - 1 + ss[2] + ts[0]));
            //-1 betyr ikke eksisterer
            if (findIndex == -1) {

                time.push(d);
                trackerTime.push((ss[0] + ss[1] - 1 + ss[2] + ts[0]));
                var index = time.length - 1;
            }
            else {
                var index = findIndex;
            }

            valAvg2[index] = parseFloat((item.valueAvg / divedent).toFixed(1)).toString();
            valMin2[index] = parseFloat((item.valueMin / divedent).toFixed(1)).toString();
            valMax2[index] = parseFloat((item.valueMax / divedent).toFixed(1)).toString();
            if (radioSelected == "wind") {
                var deg = parseFloat(item.dirMax).toFixed();
                dirMax2[index] = deg + "&deg;(" + windDirection(deg) + ")";
            }
            else {
                dirMax2[index] = ("");
            }
            var fs = item.timeMin.split(" ");
            var ss = fs[0].split("-");
            var ts = fs[1].split(":");
            var d = new Date(ss[0], ss[1] - 1, ss[2], ts[0], ts[1]);
            timeMin2[index] = (getDateFixed(d, "hour") + ':' + getDateFixed(d, "minute"));
            var fs = item.timeMax.split(" ");
            var ss = fs[0].split("-");
            var ts = fs[1].split(":");
            var d = new Date(ss[0], ss[1], ss[2], ts[0], ts[1]);
            timeMax2[index] = (getDateFixed(d, "hour") + ':' + getDateFixed(d, "minute"));
            isData2 = true;
        }
    });
}

function windDirection(deg) {
    if (deg >= 337 || deg < 22) {
        return "N";
    }
    else if (deg >= 22 && deg < 67) {
        return "NE";
    }
    else if (deg >= 67 && deg < 112) {
        return "E";
    }
    else if (deg >= 112 && deg < 157) {
        return "SE";
    }
    else if (deg >= 157 && deg < 202) {
        return "S";
    }
    else if (deg >= 157 && deg < 202) {
        return "S";
    }
    else if (deg >= 202 && deg < 247) {
        return "SW";
    }
    else if (deg >= 247 && deg < 292) {
        return "W";
    }
    else {
        return "NW";
    }
}

//tømmer dataarrays
function clearArrays() {
    time = [];
    trackerTime = [];
    valAvg1 = [];
    valMin1 = [];
    timeMin1 = [];
    valMax1 = [];
    timeMax1 = [];
    dirMax1 = [];
    valAvg2 = [];
    valMin2 = [];
    timeMin2 = [];
    valMax2 = [];
    timeMax2 = [];
    dirMax2 = [];

}

//fyller opp grafdata. blir kalt opp med JSON.
function fillChart() {
    data = new google.visualization.DataTable();
    data.addColumn('datetime', 'time');
    data.addColumn('number', 'Station 1');
    data.addColumn({type: 'string', role: 'tooltip', 'p': {'html': true}});
    data.addColumn('number', 'Station 2');
    data.addColumn({type: 'string', role: 'tooltip', 'p': {'html': true}});
    if (isData1 == true && isData2 == true) {
        data.addColumn('number', 'Difference');
        data.addColumn({type: 'string', role: 'tooltip', 'p': {'html': true}});
    }

    for (var i = 0; i < time.length; i++) {
        var windDirMax1 = "";
        var windDirMax2 = "";
        if (radioSelected == "wind") {
            windDirMax1 = "<br/>" + tabEntry + tabEntry + "Direction: " + dirMax1[i];
            windDirMax2 = "<br/>" + tabEntry + tabEntry + "Direction: " + dirMax2[i];
        }
        if (valAvg1[i] == null || valMax1[i] == null || valMin1[i] == null) {
            var Avg1 = errorMsg;
            var Max1 = errorMsg;
            var Min1 = errorMsg;
        }
        else {
            var Avg1 = valAvg1[i] + mTypeData;
            var Max1 = valMax1[i] + mTypeData + " " + timeMax1[i] + windDirMax1;
            var Min1 = valMin1[i] + mTypeData + " " + timeMin1[i];
        }

        if (valAvg2[i] == null || valMax2[i] == null || valMin2[i] == null) {
            var Avg2 = errorMsg;
            var Max2 = errorMsg;
            var Min2 = errorMsg;
        }
        else {
            var Avg2 = valAvg2[i] + mTypeData;
            var Max2 = valMax2[i] + mTypeData + " " + timeMax2[i] + windDirMax2;
            var Min2 = valMin2[i] + mTypeData + " " + timeMin2[i];
        }

        var tooltip1 = "<br/>" + tabEntry + "<b>Average:</b> " + Avg1 + "<br/>" + tabEntry + "<b>Max:</b> " + Max1 + "<br/>" + tabEntry + "<b>Minimum:</b> " + Min1;
        var tooltip2 = "<br/>" + tabEntry + "<b>Average:</b> " + Avg2 + "<br/>" + tabEntry + "<b>Max:</b> " + Max2 + "<br/>" + tabEntry + "<b>Minimum:</b> " + Min2;

        if (isData1 == true && isData2 == true) {
            var tooltipDif = "<br/>" + tabEntry + "<b>Average: </b><i>" + (valAvg1[i] - valAvg2[i]) + mTypeData + "</i><br/>" + tabEntry + "<b>Max: </b><i>" + (valMax1[i] - valMax2[i]) + mTypeData + "</i><br/>"
                + tabEntry + "<b>Min: </b><i>" + (valMin1[i] - valMin2[i]) + mTypeData + "</i>";
            if (isData1 == true) {
                data.addRow([time[i], parseFloat(valAvg1[i]), tooltip1, parseFloat(valAvg2[i]), tooltip2, parseFloat(valAvg1[i]), tooltipDif]);
            }
            else {
                data.addRow([time[i], parseFloat(valAvg1[i]), tooltip1, parseFloat(valAvg2[i]), tooltip2, parseFloat(valAvg2[i]), tooltipDif]);
            }

        }
        else {
            data.addRow([time[i], parseFloat(valAvg1[i]), tooltip1, parseFloat(valAvg2[i]), tooltip2]);
        }
    }
    options = {
        title: typeData,
        curveType: 'function',
        legend: { position: 'bottom'},
        backgroundColor: '#E8F6FA',
        'chartArea': {
            'backgroundColor': {
                'fill': 'white'
            }
        },
        hAxis: {showTextEvery: 1},
        focusTarget: 'category',
        series: {2: {lineWidth: 1, color: 'transparent', areaOpacity: 0.0, visibleInLegend: false}},
        animation: {
            duration: 1000,
            easing: 'in'
        },
        tooltip: { isHtml: true }
    };
    document.getElementById("chart_div").style.width = "900px";
    document.getElementById("chart_div").style.marginLeft = "-100px";
    document.getElementById("chart_div").style.marginRight = "0px";
    document.getElementById("chart_div").style.overflow = "hidden";
    drawChart();
}

//tegner opp graf med verdier satt i fillChart
function drawChart() {
    document.getElementById("loadingOverlay").style.visibility = "hidden";
    document.getElementById("loading").style.visibility = "hidden";
    if (station1 == true && station2 == true) {
        chart.draw(data, options);
    }
    else if (station1 == true) {
        view = new google.visualization.DataView(data);
        view.hideColumns([3]);
        view.hideColumns([4]);
        view.hideColumns([5]);
        view.hideColumns([6]);
        chart.draw(view, options);
    }
    else {
        view = new google.visualization.DataView(data);
        view.hideColumns([1]);
        view.hideColumns([2]);
        view.hideColumns([5]);
        view.hideColumns([6]);
        chart.draw(view, options);
    }
    stateOfDiv = "graph";
}

//deklarer graf globalt for å ikke måtte generere på nytt. Dette fører til mulgiheten for å animerte grafer med async ajax.
var options;
var chart;
var data;
var stateOfDiv;
var station1 = true;
var station2 = true;

var tabEntry = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
var radioSelected;
var hoursOverride = false;
var hours00 = false;
var hours06 = false;
var hours12 = false;
var hours18 = false;

var time = [];
var trackerTime = [];
var typeData;
var mTypeData;
var valAvg1 = [];
var valMin1 = [];
var timeMin1 = [];
var valMax1 = [];
var timeMax1 = [];
var dirMax1 = [];
var isData1 = false;

var valAvg2 = [];
var valMin2 = [];
var timeMin2 = [];
var valMax2 = [];
var timeMax2 = [];
var dirMax2 = [];
var isData2 = false;

//variabler fra kalendere
var minCalendarDate;
var maxCalendarDate;
var fromDate;
var toDate;

//språk
var errorMsg = "No data";

//laster inn grafobjekter asynkront. kjører deretter genChart for å sette opp grafobjektene
google.load("visualization", "1", {packages: ["corechart"]});
google.setOnLoadCallback(genChart);
