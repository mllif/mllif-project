using System.IO.Compression;

namespace MLLIFCSharpFrontBuild;

public static class ZipExtension
{
    public static async Task ExtractAsync(this ZipArchive archive, string entry, string to)
    {
        await using var src = archive.GetEntry(entry)!.Open();
        await using var dst = File.OpenWrite(Path.Combine(to, Path.GetFileName(entry)));
        await src.CopyToAsync(dst);
    }
}