import net.sf.json.JSONObject;
import net.sf.json.JSONSerializer;

public class DbManager {
    static int entryKey;
    static String mainDatetime;
    static JSONObject json;
    static String jsonString;

    public DbManager(String json){
        jsonString=json;
        jsonParse();
        makeMainEntry();
        makeTempEntry();
        makeHumidityEntry();
        makePressureEntry();
        makeWindEntry();
    }
    public static void jsonParse(){
        json = (JSONObject) JSONSerializer.toJSON(jsonString);
        mainDatetime = convD(json.getString("mainDatetime"));
    }
    public static String convD(String datetime){
        return datetime.replaceAll("\\W", "");
    }
    public static void makeMainEntry() {
        int stationId = json.getInt("stationId");
        Db mainEntry = new Db("INSERT INTO logId (datetime, stationId) VALUES ("+mainDatetime+","+stationId+")");
        entryKey = mainEntry.getKey();
    }
    public static void makeTempEntry(){
        JSONObject temp = json.getJSONObject("temp");
        Double avg= temp.getDouble("avg");
        Double now = temp.getDouble("now");
        Double max = temp.getDouble("max");
        Double min = temp.getDouble("min");
        String timeMax = convD(temp.getString("timeMax"));
        String timeMin = convD(temp.getString("timeMin"));
        new Db("INSERT INTO temperature (`logId_id`, `temperatureAvg`, `temperatureNow`, `temperatureMax`, `timeMax`, `temperatureMin`, `timeMin`) VALUES ("+entryKey+", "+avg+", "+now+", "+max+", "+timeMax+", "+min+", "+timeMin+");");
    }
    public static void makeHumidityEntry(){
        JSONObject humidity = json.getJSONObject("humidity");
        Double avg= humidity.getDouble("avg");
        Double now = humidity.getDouble("now");
        Double max = humidity.getDouble("max");
        Double min = humidity.getDouble("min");
        String timeMax = convD(humidity.getString("timeMax"));
        String timeMin = convD(humidity.getString("timeMin"));
        new Db("INSERT INTO humidity (`logId_id`, `humidityAvg`, `humidityNow`, `humidityMax`, `timeMax`, `humidityMin`, `timeMin`) VALUES ("+entryKey+", "+avg+", "+now+", "+max+", "+timeMax+", "+min+", "+timeMin+");");
    }
    public static void makePressureEntry(){
        JSONObject pressure = json.getJSONObject("pressure");
        Double avg= pressure.getDouble("avg");
        Double now = pressure.getDouble("now");
        Double max = pressure.getDouble("max");
        Double min = pressure.getDouble("min");
        String timeMax = convD(pressure.getString("timeMax"));
        String timeMin = convD(pressure.getString("timeMin"));
        new Db("INSERT INTO pressure (`logId_id`, `pressureAvg`, `pressureNow`, `pressureMax`, `timeMax`, `pressureMin`, `timeMin`) VALUES ("+entryKey+", "+avg+", "+now+", "+max+", "+timeMax+", "+min+", "+timeMin+");");
    }
    public static void makeWindEntry(){
        JSONObject wind = json.getJSONObject("wind");
        Double avg= wind.getDouble("avg");
        Double now = wind.getDouble("now");
        Double max = wind.getDouble("max");
        Double min = wind.getDouble("min");
        Double maxDir = wind.getDouble("maxDir");
        String timeMax = convD(wind.getString("timeMax"));
        String timeMin = convD(wind.getString("timeMin"));
        new Db("INSERT INTO wind (`logId_id`, `windspeedAvg`, `windspeedNow`, `windspeedMax`, `winddirectionMax`, `timeMax`, `windspeedMin`, `timeMin`) VALUES ("+entryKey+", "+avg+", "+now+", "+max+","+maxDir+", "+timeMax+", "+min+", "+timeMin+");");
    }
}
