using Microsoft.CodeAnalysis;

namespace MLLIFCSharpFrontBuild;

public class HeaderGenerator : IGenerator
{
    public IEnumerable<WorkspaceDiagnostic> Run(TextWriter writer, INamespaceSymbol symbol, Project project)
    {
        throw new NotImplementedException();
    }
}