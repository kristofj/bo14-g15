import java.net.*;
import java.io.*;

public class Server {
    public Server() {
        while (true){
                try {
                    ServerSocket server = new ServerSocket(3000);
                    System.out.println("Server started at " + ((server.getInetAddress()).getLocalHost()).getHostAddress() + ":3000");
                    //ready to accept client request
                    Socket socket = server.accept();
                    //opening the input stream to read data from client connection
                    BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                    new DbManager(in.readLine());
                    in.close();
                    socket.close();
                } catch (Exception err) {
                    System.err.println("* err" + err);
                }

            }
        }

    public static void main(String a[]) {
        new Server();
    }

}

