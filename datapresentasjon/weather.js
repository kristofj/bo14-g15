//bruker ajax i jQuery for å parse JSON fra .php-backend
function getJson(url,type) {
    $.ajax({
        url: url,
        type: "post",
        dataType: "json",
        //fyrer callback-funksjon for å sende behandlet data videre.
        success: function (response) {
            if(type=="weather"){
                drawChart(response);
            }
            else if (type=="formFiller"){
                fillForms(response);
            }
            else{
                alert('You shall not pass!');
            }
        }
    });
}

//fyller forms med info (kanskje bruke en kalender?)
function fillForms(jsonResponse){
    //TODO: lage kode for å fylle forms
}

//fyller globalt deklarerte variabler
function genChart(){
    chart = new google.visualization.LineChart(document.getElementById('chart_div'));

    //starter prosess for å fylle en graf basert på standardverdier i forms
    weatherFetch();
}

//laget for å unngå å endre kallargumenter på flere steder
function weatherFetch(){
    getJson('dbToJson.php?type=' + $('input[name=type]:checked').val(),"weather");
}

//tegner opp grafer. blir kalt opp med JSON.
function drawChart(jsonResponse) {
    var radioSelected = ($('input[name=type]:checked').val());
    if (radioSelected == "temp") {
        var typeData = "Temperatur";
        var mTypeData = "&deg;";
    }
    else if (radioSelected == "humidity") {
        var typeData = "Luftfuktighet";
        var mTypeData = "&#37;";
    }
    else if (radioSelected == "pressure") {
        var typeData = "Lufttrykk";
        var mTypeData = "something?";
    }
    else if (radioSelected == "wind") {
        var typeData = "Vindhastighet";
        var mTypeData = "m/s";
        var dirMax = [];
    }
    else{
        alert('Ugyldig valg.');
    }

    var options = {
        title: 'Trenger dynamisk tittel',
        curveType: 'function',
        legend: { position: 'bottom'},
        animation: {
            duration: 1000,
            easing: 'in'
        },
        tooltip: { isHtml: true }
    };
    var tabEntry = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    var time = [];
    var fullTime = [];
    var station = [];
    var valAvg = [];
    var valMin = [];
    var timeMin = [];
    var valMax = [];
    var timeMax = [];
    $.each(jsonResponse, function (i, item) {
        valAvg.push(item.valueAvg);
        valMin.push(item.valueMin);
        valMax.push(item.valueMax);
        station.push(item.station);
        if (radioSelected == "wind") {
            dirMax.push(item.dirMax);
        }
        var fs = item.time.split(" ");
        var ss = fs[0].split("-");
        var ts = fs[1].split(":");
        var d = new Date(ss[0], ss[1], ss[2], ts[0], ts[1]);
        time.push(d.getDate() + '/' + d.getMonth() + ' ' + d.getHours() + ':' + d.getMinutes());
        fullTime.push(d.getDate() + '/' + d.getMonth() + '/' + d.getYear() + ' ' + d.getHours() + ':' + d.getMinutes());
        var fs = item.timeMin.split(" ");
        var ss = fs[0].split("-");
        var ts = fs[1].split(":");
        var d = new Date(ss[0], ss[1], ss[2], ts[0], ts[1]);
        timeMin.push(d.getHours() + ':' + d.getMinutes());
        var fs = item.timeMax.split(" ");
        var ss = fs[0].split("-");
        var ts = fs[1].split(":");
        var d = new Date(ss[0], ss[1], ss[2], ts[0], ts[1]);
        timeMax.push(d.getHours() + ':' + d.getMinutes());
    });
    var data = new google.visualization.DataTable();
    data.addColumn('string', 'time');
    data.addColumn('number', typeData);
    data.addColumn({type: 'string', role: 'tooltip', 'p': {'html': true}});

    for (i = 0; i < time.length; i++) {
        if (radioSelected == "wind") {
            var windDirMax = "<br/>" + tabEntry + tabEntry + "Retning: " + dirMax[i];
        }
        else {
            var windDirMax = "";
        }
        data.addRow([time[i], parseFloat(valAvg[i]), "<b>Stasjon " + station[i] + "</b><br/>" +
            fullTime[i] + "<br/>" + typeData + "<br/>" + tabEntry + "Gjennomsnitt: " + valAvg[i] +
            mTypeData + "<br/>" + tabEntry + "Maks: " + valMax[i] + mTypeData + " kl " + timeMax[i] +
            windDirMax + "<br/>" + tabEntry + "Minimum: " + valMin[i] + mTypeData + " kl " + timeMin[i]]);
    }
    chart.draw(data, options);
}

//deklarer graf globalt for å ikke måtte generere på nytt. Dette fører til mulgiheten for å animerte grafer med async ajax.
var chart;

//laster inn grafobjekter asynkront. kjører deretter genChart for å sette opp grafobjektene
google.load("visualization", "1", {packages: ["corechart"]});
google.setOnLoadCallback(genChart);