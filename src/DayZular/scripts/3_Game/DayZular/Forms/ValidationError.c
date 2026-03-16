// DayZular Framework - Validation Error
// Represents a validation error with key and message

class ValidationError
{
    string key;
    string message;
    
    void ValidationError(string k, string msg)
    {
        key = k;
        message = msg;
    }
    
    string GetErrorString()
    {
        return "[" + key + "] " + message;
    }
}
