import net.sf.json.JSONException;
import net.sf.json.JSONObject;
import net.sf.json.JSONSerializer;
import java.io.IOException;
import java.io.PrintStream;
import java.net.Socket;

public class DbManager implements Runnable {
    static int entryKey;
    static String mainDatetime;
    static JSONObject json;
    static String jsonString;
    private Socket sock;
    private PrintStream out;

    public DbManager(String json, Socket sock, PrintStream out) {
        jsonString = json;
        this.sock = sock;
        this.out = out;
    }

    public void run() {
        if(jsonParse()){
            makeMainEntry();
            makeTempEntry();
            makeHumidityEntry();
            makePressureEntry();
            makeWindEntry();
        }
        else{
            out.print("Done");
            return;
        }

        out.print("Done");
        try {
            out.close();
            sock.close();
        } catch (IOException E) {

        }
    }

    public static boolean jsonParse() {
        try{
            json = (JSONObject) JSONSerializer.toJSON(jsonString);
            mainDatetime = convD(json.getString("mainDatetime"));
            return true;
        }
        catch (JSONException E){
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

        return year+month+date+hour+minute+seconds;
    }
    public static String addZero(String s){
        if(s.length()==1){
            return s+"0";
        }
        else{
            return s;
        }
    }

    public static void makeMainEntry() {
        int stationId = json.getInt("stationId");
        String sql = "INSERT INTO logId (datetime, stationId) VALUES (" + mainDatetime + "," + stationId + ")";
        Db mainEntry = new Db(sql);
        entryKey = mainEntry.getKey();
    }

    public static void makeTempEntry() {
        JSONObject temp = json.getJSONObject("temp");
        Double avg = temp.getDouble("avg");
        Double now = temp.getDouble("now");
        Double max = temp.getDouble("max");
        Double min = temp.getDouble("min");
        String timeMax = convD(temp.getString("timeMax"));
        String timeMin = convD(temp.getString("timeMin"));
        String sql = "INSERT INTO temperature (`logId_id`, `temperatureAvg`, `temperatureNow`, `temperatureMax`, `timeMax`, `temperatureMin`, `timeMin`) VALUES (" + entryKey + ", " + avg + ", " + now + ", " + max + ", " + timeMax + ", " + min + ", " + timeMin + ");";
        new Db(sql);
    }

    public static void makeHumidityEntry() {
        JSONObject humidity = json.getJSONObject("humidity");
        Double avg = humidity.getDouble("avg");
        Double now = humidity.getDouble("now");
        Double max = humidity.getDouble("max");
        Double min = humidity.getDouble("min");
        String timeMax = convD(humidity.getString("timeMax"));
        String timeMin = convD(humidity.getString("timeMin"));
        String sql = "INSERT INTO humidity (`logId_id`, `humidityAvg`, `humidityNow`, `humidityMax`, `timeMax`, `humidityMin`, `timeMin`) VALUES (" + entryKey + ", " + avg + ", " + now + ", " + max + ", " + timeMax + ", " + min + ", " + timeMin + ");";
        new Db(sql);
    }

    public static void makePressureEntry() {
        JSONObject pressure = json.getJSONObject("pressure");
        Double avg = pressure.getDouble("avg");
        Double now = pressure.getDouble("now");
        Double max = pressure.getDouble("max");
        Double min = pressure.getDouble("min");
        String timeMax = convD(pressure.getString("timeMax"));
        String timeMin = convD(pressure.getString("timeMin"));
        String sql = "INSERT INTO pressure (`logId_id`, `pressureAvg`, `pressureNow`, `pressureMax`, `timeMax`, `pressureMin`, `timeMin`) VALUES (" + entryKey + ", " + avg + ", " + now + ", " + max + ", " + timeMax + ", " + min + ", " + timeMin + ");";
        new Db(sql);
    }

    public static void makeWindEntry() {
        JSONObject wind = json.getJSONObject("wind");
        Double avg = wind.getDouble("avg");
        Double now = wind.getDouble("now");
        Double max = wind.getDouble("max");
        Double min = wind.getDouble("min");
        Double maxDir = wind.getDouble("maxDir");
        String timeMax = convD(wind.getString("timeMax"));
        String timeMin = convD(wind.getString("timeMin"));
        String sql = "INSERT INTO wind (`logId_id`, `windspeedAvg`, `windspeedNow`, `windspeedMax`, `winddirectionMax`, `timeMax`, `windspeedMin`, `timeMin`) VALUES (" + entryKey + ", " + avg + ", " + now + ", " + max + "," + maxDir + ", " + timeMax + ", " + min + ", " + timeMin + ");";
        new Db(sql);
    }
}
