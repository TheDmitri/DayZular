// DayZular - Component Event Binding for Layout-Driven Event Handling
// Allows widgets to specify event handlers directly in layout files
// Follows DabsFramework pattern: Managed class + Handler class

class ComponentEventBinding : Managed
{
    // Layout-editable fields (using 'reference' makes them visible in Workbench)
    reference string OnClick;
    reference string OnDoubleClick;
    reference string OnMouseEnter;
    reference string OnMouseLeave;
    reference string OnFocus;
    reference string OnFocusLost;
    reference string OnChange;
    
    // Reference to the component that owns this widget
    protected DZlrComponentBase m_Component;
    
    // Handler instance
    protected ref ComponentEventBindingHandler m_Handler;
    
    void ComponentEventBinding()
    {
        m_Handler = new ComponentEventBindingHandler(this);
    }
    
    void SetComponent(DZlrComponentBase component)
    {
        m_Component = component;
    }
    
    DZlrComponentBase GetComponent()
    {
        return m_Component;
    }
    
    ComponentEventBindingHandler GetHandler()
    {
        return m_Handler;
    }
    
    // Event handler methods (called by ComponentBindingHandler)
    bool HandleOnClick(Widget w, int x, int y, int button)
    {
        if (OnClick != "" && m_Component)
        {
            return InvokeMethod(OnClick, new ClickEventArgs(w, x, y, button));
        }
        
        // Bubble to component's OnClick
        if (m_Component)
        {
            return m_Component.OnClick(w, x, y, button);
        }
        
        return false;
    }
    
    bool HandleOnDoubleClick(Widget w, int x, int y, int button)
    {
        if (OnDoubleClick != "" && m_Component)
        {
            return InvokeMethod(OnDoubleClick, new ClickEventArgs(w, x, y, button));
        }
        
        if (m_Component)
        {
            return m_Component.OnDoubleClick(w, x, y, button);
        }
        
        return false;
    }
    
    bool HandleOnMouseEnter(Widget w, int x, int y)
    {
        if (OnMouseEnter != "" && m_Component)
        {
            return InvokeMethod(OnMouseEnter, new MouseEventArgs(w, x, y));
        }
        
        if (m_Component)
        {
            return m_Component.OnMouseEnter(w, x, y);
        }
        
        return false;
    }
    
    bool HandleOnMouseLeave(Widget w, Widget enterW, int x, int y)
    {
        if (OnMouseLeave != "" && m_Component)
        {
            return InvokeMethod(OnMouseLeave, new MouseEventArgs(w, x, y));
        }
        
        if (m_Component)
        {
            return m_Component.OnMouseLeave(w, enterW, x, y);
        }
        
        return false;
    }
    
    bool HandleOnFocus(Widget w, int x, int y)
    {
        if (OnFocus != "" && m_Component)
        {
            return InvokeMethod(OnFocus, new MouseEventArgs(w, x, y));
        }
        
        if (m_Component)
        {
            return m_Component.OnFocus(w, x, y);
        }
        
        return false;
    }
    
    bool HandleOnFocusLost(Widget w, int x, int y)
    {
        if (OnFocusLost != "" && m_Component)
        {
            return InvokeMethod(OnFocusLost, new MouseEventArgs(w, x, y));
        }
        
        if (m_Component)
        {
            return m_Component.OnFocusLost(w, x, y);
        }
        
        return false;
    }
    
    bool HandleOnChange(Widget w, int x, int y, bool finished)
    {
        if (OnChange != "" && m_Component)
        {
            return InvokeMethod(OnChange, new ChangeEventArgs(w, x, y, finished));
        }
        
        if (m_Component)
        {
            return m_Component.OnChange(w, x, y, finished);
        }
        
        return false;
    }
    
    // Invoke a method on the component by name
    protected bool InvokeMethod(string methodName, EventArgs args)
    {
        if (!m_Component || methodName == "")
        {
            return false;
        }
        
        Print("Invoking method: " + methodName + " on " + m_Component.ToString());
        
        bool result = false;
        
        // Try to call the method with EventArgs parameter
        if (g_Script.CallFunction(m_Component, methodName, result, args))
        {
            return result;
        }
        
        // Try to call without parameters (for simple handlers)
        if (g_Script.CallFunction(m_Component, methodName, result, null))
        {
            return result;
        }
        
        DZLog().LogWarning("Method '" + methodName + "' not found on component " + m_Component.ToString());
        return false;
    }
}

// Event argument classes
class EventArgs
{
    Widget widget;
    int x;
    int y;
    
    void EventArgs(Widget w, int _x, int _y)
    {
        widget = w;
        x = _x;
        y = _y;
    }
}

class ClickEventArgs : EventArgs
{
    int button;
    
    void ClickEventArgs(Widget w, int _x, int _y, int _button)
    {
        widget = w;
        x = _x;
        y = _y;
        button = _button;
    }
}

class MouseEventArgs : EventArgs
{
    void MouseEventArgs(Widget w, int _x, int _y)
    {
        widget = w;
        x = _x;
        y = _y;
    }
}

class ChangeEventArgs : EventArgs
{
    bool finished;
    
    void ChangeEventArgs(Widget w, int _x, int _y, bool _finished)
    {
        widget = w;
        x = _x;
        y = _y;
        finished = _finished;
    }
}

// ComponentEventBindingHandler - The actual ScriptedWidgetEventHandler
// Delegates all events to the ComponentEventBinding instance
class ComponentEventBindingHandler : ScriptedWidgetEventHandler
{
    protected ComponentEventBinding m_ComponentBinding;
    
    void ComponentEventBindingHandler(ComponentEventBinding binding)
    {
        m_ComponentBinding = binding;
    }
    
    override bool OnClick(Widget w, int x, int y, int button)
    {
        return m_ComponentBinding && m_ComponentBinding.HandleOnClick(w, x, y, button);
    }
    
    override bool OnDoubleClick(Widget w, int x, int y, int button)
    {
        return m_ComponentBinding && m_ComponentBinding.HandleOnDoubleClick(w, x, y, button);
    }
    
    override bool OnMouseEnter(Widget w, int x, int y)
    {
        return m_ComponentBinding && m_ComponentBinding.HandleOnMouseEnter(w, x, y);
    }
    
    override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
    {
        return m_ComponentBinding && m_ComponentBinding.HandleOnMouseLeave(w, enterW, x, y);
    }
    
    override bool OnFocus(Widget w, int x, int y)
    {
        return m_ComponentBinding && m_ComponentBinding.HandleOnFocus(w, x, y);
    }
    
    override bool OnFocusLost(Widget w, int x, int y)
    {
        return m_ComponentBinding && m_ComponentBinding.HandleOnFocusLost(w, x, y);
    }
    
    override bool OnChange(Widget w, int x, int y, bool finished)
    {
        return m_ComponentBinding && m_ComponentBinding.HandleOnChange(w, x, y, finished);
    }
}
