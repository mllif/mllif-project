using System.Runtime.InteropServices;
using MLLIF;

namespace MLLIFCSharpFrontTest;

public partial class Program
{
    public static int Main()
    {
        Console.WriteLine("Hello, World!");
        Console.WriteLine(typeof(__delegate__mllif__New).AssemblyQualifiedName);

        var prog = new Program();

        var self = GCHandle.Alloc(prog);
        var r = __mllif__Hello_s32_fp32(GCHandle.ToIntPtr(self), 1, 1.2f);
        self.Free();
        
        Console.WriteLine(r);
        
        return 0;
    }

    [Export]
    public static Program New()
    {
        return new Program();
    }

    [Export]
    public float Hello(int a, float b)
    {
        var ret = a + b;
        Console.Write("Hello from C#: ");
        Console.WriteLine(ret);
        return ret;
    }
}