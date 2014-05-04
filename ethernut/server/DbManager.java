import net.sf.json.JSONArray;
import net.sf.json.JSONException;
import net.sf.json.JSONObject;
import net.sf.json.JSONSerializer;

import java.io.IOException;
import java.io.PrintStream;
import java.net.Socket;

public class DbManager implements Runnable {
    static int entryKey;
    static String mainDatetime;
    static JSONArray json;
    static String jsonString;
    private Socket sock;
    private PrintStream out;

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

    public static String addZero(String s) {
        if (s.length() == 1) {
            return "0" + s;
        } else {
            return s;
        }
    }

    public static boolean calculateData() {

        for (int i = 0; i < json.size() - 1; i++) {
            try {
                JSONObject j = json.getJSONObject(json.size() - 1);
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
                }
                avgTemp = avgTemp / json.size() - 1;
                avgHumidity = avgHumidity / json.size() - 1;
                avgPressure = avgPressure / json.size() - 1;
                avgWindSpeed = avgWindSpeed / json.size() - 1;
                nowTemp = temp.getDouble("now");
                nowHumidity = humidity.getDouble("now");
                nowPressure = pressure.getDouble("now");
                nowWindSpeed = wind.getDouble("now");
            }
            catch (JSONException e){
                System.out.println(e.toString());
                return false;
            }
        }
        return true;
    }

    public static void makeMainEntry() {
        String sql = "INSERT INTO logId (datetime, stationId) VALUES (" + mainDatetime + "," + stationId + ")";
        Db mainEntry = new Db(sql);
        entryKey = mainEntry.getKey();
    }

    public static void makeTempEntry() {
        String sql = "INSERT INTO temperature (`logId_id`, `temperatureAvg`, `temperatureNow`, `temperatureMax`, `timeMax`, `temperatureMin`, `timeMin`) VALUES (" + entryKey + ", " + avgTemp + ", " + nowTemp + ", " + maxTemp + ", " + convD(maxTempTime) + ", " + minTemp + ", " + convD(minTempTime) + ");";
        new Db(sql);
    }

    public static void makeHumidityEntry() {
        String sql = "INSERT INTO humidity (`logId_id`, `humidityAvg`, `humidityNow`, `humidityMax`, `timeMax`, `humidityMin`, `timeMin`) VALUES (" + entryKey + ", " + avgHumidity + ", " + nowHumidity + ", " + maxHumidity + ", " + convD(maxHumidityTime) + ", " + minHumidity + ", " + convD(minHumidityTime) + ");";
        new Db(sql);
    }

    public static void makePressureEntry() {
        String sql = "INSERT INTO pressure (`logId_id`, `pressureAvg`, `pressureNow`, `pressureMax`, `timeMax`, `pressureMin`, `timeMin`) VALUES (" + entryKey + ", " + avgPressure + ", " + nowPressure + ", " + maxPressure + ", " + convD(maxPressureTime) + ", " + minPressure + ", " + convD(minPressureTime) + ");";
        new Db(sql);
    }

    public static void makeWindEntry() {
        String sql = "INSERT INTO wind (`logId_id`, `windspeedAvg`, `windspeedNow`, `windspeedMax`, `winddirectionMax`, `timeMax`, `windspeedMin`, `timeMin`) VALUES (" + entryKey + ", " + avgWindSpeed + ", " + nowWindSpeed + ", " + maxWindSpeed + "," + maxWindDirection + ", " + convD(maxWindTime) + ", " + minWindSpeed + ", " + convD(minWindTime) + ");";
        new Db(sql);
    }
}
