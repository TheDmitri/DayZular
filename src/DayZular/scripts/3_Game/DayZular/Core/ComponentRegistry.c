// DayZular Framework - Component Registry
// Enables dynamic component creation by selector

class ComponentRegistry
{
    private static ref map<string, typename> s_Components;
    
    // Register a component type with a selector
    static void RegisterComponent(string selector, typename componentType)
    {
        if (!s_Components)
        {
            s_Components = new map<string, typename>();
            Print("ComponentRegistry initialized");
        }
        
        // Check if already registered
        if (s_Components.Contains(selector))
        {
            DZLog().LogWarning("Component '" + selector + "' already registered, overwriting");
        }
        
        s_Components.Set(selector, componentType);
        Print("Registered component: " + selector + " -> " + componentType.ToString());
    }
    
    // Get component type by selector
    static typename GetComponent(string selector)
    {
        typename type;
        
        if (!s_Components)
        {
            DZLog().LogError("ComponentRegistry not initialized");
            return type;
        }
        
        type = s_Components.Get(selector);
        if (!type)
        {
            DZLog().LogError("Component '" + selector + "' not found in registry");
        }
        
        return type;
    }
    
    // Create component instance by selector
    static DZlrComponentBase CreateComponent(string selector)
    {
        typename type = GetComponent(selector);
        if (!type)
        {
            return null;
        }
        
        // Spawn instance
        DZlrComponentBase component = DZlrComponentBase.Cast(type.Spawn());
        if (!component)
        {
            DZLog().LogError("Failed to spawn component '" + selector + "'");
            return null;
        }
        
        Print("Created component instance: " + selector);
        return component;
    }
    
    // Check if component is registered
    static bool IsRegistered(string selector)
    {
        if (!s_Components)
            return false;
        
        return s_Components.Contains(selector);
    }
    
    // Get all registered selectors
    static array<string> GetRegisteredSelectors()
    {
        array<string> selectors = new array<string>();
        
        if (!s_Components)
            return selectors;
        
        for (int i = 0; i < s_Components.Count(); i++)
        {
            selectors.Insert(s_Components.GetKey(i));
        }
        
        return selectors;
    }
    
    // Debug: Print all registered components
    static void PrintRegistry()
    {
        DZLog().LogInfo("===== Component Registry =====");
        
        if (!s_Components || s_Components.Count() == 0)
        {
            DZLog().LogInfo("No components registered");
            return;
        }
        
        for (int i = 0; i < s_Components.Count(); i++)
        {
            string selector = s_Components.GetKey(i);
            typename type = s_Components.GetElement(i);
            DZLog().LogInfo(selector + " -> " + type.ToString());
        }
        
        DZLog().LogInfo("=============================");
    }
}

// Auto-registration helper
// Usage: private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("selector", ComponentClass);
class ComponentAutoRegister
{
    void ComponentAutoRegister(string selector, typename componentType)
    {
        ComponentRegistry.RegisterComponent(selector, componentType);
    }
}
