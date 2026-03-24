// DayZular Framework - Component Binding
// Dynamically creates/destroys components based on selector binding

class ComponentBinding extends PropertyBinding
{
    protected string m_CurrentSelector = "";
    protected ref DZlrComponentBase m_CurrentComponent;
    protected DZlrComponentBase m_ParentComponent;
    
    void ComponentBinding(Widget widget, string propertyName, string expression, DZlrComponentBase parent)
    {
        m_Widget = widget;
        m_PropertyName = propertyName;
        m_Expression = expression;
        m_ParentComponent = parent;
    }
    
    override void Update(DZlrComponentBase component)
    {
        if (!component)
            return;
        
        // Evaluate expression to get selector
        string newSelector = EvaluateExpression(component, m_Expression);
        
        
        // Only recreate if selector changed
        if (newSelector != m_CurrentSelector)
        {
            // Destroy old component
            DestroyCurrentComponent();
            
            // Create new component if selector is not empty
            if (newSelector != "")
            {
                CreateComponent(newSelector);
            }
            
            m_CurrentSelector = newSelector;
        }
    }
    
    void CreateComponent(string selector)
    {
        
        // Create component via registry
        DZlrComponentBase component = ComponentRegistry.CreateComponent(selector);
        if (!component)
        {
            DZLog().LogError("ComponentBinding: Failed to create component: " + selector);
            return;
        }
        
        // Initialize component (loads its own template)
        component.InitComponent(null);
        
        // Parent the component's root widget to our container
        Widget componentRoot = component.GetRoot();
        
        if (componentRoot && m_Widget)
        {
            m_Widget.AddChild(componentRoot);
            componentRoot.Show(true);
        }
        else
        {
            DZLog().LogError("ComponentBinding: Cannot add widget - root or container is null");
        }
        
        // Add to parent's child components
        if (m_ParentComponent)
        {
            m_ParentComponent.m_ChildComponents.Insert(component);
        }
        
        m_CurrentComponent = component;
        
        // DZLog().LogDebug("ComponentBinding: Created component: " + selector);
    }
    
    void DestroyCurrentComponent()
    {
        if (!m_CurrentComponent)
            return;
        
        
        // Remove from parent's child components
        if (m_ParentComponent)
        {
            int index = m_ParentComponent.m_ChildComponents.Find(m_CurrentComponent);
            if (index != -1)
            {
                m_ParentComponent.m_ChildComponents.Remove(index);
            }
        }
        
        // Destroy component
        m_CurrentComponent.Destroy();
        delete m_CurrentComponent;
        m_CurrentComponent = null;
    }
    
    // Get current component (for accessing outputs, etc.)
    DZlrComponentBase GetComponent()
    {
        return m_CurrentComponent;
    }
    
    // Cleanup
    void ~ComponentBinding()
    {
        DestroyCurrentComponent();
    }
}
