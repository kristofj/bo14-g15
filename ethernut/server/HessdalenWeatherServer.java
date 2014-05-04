import java.net.*;
import java.io.*;
import java.util.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.lang.Thread;

public class HessdalenWeatherServer {

    public static void main(String a[]) {
        System.out.println("Hessdalen Weather Station - Server");
        serverListener();
    }

    private static void serverListener() {
        DateFormat dateFormat = new SimpleDateFormat("dd/MM/yyyy HH:mm:ss");
        try {
            ServerSocket server = new ServerSocket(3123);
            System.out.println("Server started at " + ((server.getInetAddress()).getLocalHost()).getHostAddress() + ":3123");
            while (true) {
                System.out.println("Ready to receive data.");
                Socket socket = server.accept();
                socket.setSoTimeout(12000);
                BufferedReader in = null;
                PrintStream out = null;
                try {
                    in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                    out = new PrintStream(socket.getOutputStream());
                    Date dateconn = new Date();
                    System.out.println("Connection established at " + dateFormat.format(dateconn) + ".");
                    String rec = in.readLine();
                    Date datedata = new Date();
                    System.out.println("Caught data at " + dateFormat.format(datedata) + ".");
                    System.out.println(rec);
		    DbManager dbManager = new DbManager(rec, socket, out); //Lager ny tråd som håndterer data.
		    new Thread(dbManager).run();
                } catch (SocketTimeoutException s) {
                    System.out.println("Socket timed out!");
                } catch (IOException e) {
                    e.printStackTrace();
                } finally {
                    in.close();
                }

            }
        } catch (Exception err) {
            System.err.println("* err" + err);
        }
    }
}

