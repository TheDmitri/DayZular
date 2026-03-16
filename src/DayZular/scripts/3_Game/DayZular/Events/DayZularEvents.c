// DayZular Framework - Event Bus
// Global event system for lifecycle hooks
// Services subscribe to events they need (explicit, flexible)

static ref DayZularEvents g_DayZularEvents;

DayZularEvents GetDayZularEvents()
{
    if (!g_DayZularEvents)
        g_DayZularEvents = new DayZularEvents();
    return g_DayZularEvents;
}

class DayZularEvents
{
    // ===== MISSION LIFECYCLE EVENTS =====
    
    // Invoked when mission starts (after world is loaded)
    // Signature: void OnMissionStart()
    ref ScriptInvoker OnMissionStart;
    
    // Invoked when mission is fully loaded
    // Signature: void OnMissionLoaded()
    ref ScriptInvoker OnMissionLoaded;
    
    // Invoked every frame (services must explicitly subscribe)
    // Signature: void OnUpdate(float deltaTime)
    ref ScriptInvoker OnUpdate;
    
    // Invoked when mission ends
    // Signature: void OnMissionFinish()
    ref ScriptInvoker OnMissionFinish;
    
    // ===== PLAYER LIFECYCLE EVENTS (Server-side) =====
    
    // Invoked when a player joins for the first time
    // Signature: void OnPlayerJoined(PlayerBase player, PlayerIdentity identity)
    // USE THIS: Perfect for sending config to clients!
    ref ScriptInvoker OnPlayerJoined;
    
    // Invoked when a player is ready (OnClientReadyEvent)
    // Signature: void OnPlayerReady(PlayerBase player, PlayerIdentity identity)
    ref ScriptInvoker OnPlayerReady;
    
    // Invoked when a player reconnects (OnClientReconnectEvent)
    // Signature: void OnPlayerReconnect(PlayerBase player, PlayerIdentity identity)
    ref ScriptInvoker OnPlayerReconnect;
    
    // Invoked when a player respawns (OnClientRespawnEvent)
    // Signature: void OnPlayerRespawn(PlayerBase player, PlayerIdentity identity)
    ref ScriptInvoker OnPlayerRespawn;
    
    // Invoked when a player disconnects (OnClientDisconnectedEvent)
    // Signature: void OnPlayerDisconnected(PlayerBase player, PlayerIdentity identity, int logoutTime, bool authFailed)
    ref ScriptInvoker OnPlayerDisconnected;
    
    // Invoked on InvokeOnConnect (early connection)
    // Signature: void OnInvokeConnect(PlayerBase player, PlayerIdentity identity)
    ref ScriptInvoker OnInvokeConnect;
    
    // Invoked on InvokeOnDisconnect (early disconnection)
    // Signature: void OnInvokeDisconnect(PlayerBase player)
    ref ScriptInvoker OnInvokeDisconnect;
    
    // Invoked on OnClientNewEvent (new player creation)
    // Signature: void OnClientNew(PlayerBase player, PlayerIdentity identity, vector pos, ParamsReadContext ctx)
    ref ScriptInvoker OnClientNew;
    
    // Invoked on OnClientPrepareEvent (before player spawns)
    // Signature: void OnClientPrepare(PlayerIdentity identity, bool useDB, vector pos, float yaw, int preloadTimeout)
    ref ScriptInvoker OnClientPrepare;
    
    // ===== CONSTRUCTOR =====
    
    void DayZularEvents()
    {
        // Initialize all invokers
        OnMissionStart = new ScriptInvoker();
        OnMissionLoaded = new ScriptInvoker();
        OnUpdate = new ScriptInvoker();
        OnMissionFinish = new ScriptInvoker();
        
        OnPlayerJoined = new ScriptInvoker();
        OnPlayerReady = new ScriptInvoker();
        OnPlayerReconnect = new ScriptInvoker();
        OnPlayerRespawn = new ScriptInvoker();
        OnPlayerDisconnected = new ScriptInvoker();
        OnInvokeConnect = new ScriptInvoker();
        OnInvokeDisconnect = new ScriptInvoker();
        OnClientNew = new ScriptInvoker();
        OnClientPrepare = new ScriptInvoker();
        
        DZLog().LogInfo("DayZularEvents initialized");
    }
    
    // ===== HELPER METHODS =====
    
    // Note: ScriptInvoker.Count(func fn) counts how many times a specific function is registered
    // There's no way to get total subscriber count without iterating all possible functions
    // So we just log that the event bus is initialized
    void LogSubscriptions()
    {
        DZLog().LogInfo("=== DayZular Event Bus Initialized ===");
        DZLog().LogInfo("Mission events: OnMissionStart, OnMissionLoaded, OnMissionFinish, OnUpdate");
        DZLog().LogInfo("Player events: OnPlayerJoined, OnPlayerReady, OnPlayerReconnect, OnPlayerRespawn, OnPlayerDisconnected");
        DZLog().LogInfo("Advanced events: OnInvokeConnect, OnInvokeDisconnect, OnClientNew, OnClientPrepare");
        DZLog().LogInfo("Services can subscribe to events in their constructors");
    }
}
