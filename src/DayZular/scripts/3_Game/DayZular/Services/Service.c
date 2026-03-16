// DayZular Framework - Service System
// Dependency injection for shared business logic

// Service base class
class DayZularService
{
    void DayZularService() {}
}

// Service registry (singleton pattern for DI)
class DZlrServiceRegistry
{
    private static ref DZlrServiceRegistry s_Instance;
    private ref map<typename, ref DayZularService> m_Services;
    
    void DZlrServiceRegistry()
    {
        m_Services = new map<typename, ref DayZularService>();
    }
    
    static DZlrServiceRegistry GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new DZlrServiceRegistry();
            Print("DZlrServiceRegistry initialized");
        }
        return s_Instance;
    }
    
    void RegisterService(typename type, DayZularService service)
    {
        m_Services.Set(type, service);
        Print("Service registered: " + type.ToString());
    }
    
    DayZularService GetService(typename type)
    {
        if (m_Services.Contains(type))
        {
            return m_Services.Get(type);
        }
        
        DZLog().LogWarning("Service not found: " + type.ToString());
        return null;
    }
    
    bool HasService(typename type)
    {
        return m_Services.Contains(type);
    }
}
