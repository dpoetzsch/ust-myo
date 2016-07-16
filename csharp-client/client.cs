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

struct Rotation {
    public readonly double angleX;
    public readonly double angleY;
    public readonly double angleZ;

    public Rotation(double angleX, double angleY, double angleZ) {
        this.angleX = angleX;
        this.angleY = angleY;
        this.angleZ = angleZ;
    }
}

class GrabClient {
    private Action<Extremity, int, Rotation> onGrabbed;
    private Action onDeleted;
    private TcpClient tcpclnt;
    private Thread listenThread;

    public GrabClient(Action<Extremity, int, Rotation> onGrabbed, Action onDeleted) {
        this.onGrabbed = onGrabbed;
        this.onDeleted = onDeleted;
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

    private static Extremity fromInt(int extremity) {
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
                if (stuff.type == "grab") {
                    int likelyhood = stuff.likelyhood;
                    int extremity = stuff.extremity;
                    double angleX = stuff.angleX;
                    double angleY = stuff.angleY;
                    double angleZ = stuff.angleZ;
                    onGrabbed(fromInt(extremity), likelyhood, new Rotation(angleX, angleY, angleZ));
                } else if (stuff.type == "delete") {
                    onDeleted();
                }
            }
        }
    }

    public void Close() {
        tcpclnt.Close();
    }
}
