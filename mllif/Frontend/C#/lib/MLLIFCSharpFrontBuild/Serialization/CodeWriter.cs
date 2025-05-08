using System.Text;

namespace MLLIFCSharpFrontBuild.Serialization;

public class CodeWriter : TextWriter
{
    private TextWriter _writer;
    private bool _tabRequired;
    
    public CodeWriter(TextWriter writer, string tab)
    {
        _writer = writer;
        Tab     = tab;
    }

    public override Encoding Encoding { get; } = Encoding.UTF8;

    private string Tab { get; set; }
    
    public int Indent { get; set; }

    private void WriteIndent()
    {
        for (var i = 0; i < Indent; i++)
            _writer.Write(Tab);
    }
    
    public override void Write(char value)
    {
        if (value == '\r')
            return;
        if (value == '\n')
        {
            _tabRequired = true;
            _writer.Write(value);
            return;
        }

        if (_tabRequired)
        {
            WriteIndent();
            _tabRequired = false;
        }
        
        _writer.Write(value);
    }
}