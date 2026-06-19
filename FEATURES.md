# DayZular Framework - Complete Feature Guide

## Table of Contents
1. [Core Features](#core-features)
2. [Component System](#component-system)
3. [Data Binding](#data-binding)
4. [Services & Dependency Injection](#services--dependency-injection)
5. [Event Bus](#event-bus)
6. [RPC System](#rpc-system)
7. [Observables](#observables)
8. [Form Validation](#form-validation)
9. [Pipes](#pipes)
10. [Logging](#logging)

---

## Core Features

### Angular-Inspired Architecture
DayZular brings modern web framework patterns to DayZ modding:
- Component-based UI development
- Reactive data binding
- Dependency injection
- Event-driven architecture
- Declarative templates

### Zero External Dependencies
- No CF required
- No DabsFramework required
- Pure DayZ game scripts only
- Can coexist with other frameworks

---

## Component System

### Component Lifecycle
```c
class MyComponent extends ComponentBase
{
    override void OnInit()        // Component initialized
    override void OnViewInit()    // View/widgets loaded
    override void OnDestroy()     // Component destroyed
    override void OnShow()        // Menu shown
    override void OnHide()        // Menu hidden
}
```

### Auto-Registration
```c
class MyComponent extends ComponentBase
{
    // Automatically registers component with selector
    private static ref ComponentAutoRegister s_Register = 
        new ComponentAutoRegister("my-component", MyComponent);
}
```

### Component Metadata
```c
override protected void RegisterMetadata()
{
    m_Metadata = new ComponentMetadata();
    m_Metadata.selector = "my-component";
    m_Metadata.templatePath = "MyMod/gui/layouts/component.layout";
}
```

### Nested Components
Components can contain other components:
```xml
<!-- Parent layout -->
<PanelWidgetClass child_container {
    scriptclass "ComponentBinding"
    {
        ScriptParamsClass {
            Component_Selector "child-component"
        }
    }
}
```

---

## Data Binding

### Property Binding
Bind widget text to component properties:
```xml
<TextWidgetClass title {
    text "{{ title }}"
}
```

```c
class MyComponent extends ComponentBase
{
    string title = "Hello World";
}
```

### Method Binding
Call component methods in bindings:
```xml
<TextWidgetClass counter {
    text "{{ GetCounterText() }}"
}
```

```c
string GetCounterText()
{
    return "Count: " + m_Counter.ToString();
}
```

### Nested Property Access
```xml
<TextWidgetClass player_name {
    text "{{ player.name }}"
}
```

### Event Binding
Bind widget events to component methods:
```xml
<ButtonWidgetClass my_button {
    scriptclass "ButtonBinding"
    {
        ScriptParamsClass {
            OnClick "OnButtonClick"
            OnRightClick "OnButtonRightClick"
            OnDoubleClick "OnButtonDoubleClick"
            OnMouseEnter "OnMouseEnter"
            OnMouseLeave "OnMouseLeave"
        }
    }
}
```

`OnRightClick` is delivered through `OnMouseButtonDown` (DayZ `ButtonWidget.OnClick` fires for the left mouse button only).

### Player Preview Controls
`PlayerPreviewBinding` supports interactive rotation and zoom:
```xml
<PlayerPreviewWidgetClass player_preview {
    scriptclass "PlayerPreviewBinding"
    {
        ScriptParamsClass {
            Binding_Player "previewPlayer"
            Enable_Rotation 1
            Enable_Scale 1
        }
    }
}
```
- `Enable_Rotation 1` — drag the model to rotate it
- `Enable_Scale 1` — mouse wheel to zoom in/out

Controls work whether the player is set via `Binding_Player` or assigned in code.

### ForEach Binding
Repeat widgets from collections:
```xml
<WrapSpacerWidgetClass item_list {
    scriptclass "ForEachBinding"
    {
        ScriptParamsClass {
            Component_ForEach "items"
            Component_Selector "item-component"
        }
    }
}
```

```c
ref DZObservableCollection<ref ItemData> items = new DZObservableCollection<ref ItemData>();
```

---

## Services & Dependency Injection

### Service Registration
```c
// In your mod's AppModule subscription
modded class AppModule
{
    static void RegisterMyModServices()
    {
        DZlrServiceRegistry.GetInstance().RegisterService(
            MyService, 
            new MyService()
        );
    }
}

// Subscribe to registration event
void OnInit()
{
    AppModule.GetOnRegisterServices().Insert(RegisterMyModServices);
}
```

### Service Retrieval
```c
MyService svc = MyService.Cast(
    DZlrServiceRegistry.GetInstance().GetService(MyService)
);
```

### Service Base Class
```c
class MyService extends DayZularService
{
    void MyService()
    {
        // Subscribe to events
        GetDayZularEvents().OnMissionStart.Insert(OnMissionStart);
    }
    
    void OnMissionStart()
    {
        // Initialize service
    }
}
```

---

## Event Bus

### Available Events

**Mission Events:**
- `OnMissionStart` - Mission started (client/server)
- `OnMissionLoaded` - Mission fully loaded
- `OnMissionFinish` - Mission ending
- `OnUpdate` - Every frame (deltaTime)

**Player Events:**
- `OnPlayerJoined` - Player connected (server)
- `OnPlayerReady` - Player fully loaded (server)
- `OnPlayerReconnect` - Player reconnected (server)
- `OnPlayerRespawn` - Player respawned (server)
- `OnPlayerDisconnected` - Player left (server)

**Advanced Events:**
- `OnInvokeConnect` - Low-level connection
- `OnInvokeDisconnect` - Low-level disconnection
- `OnClientNew` - New client instance
- `OnClientPrepare` - Client preparing

### Event Subscription
```c
class MyService extends DayZularService
{
    void MyService()
    {
        GetDayZularEvents().OnPlayerJoined.Insert(OnPlayerJoined);
        GetDayZularEvents().OnUpdate.Insert(OnUpdate);
    }
    
    void OnPlayerJoined(PlayerIdentity identity, PlayerBase player)
    {
        DZLog().LogInfo("Player joined: " + identity.GetName());
    }
    
    void OnUpdate(float deltaTime)
    {
        // Update logic
    }
}
```

---

## RPC System

### Features
- Request/Response pattern with callbacks
- Middleware support
- Rate limiting
- Message queuing for offline players
- Automatic retry with exponential backoff

### RPC Registration
```c
// Server-side
if (GetGame().IsServer())
{
    GetDayZularRPCManager().AddRPC(
        "MyMod", 
        "GetData", 
        this, 
        DayZularRPCType.Server
    );
}

// Client-side
if (GetGame().IsClient())
{
    GetDayZularRPCManager().AddRPC(
        "MyMod", 
        "DataResponse", 
        this, 
        DayZularRPCType.Client
    );
}
```

### RPC Handler
```c
void GetData(PlayerIdentity sender, ParamsReadContext ctx)
{
    Param1<string> data;
    if (!ctx.Read(data))
        return;
    
    // Process request
    string query = data.param1;
    
    // Send response
    GetDayZularRPCManager().SendRPC(
        "MyMod",
        "DataResponse",
        new Param1<string>("Result"),
        true,
        sender
    );
}
```

### Request/Response Pattern
```c
// Send request with callback
int requestID = GetDayZularRPCManager().SendRequest(
    "MyMod",
    "GetPlayerData",
    new Param1<string>(playerUID),
    this,
    "OnPlayerDataReceived",
    10.0,  // timeout seconds
    3      // max retries
);

// Callback receives response
void OnPlayerDataReceived(bool success, ParamsReadContext ctx)
{
    if (!success)
    {
        DZLog().LogError("Request failed");
        return;
    }
    
    Param1<ref PlayerData> response;
    if (ctx.Read(response))
    {
        // Process response
    }
}
```

### Middleware
```c
class MyAuthMiddleware extends DayZularMiddleware
{
    override bool OnBefore(DayZularRPCContext ctx)
    {
        // Check authentication
        if (!IsAuthorized(ctx.sender))
        {
            DZLog().LogWarning("Unauthorized RPC attempt");
            return false; // Block RPC
        }
        return true;
    }
}

// Register middleware
GetDayZularRPCManager().Use(new MyAuthMiddleware());
```

### Rate Limiting
```c
// Limit endpoint to 10 calls per 60 seconds
GetDayZularRPCManager().SetRateLimit(
    "MyMod",
    "ExpensiveOperation",
    10,
    60.0
);
```

---

## Observables

### Observable Types
```c
ref ObservableInt counter = new ObservableInt(0);
ref ObservableFloat health = new ObservableFloat(100.0);
ref ObservableString playerName = new ObservableString("Player");
ref ObservableBool isAlive = new ObservableBool(true);
```

### Observable Collections
```c
ref DZObservableCollection<ref ItemData> items = 
    new DZObservableCollection<ref ItemData>();

items.Insert(new ItemData("Apple"));
items.Remove(0);
items.Clear();
```

### Change Detection
Observables automatically trigger component updates when changed:
```c
counter.Set(counter.Get() + 1); // Component re-renders
```

---

## Form Validation

### Form Builder
```c
FormGroup form = new FormGroup("myForm");
form.AddControl("username", FormBuilder.String("username", "", 3, 20));
form.AddControl("age", FormBuilder.Integer("age", "18", 1, 120));
form.AddControl("email", FormBuilder.Email("email", ""));
```

### Built-in Validators
- `required` - Field must have value
- `min:X` - Minimum value/length
- `max:X` - Maximum value/length
- `range:X,Y` - Value between X and Y
- `pattern:regex` - Match regex pattern
- `email` - Valid email format
- `integer` - Must be integer
- `float` - Must be float

### Layout Integration
```xml
<EditBoxWidgetClass username_input {
    scriptclass "EditBoxBinding"
    {
        ScriptParamsClass {
            Form_Control_Name "username"
            Validators "required,min:3,max:20"
        }
    }
}
```

### Validation
```c
if (form.Validate())
{
    map<string, string> values = form.GetValues();
    string username = values.Get("username");
}
else
{
    map<string, ref ValidationError> errors = form.GetErrors();
    // Display errors
}
```

---

## Pipes

### Built-in Pipes
```xml
{{ value | uppercase }}      <!-- "HELLO" -->
{{ value | lowercase }}      <!-- "hello" -->
{{ value | capitalize }}     <!-- "Hello" -->
{{ price | currency }}       <!-- "$1,234.56" -->
{{ price | eur }}           <!-- "€1,234.56" -->
{{ ratio | percent }}       <!-- "75%" -->
{{ data | json }}           <!-- JSON string -->
{{ value | default:"N/A" }} <!-- Default if empty -->
```

### Custom Pipes
```c
class ReversePipe extends Pipe
{
    override string Transform(string value, array<string> args)
    {
        string result = "";
        for (int i = value.Length() - 1; i >= 0; i--)
        {
            result += value.Get(i);
        }
        return result;
    }
}

// Register
PipeRegistry.RegisterPipe("reverse", new ReversePipe());
```

---

## Logging

### Log Levels
```c
DZLog().LogDebug("Detailed debug info");   // Level 0
DZLog().LogInfo("General information");    // Level 1
DZLog().LogWarning("Warning message");     // Level 2
DZLog().LogError("Error occurred");        // Level 3
```

### Configuration
Create `$profile:DayZular/Logging/Config/LoggerSettings.json`:
```json
{
    "logLevel": 1,
    "refreshRateInSeconds": 60,
    "enableFileLogging": true,
    "enableConsoleOutput": false
}
```

### File Logging
Logs written to: `$profile:DayZular/Logging/Logs/DayZular_YYYY_MM_DD-HH_MM_SS.log`

### Runtime Configuration
```c
DZLog().SetLogLevel(DayZularLogLevel.Debug);
DZLog().EnableFileLogging();
DZLog().SetConsoleOutput(true);
```

---

## Performance Considerations

### Change Detection
- Use `MarkForCheck()` to trigger component updates
- Avoid excessive observable changes in loops
- Batch updates when possible

### Component Lifecycle
- Clean up subscriptions in `OnDestroy()`
- Unregister event listeners
- Clear large collections

### RPC Best Practices
- Use rate limiting for expensive operations
- Batch multiple small RPCs into one
- Implement request/response for critical data
- Use middleware for common validation

---

## Best Practices

1. **Component Design**
   - Keep components focused and small
   - Use services for shared logic
   - Leverage lifecycle hooks properly

2. **Data Binding**
   - Use observables for reactive data
   - Avoid complex logic in bindings
   - Use pipes for transformations

3. **Services**
   - Register services early in bootstrap
   - Use singleton pattern
   - Subscribe to events in constructor

4. **RPC**
   - Always validate input data
   - Use explicit namespaces
   - Implement proper error handling
   - Add rate limiting for public endpoints

5. **Logging**
   - Use appropriate log levels
   - Disable console output in production
   - Enable file logging for debugging
   - Keep log messages concise

---

## Migration from Other Frameworks

### From CF Modules
```c
// CF Module
class MyModule : CF_ModuleGame
{
    override void OnInit()
    {
        super.OnInit();
        GetRPCManager().AddRPC("MyMod", "MyRPC", this);
    }
}

// DayZular Service
class MyService : DayZularService
{
    void MyService()
    {
        GetDayZularEvents().OnMissionStart.Insert(OnMissionStart);
        GetDayZularRPCManager().AddRPC("MyMod", "MyRPC", this, DayZularRPCType.Server);
    }
}
```

### From DabsFramework MVC
DayZular components can work alongside Dabs controllers:
```c
// Use Dabs for data models
class PlayerModel : ViewModel { }

// Use DayZular for UI
class PlayerComponent : ComponentBase
{
    ref PlayerModel model;
    
    override void OnInit()
    {
        model = new PlayerModel();
    }
}
```

---

## Examples

See the `examples/` directory for complete working examples:
- `ServiceRegistrationExample.c` - Multi-mod service registration
- `ServiceWithEventsExample.c` - Event bus usage
- `OnPlayerJoinedExample.c` - Player event handling
- Component examples in DayZular_Examples mod

---

## Support and Community

- **Issues:** Report bugs and request features on GitHub
- **Discord:** Join the DayZ modding community
- **Documentation:** Check ARCHITECTURE.md for technical details
- **Credits:** See CREDITS.md for acknowledgments

---

**Happy Modding!** 🎮
