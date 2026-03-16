# DayZular Framework - Configuration Guide

## Table of Contents
1. [Logger Configuration](#logger-configuration)
2. [Service Registration](#service-registration)
3. [RPC Configuration](#rpc-configuration)
4. [Component Settings](#component-settings)
5. [Production vs Development](#production-vs-development)

---

## Logger Configuration

### Default Settings

By default, DayZular logging is configured for **production use**:
- **Log Level:** Info (only Info, Warning, and Error messages)
- **Console Output:** Disabled
- **File Logging:** Disabled

### Configuration File

Create or edit: `$profile:DayZular/Logging/Config/LoggerSettings.json`

```json
{
    "logLevel": 1,
    "refreshRateInSeconds": 60,
    "enableFileLogging": false,
    "enableConsoleOutput": false
}
```

### Log Levels

| Level | Value | Description |
|-------|-------|-------------|
| Debug | 0 | Verbose debugging information |
| Info | 1 | General informational messages |
| Warn | 2 | Warning messages |
| Error | 3 | Error messages only |
| None | 4 | Disable all logging |

### Runtime Configuration

```c
// Enable debug logging for development
DZLog().SetLogLevel(DayZularLogLevel.Debug);
DZLog().SetConsoleOutput(true);
DZLog().EnableFileLogging();

// Production settings
DZLog().SetLogLevel(DayZularLogLevel.Info);
DZLog().SetConsoleOutput(false);
DZLog().DisableFileLogging();
```

### Log File Location

When file logging is enabled, logs are written to:
```
$profile:DayZular/Logging/Logs/DayZular_YYYY_MM_DD-HH_MM_SS.log
```

Example:
```
C:\Users\YourName\Documents\DayZ\DayZular\Logging\Logs\DayZular_2026_03_16-14_30_45.log
```

### Hot Reload

The logger automatically reloads settings every 60 seconds (configurable via `refreshRateInSeconds`). You can change log settings without restarting the server.

---

## Service Registration

### Multi-Mod Service Registration

DayZular supports multiple mods registering services without conflicts:

```c
// In your mod's 5_Mission/MissionServer.c or MissionGameplay.c
modded class MissionServer
{
    void MissionServer()
    {
        // Subscribe to the registration event
        AppModule.GetOnRegisterServices().Insert(RegisterMyModServices);
    }
    
    static void RegisterMyModServices()
    {
        DZLog().LogInfo("Registering MyMod services...");
        
        // Register your services
        DZlrServiceRegistry.GetInstance().RegisterService(
            MyConfigService, 
            new MyConfigService()
        );
        
        DZlrServiceRegistry.GetInstance().RegisterService(
            MyDataService, 
            new MyDataService()
        );
    }
}
```

### Service Registration Order

Services are registered in the order mods subscribe to the event. If your service depends on another:

1. Register dependencies first
2. Or check for dependencies in your service's constructor
3. Or use lazy initialization

```c
class MyService : DayZularService
{
    private ref OtherService m_OtherService;
    
    void MyService()
    {
        // Lazy initialization
        GetDayZularEvents().OnMissionStart.Insert(OnMissionStart);
    }
    
    void OnMissionStart()
    {
        // Get dependency after all services are registered
        m_OtherService = OtherService.Cast(
            DZlrServiceRegistry.GetInstance().GetService(OtherService)
        );
    }
}
```

---

## RPC Configuration

### RPC Handler Signature

All RPC handlers **must** use this exact signature:

```c
void HandlerName(PlayerIdentity sender, ParamsReadContext ctx)
{
    // Read parameters
    Param1<string> data;
    if (!ctx.Read(data))
        return;
    
    // Process request
}
```

**Important:** Parameters are `(PlayerIdentity sender, ParamsReadContext ctx)` - not reversed!

### RPC Registration

```c
// Server-side handlers
if (GetGame().IsServer())
{
    GetDayZularRPCManager().AddRPC(
        "MyModNamespace",    // Namespace (usually your mod name)
        "HandlerName",       // Endpoint name
        this,                // Handler instance
        DayZularRPCType.Server
    );
}

// Client-side handlers
if (GetGame().IsClient())
{
    GetDayZularRPCManager().AddRPC(
        "MyModNamespace",
        "ResponseHandler",
        this,
        DayZularRPCType.Client
    );
}
```

### Rate Limiting

Protect expensive operations with rate limiting:

```c
// Limit to 5 calls per 10 seconds
GetDayZularRPCManager().SetRateLimit(
    "MyMod",
    "ExpensiveOperation",
    5,      // max calls
    10.0    // window in seconds
);
```

### Middleware

Add global RPC middleware for logging, auth, etc:

```c
class MyLoggingMiddleware : DayZularMiddleware
{
    override bool OnBefore(DayZularRPCContext ctx)
    {
        DZLog().LogDebug("RPC: " + ctx.modNamespace + "::" + ctx.endpoint);
        return true; // Continue processing
    }
    
    override void OnAfter(DayZularRPCContext ctx)
    {
        // Post-processing
    }
}

// Register middleware
GetDayZularRPCManager().Use(new MyLoggingMiddleware());
```

---

## Component Settings

### Component Registration

Components must be registered before use:

```c
class MyComponent : ComponentBase
{
    // Auto-registration (recommended)
    private static ref ComponentAutoRegister s_Register = 
        new ComponentAutoRegister("my-component", MyComponent);
    
    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "my-component";
        m_Metadata.templatePath = "MyMod/gui/layouts/my_component.layout";
    }
}
```

### Change Detection Strategy

Components use manual change detection for performance:

```c
class MyComponent : ComponentBase
{
    int counter = 0;
    
    void IncrementCounter()
    {
        counter++;
        MarkForCheck(); // Trigger re-render
    }
}
```

### Component Cleanup

Always clean up in `OnDestroy()`:

```c
override void OnDestroy()
{
    // Unsubscribe from events
    GetDayZularEvents().OnUpdate.Remove(OnUpdate);
    
    // Clear collections
    if (items)
        items.Clear();
    
    // Call parent cleanup
    super.OnDestroy();
}
```

---

## Production vs Development

### Development Settings

For debugging and development:

```json
{
    "logLevel": 0,
    "refreshRateInSeconds": 10,
    "enableFileLogging": true,
    "enableConsoleOutput": true
}
```

```c
// Enable verbose RPC logging
GetDayZularRPCManager().Use(new DayZularLoggingMiddleware());

// Enable debug mode
DZLog().SetLogLevel(DayZularLogLevel.Debug);
```

### Production Settings

For live servers:

```json
{
    "logLevel": 1,
    "refreshRateInSeconds": 60,
    "enableFileLogging": false,
    "enableConsoleOutput": false
}
```

```c
// Minimal logging
DZLog().SetLogLevel(DayZularLogLevel.Info);
DZLog().SetConsoleOutput(false);

// Add rate limiting to all public RPCs
GetDayZularRPCManager().SetRateLimit("MyMod", "PublicEndpoint", 10, 60.0);
```

### Performance Optimization

1. **Disable Console Output**
   - Console prints are expensive
   - Use file logging only when needed

2. **Use Info Log Level**
   - Debug logs add overhead
   - Only enable for troubleshooting

3. **Rate Limit RPCs**
   - Prevent abuse
   - Protect server performance

4. **Batch Component Updates**
   - Call `MarkForCheck()` once after multiple changes
   - Avoid excessive re-renders

5. **Clean Up Properly**
   - Remove event listeners
   - Clear large collections
   - Prevent memory leaks

---

## Environment-Specific Configuration

### Conditional Configuration

```c
class MyService : DayZularService
{
    void MyService()
    {
        #ifdef DAYZULAR_DEBUG
            DZLog().SetLogLevel(DayZularLogLevel.Debug);
            DZLog().SetConsoleOutput(true);
        #else
            DZLog().SetLogLevel(DayZularLogLevel.Info);
            DZLog().SetConsoleOutput(false);
        #endif
    }
}
```

### Server vs Client

```c
void MyService()
{
    if (GetGame().IsServer())
    {
        // Server-specific configuration
        GetDayZularRPCManager().AddRPC("MyMod", "ServerHandler", this, DayZularRPCType.Server);
    }
    
    if (GetGame().IsClient())
    {
        // Client-specific configuration
        GetDayZularRPCManager().AddRPC("MyMod", "ClientHandler", this, DayZularRPCType.Client);
    }
}
```

---

## Troubleshooting

### Enable Debug Logging

If something isn't working:

1. Enable debug logging:
```json
{
    "logLevel": 0,
    "enableFileLogging": true,
    "enableConsoleOutput": true
}
```

2. Check the log file for errors
3. Look for RPC registration messages
4. Verify service registration

### Common Issues

**RPC Not Firing:**
- Check handler signature: `(PlayerIdentity sender, ParamsReadContext ctx)`
- Verify namespace matches between registration and send
- Ensure handler is registered on correct side (client/server)

**Service Not Found:**
- Verify service is registered before retrieval
- Check registration happens in event subscription
- Ensure AppModule.Bootstrap() is called

**Component Not Rendering:**
- Verify component is registered with ComponentRegistry
- Check template path is correct
- Call `MarkForCheck()` after data changes

**High CPU Usage:**
- Disable console output
- Reduce log level to Info or Warn
- Check for infinite update loops
- Profile component re-renders

---

## Best Practices

1. **Use configuration files** for settings that may change
2. **Disable console output** in production
3. **Use appropriate log levels** - Debug for development, Info for production
4. **Rate limit public RPCs** to prevent abuse
5. **Clean up resources** in OnDestroy()
6. **Batch updates** to minimize re-renders
7. **Test with production settings** before release
8. **Monitor log files** for errors and warnings
9. **Document your configuration** for server admins
10. **Provide sensible defaults** that work out of the box

---

## Configuration Examples

### Minimal Production Config
```json
{
    "logLevel": 2,
    "refreshRateInSeconds": 300,
    "enableFileLogging": false,
    "enableConsoleOutput": false
}
```

### Development Config
```json
{
    "logLevel": 0,
    "refreshRateInSeconds": 5,
    "enableFileLogging": true,
    "enableConsoleOutput": true
}
```

### Troubleshooting Config
```json
{
    "logLevel": 0,
    "refreshRateInSeconds": 10,
    "enableFileLogging": true,
    "enableConsoleOutput": true
}
```

---

For more information, see:
- **FEATURES.md** - Complete feature documentation
- **ARCHITECTURE.md** - Technical architecture details
- **README.md** - Quick start guide
- **CREDITS.md** - Acknowledgments and philosophy
