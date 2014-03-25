//bruker ajax i jQuery for å parse JSON fra .php-backend
function getJson(url, type) {
    $.ajax({
        url: url,
        type: "post",
        dataType: "json",
        //fyrer callback-funksjon for å sende behandlet data videre.
        success: function (response) {
            if (type == "weather") {
                drawChart(response);
            }
            else if (type == "formFiller") {
                fillForms(response);
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
    weatherFetch();
}

function changedForms(){
    fromDate = $("#from").datepicker('getDate');
    toDate = $("#to").datepicker('getDate');
    $('#from').datepicker('option', 'maxDate', toDate);
    $('#to').datepicker('option', 'minDate', fromDate);
}

//fyller globalt deklarerte variabler
function genChart() {
    chart = new google.visualization.LineChart(document.getElementById('chart_div'));

    //starter prosess for å fylle forms og en standardgraf
    getJson('dbToJson.php?type=getRange','formFiller');
}

//laget for å unngå å endre kallargumenter på flere steder
function weatherFetch() {
    var fromDateStr=fromDate.getFullYear()+"-"+(fromDate.getMonth()+1)+"-"+fromDate.getDate()+" 00:00:00";
    var toDateStr=toDate.getFullYear()+"-"+(toDate.getMonth()+1)+"-"+toDate.getDate()+" 23:59:59";
    getJson('dbToJson.php?type=' + $('input[name=type]:checked').val()+'&from='+fromDateStr+'&to='+toDateStr, "weather");
}

//tegner opp grafer. blir kalt opp med JSON.
function drawChart(jsonResponse) {
    var radioSelected = ($('input[name=type]:checked').val());
    if (radioSelected == "temperature") {
        var typeData = "Temperatur";
        var mTypeData = "&deg;";
    }
    else if (radioSelected == "humidity") {
        var typeData = "Luftfuktighet";
        var mTypeData = "&#37;";
    }
    else if (radioSelected == "pressure") {
        var typeData = "Lufttrykk";
        var mTypeData = " hPa";
    }
    else if (radioSelected == "wind") {
        var typeData = "Vindhastighet";
        var mTypeData = " m/s";
        var dirMax1 = [];
        var dirMax2 = [];
    }
    else {
        alert('Ugyldig valg.');
    }
    var tabEntry = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    var time = [];

    var valAvg1 = [];
    var valMin1 = [];
    var timeMin1 = [];
    var valMax1 = [];
    var timeMax1 = [];

    var valAvg2 = [];
    var valMin2 = [];
    var timeMin2 = [];
    var valMax2 = [];
    var timeMax2 = [];
    $.each(jsonResponse, function (i, item) {
        if (item.station==1){
            valAvg1.push(item.valueAvg);
            valMin1.push(item.valueMin);
            valMax1.push(item.valueMax);
            if (radioSelected == "wind") {
                dirMax1.push(item.dirMax);
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

            /*
            var fs = item.time.split(" ");
            var ss = fs[0].split("-");
            var ts = fs[1].split(":");
            var d = new Date(ss[0], ss[1]-1, ss[2], ts[0], ts[1]);
            time2.push(d);
            fullTime2.push(getDateFixed(d, "date") + '/' + getDateFixed(d, "month") + '/' + getDateFixed(d, "year") + ' ' + getDateFixed(d, "hour") + ':' + getDateFixed(d, "minute"));
            */
        }
    });
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
    chart.draw(data, options);
}

//deklarer graf globalt for å ikke måtte generere på nytt. Dette fører til mulgiheten for å animerte grafer med async ajax.
var options;
var chart;
var data;

//variabler fra kalendere
var minCalendarDate;
var maxCalendarDate;
var fromDate;
var toDate;

//laster inn grafobjekter asynkront. kjører deretter genChart for å sette opp grafobjektene
google.load("visualization", "1", {packages: ["corechart"]});
google.setOnLoadCallback(genChart);