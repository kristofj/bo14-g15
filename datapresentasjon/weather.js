//bruker ajax i jQuery for å parse JSON fra .php-backend
function getJson(url, type) {
    $.ajax({
        url: url,
        type: "post",
        dataType: "json",
        //fyrer callback-funksjon for å sende behandlet data videre.
        success: function (response) {
            if (type == "graph") {
                drawChart(response);
            }
            else if (type == "formFiller") {
                fillForms(response);
            }
            else if (type == "table") {
                makeTable(response);
            }
            else {
                alert('You shall not pass!');
            }
        }
    });
}

//legger til 0 der verdi starter med det. f.eks 3.8.1991 blir til 03.08.1991
function getDateFixed(date, type) {
    if (type == "date") {
        if (date.getDate() < 10)
            return "0" + date.getDate();
        return date.getDate();
    }
    else if (type == "month") {
        if (date.getMonth() < 10)
            return "0" + date.getMonth();
        return date.getMonth();
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
    minCalendarDate = new Date(ss[0], ss[1]-1, ss[2], ts[0], ts[1]);
    var fs = jsonResponse.maxdate.split(" ");
    var ss = fs[0].split("-");
    var ts = fs[1].split(":");
    maxCalendarDate = new Date(ss[0], ss[1]-1, ss[2], ts[0], ts[1]);
    toDate = new Date(maxCalendarDate.getTime());
    fromDate = new Date(maxCalendarDate.getTime());
    fromDate.setDate(fromDate.getDate() - 7);

    $("#from").datepicker({ minDate: minCalendarDate, maxDate: maxCalendarDate });
    $("#from").datepicker("option", "dateFormat", "dd/mm/yy");
    $("#from").datepicker("setDate", fromDate);

    $("#to").datepicker({ minDate: fromDate, maxDate: maxCalendarDate });
    $("#to").datepicker("option", "dateFormat", "dd/mm/yy");
    $("#to").datepicker("setDate", maxCalendarDate);
    weatherFetch('graph');
}

//kalles av onChange på form
function changedForms(){
    fromDate = $("#from").datepicker('getDate');
    toDate = $("#to").datepicker('getDate');
    $('#from').datepicker('option', 'maxDate', toDate);
    $('#to').datepicker('option', 'minDate', fromDate);
}

//kalles av onChange på form
function changedType(){
    radioSelected = ($('input[name=type]:checked').val());
}

//fyller globalt deklarerte variabler
function genChart() {
    radioSelected = ($('input[name=type]:checked').val());
    chart = new google.visualization.LineChart(document.getElementById('chart_div'));

    //starter prosess for å fylle forms og en standardgraf
    getJson('dbToJson.php?type=getRange','formFiller');
}

//laget for å unngå å endre kallargumenter på flere steder
function weatherFetch(type) {
    var fromDateStr=fromDate.getFullYear()+"-"+(fromDate.getMonth()+1)+"-"+fromDate.getDate()+" 00:00:00";
    var toDateStr=toDate.getFullYear()+"-"+(toDate.getMonth()+1)+"-"+toDate.getDate()+" 23:59:59";

    if (type=='graph'){
        getJson('dbToJson.php?type=' + radioSelected+'&from='+fromDateStr+'&to='+toDateStr+'&allData=false', "graph");
    }
    else if(type=='table'){
        getJson('dbToJson.php?type=' + radioSelected+'&from='+fromDateStr+'&to='+toDateStr+'&allData=true', "table");
    }
}

function makeTable(response){
    populateArrays(response);
    var htmlTable='<head><title>'+typeData+'</title><link rel="stylesheet" type="text/css" href="weather_style.css"></head><body><table><tr><th>Dato</th><th>Kl</th><th>Stasjon 1<br/>Gjennomsnitt</th><th>Stasjon 2<br/>Gjennomsnitt</th><th>Stasjon 1<br/>Maks</th><th>Stasjon 2<br/>Maks</th><th>Stasjon 1<br/>Min</th><th>Stasjon 2<br/>Min</th></tr>';
    for (i = 0; i < time.length; i++) {
        var dateFixed=getDateFixed(time[i],"date")+"/"+getDateFixed(time[i],"month")+"/"+getDateFixed(time[i],"year");
        var clockFixed=getDateFixed(time[i],"hour")+":"+getDateFixed(time[i],"minute");
        htmlTable+="<tr><td>"+dateFixed+"</td><td>"+clockFixed+"</td><td>"+valAvg1[i]+mTypeData+"</td><td>"+valAvg2[i]+mTypeData+"</td><td>"+valMax1[i]+mTypeData+" "+dirMax1[i]+"</td><td>"+valMax2[i]+mTypeData+"</td><td>"+valMin1[i]+mTypeData+"</td><td>"+valMin2[i]+mTypeData+"</td></tr>";
    }
    htmlTable+="</body>";
    document.getElementById("chart_div").style.overflow="auto";
    document.getElementById("chart_div").style.width="750px";
    document.getElementById("chart_div").innerHTML = htmlTable;
    clearArrays();
    /*var x=window.open('','','toolbars=0,width=750,height=600,left=200,top=200,scrollbars=1,resizable=1');
    x.document.open();
    x.document.write(htmlTable);
    x.document.close();*/
    chart = new google.visualization.LineChart(document.getElementById('chart_div'));
}

function populateArrays(jsonResponse){
    if (radioSelected == "temperature") {
        typeData = "Temperatur";
        mTypeData = "&deg;";
    }
    else if (radioSelected == "humidity") {
        typeData = "Luftfuktighet";
        mTypeData = "&#37;";
    }
    else if (radioSelected == "pressure") {
        typeData = "Lufttrykk";
        mTypeData = " hPa";
    }
    else if (radioSelected == "wind") {
        typeData = "Vindhastighet";
        mTypeData = " m/s";
    }
    else {
        alert('Ugyldig valg.');
    }
    $.each(jsonResponse, function (i, item) {
        if (item.station==1){
            valAvg1.push(item.valueAvg);
            valMin1.push(item.valueMin);
            valMax1.push(item.valueMax);
            if (radioSelected == "wind") {
                dirMax1.push(item.dirMax);
            }
            else{
                dirMax1.push("");
            }
            var fs = item.timeMin.split(" ");
            var ss = fs[0].split("-");
            var ts = fs[1].split(":");
            var d = new Date(ss[0], ss[1], ss[2], ts[0], ts[1]);
            timeMin1.push(getDateFixed(d, "hour") + ':' + getDateFixed(d, "minute"));
            var fs = item.timeMax.split(" ");
            var ss = fs[0].split("-");
            var ts = fs[1].split(":");
            var d = new Date(ss[0], ss[1], ss[2], ts[0], ts[1]);
            timeMax1.push(getDateFixed(d, "hour") + ':' + getDateFixed(d, "minute"));

            var fs = item.time.split(" ");
            var ss = fs[0].split("-");
            var ts = fs[1].split(":");
            var d = new Date(ss[0], ss[1]-1, ss[2], ts[0], ts[1],ts[2]);
            time.push(d);
        }
        else{
            valAvg2.push(item.valueAvg);
            valMin2.push(item.valueMin);
            valMax2.push(item.valueMax);
            if (radioSelected == "wind") {
                dirMax2.push(item.dirMax);
            }
            else{
                dirMax2.push("");
            }
            var fs = item.timeMin.split(" ");
            var ss = fs[0].split("-");
            var ts = fs[1].split(":");
            var d = new Date(ss[0], ss[1]-1, ss[2], ts[0], ts[1]);
            timeMin2.push(getDateFixed(d, "hour") + ':' + getDateFixed(d, "minute"));
            var fs = item.timeMax.split(" ");
            var ss = fs[0].split("-");
            var ts = fs[1].split(":");
            var d = new Date(ss[0], ss[1], ss[2], ts[0], ts[1]);
            timeMax2.push(getDateFixed(d, "hour") + ':' + getDateFixed(d, "minute"));
        }
    });
}

//tømmer dataarrays
function clearArrays(){
    time = [];
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

//tegner opp grafer. blir kalt opp med JSON.
function drawChart(jsonResponse) {

    populateArrays(jsonResponse);
    data = new google.visualization.DataTable();
    data.addColumn('datetime', 'time');
    data.addColumn('number', 'Stasjon 1');
    data.addColumn({type: 'string', role: 'tooltip', 'p': {'html': true}});
    data.addColumn('number', 'Stasjon 2');
    data.addColumn({type: 'string', role: 'tooltip', 'p': {'html': true}});
    data.addColumn('number', 'Differanse');
    data.addColumn({type: 'string', role: 'tooltip', 'p': {'html': true}});

    for (i = 0; i < time.length; i++) {
        var windDirMax1 = "";
        var windDirMax2 = "";
        if (radioSelected == "wind") {
            windDirMax1 = "<br/>" + tabEntry + tabEntry + "Retning: " + dirMax1[i];
            windDirMax2 = "<br/>" + tabEntry + tabEntry + "Retning: " + dirMax2[i];
        }
        data.addRow([time[i], parseFloat(valAvg1[i]),"<br/>" + tabEntry + "<b>Gjennomsnitt:</b> " + valAvg1[i] +
            mTypeData + "<br/>" + tabEntry + "<b>Maks:</b> " + valMax1[i] + mTypeData + " kl " + timeMax1[i] +
            windDirMax1 + "<br/>" + tabEntry + "<b>Minimum:</b> " + valMin1[i] + mTypeData + " kl " + timeMin1[i],parseFloat(valAvg2[i]),"<br/>" + tabEntry + "<b>Gjennomsnitt:</b> " + valAvg2[i] +
            mTypeData + "<br/>" + tabEntry + "<b>Maks:</b> " + valMax2[i] + mTypeData + " kl " + timeMax2[i] +
            windDirMax2 + "<br/>" + tabEntry + "<b>Minimum:</b> " + valMin2[i] + mTypeData + " kl " + timeMin2[i],
            0,"<br/>"+tabEntry+"<b>Gjennomsnitt: </b><i>"+(valAvg1[i]-valAvg2[i])+mTypeData+"</i><br/>"+tabEntry+"<b>Maks: </b><i>"+(valMax1[i]-valMax2[i])+mTypeData+"</i><br/>"
                +tabEntry+"<b>Min: </b><i>"+(valMin1[i]-valMin2[i])+mTypeData+"</i>"]);
    }
    clearArrays();
    options = {
        title: typeData,
        curveType: 'function',
        legend: { position: 'bottom'},
        hAxis: {showTextEvery: 1},
        focusTarget: 'category',
        series: {2: {lineWidth:1,color:'transparent',areaOpacity: 0.0,visibleInLegend:false}},
        animation: {
            duration: 1000,
            easing: 'in'
        },
        tooltip: { isHtml: true }
    };
    document.getElementById("chart_div").style.width="900px";
    document.getElementById("chart_div").style.overflow="hidden";
    chart.draw(data, options);
}

//deklarer graf globalt for å ikke måtte generere på nytt. Dette fører til mulgiheten for å animerte grafer med async ajax.
var options;
var chart;
var data;

var tabEntry = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
var radioSelected;

var time = [];
var typeData;
var mTypeData;
var valAvg1 = [];
var valMin1 = [];
var timeMin1 = [];
var valMax1 = [];
var timeMax1 = [];
var dirMax1 = [];

var valAvg2 = [];
var valMin2 = [];
var timeMin2 = [];
var valMax2 = [];
var timeMax2 = [];
var dirMax2 = [];


//variabler fra kalendere
var minCalendarDate;
var maxCalendarDate;
var fromDate;
var toDate;

//laster inn grafobjekter asynkront. kjører deretter genChart for å sette opp grafobjektene
google.load("visualization", "1", {packages: ["corechart"]});
google.setOnLoadCallback(genChart);