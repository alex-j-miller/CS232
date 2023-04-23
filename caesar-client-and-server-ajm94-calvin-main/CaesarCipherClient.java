// Alex Miller

import java.net.Socket;
import java.io.DataOutput;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.net.UnknownHostException;
import java.util.Scanner;

import java.net.*;
import java.io.*;


public class CaesarCipherClient {
    private Socket clientSocket;
    private PrintWriter out;
    private BufferedReader in;
    private static int Rotation;

    public static void main(String[] args) {
        System.out.print("Hello World!\n");
        CaesarCipherClient c = new CaesarCipherClient();

        if (args.length < 2) {
            c.run("cs232.cs.calvin.edu", 9876);
        } else {
            c.run(args[0], Integer.parseInt(args[1]));
        }

        c.stopConnection();
    }

    public void run(String ip, int port) {
        Scanner scanner = new Scanner(System.in);
        System.out.print("How many rotations?\n");
        Rotation = scanner.nextInt(); // get the number of rotations
        startConnection(ip, port);
        String resp = sendMessage(String.valueOf(Rotation));
        System.out.println("Server Response: " + resp);
        String usr = "";

        while (true) {
            usr = scanner.nextLine();
            // System.out.println("User Input: " + usr);
            if (usr.equals("quit")) break;
            resp = sendMessage(usr);
            System.out.println("Server Response: " + resp);
        }
    }

    public void startConnection(String ip, int port) {
        try {
            try {
                clientSocket = new Socket(ip, port);
                out = new PrintWriter(clientSocket.getOutputStream(), true);
                in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            } catch (UnknownHostException uk) { 
                System.out.print("startConnection failed, UnknownHostException.\n");
            }
        } catch (IOException io) {
            System.out.print("startConnection failed, IOException.\n");
        }
    }

    public String sendMessage(String msg) {
        out.println(msg);
        String resp = "";
        try {
            resp = in.readLine();
        } catch (IOException e) {
            System.out.print("sendMessage failed, IOExcetion.\n");
        }
        return resp;
    }

    public void stopConnection() {
        try {
            in.close();
            clientSocket.close();
        } catch (IOException e) {
            System.out.print("stopConnection failed, IOExcetion.\n");
        }
        out.close();
        
    }

}