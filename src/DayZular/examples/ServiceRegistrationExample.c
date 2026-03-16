// DayZular Framework - Service Registration Example
// Shows how multiple mods can register services without conflicts

// ===== MOD 1: Workbench Mod =====
class WorkbenchModAppInit
{
    void WorkbenchModAppInit()
    {
        // Subscribe to AppModule's service registration event
        AppModule.GetOnRegisterServices().Insert(RegisterWorkbenchServices);
    }
    
    static void RegisterWorkbenchServices()
    {
        auto registry = DZlrServiceRegistry.GetInstance();
        
        DZLog().LogInfo("Registering Workbench mod services...");
        
        registry.RegisterService(WorkbenchService, new WorkbenchService());
        registry.RegisterService(CraftingService, new CraftingService());
        
        DZLog().LogInfo("Workbench mod services registered");
    }
}

// Auto-initialize on script load
WorkbenchModAppInit g_WorkbenchModAppInit = new WorkbenchModAppInit();

// ===== MOD 2: Territory Mod =====
class TerritoryModAppInit
{
    void TerritoryModAppInit()
    {
        // Subscribe to AppModule's service registration event
        AppModule.GetOnRegisterServices().Insert(RegisterTerritoryServices);
    }
    
    static void RegisterTerritoryServices()
    {
        auto registry = DZlrServiceRegistry.GetInstance();
        
        DZLog().LogInfo("Registering Territory mod services...");
        
        registry.RegisterService(TerritoryService, new TerritoryService());
        registry.RegisterService(BuildingService, new BuildingService());
        
        DZLog().LogInfo("Territory mod services registered");
    }
}

// Auto-initialize on script load
TerritoryModAppInit g_TerritoryModAppInit = new TerritoryModAppInit();

// ===== MOD 3: Economy Mod =====
class EconomyModAppInit
{
    void EconomyModAppInit()
    {
        // Subscribe to AppModule's service registration event
        AppModule.GetOnRegisterServices().Insert(RegisterEconomyServices);
    }
    
    static void RegisterEconomyServices()
    {
        auto registry = DZlrServiceRegistry.GetInstance();
        
        DZLog().LogInfo("Registering Economy mod services...");
        
        registry.RegisterService(EconomyService, new EconomyService());
        registry.RegisterService(MarketService, new MarketService());
        
        DZLog().LogInfo("Economy mod services registered");
    }
}

// Auto-initialize on script load
EconomyModAppInit g_EconomyModAppInit = new EconomyModAppInit();

// ===== RESULT =====
/*
When AppModule.Bootstrap() is called:
1. AppModule initializes event bus
2. AppModule invokes Event_OnRegisterServices
3. ALL subscribed mods register their services:
   - WorkbenchModAppInit.RegisterWorkbenchServices()
   - TerritoryModAppInit.RegisterTerritoryServices()
   - EconomyModAppInit.RegisterEconomyServices()
4. All services auto-subscribe to lifecycle events in their constructors
5. Framework is ready

Log output:
[DayZular] Bootstrapping DayZular framework...
[DayZular] Invoking service registration...
[Workbench] Registering Workbench mod services...
[Workbench] Workbench mod services registered
[Territory] Registering Territory mod services...
[Territory] Territory mod services registered
[Economy] Registering Economy mod services...
[Economy] Economy mod services registered
[DayZular] DayZular framework bootstrapped successfully

✅ Multiple mods can coexist
✅ No static override conflicts
✅ Clean separation of concerns
*/
