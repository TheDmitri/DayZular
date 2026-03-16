// DayZular Framework - Message Queuing
// Queue messages for offline players, deliver when they reconnect

class DayZularQueuedMessage
{
    string modNamespace;
    string endpoint;
    ref Param params;
    bool guaranteed;
    string targetPlayerUID;
    float queuedTime;
    float expirySeconds;
    
    void DayZularQueuedMessage()
    {
        queuedTime = GetGame().GetTickTime();
        expirySeconds = 300.0; // 5 minutes default
    }
    
    bool IsExpired()
    {
        return (GetGame().GetTickTime() - queuedTime) > expirySeconds;
    }
}
