using Microsoft.CodeAnalysis;

namespace MLLIFCSharpFrontBuild.Diagnostics;

public static class DiagnosticExtension
{
    public static void PrintDiagnostics(this IEnumerable<WorkspaceDiagnostic> diagnostics)
    {
        foreach (var diagnostic in diagnostics)
        {
            if (diagnostic is not BindingDiagnostic)
            {
                var type = diagnostic.GetType();
                Console.Error.WriteLine($"unrecommended diagnostic type: {type.FullName ?? type.Name}");
            }
            
            Console.Error.WriteLine(diagnostic.ToString());
        }
    }
}