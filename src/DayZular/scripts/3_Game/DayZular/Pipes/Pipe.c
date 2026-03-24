// DayZular Framework - Pipe System
// Data transformation pipes (like Angular pipes)

// Base pipe class (renamed to avoid conflict with DayZ's Pipe item)
class DZPipe
{
    string Transform(string value, array<string> args)
    {
        return value;
    }
}

// Uppercase pipe
class UpperCasePipe extends DZPipe
{
    override string Transform(string value, array<string> args)
    {
        string result = value;
        result.ToUpper();
        return result;
    }
}

// Lowercase pipe
class LowerCasePipe extends DZPipe
{
    override string Transform(string value, array<string> args)
    {
        string result = value;
        result.ToLower();
        return result;
    }
}

// Number pipe (format numbers with decimals)
class NumberPipe extends DZPipe
{
    override string Transform(string value, array<string> args)
    {
        float num = value.ToFloat();
        int decimals = 2; // Default 2 decimals
        
        if (args && args.Count() > 0)
        {
            decimals = args[0].ToInt();
        }
        
        // Format with decimals
        string result = num.ToString();
        
        // Simple decimal formatting (DayZ doesn't have advanced formatting)
        if (decimals == 0)
        {
            int intVal = num;
            return intVal.ToString();
        }
        
        return result;
    }
}

// Percent pipe
class PercentPipe extends DZPipe
{
    override string Transform(string value, array<string> args)
    {
        float num = value.ToFloat();
        // Values < 1.0 are decimals (e.g., 0.5 means 50%)
        // Values >= 1.0 are already percentages (e.g., 1 means 1%, 50 means 50%)
        if(num < 1.0)
            return (num * 100).ToString() + "%";

        return num.ToString() + "%";
    }
}

// EUR pipe
class EURPipe extends DZPipe
{
    override string Transform(string value, array<string> args)
    {
        float num = value.ToFloat();
        return "€" + num.ToString();
    }
}

// USD pipe
class USDPipe extends DZPipe
{
    override string Transform(string value, array<string> args)
    {
        float num = value.ToFloat();
        return "$" + num.ToString();
    }
}

// Slice pipe (substring)
// Usage: {{ text | slice:0:10 }} or {{ text | slice:5 }}
class SlicePipe extends DZPipe
{
    override string Transform(string value, array<string> args)
    {
        if (!args || args.Count() == 0)
            return value;
        
        int start = args[0].ToInt();
        int length = value.Length() - start;
        
        if (args.Count() > 1)
        {
            int end = args[1].ToInt();
            length = end - start;
        }
        
        if (start < 0 || start >= value.Length())
            return "";
        
        if (length <= 0)
            return "";
        
        return value.Substring(start, length);
    }
}

// Truncate pipe (limit length with ellipsis)
// Usage: {{ text | truncate:20 }}
class TruncatePipe extends DZPipe
{
    override string Transform(string value, array<string> args)
    {
        int maxLength = 50; // Default
        
        if (args && args.Count() > 0)
        {
            maxLength = args[0].ToInt();
        }
        
        if (value.Length() <= maxLength)
            return value;
        
        return value.Substring(0, maxLength - 3) + "...";
    }
}

// Replace pipe
// Usage: {{ text | replace:old:new }}
class ReplacePipe extends DZPipe
{
    override string Transform(string value, array<string> args)
    {
        if (!args || args.Count() < 2)
            return value;
        
        string oldStr = args[0];
        string newStr = args[1];
        
        string result = value;
        result.Replace(oldStr, newStr);
        return result;
    }
}

// Default pipe (return default if empty)
// Usage: {{ value | default:'N/A' }}
class DefaultPipe extends DZPipe
{
    override string Transform(string value, array<string> args)
    {
        if (value == "" && args && args.Count() > 0)
        {
            return args[0];
        }
        return value;
    }
}

// JSON pipe (format as JSON-like string)
class JsonPipe extends DZPipe
{
    override string Transform(string value, array<string> args)
    {
        return "\"" + value + "\"";
    }
}

// Time pipe (format seconds to MM:SS)
// Usage: {{ seconds | time }} or {{ seconds | time:MM:SS }}
class TimePipe extends DZPipe
{
    override string Transform(string value, array<string> args)
    {
        int totalSeconds = value.ToInt();
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds - (minutes * 60);
        
        string result;
        
        // Format minutes
        if (minutes < 10)
            result = "0" + minutes.ToString();
        else
            result = minutes.ToString();
        
        result = result + ":";
        
        // Format seconds
        if (seconds < 10)
            result = result + "0" + seconds.ToString();
        else
            result = result + seconds.ToString();
        
        return result;
    }
}

// Pipe registry
class PipeRegistry
{
    private static ref map<string, ref DZPipe> s_Pipes;
    
    static void RegisterPipe(string name, DZPipe pipe)
    {
        if (!s_Pipes)
        {
            s_Pipes = new map<string, ref DZPipe>();
        }
        s_Pipes.Set(name, pipe);
    }
    
    static DZPipe GetPipe(string name)
    {
        if (s_Pipes && s_Pipes.Contains(name))
        {
            return s_Pipes.Get(name);
        }
        return null;
    }
    
    static void RegisterDefaultPipes()
    {
        // Text transformation
        RegisterPipe("uppercase", new UpperCasePipe());
        RegisterPipe("lowercase", new LowerCasePipe());
        RegisterPipe("slice", new SlicePipe());
        RegisterPipe("truncate", new TruncatePipe());
        RegisterPipe("replace", new ReplacePipe());
        
        // Number formatting
        RegisterPipe("number", new NumberPipe());
        RegisterPipe("percent", new PercentPipe());
        
        // Currency
        RegisterPipe("eur", new EURPipe());
        RegisterPipe("usd", new USDPipe());
        
        // Utility
        RegisterPipe("default", new DefaultPipe());
        RegisterPipe("json", new JsonPipe());
        RegisterPipe("time", new TimePipe());
        
         DZLog().LogDebug("Default pipes registered (12 pipes)");
    }
}
