// DayZular Framework - RPC Manager (Global Singleton)
// Main RPC manager with all advanced features integrated

// Global singleton accessor (like CF's GetRPCManager())
static ref DayZularRPCManagerGlobal g_DayZularRPCManager;

DayZularRPCManagerGlobal GetDayZularRPCManager()
{
    if (!g_DayZularRPCManager)
        g_DayZularRPCManager = new DayZularRPCManagerGlobal();
    return g_DayZularRPCManager;
}

class DayZularRPCManagerGlobal
{
    // Registered RPCs: namespace -> (endpoint -> handler instance)
    protected ref map<string, ref map<string, Managed>> m_RPCHandlers = new map<string, ref map<string, Managed>>();
    
    // ===== ADVANCED FEATURES =====
    
    // Middleware chain
    protected ref array<ref DayZularMiddleware> m_Middleware = new array<ref DayZularMiddleware>();
    
    // Request/Response tracking
    protected ref map<int, ref DayZularPendingRequest> m_PendingRequests = new map<int, ref DayZularPendingRequest>();
    protected int m_NextRequestID = 1;
    
    // Rate limiting
    protected ref map<string, ref DayZularRateLimitEntry> m_RateLimits = new map<string, ref DayZularRateLimitEntry>();
    
    // Message queue (for offline players)
    protected ref map<string, ref array<ref DayZularQueuedMessage>> m_MessageQueue = new map<string, ref array<ref DayZularQueuedMessage>>();
    
    // Reserved RPC IDs for internal use
    static const int RPC_RESPONSE = -1;
    static const int RPC_ACK = -2;
    
    void DayZularRPCManagerGlobal()
    {
        // Start the update loop for timeouts and retries
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ProcessPendingRequests, 1000, true);
    }
    
    // ===== MIDDLEWARE SYSTEM =====
    
    // Add middleware to the chain (like Express app.use())
    void Use(DayZularMiddleware middleware)
    {
        m_Middleware.Insert(middleware);
        DZLog().LogDebug("RPC: Middleware added - " + middleware.ClassName());
    }
    
    // Run middleware chain
    protected bool RunMiddlewareBefore(DayZularRPCContext ctx)
    {
        int i;
        for (i = 0; i < m_Middleware.Count(); i++)
        {
            if (!m_Middleware[i].OnBefore(ctx))
                return false;
            if (ctx.blocked)
                return false;
        }
        return true;
    }
    
    protected void RunMiddlewareAfter(DayZularRPCContext ctx)
    {
        for (int i = m_Middleware.Count() - 1; i >= 0; i--)
        {
            m_Middleware[i].OnAfter(ctx);
        }
    }
    
    // ===== RATE LIMITING =====
    
    // Set rate limit for an endpoint
    void SetRateLimit(string modNamespace, string endpoint, int maxCalls, float windowSeconds)
    {
        string fullName = modNamespace + "::" + endpoint;
        m_RateLimits[fullName] = new DayZularRateLimitEntry(maxCalls, windowSeconds);
        DZLog().LogDebug("RPC: Rate limit configured - " + fullName);
    }
    
    // Check rate limit
    protected bool CheckRateLimit(string fullName, PlayerIdentity sender)
    {
        DayZularRateLimitEntry limit;
        if (!m_RateLimits.Find(fullName, limit))
            return true; // No limit set
        
        string playerUID = "";
        if (sender)
            playerUID = sender.GetPlainId();
        
        if (!limit.IsAllowed(playerUID))
        {
            DZLog().LogWarning("DayZularRPC: Rate limit exceeded for " + fullName + " by " + playerUID);
            return false;
        }
        
        return true;
    }
    
    // ===== REQUEST/RESPONSE PATTERN =====
    
    // Send request with callback (returns request ID for tracking)
    int SendRequest(string modNamespace, string endpoint, Param params, Managed callbackInstance, string callbackMethod, float timeoutSeconds, int maxRetries, PlayerIdentity recipient = NULL)
    {
        int requestID = m_NextRequestID;
        m_NextRequestID = m_NextRequestID + 1;
        
        DayZularPendingRequest pending = new DayZularPendingRequest();
        pending.requestID = requestID;
        pending.callbackMethod = callbackMethod;
        pending.callbackInstance = callbackInstance;
        pending.timeoutSeconds = timeoutSeconds;
        pending.sentTime = GetGame().GetTickTime();
        pending.retryCount = 0;
        pending.maxRetries = maxRetries;
        pending.recipient = recipient;
        pending.modNamespace = modNamespace;
        pending.endpoint = endpoint;
        pending.params = params;
        pending.guaranteed = true;
        
        m_PendingRequests[requestID] = pending;
        
        // Send request with metadata
        auto sendData = new array<ref Param>;
        sendData.Insert(new Param2<string, string>(modNamespace, endpoint));
        sendData.Insert(new Param1<int>(requestID));
        if (params)
            sendData.Insert(params);
        
        GetGame().RPC(NULL, DAYZULAR_RPC_UNTARGETED, sendData, true, recipient);
        
        DZLog().LogDebug("RPC: Request sent #" + requestID);
        
        return requestID;
    }
    
    // Simplified SendRequest with defaults
    int SendRequest(string modNamespace, string endpoint, Param params, Managed callbackInstance, string callbackMethod)
    {
        return SendRequest(modNamespace, endpoint, params, callbackInstance, callbackMethod, 10.0, 3);
    }
    
    // Send response to a request
    void SendResponse(int requestID, Param responseParams, PlayerIdentity recipient)
    {
        auto sendData = new array<ref Param>;
        sendData.Insert(new Param2<string, string>("__internal", "__response"));
        sendData.Insert(new Param1<int>(requestID));
        if (responseParams)
            sendData.Insert(responseParams);
        
        GetGame().RPC(NULL, DAYZULAR_RPC_UNTARGETED, sendData, true, recipient);
        
        DZLog().LogDebug("RPC: Response sent #" + requestID);
    }
    
    // Process pending requests (timeouts and retries)
    protected void ProcessPendingRequests()
    {
        if (!GetGame())
            return;
        
        array<int> toRemove = new array<int>();
        
        foreach (int reqID, DayZularPendingRequest pending : m_PendingRequests)
        {
            if (pending.IsExpired())
            {
                if (pending.CanRetry())
                {
                    // Retry with exponential backoff
                    pending.retryCount = pending.retryCount + 1;
                    pending.sentTime = GetGame().GetTickTime();
                    pending.timeoutSeconds = pending.timeoutSeconds * 2; // Exponential backoff
                    
                    DZLog().LogDebug("RPC: Retry #" + reqID + " (" + pending.retryCount + ")");
                    
                    // Resend with metadata
                    auto resendData = new array<ref Param>;
                    resendData.Insert(new Param2<string, string>(pending.modNamespace, pending.endpoint));
                    resendData.Insert(new Param1<int>(reqID));
                    if (pending.params)
                        resendData.Insert(pending.params);
                    
                    GetGame().RPC(NULL, DAYZULAR_RPC_UNTARGETED, resendData, true, pending.recipient);
                }
                else
                {
                    // Max retries exceeded - call callback with failure
                    DZLog().LogWarning("DayZularRPC: Request #" + reqID + " timed out after " + pending.retryCount + " retries");
                    
                    if (pending.callbackInstance && pending.callbackMethod != "")
                    {
                        auto failParams = new Param2<bool, Param>(false, NULL);
                        GetGame().GameScript.CallFunctionParams(pending.callbackInstance, pending.callbackMethod, NULL, failParams);
                    }
                    
                    toRemove.Insert(reqID);
                }
            }
        }
        
        foreach (int removeID : toRemove)
        {
            m_PendingRequests.Remove(removeID);
        }
    }
    
    // Handle response
    protected void HandleResponse(int requestID, ParamsReadContext ctx)
    {
        DayZularPendingRequest pending;
        if (!m_PendingRequests.Find(requestID, pending))
        {
            DZLog().LogWarning("DayZularRPC: Received response for unknown request #" + requestID);
            return;
        }
        
        DZLog().LogDebug("RPC: Response received #" + requestID);
        
        // Call the callback with success
        if (pending.callbackInstance && pending.callbackMethod != "")
        {
            auto successParams = new Param2<bool, ParamsReadContext>(true, ctx);
            GetGame().GameScript.CallFunctionParams(pending.callbackInstance, pending.callbackMethod, NULL, successParams);
        }
        
        m_PendingRequests.Remove(requestID);
    }
    
    // ===== MESSAGE QUEUING =====
    
    // Queue a message for an offline player
    void QueueMessage(string targetPlayerUID, string modNamespace, string endpoint, Param params, float expirySeconds)
    {
        if (!m_MessageQueue.Contains(targetPlayerUID))
            m_MessageQueue[targetPlayerUID] = new array<ref DayZularQueuedMessage>();
        
        DayZularQueuedMessage msg = new DayZularQueuedMessage();
        msg.targetPlayerUID = targetPlayerUID;
        msg.modNamespace = modNamespace;
        msg.endpoint = endpoint;
        msg.params = params;
        msg.expirySeconds = expirySeconds;
        
        m_MessageQueue[targetPlayerUID].Insert(msg);
        
        DZLog().LogDebug("RPC: Message queued for player");
    }
    
    // Flush queued messages when player connects
    void FlushQueuedMessages(PlayerIdentity player)
    {
        if (!player)
            return;
        
        string playerUID = player.GetPlainId();
        
        array<ref DayZularQueuedMessage> queue;
        if (!m_MessageQueue.Find(playerUID, queue))
            return;
        
        DZLog().LogDebug("RPC: Flushing " + queue.Count() + " queued messages");
        
        for (int i = queue.Count() - 1; i >= 0; i--)
        {
            DayZularQueuedMessage msg = queue[i];
            
            if (msg.IsExpired())
            {
                queue.Remove(i);
                continue;
            }
            
            // Send the queued message
            SendRPC(msg.modNamespace, msg.endpoint, msg.params, true, player);
            queue.Remove(i);
        }
        
        if (queue.Count() == 0)
            m_MessageQueue.Remove(playerUID);
    }
    
    // ===== CORE RPC METHODS =====
    
    // Add RPC endpoint (like CF's AddRPC)
    void AddRPC(string modNamespace, string endpoint, Managed instance, DayZularRPCType type)
    {
        if (!m_RPCHandlers.Contains(modNamespace))
        {
            m_RPCHandlers.Set(modNamespace, new map<string, Managed>());
        }
        
        m_RPCHandlers[modNamespace].Set(endpoint, instance);
        
        DZLog().LogDebug("RPC: Registered " + modNamespace + "::" + endpoint);
    }
    
    // Send RPC (like CF's SendRPC)
    void SendRPC(string modNamespace, string endpoint, Param params, bool guaranteed, PlayerIdentity recipient)
    {
        string recipientName = "ALL";
        if (recipient)
            recipientName = recipient.GetName();
        DZLog().LogDebug("RPC: Send " + modNamespace + "::" + endpoint);
        
        // Use CF's approach - send metadata in payload
        auto sendData = new array<ref Param>;
        sendData.Insert(new Param2<string, string>(modNamespace, endpoint));
        if (params)
            sendData.Insert(params);
        
        GetGame().RPC(NULL, DAYZULAR_RPC_UNTARGETED, sendData, guaranteed, recipient);
    }
    
    // Send RPC to specific target object
    void SendRPCToTarget(string modNamespace, string endpoint, Object target, Param params, bool guaranteed, PlayerIdentity recipient)
    {
        DZLog().LogDebug("RPC: SendToTarget " + modNamespace + "::" + endpoint);
        
        auto sendData = new array<ref Param>;
        sendData.Insert(new Param2<string, string>(modNamespace, endpoint));
        if (params)
            sendData.Insert(params);
        
        GetGame().RPC(target, DAYZULAR_RPC_TARGETED, sendData, guaranteed, recipient);
    }
    
    // Handle incoming RPC (called from modded DayZGame.OnRPC)
    bool OnRPC(PlayerIdentity sender, Object target, ParamsReadContext ctx)
    {
        #ifdef SERVER
        if (!sender)
            return false;
        #endif
        
        // Read metadata from payload (like CF does)
        Param2<string, string> metadata;
        if (!ctx.Read(metadata))
        {
            DZLog().LogError("DayZularRPC: Couldn't read RPC metadata");
            return false;
        }
        
        string modNamespace = metadata.param1;
        string endpoint = metadata.param2;
        string fullName = modNamespace + "::" + endpoint;
        
        // Handle internal response
        if (modNamespace == "__internal" && endpoint == "__response")
        {
            Param1<int> requestIDParam;
            if (ctx.Read(requestIDParam))
                HandleResponse(requestIDParam.param1, ctx);
            return true;
        }
        
        // ===== RATE LIMITING =====
        if (!CheckRateLimit(fullName, sender))
        {
            DZLog().LogWarning("DayZularRPC: Rate limited: " + fullName);
            return false;
        }
        
        // ===== MIDDLEWARE (Before) =====
        DayZularRPCContext rpcCtx = new DayZularRPCContext();
        rpcCtx.sender = sender;
        rpcCtx.modNamespace = modNamespace;
        rpcCtx.endpoint = endpoint;
        rpcCtx.readCtx = ctx;
        rpcCtx.isRequest = true;
        
        if (!RunMiddlewareBefore(rpcCtx))
        {
            if (rpcCtx.blocked)
                DZLog().LogWarning("DayZularRPC: Blocked by middleware: " + rpcCtx.blockReason);
            return false;
        }
        
        // ===== CALL HANDLER =====
        map<string, Managed> endpoints;
        if (!m_RPCHandlers.Find(modNamespace, endpoints))
        {
            DZLog().LogError("DayZularRPC: No handlers registered for namespace: " + modNamespace);
            return false;
        }
        
        Managed handler;
        if (!endpoints.Find(endpoint, handler))
        {
            DZLog().LogError("DayZularRPC: No handler for: " + fullName);
            return false;
        }
        
        if (!handler)
        {
            DZLog().LogError("DayZularRPC: Handler is null for: " + fullName);
            return false;
        }
        
        DZLog().LogDebug("RPC: Calling " + fullName);
        
        // Call the handler with (PlayerIdentity sender, ParamsReadContext ctx) signature
        auto callParams = new Param2<PlayerIdentity, ParamsReadContext>(sender, ctx);
        GetGame().GameScript.CallFunctionParams(handler, endpoint, NULL, callParams);
        
        // ===== MIDDLEWARE (After) =====
        RunMiddlewareAfter(rpcCtx);
        
        return true;
    }
}
