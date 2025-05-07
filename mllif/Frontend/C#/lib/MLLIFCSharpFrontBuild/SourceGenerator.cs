using Microsoft.CodeAnalysis;

namespace MLLIFCSharpFrontBuild;

public class SourceGenerator : IGenerator
{
    public IEnumerable<WorkspaceDiagnostic> Run(TextWriter writer, INamespaceSymbol symbol, Project project)
    {
        throw new NotImplementedException();
    }
}