using Microsoft.CodeAnalysis;

namespace MLLIFCSharpFrontBuild.Serialization;

public interface ICodeWriter
{
    public IEnumerable<WorkspaceDiagnostic> WriteTo(CodeWriter w, CodeContext ctx);
}