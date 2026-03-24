# DayZular RPC System

A modern, feature-rich RPC system for DayZ modding that goes far beyond CF RPC.

## File Structure

The RPC system is organized into focused, modular files:

- **`DayZularRPCTypes.c`** - Core types, enums, and constants
- **`DayZularRPCMessage.c`** - RPC message wrapper (ScriptRPC)
- **`DayZularMiddleware.c`** - Middleware system and built-in middleware
- **`DayZularRateLimiting.c`** - Rate limiting implementation
- **`DayZularRequestResponse.c`** - Request/response pattern classes
- **`DayZularMessageQueue.c`** - Message queuing for offline players
- **`DayZularRPCManager.c`** - Main RPC manager (global singleton)
- **`README.md`** - This documentation

## What Makes DayZular RPC Different

| Feature | CF RPC | Expansion RPC | **DayZular RPC** |
|---------|--------|---------------|------------------|
| Global Singleton | ✅ | ✅ | ✅ |
| Hash-based Routing | ✅ | ✅ | ✅ |
| Collision Detection | ❌ | ✅ | ✅ |
| **Request/Response** | ❌ | ❌ | ✅ |
| **Middleware** | ❌ | ❌ | ✅ |
| **Rate Limiting** | ❌ | ❌ | ✅ |
| **Auto Retry** | ❌ | ❌ | ✅ |
| **Message Queue** | ❌ | ❌ | ✅ |
| Dependencies | CF | Expansion | **None** |

## Quick Start

```enforce
// Register endpoints
GetDayZularRPCManager().AddRPC("MyMod", "OnDataReceived", this, DayZularRPCType.Client);
GetDayZularRPCManager().AddRPC("MyMod", "GetPlayerData", this, DayZularRPCType.Server);

// Send RPC (fire-and-forget)
GetDayZularRPCManager().SendRPC("MyMod", "OnDataReceived", params, true, recipient);

// Handler
void OnDataReceived(PlayerIdentity sender, ParamsReadContext ctx)
{
    Param1<string> data;
    if (ctx.Read(data))
         DZLog().LogDebug("Received: " + data.param1);
}
```

---

## Advanced Features

### 1. Request/Response Pattern

Like `fetch().then()` in JavaScript - send a request and get a callback when the response arrives.

```enforce
// CLIENT: Send request with callback
GetDayZularRPCManager().SendRequest(
    "MyMod",           // namespace
    "GetPlayerData",   // endpoint
    new Param1<string>(playerUID),  // params
    this,              // callback instance
    "OnPlayerDataReceived"  // callback method
);

// CLIENT: Handle response
void OnPlayerDataReceived(bool success, ParamsReadContext ctx)
{
    if (!success)
    {
         DZLog().LogDebug("Request failed or timed out!");
        return;
    }
    
    Param1<ref PlayerData> data;
    if (ctx.Read(data))
         DZLog().LogDebug("Got player data: " + data.param1.name);
}

// SERVER: Handle request and send response
void GetPlayerData(PlayerIdentity sender, ParamsReadContext ctx)
{
    // Read request ID first (automatically added)
    int requestID;
    ctx.Read(requestID);
    
    Param1<string> request;
    ctx.Read(request);
    
    // Process and send response
    PlayerData data = LoadPlayerData(request.param1);
    GetDayZularRPCManager().SendResponse(requestID, new Param1<ref PlayerData>(data), sender);
}
```

**Features:**
- Automatic timeout (default: 10 seconds)
- Automatic retry with exponential backoff (default: 3 retries)
- Callback on success or failure

---

### 2. Middleware/Interceptors

Like Express.js middleware - process RPCs before and after handling.

```enforce
// Add middleware (order matters!)
GetDayZularRPCManager().Use(new DayZularLoggingMiddleware());
GetDayZularRPCManager().Use(new MyAuthMiddleware());
GetDayZularRPCManager().Use(new MyValidationMiddleware());

// Create custom middleware
class MyAuthMiddleware extends DayZularMiddleware
{
    override bool OnBefore(DayZularRPCContext ctx)
    {
        // Block unauthenticated requests to admin endpoints
        if (ctx.endpoint.Contains("Admin") && !IsAdmin(ctx.sender))
        {
            ctx.blocked = true;
            ctx.blockReason = "Admin access required";
            return false;
        }
        return true;
    }
    
    override void OnAfter(DayZularRPCContext ctx)
    {
        // Log successful admin actions
        if (ctx.endpoint.Contains("Admin"))
            LogAdminAction(ctx.sender, ctx.endpoint);
    }
}
```

**Built-in Middleware:**
- `DayZularLoggingMiddleware` - Logs all RPC traffic
- `DayZularAuthMiddleware` - Protect endpoints with authentication

---

### 3. Rate Limiting

Prevent spam and abuse with per-endpoint rate limits.

```enforce
// Limit to 5 calls per second per player
GetDayZularRPCManager().SetRateLimit("MyMod", "SpawnItem", 5, 1.0);

// Limit to 10 calls per minute per player
GetDayZularRPCManager().SetRateLimit("MyMod", "TeleportRequest", 10, 60.0);
```

**Features:**
- Per-player tracking
- Sliding window algorithm
- Automatic cleanup of old entries

---

### 4. Automatic Retry with Exponential Backoff

Built into `SendRequest` - automatically retries failed requests.

```enforce
// Custom timeout and retry settings
GetDayZularRPCManager().SendRequest(
    "MyMod",
    "CriticalOperation",
    params,
    this,
    "OnCriticalResult",
    30.0,  // 30 second timeout
    5      // 5 max retries
);

// Backoff schedule: 30s -> 60s -> 120s -> 240s -> 480s
```

---

### 5. Message Queuing

Queue messages for offline players - delivered when they reconnect.

```enforce
// SERVER: Queue message for offline player
GetDayZularRPCManager().QueueMessage(
    targetPlayerUID,
    "MyMod",
    "PendingReward",
    new Param1<int>(1000),  // 1000 coins
    3600.0  // Expires in 1 hour
);

// SERVER: Flush queue when player connects (call from OnClientNew or similar)
GetDayZularRPCManager().FlushQueuedMessages(player.GetIdentity());
```

---

## Migration from CF RPC

### Before (CF)
```enforce
GetRPCManager().AddRPC("MyMod", "OnData", this, SingeplayerExecutionType.Client);
GetRPCManager().SendRPC("MyMod", "OnData", params, true, recipient);

void OnData(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Client) return;
    // ...
}
```

### After (DayZular)
```enforce
GetDayZularRPCManager().AddRPC("MyMod", "OnData", this, DayZularRPCType.Client);
GetDayZularRPCManager().SendRPC("MyMod", "OnData", params, true, recipient);

void OnData(PlayerIdentity sender, ParamsReadContext ctx)
{
    // No type check needed - cleaner!
    // ...
}
```

---

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    DayZular RPC Flow                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  SendRPC/SendRequest                                            │
│         │                                                        │
│         ▼                                                        │
│  ┌──────────────┐                                               │
│  │ Rate Limit   │ ◀── SetRateLimit()                            │
│  │   Check      │                                               │
│  └──────────────┘                                               │
│         │                                                        │
│         ▼                                                        │
│  ┌──────────────┐                                               │
│  │  Middleware  │ ◀── Use(middleware)                           │
│  │    Chain     │     OnBefore() → OnAfter()                    │
│  └──────────────┘                                               │
│         │                                                        │
│         ▼                                                        │
│  ┌──────────────┐                                               │
│  │   Handler    │ ◀── AddRPC()                                  │
│  │  (Your Code) │                                               │
│  └──────────────┘                                               │
│         │                                                        │
│         ▼                                                        │
│  ┌──────────────┐                                               │
│  │  Response    │ ◀── SendResponse() (for requests)             │
│  │  (Optional)  │                                               │
│  └──────────────┘                                               │
│                                                                  │
│  ┌─────────────────────────────────────────────────────┐        │
│  │              Pending Requests                        │        │
│  │  [Waiting] ──▶ [Timeout] ──▶ [Retry] ──▶ [Callback] │        │
│  └─────────────────────────────────────────────────────┘        │
│                                                                  │
│  ┌─────────────────────────────────────────────────────┐        │
│  │              Message Queue                           │        │
│  │  [Queued] ──▶ [Player Connects] ──▶ [Delivered]     │        │
│  └─────────────────────────────────────────────────────┘        │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

---

## Why DayZular RPC?

1. **Zero Dependencies** - No CF, no Expansion, standalone
2. **Modern API** - Inspired by web frameworks (Express, fetch)
3. **Production Ready** - Rate limiting, retries, queuing
4. **Extensible** - Middleware system for custom logic
5. **Clean Code** - Simpler handler signatures
6. **Future Proof** - Independent of third-party framework changes

---

## License

MIT - Use freely in your mods.
