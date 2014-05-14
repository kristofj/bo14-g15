import net.sf.json.JSONArray;
import net.sf.json.JSONException;
import net.sf.json.JSONObject;
import net.sf.json.JSONSerializer;

import java.io.IOException;
import java.io.PrintStream;
import java.net.Socket;

//Parser JSON-array til et objekter, og bygger SQL-spøøringer for innsetting in databasen.
public class DbManager implements Runnable {
    static int entryKey;
    static String mainDatetime;
    static JSONArray json;
    static String jsonString;
    private Socket sock;
    private PrintStream out;

    //globale variabler for å forenkle kommunikasjon mellom funksjoner
    static int stationId;

    static Double maxTemp;
    static String maxTempTime;
    static Double avgTemp;
    static Double minTemp;
    static String minTempTime;
    static Double nowTemp;

    static Double maxHumidity;
    static String maxHumidityTime;
    static Double avgHumidity;
    static Double minHumidity;
    static String minHumidityTime;
    static Double nowHumidity;

    static Double maxPressure;
    static String maxPressureTime;
    static Double avgPressure;
    static Double minPressure;
    static String minPressureTime;
    static Double nowPressure;

    static Double maxWindSpeed;
    static String maxWindTime;
    static Double maxWindDirection;
    static Double avgWindSpeed;
    static Double minWindSpeed;
    static String minWindTime;
    static Double nowWindSpeed;

    public DbManager(String json, Socket sock, PrintStream out) {
        jsonString = json;
        this.sock = sock;
        this.out = out;
    }

    //Gjør funksjonskallene nødvendig for å parse data og bygge SQL.
    //Skriver deretter "Done" på out-stream tilbake til værstasjonen.
    public void run() {
        if (jsonParse() && calculateData()) {
            makeMainEntry();
            makeTempEntry();
            makeHumidityEntry();
            makePressureEntry();
            makeWindEntry();
        } else {
            out.print("Done");
            return;
        }

        out.print("Done");
        try {
            out.close();
            sock.close();
        } catch (IOException E) {
            System.out.println(E.toString());
        }
    }

    //bygger JSON-objekt av siste element i array for å hente ut stasjonsID og tidspunktet det ble sendt på.
    public static boolean jsonParse() {
        try {
            json = (JSONArray) JSONSerializer.toJSON(jsonString);
            JSONObject j = json.getJSONObject(json.size() - 1);
            mainDatetime = convD(j.getString("mainDatetime"));
            stationId = j.getInt("stationId");
            return true;
        } catch (JSONException E) {
            System.out.println(E.toString());
            return false;
        }

    }

    //"convD" står for "Convert Date".
    //laget for å gjøre slik at datoer blir riktige formaterte for SQL (innkludere nuller)
    public static String convD(String datetime) {
        String[] parts1 = datetime.split(" ");
        String[] parts2 = parts1[0].split("-");
        String year = addZero(parts2[0]);
        String month = addZero(parts2[1]);
        String date = addZero(parts2[2]);

        String[] parts3 = parts1[1].split(":");
        String hour = addZero(parts3[0]);
        String minute = addZero(parts3[1]);
        String seconds = addZero(parts3[2]);

        return year + month + date + hour + minute + seconds;
    }

    //laget for å forenkle funksjonen convD ved å legge til null dersom verdi er på ett siffer
    public static String addZero(String s) {
        if (s.length() == 1) {
            return "0" + s;
        } else {
            return s;
        }
    }

    //bruker alle elementene i JSON-arrayen for å regne ut hvilke timesverdier som skal inn i databasen.
    //setter så de relevante verdiene i globale variabler
    public static boolean calculateData() {
        System.out.println(json.size());
        for (int i = 0; i < json.size(); i++) {
            try {
                JSONObject j = json.getJSONObject(i);
                JSONObject temp = j.getJSONObject("temp");
                JSONObject humidity = j.getJSONObject("humidity");
                JSONObject pressure = j.getJSONObject("pressure");
                JSONObject wind = j.getJSONObject("wind");
                if (i == 0) {
                    //temp
                    avgTemp = temp.getDouble("avg");
                    maxTemp = temp.getDouble("max");
                    maxTempTime = temp.getString("timeMax");
                    minTemp = temp.getDouble("min");
                    minTempTime = temp.getString("timeMin");

                    //humidity
                    avgHumidity = humidity.getDouble("avg");
                    maxHumidity = humidity.getDouble("max");
                    maxHumidityTime = humidity.getString("timeMax");
                    minHumidity = humidity.getDouble("min");
                    minHumidityTime = humidity.getString("timeMin");

                    //pressure
                    avgPressure = pressure.getDouble("avg");
                    maxPressure = pressure.getDouble("max");
                    maxPressureTime = pressure.getString("timeMax");
                    minPressure = pressure.getDouble("min");
                    minPressureTime = pressure.getString("timeMin");

                    //wind
                    avgWindSpeed = wind.getDouble("avg");
                    maxWindSpeed = wind.getDouble("max");
                    maxWindTime = wind.getString("timeMax");
                    minWindSpeed = wind.getDouble("min");
                    minWindTime = wind.getString("timeMin");
                    maxWindDirection = wind.getDouble("maxDir");
                } else {
                    avgTemp += temp.getDouble("avg");
                    avgHumidity += humidity.getDouble("avg");
                    avgPressure += pressure.getDouble("avg");
                    avgWindSpeed += wind.getDouble("avg");
                    if (temp.getDouble("max") > maxTemp) {
                        maxTemp = temp.getDouble("max");
                        maxTempTime = temp.getString("timeMax");
                    }
                    if (temp.getDouble("min") < minTemp) {
                        minTemp = temp.getDouble("min");
                        minTempTime = temp.getString("timeMin");
                    }
                    if (humidity.getDouble("max") > maxHumidity) {
                        maxHumidity = humidity.getDouble("max");
                        maxHumidityTime = humidity.getString("timeMax");
                    }
                    if (humidity.getDouble("min") < minHumidity) {
                        minHumidity = humidity.getDouble("min");
                        minHumidityTime = humidity.getString("timeMin");
                    }
                    if (pressure.getDouble("max") > maxPressure) {
                        maxPressure = pressure.getDouble("max");
                        maxPressureTime = pressure.getString("timeMax");
                    }
                    if (pressure.getDouble("min") < minPressure) {
                        minPressure = pressure.getDouble("min");
                        minPressureTime = pressure.getString("timeMin");
                    }
                    if (wind.getDouble("max") > maxWindSpeed) {
                        maxWindSpeed = wind.getDouble("max");
                        maxWindDirection = wind.getDouble("maxDir");
                        maxWindTime = wind.getString("timeMax");
                    }
                    if (wind.getDouble("min") < minWindSpeed) {
                        minWindSpeed = wind.getDouble("min");
                        minWindTime = wind.getString("timeMin");
                    }
                    nowTemp = temp.getDouble("now");
                    nowHumidity = humidity.getDouble("now");
                    nowPressure = pressure.getDouble("now");
                    nowWindSpeed = wind.getDouble("now");
                }
            }
            catch (JSONException e){
                System.out.println(e.toString());
                return false;
            }
        }
        avgTemp = avgTemp / json.size();
        avgHumidity = avgHumidity / json.size();
        avgPressure = avgPressure / json.size();
        avgWindSpeed = avgWindSpeed / json.size();
        return true;
    }

    //setter inn i databasetabellen logId, og henter ut hvilken nøkkel MySQL genererte
    public static void makeMainEntry() {
        String sql = "INSERT INTO logId (datetime, stationId) VALUES (" + mainDatetime + "," + stationId + ")";
        Db mainEntry = new Db(sql);
        entryKey = mainEntry.getKey();
    }

    //setter inn i databasetabellen temperature
    public static void makeTempEntry() {
        String sql = "INSERT INTO temperature (`logId_id`, `temperatureAvg`, `temperatureNow`, `temperatureMax`, `timeMax`, `temperatureMin`, `timeMin`) VALUES (" + entryKey + ", " + avgTemp + ", " + nowTemp + ", " + maxTemp + ", " + convD(maxTempTime) + ", " + minTemp + ", " + convD(minTempTime) + ");";
        new Db(sql);
    }

    //setter inn i databasetabellen humidity
    public static void makeHumidityEntry() {
        String sql = "INSERT INTO humidity (`logId_id`, `humidityAvg`, `humidityNow`, `humidityMax`, `timeMax`, `humidityMin`, `timeMin`) VALUES (" + entryKey + ", " + avgHumidity + ", " + nowHumidity + ", " + maxHumidity + ", " + convD(maxHumidityTime) + ", " + minHumidity + ", " + convD(minHumidityTime) + ");";
        new Db(sql);
    }

    //setter inn i databasetabellen pressure
    public static void makePressureEntry() {
        String sql = "INSERT INTO pressure (`logId_id`, `pressureAvg`, `pressureNow`, `pressureMax`, `timeMax`, `pressureMin`, `timeMin`) VALUES (" + entryKey + ", " + avgPressure + ", " + nowPressure + ", " + maxPressure + ", " + convD(maxPressureTime) + ", " + minPressure + ", " + convD(minPressureTime) + ");";
        new Db(sql);
    }

    //setter inn i databasetabellen wind
    public static void makeWindEntry() {
        String sql = "INSERT INTO wind (`logId_id`, `windspeedAvg`, `windspeedNow`, `windspeedMax`, `winddirectionMax`, `timeMax`, `windspeedMin`, `timeMin`) VALUES (" + entryKey + ", " + avgWindSpeed + ", " + nowWindSpeed + ", " + maxWindSpeed + "," + maxWindDirection + ", " + convD(maxWindTime) + ", " + minWindSpeed + ", " + convD(minWindTime) + ");";
        new Db(sql);
    }
}
