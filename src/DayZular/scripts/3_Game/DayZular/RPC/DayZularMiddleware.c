// DayZular Framework - Middleware System
// Express.js-style middleware for processing RPCs before/after handling

class DayZularRPCContext
{
    PlayerIdentity sender;
    string modNamespace;
    string endpoint;
    ParamsReadContext readCtx;
    Param writeParams;
    bool isRequest;      // true = incoming request, false = outgoing
    bool isResponse;     // true = this is a response to a request
    int requestID;       // for request/response correlation
    bool handled;        // set to true to stop processing
    bool blocked;        // set to true to block the RPC
    string blockReason;
    
    void DayZularRPCContext()
    {
        handled = false;
        blocked = false;
        isRequest = true;
        isResponse = false;
    }
}

class DayZularMiddleware
{
    // Called before RPC is processed (incoming) or sent (outgoing)
    // Return false to stop the middleware chain
    bool OnBefore(DayZularRPCContext ctx)
    {
        return true; // Continue processing
    }
    
    // Called after RPC is processed
    void OnAfter(DayZularRPCContext ctx)
    {
        // Override for post-processing
    }
}

// ============================================================================
// Built-in Middleware
// ============================================================================

// Logging Middleware
class DayZularLoggingMiddleware extends DayZularMiddleware
{
    override bool OnBefore(DayZularRPCContext ctx)
    {
        string direction = "SEND";
        if (ctx.isRequest)
            direction = "RECV";
        DZLog().LogDebug("RPC " + direction + " " + ctx.modNamespace + "::" + ctx.endpoint);
        return true;
    }
}

// Auth Middleware
class DayZularAuthMiddleware extends DayZularMiddleware
{
    protected ref array<string> m_ProtectedEndpoints = new array<string>();
    
    void AddProtectedEndpoint(string modNamespace, string endpoint)
    {
        m_ProtectedEndpoints.Insert(modNamespace + "::" + endpoint);
    }
    
    override bool OnBefore(DayZularRPCContext ctx)
    {
        if (!ctx.isRequest)
            return true; // Only check incoming requests
        
        string fullName = ctx.modNamespace + "::" + ctx.endpoint;
        
        if (m_ProtectedEndpoints.Find(fullName) != -1)
        {
            if (!ctx.sender)
            {
                ctx.blocked = true;
                ctx.blockReason = "Authentication required";
                DZLog().LogWarning("[RPC AUTH] Blocked unauthenticated request to: " + fullName);
                return false;
            }
        }
        
        return true;
    }
}
