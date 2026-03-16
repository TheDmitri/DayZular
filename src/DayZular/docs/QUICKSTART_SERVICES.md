# DayZular Services - Quick Start Guide

## The DayZular Way: Services + Events

**No module base class.** Services subscribe to events they need.

## 3-Step Setup

### Step 1: Create Your Service

```c
class WorkbenchService : DayZularService
{
    private ref WorkbenchConfig m_Config;
    
    void WorkbenchService()
    {
        // Explicit event subscription - crystal clear what this service needs
        GetDayZularEvents().OnMissionStart.Insert(Initialize);
        GetDayZularEvents().OnPlayerJoined.Insert(OnPlayerJoined);
        
        // Register RPCs
        if (GetGame().IsClient())
        {
            GetDayZularRPCManager().AddRPC("Workbench", "ConfigResponse", this, DayZularRPCType.Client);
        }
        if (GetGame().IsServer())
        {
            GetDayZularRPCManager().AddRPC("Workbench", "StartCraft", this, DayZularRPCType.Server);
        }
    }
    
    void Initialize()
    {
        if (GetGame().IsServer())
        {
            m_Config = WorkbenchConfig.Load();
        }
    }
    
    void OnPlayerJoined(PlayerBase player, PlayerIdentity identity)
    {
        if (GetGame().IsServer())
        {
            // Perfect place to send config!
            GetDayZularRPCManager().SendRPC("Workbench", "ConfigResponse", new Param1<WorkbenchConfig>(m_Config), true, identity);
        }
    }
    
    // RPC Handler
    void ConfigResponse(PlayerIdentity sender, ParamsReadContext ctx)
    {
        Param1<WorkbenchConfig> data;
        if (!ctx.Read(data))
            return;
        
        m_Config = data.param1;
    }
}
```

### Step 2: Register Your Service

```c
// In your mod, subscribe to AppModule's registration event
class MyModAppInit
{
    void MyModAppInit()
    {
        // Subscribe to service registration event
        AppModule.GetOnRegisterServices().Insert(RegisterServices);
    }
    
    static void RegisterServices()
    {
        auto registry = DZlrServiceRegistry.GetInstance();
        
        // Register services - they auto-subscribe to events in their constructors
        registry.RegisterService(WorkbenchService, new WorkbenchService());
        registry.RegisterService(TerritoryService, new TerritoryService());
    }
}

// Auto-initialize on script load
MyModAppInit g_MyModAppInit = new MyModAppInit();
```

### Step 3: Access Your Service

```c
// Option A: Via service registry (recommended)
WorkbenchService service = WorkbenchService.Cast(DZlrServiceRegistry.GetInstance().GetService(WorkbenchService));

// Option B: Via global accessor (convenient)
static ref WorkbenchService g_WorkbenchService;

WorkbenchService GetWorkbenchService()
{
    if (!g_WorkbenchService)
        g_WorkbenchService = WorkbenchService.Cast(DZlrServiceRegistry.GetInstance().GetService(WorkbenchService));
    return g_WorkbenchService;
}
```

## Available Events

### Mission Lifecycle
- `OnMissionStart` - Load configs, initialize services
- `OnMissionLoaded` - Post-initialization
- `OnUpdate` - Every frame (opt-in)
- `OnMissionFinish` - Cleanup, save data

### Player Lifecycle (Server-side)
- **`OnPlayerJoined`** - First-time join ← **Use this for config!**
- `OnPlayerReady` - Player fully spawned
- `OnPlayerReconnect` - Player reconnects
- `OnPlayerRespawn` - Player respawns
- `OnPlayerDisconnected` - Player leaves
- `OnInvokeConnect` - Early connection
- `OnInvokeDisconnect` - Early disconnection
- `OnClientNew` - New player created
- `OnClientPrepare` - Before spawn

## Common Patterns

### Pattern: Config Distribution
```c
void MyService()
{
    GetDayZularEvents().OnMissionStart.Insert(LoadConfig);
    GetDayZularEvents().OnPlayerJoined.Insert(SendConfig);
}

void LoadConfig()
{
    if (GetGame().IsServer())
        m_Config = MyConfig.Load();
}

void SendConfig(PlayerBase player, PlayerIdentity identity)
{
    if (GetGame().IsServer())
        GetDayZularRPCManager().SendRPC("MyMod", "Config", new Param1<MyConfig>(m_Config), true, identity);
}
```

### Pattern: Player State Management
```c
void MyService()
{
    GetDayZularEvents().OnPlayerJoined.Insert(CreatePlayerState);
    GetDayZularEvents().OnPlayerDisconnected.Insert(SavePlayerState);
}

void CreatePlayerState(PlayerBase player, PlayerIdentity identity)
{
    string uid = identity.GetPlainId();
    m_PlayerStates[uid] = new PlayerState();
}

void SavePlayerState(PlayerBase player, PlayerIdentity identity, int logoutTime, bool authFailed)
{
    string uid = identity.GetPlainId();
    m_PlayerStates[uid].Save();
}
```

### Pattern: Service with Dependencies
```c
void CraftingService()
{
    // Get dependencies from registry
    m_WorkbenchService = WorkbenchService.Cast(DZlrServiceRegistry.GetInstance().GetService(WorkbenchService));
    
    // Subscribe to events
    GetDayZularEvents().OnMissionStart.Insert(Initialize);
}
```

### Pattern: Client-Only Service
```c
void UIService()
{
    if (!GetGame().IsClient())
        return; // Don't initialize on server
    
    GetDayZularEvents().OnMissionStart.Insert(InitializeUI);
}
```

## Why This Architecture?

### vs CF Modules
- **More explicit** - Event subscriptions visible in constructor
- **More flexible** - Subscribe only to events you need
- **Simpler** - No base class with 20+ methods
- **Different** - No module manager pattern

### vs Pure Event Bus
- **More structured** - Services provide organization
- **Type-safe** - Service registry provides type safety
- **Discoverable** - IDE autocomplete shows available services

### vs Singleton Hell
- **Testable** - Services can be mocked
- **Injectable** - Services retrieved from registry
- **Optional globals** - Can use global accessors for convenience

## Files

**Core:**
- `DayZularEvents.c` - Event bus with all lifecycle invokers
- `Service.c` - Lightweight service base class
- `AppModule.c` - Bootstrap with `OnRegisterServices()`

**Examples:**
- `ServiceWithEventsExample.c` - Comprehensive examples
- `OnPlayerJoinedExample.c` - Config distribution pattern

**Integration:**
- `MissionServer.c` - Wires vanilla events to event bus
- `MissionGameplay.c` - Client-side event wiring

## Next Steps

1. Create your service extending `DayZularService`
2. Subscribe to events in constructor
3. Register in `AppModule.OnRegisterServices()`
4. Access via service registry or global accessor
