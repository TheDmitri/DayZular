// DayZular Framework - Nav Button Binding
// Simplifies tab navigation by binding selector directly in layout

class NavButtonBinding : WidgetBindingBase
{
    // Workbench-visible: which component to load when clicked
    reference string TargetSelector = "";
    
    protected ButtonWidget m_Button;
    protected string m_CurrentSelector = "";
    
    void NavButtonBinding()
    {
        m_Handler = new NavButtonBindingHandler(this);
    }
    
    override void SetComponent(DZlrComponentBase component)
    {
        super.SetComponent(component);
        
        // Subscribe to component changes for visual feedback
        if (component)
        {
            component.Event_OnComponentChanged.Insert(OnComponentChanged);
        }
    }
    
    override PropertyBinding CreateDataBinding(Widget widget)
    {
        m_Button = ButtonWidget.Cast(widget);
        if (!m_Button)
        {
            DZLog().LogError("NavButtonBinding: Widget is not a ButtonWidget");
        }
        
        // No data binding needed - we handle clicks directly
        return null;
    }
    
    bool HandleOnClick(Widget w, int x, int y, int button)
    {
        if (!m_Component || TargetSelector == "")
            return false;
        
        // Call SelectComponent on the parent component
        g_Script.CallFunction(m_Component, "SelectComponent", null, TargetSelector);
        return true;
    }
    
    void OnComponentChanged(string newSelector)
    {
        if (!m_Button)
            return;
        
        m_CurrentSelector = newSelector;
        
        // Update visual state - active if this button's target matches current
        if (TargetSelector == newSelector)
        {
            m_Button.SetTextColor(ARGB(255, 175, 188, 56));
            m_Button.SetAlpha(1.0);
        }
        else
        {
            m_Button.SetTextColor(ARGB(255, 165, 165, 165));
            m_Button.SetAlpha(0.7);
        }
    }
}

class NavButtonBindingHandler : ScriptedWidgetEventHandler
{
    protected NavButtonBinding m_Binding;
    
    void NavButtonBindingHandler(NavButtonBinding binding)
    {
        m_Binding = binding;
    }
    
    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (m_Binding)
            return m_Binding.HandleOnClick(w, x, y, button);
        return false;
    }
}
