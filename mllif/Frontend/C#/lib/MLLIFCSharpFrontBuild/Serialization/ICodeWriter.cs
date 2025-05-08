namespace MLLIFCSharpFrontBuild.Serialization;

public interface ICodeWriter
{
    public void WriteTo(CodeWriter w, CodeContext ctx);
}