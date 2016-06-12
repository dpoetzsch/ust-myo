using System;
using System.IO;
using System.Net;
using System.Text;
using System.Net.Sockets;
using System.Threading;
using Newtonsoft.Json;

class GrabClient {
    private Action<int> onGrabbed;
    private TcpClient tcpclnt;
    private Thread listenThread;

    public GrabClient(Action<int> onGrabbed) {
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

    private void Run() {
        using (StreamReader reader = new StreamReader(tcpclnt.GetStream())) {
            while (true) {
                String result = reader.ReadLine();
                dynamic stuff = JsonConvert.DeserializeObject(result);

                // TODO if needed we should check if the timing is fine using the stuff.time
                int likelyhood = stuff.likelyhood;
                onGrabbed(likelyhood);
            }
        }
    }

    public void Close() {
        tcpclnt.Close();
    }
}


class MainClass {
    public static void OnGrabbed(int likelyhood) {
        Console.WriteLine(likelyhood);
    }

    public static void Main() {
        MainClass m = new MainClass();

        GrabClient c = new GrabClient(OnGrabbed);
        c.Connect();
        c.Listen();

        while(true) {
            Thread.Sleep(1000);
        }
    }
}