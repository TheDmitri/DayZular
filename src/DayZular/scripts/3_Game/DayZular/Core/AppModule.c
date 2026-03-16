// DayZular Framework - Application Module
// Bootstrap and initialization (like Angular NgModule)
// Multiple mods can subscribe to Event_OnRegisterServices to register their services

class AppModule
{
    private static bool s_Initialized = false;
    private static ref ScriptInvoker Event_OnRegisterServices = new ScriptInvoker();
    
    // Subscribe to this event in your mod to register services
    // Example: AppModule.GetOnRegisterServices().Insert(MyMod.RegisterServices);
    static ScriptInvoker GetOnRegisterServices()
    {
        return Event_OnRegisterServices;
    }
    
    static void Bootstrap()
    {
        if (s_Initialized)
            return;
        
        DZLog().LogInfo("Bootstrapping DayZular framework...");
        
        // Register built-in pipes
        PipeRegistry.RegisterDefaultPipes();
        
        // Register the logger as a service
        DZlrServiceRegistry.GetInstance().RegisterService(DayZularLogger, DayZularLogger.GetInstance());
        
        // Initialize event bus
        GetDayZularEvents();
        
        // Invoke service registration event - all subscribed mods will register their services
        DZLog().LogInfo("Invoking service registration...");
        Event_OnRegisterServices.Invoke();
        
        s_Initialized = true;
        
        DZLog().LogInfo("DayZular framework bootstrapped successfully");
        
        // Log event subscriptions for debugging
        if (DZLog().GetLogLevel() == DayZularLogLevel.Debug)
        {
            GetDayZularEvents().LogSubscriptions();
        }
    }
    
    static void Shutdown()
    {
        DZLog().LogInfo("Shutting down DayZular framework...");
        
        DayZularLogger.Destroy();
        s_Initialized = false;
    }
    
    static bool IsInitialized()
    {
        return s_Initialized;
    }
}
