// DayZular Framework - Rate Limiting
// Prevent spam and abuse with per-endpoint rate limits

class DayZularRateLimitEntry
{
    int maxCalls;
    float windowSeconds;
    ref map<string, ref array<float>> callHistory = new map<string, ref array<float>>();
    
    void DayZularRateLimitEntry(int calls, float window)
    {
        maxCalls = calls;
        windowSeconds = window;
    }
    
    bool IsAllowed(string playerUID)
    {
        float now = GetGame().GetTickTime();
        
        if (!callHistory.Contains(playerUID))
            callHistory[playerUID] = new array<float>();
        
        array<float> history = callHistory[playerUID];
        
        // Remove old entries outside the window
        for (int i = history.Count() - 1; i >= 0; i--)
        {
            if (now - history[i] > windowSeconds)
                history.Remove(i);
        }
        
        // Check if under limit
        if (history.Count() >= maxCalls)
            return false;
        
        // Record this call
        history.Insert(now);
        return true;
    }
}
