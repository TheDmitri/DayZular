// DayZular Framework - Request/Response Pattern
// Async callbacks like fetch().then() - send request, get callback on response

class DayZularPendingRequest
{
    int requestID;
    string callbackMethod;
    Managed callbackInstance;
    float timeoutSeconds;
    float sentTime;
    int retryCount;
    int maxRetries;
    string modNamespace;
    string endpoint;
    ref Param params;
    bool guaranteed;
    PlayerIdentity recipient;
    
    void DayZularPendingRequest()
    {
        sentTime = GetGame().GetTickTime();
        retryCount = 0;
        maxRetries = 3;
        timeoutSeconds = 10.0;
    }
    
    bool IsExpired()
    {
        return (GetGame().GetTickTime() - sentTime) > timeoutSeconds;
    }
    
    bool CanRetry()
    {
        return retryCount < maxRetries;
    }
}
