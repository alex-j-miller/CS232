// Alex Miller

import java.net.*;
import java.io.*;
import java.util.*;

public class CaesarCipherServer extends Thread{
    private ServerSocket serverSocket;
    private Socket clientSocket;
    private PrintWriter out;
    private BufferedReader in;

    private char[] alphabet = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

    private int shift;

    public static void main(String[] args) {
        CaesarCipherServer server = new CaesarCipherServer();
        int port = Integer.parseInt(args[0]);
        System.out.println("Server started at port: " + port);
        server.begin(port);
    }

    public void set(int s, PrintWriter o, BufferedReader i) {
        shift = s;
        out = o;
        in = i;
    }

    public void begin(int port) {
        String greeting = "";
        try {
            serverSocket = new ServerSocket(port);
            clientSocket = serverSocket.accept();
            out = new PrintWriter(clientSocket.getOutputStream(), false);
            in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            //greeting = in.readLine();
            shift = Integer.parseInt(in.readLine());
            System.out.println("parsed Int " + shift);//debug
        } catch (IOException e) {
            System.out.println("Start failed, IOException.");
        }

        System.out.println(greeting);

        //n = Integer.parseInt(greeting);
        //n = Integer.valueOf(greeting);
        //shift = Integer.decode(greeting);
        CaesarCipherServer object = new CaesarCipherServer();
        object.set(shift, out, in);
        object.start();
    }

    public void close() {
        try {
            in.close();
            clientSocket.close();
            serverSocket.close();
        } catch (IOException e) {

        }
        out.close();
    }

    public void run() {
        try {
             // Displaying the thread that is running
            System.out.println("Thread " + Thread.currentThread().getId() + " is running");
        } catch (Exception e) {
            // Throwing an exception
            System.out.println("Exception is caught");
        }

        System.out.println("1, " + shift);//debug

        if (shift > 0 && shift <= 25) {
            out.println(shift);
            out.flush();
            System.out.println("2");//debug
        }
        System.out.println("3");//debug
        

        String input = "";
        String encrypted = "";

        while (true) {
            System.out.println("In da loop"); //debug
            try {
                input = in.readLine();
                System.out.println("Recieved: " + input); //debug
            } catch (IOException e) {
                System.out.println("IOExceoption Error");
            }

            if (input == null) break;

            System.out.println("out da try"); //debug
            out.println(encrypt(input, shift));
            out.flush();
            
        }
        //System.out.println("Finished while loop"); //debug
    }

    private static String encrypt(String message, int shift) {
        StringBuilder sb = new StringBuilder();
        for (char c : message.toCharArray()) {
            if (Character.isLetter(c)) {
                char base = Character.isUpperCase(c) ? 'A' : 'a';
                c = (char) (((c - base + shift) % 26) + base);
            }
            sb.append(c);
        }
        return sb.toString();
    }
}
