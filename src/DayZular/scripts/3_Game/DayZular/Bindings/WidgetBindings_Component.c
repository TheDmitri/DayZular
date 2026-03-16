// DayZular Framework - Component Widget Binding
// Workbench-visible binding for dynamic components (single or foreach)

// Unified component binding for Workbench
class ComponentWidgetBinding : WidgetBindingBase
{
    // Workbench-visible parameters
    reference string Component_Selector;      // For single component: "{{ currentPage }}"
    reference string Component_ForEach;       // For array: "{{ _inputProducts }}"
    reference string Component_Template;      // Component selector to create
    reference string Component_ItemProperty;  // Optional: property to set (auto-detected if empty)
    
    override PropertyBinding CreateDataBinding(Widget widget)
    {
        // Determine which type of binding to create
        if (Component_ForEach != "" && Component_Template != "")
        {
            // ForEach binding (multiple components)
            return CreateForEachBinding(widget);
        }
        else if (Component_Selector != "")
        {
            // Single component binding
            return CreateComponentBinding(widget);
        }
        
        return null;
    }
    
    // Create ForEachBinding
    protected PropertyBinding CreateForEachBinding(Widget widget)
    {
        // Parent component is already set via SetComponent()
        if (!m_Component)
        {
            DZLog().LogError("ComponentWidgetBinding: No parent component found for ForEach");
            return null;
        }
        
        string itemProperty = Component_ItemProperty;
        if (itemProperty == "")
        {
            itemProperty = "_inputItem";  // Default
        }
        
        ForEachBinding binding = new ForEachBinding(widget, "foreach", Component_ForEach, Component_Template, itemProperty, m_Component);
        
        Print("ComponentWidgetBinding: Created ForEachBinding: " + Component_Template);
        return binding;
    }
    
    // Create ComponentBinding (single)
    protected PropertyBinding CreateComponentBinding(Widget widget)
    {
        // Parent component is already set via SetComponent()
        if (!m_Component)
        {
            DZLog().LogError("ComponentWidgetBinding: No parent component found for Component");
            return null;
        }
        
        ComponentBinding binding = new ComponentBinding(widget, "component", Component_Selector, m_Component);
        
        Print("ComponentWidgetBinding: Created ComponentBinding: " + Component_Selector);
        return binding;
    }
}
