# DayZular Framework - Service Architecture

## What You Have Now

A **Service + Event Bus** architecture that is:
- ✅ **Different from CF** - No module base class, no manager pattern
- ✅ **Flexible** - Services subscribe only to events they need
- ✅ **Maintainable** - Event subscriptions visible in constructor
- ✅ **Teaches best practices** - DI, pub/sub, single responsibility
- ✅ **Self-sufficient** - Works with RPC, Components, DI systems

## Quick Example

```c
// 1. Create service
class WorkbenchService : DayZularService
{
    private ref WorkbenchConfig m_Config;
    
    void WorkbenchService()
    {
        // Subscribe to events you need (explicit!)
        GetDayZularEvents().OnMissionStart.Insert(Initialize);
        GetDayZularEvents().OnPlayerJoined.Insert(OnPlayerJoined);
        
        // Register RPCs
        if (GetGame().IsServer())
            GetDayZularRPCManager().AddRPC("Workbench", "StartCraft", this, DayZularRPCType.Server);
    }
    
    void Initialize()
    {
        if (GetGame().IsServer())
            m_Config = WorkbenchConfig.Load();
    }
    
    void OnPlayerJoined(PlayerBase player, PlayerIdentity identity)
    {
        if (GetGame().IsServer())
            GetDayZularRPCManager().SendRPC("Workbench", "Config", new Param1<WorkbenchConfig>(m_Config), true, identity);
    }
}

// 2. Register service
class MyModAppInit
{
    void MyModAppInit()
    {
        AppModule.GetOnRegisterServices().Insert(RegisterServices);
    }
    
    static void RegisterServices()
    {
        DZlrServiceRegistry.GetInstance().RegisterService(WorkbenchService, new WorkbenchService());
    }
}

MyModAppInit g_MyModAppInit = new MyModAppInit();

// 3. Access service
WorkbenchService service = WorkbenchService.Cast(DZlrServiceRegistry.GetInstance().GetService(WorkbenchService));
```

## Available Events

**Mission:** `OnMissionStart`, `OnMissionLoaded`, `OnMissionFinish`, `OnUpdate`

**Player:** `OnPlayerJoined`, `OnPlayerReady`, `OnPlayerReconnect`, `OnPlayerRespawn`, `OnPlayerDisconnected`

**Advanced:** `OnInvokeConnect`, `OnInvokeDisconnect`, `OnClientNew`, `OnClientPrepare`

## Architecture Benefits

### 1. Explicit Over Magic
```c
// You see exactly what events a service needs
void MyService()
{
    GetDayZularEvents().OnMissionStart.Insert(Init);      // ← Clear!
    GetDayZularEvents().OnPlayerJoined.Insert(OnJoin);    // ← Clear!
}

// vs CF's hidden opt-in
override void OnInit()
{
    EnableMissionStart();  // ← What does this do?
    EnableUpdate();        // ← Hidden subscription
}
```

### 2. Single Responsibility
```c
// Each service does ONE thing well
class WorkbenchService : DayZularService { }  // Handles workbench
class TerritoryService : DayZularService { }  // Handles territory
class CraftingService : DayZularService { }   // Handles crafting

// vs CF's god modules
class MegaModule : CF_ModuleWorld { }  // Does everything
```

### 3. Dependency Injection
```c
// Services get dependencies from registry
void CraftingService()
{
    m_WorkbenchService = WorkbenchService.Cast(DZlrServiceRegistry.GetInstance().GetService(WorkbenchService));
    m_TerritoryService = TerritoryService.Cast(DZlrServiceRegistry.GetInstance().GetService(TerritoryService));
}

// vs CF's direct calls
void CraftingModule()
{
    auto workbench = WorkbenchModule.Cast(CF_ModuleCoreManager.Get(WorkbenchModule));  // Tight coupling
}
```

### 4. Testability
```c
// Can instantiate services independently
void TestMyService()
{
    WorkbenchService service = new WorkbenchService();
    service.Initialize();
    // Test methods
}

// vs CF modules (require full framework)
```

## Files Structure

```
DayZular/
├── scripts/3_Game/DayZular/
│   ├── Core/
│   │   ├── AppModule.c              ← Bootstrap + OnRegisterServices()
│   │   ├── DayZGame.c               ← RPC hook
│   │   └── ComponentBase.c          ← UI components
│   ├── Events/
│   │   └── DayZularEvents.c         ← Event bus (NEW!)
│   ├── Services/
│   │   ├── Service.c                ← Service base class
│   │   └── LoggerService.c          ← Built-in logger
│   └── RPC/
│       └── DayZularRPCManager.c     ← RPC system
├── scripts/5_Mission/
│   ├── MissionServer.c              ← Wires vanilla events to event bus
│   └── mission/MissionGameplay.c    ← Client-side event wiring
├── examples/
│   ├── ServiceWithEventsExample.c   ← Complete examples
│   ├── MigrationFromCF.c            ← CF → DayZular migration
│   └── OnPlayerJoinedExample.c      ← Config distribution pattern
├── ARCHITECTURE.md                  ← This file
├── QUICKSTART_SERVICES.md           ← Quick reference
└── Modules/_archived/               ← Old module system (reference)
```

## When to Use What

**Use DayZularService when:**
- You need lifecycle hooks (mission start, player events)
- You need RPC registration
- You need config loading
- You're coordinating business logic

**Use DZlrComponentBase when:**
- You're building UI
- You need data binding
- You need widget management

**Use plain classes when:**
- You have simple data structures
- You don't need lifecycle or DI

## Next Steps

1. Read `QUICKSTART_SERVICES.md` for quick reference
2. See `ServiceWithEventsExample.c` for comprehensive examples
3. See `MigrationFromCF.c` for CF migration guide
4. Create your first service
5. Register in `AppModule.OnRegisterServices()`

## Why This Is Better

**For Flexibility:**
- Services choose their events (not forced to handle all)
- Can subscribe/unsubscribe dynamically
- No inheritance constraints

**For Maintainability:**
- Event subscriptions in one place (constructor)
- Clear service boundaries
- Easy to trace event flow

**For Teaching:**
- Explicit subscriptions show intent
- Standard patterns (DI, pub/sub)
- Clear mental model

**For IP:**
- Architecturally different from CF
- No module base class
- No manager pattern
- Event bus instead of linked lists

Your framework is now **self-sufficient, flexible, and teaches best practices** while being clearly differentiated from CF.
