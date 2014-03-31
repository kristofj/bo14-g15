import java.net.*;
import java.io.*;
import java.util.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;

public class HessdalenWeatherServer {

    public static void main(String a[]) {
        System.out.println("Hessdalen Weather Station - Server");
        DateFormat dateFormat = new SimpleDateFormat("dd/MM/yyyy HH:mm:ss");
        try {
            ServerSocket server = new ServerSocket(3000);
            System.out.println("Server started at " + ((server.getInetAddress()).getLocalHost()).getHostAddress() + ":3000");
            while (true) {
                System.out.println("Ready to receive data.");
                Socket socket = server.accept();
                BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                Date date = new Date();
                System.out.println("Caught data at "+dateFormat.format(date)+".");
                new DbManager(in.readLine());
                in.close();
                socket.close();
            }
        } catch (Exception err) {
            System.err.println("* err" + err);
        }


    }
}

