import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;


public class Replier extends Thread {
    private DatagramSocket socket;
    private byte[] buf;
    private int port;
    InetAddress remoteAddress;

 
    public Replier () {
        try {
            buf = new byte[Message.MESSAGE_SIZE];
            port = 5400;
            socket = new DatagramSocket(port);
        } catch (SocketException e) {
            System.out.println(e);
        }
    }

    public void printBytes(byte[] bytes) {
        int j = 0, lineCount = 0;
        System.out.printf("\n%04d: ", ++lineCount);
        for (byte b : bytes) {
            String st = String.format("%02X", b);
            System.out.print(st + " ");
            if (++j == 16) {
                System.out.printf("\n%04d: ", ++lineCount);
                j = 0;
            }
            if (j % 4 == 0) 
                System.out.print("\t"); 
        }
        System.out.println();
    }

    public byte[] getRequest() {
        DatagramPacket packet = new DatagramPacket(buf, buf.length);
        System.out.println("Server ON, waiting for a request\n");
        try {
            socket.receive(packet);
        } catch (IOException e) {
            System.out.println(e);
        }
        remoteAddress = packet.getAddress();
        port = packet.getPort();
        return packet.getData();
    }

    public void sendReply(byte[] reply) {
        buf = reply;
        DatagramPacket packet = new DatagramPacket(buf, buf.length, remoteAddress, port);
        byte[] bytes = packet.getData();
        printBytes(bytes);
        Message msg = new Message(bytes);
        // socket.send(packet);
        socket.close();
    }

    public void run()  {
        byte[] request = getRequest();
        printBytes(request);
        Message reply = new Message(Message.REPLY, 2147483647, Message.DUMMY);
        byte[] response = reply.serialize();
        DatagramPacket pack = new DatagramPacket(response, response.length, remoteAddress, port);
        try {
            socket.send(pack);
        } catch (IOException e) {
            System.out.println(e);
        }
        socket.close();
    }

    public static void main(String[] args) {
        Replier replier = new Replier();
        replier.start();
    }
}