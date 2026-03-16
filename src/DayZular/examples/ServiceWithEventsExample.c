// DayZular Framework - Service + Event Bus Example
// Shows the new architecture: Services subscribe to events they need

// ===== EXAMPLE 1: Simple Service with Config =====

class WorkbenchService : DayZularService
{
    private ref WorkbenchConfig m_Config;
    private ref WorkbenchCraftingServiceServer m_CraftingService;
    
    void WorkbenchService()
    {
        // Explicit event subscription - you see exactly what this service needs
        GetDayZularEvents().OnMissionStart.Insert(Initialize);
        GetDayZularEvents().OnPlayerJoined.Insert(OnPlayerJoined);
        GetDayZularEvents().OnPlayerReady.Insert(OnPlayerReady);
        
        // Register RPCs
        RegisterRPCs();
        
        DZLog().LogInfo("WorkbenchService created");
    }
    
    void RegisterRPCs()
    {
        if (GetGame().IsClient())
        {
            GetDayZularRPCManager().AddRPC("Workbench", "ConfigResponse", this, DayZularRPCType.Client);
            GetDayZularRPCManager().AddRPC("Workbench", "CraftStarted", this, DayZularRPCType.Client);
        }
        
        if (GetGame().IsServer())
        {
            GetDayZularRPCManager().AddRPC("Workbench", "StartCraft", this, DayZularRPCType.Server);
            GetDayZularRPCManager().AddRPC("Workbench", "CancelCraft", this, DayZularRPCType.Server);
        }
    }
    
    void Initialize()
    {
        if (GetGame().IsServer())
        {
            m_Config = WorkbenchConfig.Load();
            m_CraftingService = new WorkbenchCraftingServiceServer();
            
            WorkbenchCategory.LoadAllCategories();
            
            DZLog().LogInfo("WorkbenchService initialized");
        }
    }
    
    void OnPlayerJoined(PlayerBase player, PlayerIdentity identity)
    {
        if (GetGame().IsServer())
        {
            DZLog().LogInfo("Sending workbench config to new player: " + identity.GetName());
            
            // Send config to client
            GetDayZularRPCManager().SendRPC("Workbench", "ConfigResponse", new Param1<WorkbenchConfig>(m_Config), true, identity);
            
            // Send categories
            GetDayZularRPCManager().SendRPC("Workbench", "CategoriesResponse", new Param1<array<ref WorkbenchCategory>>(WorkbenchCategory.GetCategories()), true, identity);
        }
    }
    
    void OnPlayerReady(PlayerBase player, PlayerIdentity identity)
    {
        if (GetGame().IsServer())
        {
            // Send active crafts when player is fully ready
            GetDayZularRPCManager().SendRPC("Workbench", "ActiveCraftsResponse", new Param1<array<ref ActiveCraftClient>>(m_CraftingService.GetActiveCrafts()), true, identity);
        }
    }
    
    // RPC Handlers
    void ConfigResponse(PlayerIdentity sender, ParamsReadContext ctx)
    {
        Param1<WorkbenchConfig> data;
        if (!ctx.Read(data))
            return;
        
        m_Config = data.param1;
        DZLog().LogInfo("Config received from server");
    }
    
    void StartCraft(PlayerIdentity sender, ParamsReadContext ctx)
    {
        Param2<ref WorkbenchIdentifier, ref WorkbenchBlueprint> data;
        if (!ctx.Read(data))
            return;
        
        // Handle craft request
    }
}

// ===== EXAMPLE 2: Service with Update Loop =====

class TerritoryMonitorService : DayZularService
{
    private float m_CheckInterval;
    private float m_TimeSinceLastCheck;
    
    void TerritoryMonitorService()
    {
        m_CheckInterval = 30.0;
        m_TimeSinceLastCheck = 0;
        
        // Subscribe to update event
        GetDayZularEvents().OnUpdate.Insert(OnUpdate);
        
        DZLog().LogInfo("TerritoryMonitorService created");
    }
    
    void OnUpdate(float deltaTime)
    {
        if (!GetGame().IsServer())
            return;
        
        m_TimeSinceLastCheck += deltaTime;
        
        if (m_TimeSinceLastCheck >= m_CheckInterval)
        {
            m_TimeSinceLastCheck = 0;
            CheckTerritories();
        }
    }
    
    void CheckTerritories()
    {
        // Monitor territory flags, check expiry, etc.
    }
}

// ===== EXAMPLE 3: Service with Multiple Event Subscriptions =====

class PlayerStatsService : DayZularService
{
    private ref map<string, ref PlayerStats> m_PlayerStats;
    
    void PlayerStatsService()
    {
        m_PlayerStats = new map<string, ref PlayerStats>();
        
        // Subscribe to multiple player events
        GetDayZularEvents().OnPlayerJoined.Insert(OnPlayerJoined);
        GetDayZularEvents().OnPlayerReady.Insert(OnPlayerReady);
        GetDayZularEvents().OnPlayerDisconnected.Insert(OnPlayerDisconnected);
        GetDayZularEvents().OnPlayerRespawn.Insert(OnPlayerRespawn);
        
        DZLog().LogInfo("PlayerStatsService created");
    }
    
    void OnPlayerJoined(PlayerBase player, PlayerIdentity identity)
    {
        if (!GetGame().IsServer())
            return;
        
        string uid = identity.GetPlainId();
        m_PlayerStats[uid] = PlayerStats.Load(uid);
        
        DZLog().LogInfo("Loaded stats for: " + identity.GetName());
    }
    
    void OnPlayerReady(PlayerBase player, PlayerIdentity identity)
    {
        // Send stats to client
    }
    
    void OnPlayerDisconnected(PlayerBase player, PlayerIdentity identity, int logoutTime, bool authFailed)
    {
        if (!GetGame().IsServer())
            return;
        
        string uid = identity.GetPlainId();
        
        PlayerStats stats;
        if (m_PlayerStats.Find(uid, stats))
        {
            stats.Save();
            m_PlayerStats.Remove(uid);
            DZLog().LogInfo("Saved stats for: " + identity.GetName());
        }
    }
    
    void OnPlayerRespawn(PlayerBase player, PlayerIdentity identity)
    {
        // Reset stats on respawn
        string uid = identity.GetPlainId();
        PlayerStats stats;
        if (m_PlayerStats.Find(uid, stats))
        {
            stats.Reset();
        }
    }
}

// ===== EXAMPLE 4: Service Registration in AppModule =====

modded class AppModule
{
    override static void OnRegisterServices()
    {
        super.OnRegisterServices();
        
        DZLog().LogInfo("Registering custom services...");
        
        // Register services - they auto-subscribe to events in their constructors
        DZlrServiceRegistry.GetInstance().RegisterService(WorkbenchService, new WorkbenchService());
        DZlrServiceRegistry.GetInstance().RegisterService(TerritoryMonitorService, new TerritoryMonitorService());
        DZlrServiceRegistry.GetInstance().RegisterService(PlayerStatsService, new PlayerStatsService());
        
        DZLog().LogInfo("Custom services registered");
    }
}
