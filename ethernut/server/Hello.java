public class Hello {
//Main
String entryKey;
Datetime mainDatetime;

//Temperature
Double tempAvg;
Double tempNow;
Double tempMax;
Datetime tempMaxDatetime;
Double tempMin;
Datetime tempMinDatetime;

//Wind
Double windAvg;
Double windNow;
Double windMax;
Double windMaxDir;
Datetime windMaxDatetime;
Double windMin;
Datetime windMinDatetime;

//Pressure
Double pressureAvg;
Double pressureNow;
Double pressureMax;
Datetime pressureMaxDatetime;
Double pressureMin;
Datetime pressureMinDatetime;

//Humidity
Double humidityAvg;
Double humidityNow;
Double humidityMax;
Datetime humidityMaxDatetime;
Double humidityMin;
Datetime humidityMinDatetime;

	public static void main(String a[])
	{
		/*Format for sending av data:
		main;dateTime:
		temp;min;max;avg;timeMin;timeMax:
		wind;min;max;avg;timeMin;timeMax;maxDirection:
		pressure;min;max;avg;timeMin;timeMax:
		humidity;min;max;avg;timeMin;timeMax
		
		Tidsformat: "YYYY-MM-DD HH:MM:SS"*/
		splitString("");
		makeMainEntry();
		makeTempEntry();
		makeWindEntry();
		makePressureEntry();
		makeHumidityEntry();
	}
	public void makeMainEntry(){
		Db mainEntry = new Db("INSERT INTO logId (datetime) VALUES ("+mainDatetime+")");
		entrykey=mainEntry.getKey();
	}
}
