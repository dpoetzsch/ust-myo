using System;
using System.IO;
using System.Net;
using System.Text;
using System.Net.Sockets;
using System.Threading;
using Newtonsoft.Json;

enum Extremity {
    HAND_LEFT,
    HAND_RIGHT,
    FOOT_LEFT,
    FOOT_RIGHT
}

class GrabClient {
    private Action<Extremity, int> onGrabbed;
    private TcpClient tcpclnt;
    private Thread listenThread;

    public GrabClient(Action<Extremity, int> onGrabbed) {
        this.onGrabbed = onGrabbed;
    }

    public void Connect(string address = "localhost", int port = 2000) {
        tcpclnt = new TcpClient();

        Console.WriteLine("Connecting...");
        tcpclnt.Connect(address, port);
        
        Console.WriteLine("Connected");
    }

    public void Listen() {
        listenThread = new Thread(new ThreadStart(this.Run));
        listenThread.Start();
    }

    static Extremity fromInt(int extremity) {
        switch (extremity) {
        case 1:
            return Extremity.HAND_LEFT;
        case 2:
            return Extremity.HAND_RIGHT;
        case 3:
            return Extremity.FOOT_LEFT;
        case 4:
            return Extremity.FOOT_RIGHT;
        default:
            throw new ArgumentException();
        }
    }

    private void Run() {
        using (StreamReader reader = new StreamReader(tcpclnt.GetStream())) {
            while (true) {
                String result = reader.ReadLine();
                dynamic stuff = JsonConvert.DeserializeObject(result);

                // TODO if needed we should check if the timing is fine using the stuff.time
                int likelyhood = stuff.likelyhood;
                int extremity = stuff.extremity;
                onGrabbed(fromInt(extremity), likelyhood);
            }
        }
    }

    public void Close() {
        tcpclnt.Close();
    }
}


class MainClass {
    public static void OnGrabbed(Extremity extremity, int likelyhood) {
        switch (extremity) {
        case Extremity.HAND_LEFT:
            Console.Write("Left hand: ");
            break;
        case Extremity.HAND_RIGHT:
            Console.Write("Right hand: ");
            break;
        case Extremity.FOOT_LEFT:
            Console.Write("Left foot: ");
            break;
        case Extremity.FOOT_RIGHT:
            Console.Write("Right foot: ");
            break;
        }
        Console.WriteLine(likelyhood);
    }

    public static void Main() {
        GrabClient c = new GrabClient(OnGrabbed);
        c.Connect();
        c.Listen();

        while(true) {
            Thread.Sleep(1000);
        }
    }
}