using Microsoft.CodeAnalysis;

namespace MLLIFCSharpFrontBuild.Serialization;

public class CodeContext(Project project) : List<INamedTypeSymbol>
{
    public Project Project { get; set; } = project;
}
