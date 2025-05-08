using System.Runtime.InteropServices;
using MLLIF;

namespace MLLIFCSharpFrontTest;

public partial class Program
{
    public static int Main()
    {
        Console.WriteLine("Hello, World!");

        var prog = new Program();

        var self = GCHandle.Alloc(prog);
        var two  = GCHandle.Alloc("2");
        var r = __mllif__Hello_s32_voidp(GCHandle.ToIntPtr(self), 1, GCHandle.ToIntPtr(two));
        two.Free();
        self.Free();
        
        Console.WriteLine(r);
        
        return 0;
    }

    [Export]
    public int Hello(int a, string b)
    {
        return a + int.Parse(b);
    }
}