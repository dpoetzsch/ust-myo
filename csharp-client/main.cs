using System;
using System.Threading;

class MainClass {
    public static void OnGrabbed(Extremity extremity, int likelyhood, Rotation rotation) {
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
        Console.Write(likelyhood);
        Console.Write(" [rotation: ");
        Console.Write(rotation.angleX);
        Console.Write(" ");
        Console.Write(rotation.angleX);
        Console.Write(" ");
        Console.Write(rotation.angleX);
        Console.WriteLine("]");
    }

    public static void onDeleted() {
        Console.WriteLine("Deletion detected");
    }

    public static void Main() {
        GrabClient c = new GrabClient(OnGrabbed, onDeleted);
        c.Connect();
        c.Listen();

        while(true) {
            Thread.Sleep(1000);
        }
    }
}