using System.Text;

namespace MLLIFCSharpFrontBuild;

public static class Embedded
{
    public static async Task<string> Resource(string name)
    {
        await using var stream = typeof(Embedded).Assembly.GetManifestResourceStream("MLLIFCSharpFrontBuild.Resources." + name)!;
        using var reader = new StreamReader(stream, Encoding.UTF8);
        return await reader.ReadToEndAsync();
    }
}