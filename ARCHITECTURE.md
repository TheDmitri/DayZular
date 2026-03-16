# DayZular Framework - Architecture Guide

## Core Philosophy

**Service-Oriented + Event-Driven Architecture**

- **Services** handle business logic and state
- **Events** coordinate lifecycle and communication
- **Components** handle UI and data binding
- **No module base class** - services are lightweight and explicit

## Key Differences from CF

| Aspect | CF Framework | DayZular Framework |
|--------|-------------|-------------------|
| Organization | Module base class | Pure services |
| Event Subscription | Opt-in via `Enable*()` | Explicit `Insert()` in constructor |
| Discovery | Hidden in inheritance | **Visible in constructor** |
| Registration | Attribute-based | **Explicit in AppModule** |
| Lifecycle | Manager coordinates | **Event bus broadcasts** |
| Coupling | Tight (manager dependency) | **Loose (event-driven)** |

## Three-Layer Architecture

```
┌─────────────────────────────────────┐
│         UI Layer                     │
│  Components (Angular-inspired)       │
│  - Data binding                      │
│  - Event handlers                    │
│  - Widget management                 │
└─────────────────────────────────────┘
              ↓ inject services
┌─────────────────────────────────────┐
│       Business Logic Layer           │
│  Services (Spring-inspired)          │
│  - Domain logic                      │
│  - State management                  │
│  - RPC handlers                      │
└─────────────────────────────────────┘
              ↓ subscribe to events
┌─────────────────────────────────────┐
│       Infrastructure Layer           │
│  Event Bus + RPC + DI                │
│  - DayZularEvents (lifecycle)        │
│  - DayZularRPCManager (networking)   │
│  - DZlrServiceRegistry (DI)          │
└─────────────────────────────────────┘
```

## Core Components

### 1. DayZularEvents (Event Bus)

**Purpose:** Global event bus for lifecycle hooks

**Location:** `C:\DayZWorkspace\RazBentleyWorkbench\src\DayZular\scripts\3_Game\DayZular\Events\DayZularEvents.c`

**Available Events:**
- Mission: `OnMissionStart`, `OnMissionLoaded`, `OnMissionFinish`, `OnUpdate`
- Player: `OnPlayerJoined`, `OnPlayerReady`, `OnPlayerReconnect`, `OnPlayerRespawn`, `OnPlayerDisconnected`
- Advanced: `OnInvokeConnect`, `OnInvokeDisconnect`, `OnClientNew`, `OnClientPrepare`

**Usage:**
```c
GetDayZularEvents().OnPlayerJoined.Insert(this.HandlePlayerJoin);
```

### 2. DayZularService (Base Class)

**Purpose:** Lightweight base for all services

**Location:** `C:\DayZWorkspace\RazBentleyWorkbench\src\DayZular\scripts\3_Game\DayZular\Services\Service.c`

**Features:**
- Minimal (no forced methods)
- Registered in `DZlrServiceRegistry` for DI
- Services subscribe to events in constructor

### 3. DZlrServiceRegistry (DI Container)

**Purpose:** Dependency injection container

**Methods:**
- `RegisterService(typename, instance)` - Register a service
- `GetService(typename)` - Retrieve a service
- `HasService(typename)` - Check if service exists

### 4. DayZularRPCManager (Networking)

**Purpose:** Advanced RPC system with middleware, rate limiting, request/response

**Features:**
- Namespace-based routing
- Middleware chain
- Request/response pattern with callbacks
- Rate limiting
- Message queuing for offline players

### 5. DZlrComponentBase (UI Layer)

**Purpose:** Angular-inspired component system

**Features:**
- Data binding
- Property interpolation
- Event handlers
- Component hierarchy
- Lifecycle hooks (OnInit, OnDestroy, OnChanges)

## Service Pattern

### Basic Service

```c
class MyFeatureService : DayZularService
{
    private ref MyConfig m_Config;
    
    void MyFeatureService()
    {
        // Subscribe to events you need
        GetDayZularEvents().OnMissionStart.Insert(Initialize);
        GetDayZularEvents().OnPlayerJoined.Insert(OnPlayerJoined);
        
        // Register RPCs
        if (GetGame().IsServer())
        {
            GetDayZularRPCManager().AddRPC("MyFeature", "Request", this, DayZularRPCType.Server);
        }
    }
    
    void Initialize()
    {
        if (GetGame().IsServer())
        {
            m_Config = MyConfig.Load();
        }
    }
    
    void OnPlayerJoined(PlayerBase player, PlayerIdentity identity)
    {
        if (GetGame().IsServer())
        {
            GetDayZularRPCManager().SendRPC("MyFeature", "ConfigResponse", new Param1<MyConfig>(m_Config), true, identity);
        }
    }
}
```

### Service with Dependencies

```c
class CraftingService : DayZularService
{
    private ref WorkbenchService m_WorkbenchService;
    private ref TerritoryService m_TerritoryService;
    
    void CraftingService()
    {
        // Get dependencies from service registry
        m_WorkbenchService = WorkbenchService.Cast(DZlrServiceRegistry.GetInstance().GetService(WorkbenchService));
        m_TerritoryService = TerritoryService.Cast(DZlrServiceRegistry.GetInstance().GetService(TerritoryService));
        
        // Subscribe to events
        GetDayZularEvents().OnMissionStart.Insert(Initialize);
    }
    
    void Initialize()
    {
        // Use injected dependencies
        WorkbenchConfig config = m_WorkbenchService.GetConfig();
    }
}
```

### Service with Update Loop

```c
class MonitoringService : DayZularService
{
    void MonitoringService()
    {
        // Subscribe to update event
        GetDayZularEvents().OnUpdate.Insert(OnUpdate);
    }
    
    void OnUpdate(float deltaTime)
    {
        // Called every frame
    }
}
```

## Registration Pattern

```c
// In your mod, subscribe to AppModule's registration event
// This allows MULTIPLE mods to register services without conflicts
class MyModAppInit
{
    void MyModAppInit()
    {
        // Subscribe to DayZular's service registration event
        AppModule.GetOnRegisterServices().Insert(RegisterServices);
    }
    
    static void RegisterServices()
    {
        auto registry = DZlrServiceRegistry.GetInstance();
        
        // Register services - they auto-subscribe to events in constructors
        registry.RegisterService(WorkbenchService, new WorkbenchService());
        registry.RegisterService(TerritoryService, new TerritoryService());
        registry.RegisterService(CraftingService, new CraftingService());
    }
}

// Auto-initialize on script load
MyModAppInit g_MyModAppInit = new MyModAppInit();
```

**Why this pattern?**
- ✅ Multiple mods can register services (ScriptInvoker supports multiple subscribers)
- ✅ No static override conflicts
- ✅ Auto-initializes when script loads
- ✅ Clean separation between mods

## Best Practices

### ✅ DO:

1. **Subscribe to events in constructor** - Makes dependencies explicit
2. **Keep services focused** - Single responsibility principle
3. **Use dependency injection** - Get services from registry
4. **Check IsServer()/IsClient()** - Guard side-specific logic
5. **Use OnPlayerJoined for config** - Perfect timing for initial data
6. **Name services clearly** - `WorkbenchService`, not `WorkbenchManager`

### ❌ DON'T:

1. **Don't create god services** - Split into focused services
2. **Don't subscribe to OnUpdate unless needed** - Performance impact
3. **Don't call other services directly in constructor** - Use OnMissionStart
4. **Don't forget to check game side** - Wrap server/client logic
5. **Don't create circular dependencies** - Services should have clear hierarchy

## Event Subscription Patterns

### Pattern 1: Config Distribution
```c
void MyService()
{
    GetDayZularEvents().OnMissionStart.Insert(LoadConfig);
    GetDayZularEvents().OnPlayerJoined.Insert(SendConfigToPlayer);
}
```

### Pattern 2: Player State Management
```c
void MyService()
{
    GetDayZularEvents().OnPlayerJoined.Insert(CreatePlayerState);
    GetDayZularEvents().OnPlayerDisconnected.Insert(SavePlayerState);
    GetDayZularEvents().OnPlayerRespawn.Insert(ResetPlayerState);
}
```

### Pattern 3: Continuous Monitoring
```c
void MyService()
{
    GetDayZularEvents().OnMissionStart.Insert(Initialize);
    GetDayZularEvents().OnUpdate.Insert(Monitor);
}
```

### Pattern 4: Client-Only Service
```c
void MyClientService()
{
    if (!GetGame().IsClient())
        return; // Don't initialize on server
    
    GetDayZularEvents().OnMissionStart.Insert(InitializeUI);
}
```

## Service Discovery

### Option A: Service Registry (Recommended)
```c
WorkbenchService service = WorkbenchService.Cast(DZlrServiceRegistry.GetInstance().GetService(WorkbenchService));
```

### Option B: Global Accessor (Convenience)
```c
static ref WorkbenchService g_WorkbenchService;

WorkbenchService GetWorkbenchService()
{
    if (!g_WorkbenchService)
        g_WorkbenchService = WorkbenchService.Cast(DZlrServiceRegistry.GetInstance().GetService(WorkbenchService));
    return g_WorkbenchService;
}
```

## Component Integration

Services can be injected into components:

```c
class WorkbenchComponent : DZlrComponentBase
{
    private ref WorkbenchService m_Service;
    
    override void OnInit()
    {
        super.OnInit();
        
        // Inject service
        m_Service = WorkbenchService.Cast(DZlrServiceRegistry.GetInstance().GetService(WorkbenchService));
        
        // Use service
        WorkbenchConfig config = m_Service.GetConfig();
    }
}
```

## Why This Architecture?

### Flexibility ✅
- Services choose which events they need
- Can subscribe/unsubscribe dynamically
- No forced inheritance structure

### Maintainability ✅
- Event subscriptions visible in constructor
- Clear service boundaries
- Easy to test (mock event bus)

### Best Practices ✅
- **Explicit over implicit** - You see what a service needs
- **Single responsibility** - Each service does one thing
- **Dependency injection** - Loose coupling
- **Event-driven** - Services don't call each other directly

### Teaching ✅
- Clear mental model: Services + Events + Components
- Standard patterns: DI, pub/sub, component hierarchy
- Easy to understand: No complex inheritance
- Discoverable: IDE autocomplete shows all events

## Migration from CF

### CF Module
```c
[CF_RegisterModule(WorkbenchModule)]
class WorkbenchModule : CF_ModuleWorld
{
    override void OnInit()
    {
        EnableMissionStart();
        EnableUpdate();
    }
    
    override void OnMissionStart(Class sender, CF_EventArgs args)
    {
        // Initialize
    }
}
```

### DayZular Service
```c
class WorkbenchService : DayZularService
{
    void WorkbenchService()
    {
        GetDayZularEvents().OnMissionStart.Insert(Initialize);
        GetDayZularEvents().OnUpdate.Insert(OnUpdate);
    }
    
    void Initialize()
    {
        // Initialize
    }
}

// Register in AppModule
DZlrServiceRegistry.GetInstance().RegisterService(WorkbenchService, new WorkbenchService());
```

## Summary

DayZular uses a **Service + Event Bus** architecture that is:
- ✅ Different from CF (no module base class, no manager pattern)
- ✅ Flexible (services choose their events)
- ✅ Maintainable (explicit subscriptions)
- ✅ Teaches best practices (DI, pub/sub, SRP)
- ✅ Self-sufficient (works with RPC, Components, DI)
