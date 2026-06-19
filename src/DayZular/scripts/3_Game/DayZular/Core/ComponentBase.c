// DayZular Framework - Component Base Class
// Angular-inspired component system for DayZ

class DZlrComponentBase extends ScriptedWidgetEventHandler
{
    protected Widget m_Root;
    protected ref ComponentMetadata m_Metadata;
    protected ref array<ref PropertyBinding> m_Bindings;
    ref array<ref DZlrComponentBase> m_ChildComponents;  // Public for bindings to access
    protected ref map<string, Class> m_Inputs;
    protected bool m_IsInitialized;
    protected bool m_IsDestroyed;
    
    // Events for component communication
    ref ScriptInvoker Event_OnComponentChanged = new ScriptInvoker();
    
    // Lifecycle hooks (virtual methods to override)
    void OnInit() {}
    void OnDestroy() {}
    void OnChanges() {}
    
    // Destroy component
    void Destroy()
    {
        if (m_IsDestroyed)
            return;
        
        m_IsDestroyed = true;
        OnDestroy();
        
        // Destroy child components
        if (m_ChildComponents)
        {
            foreach (DZlrComponentBase child : m_ChildComponents)
            {
                if (child)
                {
                    child.Destroy();
                }
            }
            m_ChildComponents.Clear();
        }
        
        // Unlink root widget
        if (m_Root)
        {
            m_Root.Unlink();
        }
    }
    
    // Constructor
    void DZlrComponentBase()
    {
        m_Bindings = new array<ref PropertyBinding>();
        m_ChildComponents = new array<ref DZlrComponentBase>();
        m_Inputs = new map<string, Class>();
        m_IsInitialized = false;
        m_IsDestroyed = false;
        
        // Register metadata (called by child classes)
        RegisterMetadata();
        
        // Auto-detect inputs via reflection
        AutoDetectInputs();
    }
    
    // Destructor
    void ~DZlrComponentBase()
    {
        if (!m_IsDestroyed)
        {
            OnDestroy();
        }
        
        // Cleanup child components (only if not already destroyed)
        if (m_ChildComponents && !m_IsDestroyed)
        {
            foreach (DZlrComponentBase child : m_ChildComponents)
            {
                delete child;
            }
            m_ChildComponents.Clear();
        }
        
        // Cleanup bindings
        if (m_Bindings)
        {
            m_Bindings.Clear();
        }
    }
    
    // Override this to set component metadata
    protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "base-component";
        m_Metadata.templatePath = "";
    }
    
    // Initialize component with widget
    void InitComponent(Widget root)
    {
         DZLog().LogDebug("[DayZular] InitComponent: Starting initialization for " + this.Type().ToString());
        
        if (m_IsInitialized)
        {
            DZLog().LogWarning("Component already initialized");
            return;
        }
        
        // If root is null, create from layout
        if (!root && m_Metadata && m_Metadata.templatePath != "")
        {
            m_Root = GetGame().GetWorkspace().CreateWidgets(m_Metadata.templatePath);
            if (!m_Root)
            {
                DZLog().LogError("Failed to create widget from layout: " + m_Metadata.templatePath);
                return;
            }
        }
        else
        {
            m_Root = root;
        }
        
        if (m_Root)
        {
            m_Root.SetHandler(this);
        }
        else
        {
            DZLog().LogError("Component has no root widget!");
            return;
        }
        
        // Load template and create bindings
        LoadTemplate();
        LoadChildWidgets();
        
        AttachEventBindings();
        
        // NEW: Auto-create bindings from layout (Param)
         DZLog().LogDebug("[DayZular] InitComponent: About to call AutoCreateBindings for " + this.Type().ToString());
        AutoCreateBindings();
        
        // OLD: Manual binding creation (still supported for backward compatibility)
        CreateBindings();
        
        m_IsInitialized = true;
        
        // Initial binding update
        UpdateBindings();
        
        // Call lifecycle hook
        OnInit();
    }
    
    // Attach ComponentBinding event handlers to widgets
    protected void AttachEventBindings()
    {
        if (!m_Root) return;
        
        AttachEventBindingsRecursive(m_Root);
    }
    
    protected void AttachEventBindingsRecursive(Widget widget)
    {
        if (!widget) return;
        
        // Check for widget binding scripts
        Class widgetScript;
        widget.GetScript(widgetScript);
        
        if (widgetScript)
        {
            // Try unified widget bindings
            WidgetBindingBase widgetBinding = WidgetBindingBase.Cast(widgetScript);
            if (widgetBinding)
            {
                widgetBinding.SetComponent(this);

                // Attach event handler if present
                ScriptedWidgetEventHandler handler = widgetBinding.GetHandler();
                if (handler)
                {
                    widget.SetHandler(handler);
                }

                 DZLog().LogDebug("Attached WidgetBinding to widget: " + widget.GetName());
            }
            else
            {
                // Backward compatibility with ComponentEventBinding
                ComponentEventBinding legacyBinding = ComponentEventBinding.Cast(widgetScript);
                if (legacyBinding)
                {
                    legacyBinding.SetComponent(this);
                    widget.SetHandler(legacyBinding.GetHandler());
                     DZLog().LogDebug("Attached ComponentEventBinding (legacy) to widget: " + widget.GetName());
                }
            }
        }
        
        // Recursively process children
        Widget child = widget.GetChildren();
        while (child)
        {
            AttachEventBindingsRecursive(child);
            child = child.GetSibling();
        }
    }
    
    // Load layout file (if not already loaded)
    protected void LoadTemplate()
    {
        // Template is already loaded by layout system
        // This method is here for future expansion
    }
    
    // Find and initialize child widgets (override in child classes)
    protected void LoadChildWidgets()
    {
        // Auto-load widgets based on member variable names
        AutoLoadWidgets();
        
        // Override in child classes to find specific widgets
    }
    
    // Create property bindings (override in child classes)
    protected void CreateBindings()
    {
        // Override in child classes to setup bindings
    }
    
    // Input property setter (replaces Angular @Input)
    void SetInput(string name, Class value)
    {
        m_Inputs.Set(name, value);
        OnChanges();
        UpdateBindings();
    }
    
    // Get input property
    Class GetInput(string name)
    {
        return m_Inputs.Get(name);
    }
    
    
    // Query child component by type (helper for auto-created components)
    DZlrComponentBase QueryChild(typename componentType)
    {
        foreach (DZlrComponentBase child : m_ChildComponents)
        {
            if (child.IsInherited(componentType))
            {
                return child;
            }
        }
        return null;
    }
    
    // Query all child components of a specific type
    array<ref DZlrComponentBase> QueryChildren(typename componentType)
    {
        array<ref DZlrComponentBase> results = new array<ref DZlrComponentBase>();
        
        foreach (DZlrComponentBase child : m_ChildComponents)
        {
            if (child.IsInherited(componentType))
            {
                results.Insert(child);
            }
        }
        
        return results;
    }
    
    // Update all bindings
    void UpdateBindings()
    {
         DZLog().LogDebug("[DayZular] UpdateBindings: Updating " + m_Bindings.Count() + " bindings for component");
        foreach (PropertyBinding binding : m_Bindings)
        {
            binding.Update(this);
        }
    }
    
    // Mark for change detection (triggers update)
    void MarkForCheck()
    {
        UpdateBindings();
    }
    
    // Get root widget
    Widget GetRoot()
    {
        return m_Root;
    }
    
    // Query child component by selector
    DZlrComponentBase QueryChildBySelector(string selector)
    {
        foreach (DZlrComponentBase child : m_ChildComponents)
        {
            if (child && child.m_Metadata && child.m_Metadata.selector == selector)
            {
                return child;
            }
        }
        return null;
    }
    
    // Get component metadata
    ComponentMetadata GetMetadata()
    {
        return m_Metadata;
    }
    
    // Helper: Load widget into variable by name matching
    protected void AutoLoadWidgets()
    {
        // This uses reflection to auto-assign widgets to class variables
        // based on matching names (similar to Angular's ViewChild)
        typename type = Type();
        for (int i = 0; i < type.GetVariableCount(); i++)
        {
            typename varType = type.GetVariableType(i);
            string varName = type.GetVariableName(i);
            
            // Check if variable is a Widget type
            if (varType.IsInherited(Widget))
            {
                Widget widget;
                if (m_Root)
                {
                    widget = m_Root.FindAnyWidget(varName);
                }
                if (widget)
                {
                    EnScript.SetClassVar(this, varName, 0, widget);
                }
            }
        }
    }
    
    // Auto-detect inputs via reflection
    // Detects variables with _input prefix
    protected void AutoDetectInputs()
    {
        typename type = Type();
        int inputCount = 0;
        
        for (int i = 0; i < type.GetVariableCount(); i++)
        {
            string varName = type.GetVariableName(i);
            
            // Check for _input prefix (case-sensitive)
            if (varName.IndexOf("_input") == 0)
            {
                // Extract input name: _inputProduct -> Product
                string inputName = varName.Substring(6, varName.Length() - 6); // Remove "_input"
                
                // Register as input
                m_Inputs.Set(inputName, null);
                inputCount++;
                
                 DZLog().LogDebug("Found input: " + varName + " -> " + inputName);
            }
        }
        
        if (inputCount > 0)
        {
             DZLog().LogDebug("Auto-detected " + inputCount + " inputs for " + type.ToString());
        }
    }
    
    // NEW: Auto-create bindings from widget scriptclass (Param)
    protected void AutoCreateBindings()
    {
        if (!m_Root) return;
        
        int bindingCount = AutoCreateBindingsRecursive(m_Root);
        
        if (bindingCount > 0)
        {
             DZLog().LogDebug("[DayZular] Auto-created " + bindingCount + " bindings for: " + m_Root.GetName());
        }
    }
    
    protected int AutoCreateBindingsRecursive(Widget widget)
    {
        if (!widget) return 0;
        
        int count = 0;
        
        string widgetName = widget.GetName();
        
        // Check for unified widget bindings
        Class bindingScript;
        widget.GetScript(bindingScript);
        
        if (bindingScript)
        {
             DZLog().LogDebug("[DayZular] AutoCreateBindings: Widget " + widgetName + " has script: " + bindingScript.Type().ToString());
            
            // Try unified widget bindings (handles both data and events)
            WidgetBindingBase widgetBinding = WidgetBindingBase.Cast(bindingScript);
            if (widgetBinding)
            {
                 DZLog().LogDebug("[DayZular] AutoCreateBindings: Found WidgetBinding on widget: " + widgetName + " (type: " + bindingScript.Type().ToString() + ")");
                
                // Set component first!
                widgetBinding.SetComponent(this);
                
                PropertyBinding dataBinding = widgetBinding.CreateDataBinding(widget);
                if (dataBinding)
                {
                    m_Bindings.Insert(dataBinding);
                    count++;
                     DZLog().LogDebug("[DayZular] AutoCreateBindings: Created data binding for widget: " + widgetName);
                }
                else
                {
                     DZLog().LogDebug("[DayZular] AutoCreateBindings: No data binding created for widget: " + widgetName + " - Binding_Expression may be empty");
                }
            }
            else
            {
                 DZLog().LogDebug("[DayZular] AutoCreateBindings: Widget " + widgetName + " script is not a WidgetBindingBase");
            }
        }
        
        // Recurse children
        Widget child = widget.GetChildren();
        int childCount = 0;
        while (child)
        {
            childCount++;
            count += AutoCreateBindingsRecursive(child);
            child = child.GetSibling();
        }
        
        if (childCount > 0)
        {
             DZLog().LogDebug("[DayZular] AutoCreateBindings: Widget " + widgetName + " has " + childCount + " children");
        }
        
        return count;
    }
}
