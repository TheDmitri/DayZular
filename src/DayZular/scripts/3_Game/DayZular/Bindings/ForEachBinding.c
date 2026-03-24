// DayZular Framework - ForEach Binding
// Automatically creates/destroys components based on array data (like Angular *ngFor)

class ForEachBinding extends PropertyBinding
{
    // m_Expression inherited from PropertyBinding
    protected string m_ComponentTemplate;
    protected string m_ItemProperty;
    protected ref array<ref DZlrComponentBase> m_CreatedComponents;
    protected DZlrComponentBase m_ParentComponent;
    
    protected int m_LastArrayCount = 0;
    
    // Constructor matches PropertyBinding signature, then adds extra params
    void ForEachBinding(Widget widget, string propertyName, string expression, string componentTemplate, string itemProperty, DZlrComponentBase parent)
    {
        m_Widget = widget;
        m_PropertyName = propertyName;  // Use "foreach" or array property name
        m_Expression = expression;
        m_ComponentTemplate = componentTemplate;
        m_ItemProperty = itemProperty;
        m_ParentComponent = parent;
        m_CreatedComponents = new array<ref DZlrComponentBase>();
    }
    
    override void Update(DZlrComponentBase component)
    {
        if (!m_Widget || !component)
            return;
        
        // Evaluate expression to get array
        array<Class> items = GetArrayFromExpression(component, m_Expression);
        
        if (!items)
        {
             DZLog().LogDebug("ForEachBinding: Array is null for expression: " + m_Expression);
            ClearAllComponents();
            return;
        }
        
        int newCount = items.Count();
         DZLog().LogDebug("ForEachBinding: Expression=" + m_Expression + " newCount=" + newCount + " lastCount=" + m_LastArrayCount);
        
        // Check if array changed
        if (newCount != m_LastArrayCount)
        {
             DZLog().LogDebug("ForEachBinding: Array count changed, rebuilding components");
            RebuildComponents(items);
            m_LastArrayCount = newCount;
        }
        else
        {
            // Update existing components with new data
             DZLog().LogDebug("ForEachBinding: Array count same, updating existing components");
            UpdateComponents(items);
        }
    }
    
    // Get array from component property
    protected array<Class> GetArrayFromExpression(DZlrComponentBase component, string expr)
    {
        // Remove {{ }}
        expr.Replace("{{", "");
        expr.Replace("}}", "");
        expr = expr.Trim();
        
        
        // Get property value
        Class value;
        EnScript.GetClassVar(component, expr, 0, value);
        
        
        if (value)
        {
            
            // The value is an array, but it's a typed array (e.g., array<ref ProductData>)
            // We need to treat it as array<Class> for generic handling
            // Use reflection to get the count and iterate
            
            int count;
            if (g_Script.CallFunction(value, "Count", count, null))
            {
                // Create a new array<Class> and copy items
                array<Class> result = new array<Class>();
                for (int i = 0; i < count; i++)
                {
                    Class item;
                    // Try using Get() method instead of GetClassVar
                    if (g_Script.CallFunctionParams(value, "Get", item, new Param1<int>(i)))
                    {
                        result.Insert(item);
                    }
                }
                return result;
            }
            else
            {
                DZLog().LogWarning("ForEachBinding: Value doesn't have Count() method - not an array");
            }
        }
        
        return null;
    }
    
    // Rebuild all components
    protected void RebuildComponents(array<Class> items)
    {
         DZLog().LogDebug("ForEachBinding: Rebuilding " + items.Count() + " components");
        
        // Clear existing
        ClearAllComponents();
        
        // Create new components
        for (int i = 0; i < items.Count(); i++)
        {
            CreateComponentForItem(items[i], i);
        }
        
         DZLog().LogDebug("ForEachBinding: Finished rebuilding, created " + m_CreatedComponents.Count() + " components");
    }
    
    // Update existing components with new data
    protected void UpdateComponents(array<Class> items)
    {
         DZLog().LogDebug("ForEachBinding: Updating " + items.Count() + " existing components");
        
        for (int i = 0; i < items.Count() && i < m_CreatedComponents.Count(); i++)
        {
            DZlrComponentBase comp = m_CreatedComponents[i];
            if (comp)
            {
                SetComponentInput(comp, items[i]);
                comp.OnChanges();
                comp.MarkForCheck();
            }
        }
    }
    
    // Create component for single item
    protected void CreateComponentForItem(Class item, int index)
    {
         DZLog().LogDebug("ForEachBinding: Creating component #" + index + " template=" + m_ComponentTemplate);
        
        // Create component via registry
        DZlrComponentBase component = ComponentRegistry.CreateComponent(m_ComponentTemplate);
        if (!component)
        {
            DZLog().LogError("ForEachBinding: Failed to create component: " + m_ComponentTemplate);
            return;
        }
        
        // Set input property BEFORE initialization so bindings can access it
        SetComponentInput(component, item);
        
        // Initialize component (this will create and update bindings)
        component.InitComponent(null);
        
        // Add to parent's child components
        if (m_ParentComponent)
        {
            m_ParentComponent.m_ChildComponents.Insert(component);
        }
        
        // Add to container widget
        if (component.GetRoot() && m_Widget)
        {
            m_Widget.AddChild(component.GetRoot());
            component.GetRoot().Show(true);
             DZLog().LogDebug("ForEachBinding: Component #" + index + " added to widget tree");
        }
        else
        {
            DZLog().LogWarning("ForEachBinding: Component #" + index + " has no root or widget parent");
        }
        
        // Track created component
        m_CreatedComponents.Insert(component);
    }
    
    // Set input property on component
    protected void SetComponentInput(DZlrComponentBase component, Class item)
    {
        if (!component || !item)
            return;
        
        // Try specified property first
        if (m_ItemProperty != "" && EnScript.SetClassVar(component, m_ItemProperty, 0, item))
        {
            return;
        }
        
        // Auto-detect: Try to find _input* property that matches item type
        string itemTypeName = item.Type().ToString();
        
        // Try common patterns
        array<string> propertyPatterns = new array<string>();
        propertyPatterns.Insert("_inputItem");           // Generic
        propertyPatterns.Insert("_inputProduct");        // Common pattern
        propertyPatterns.Insert("_input" + itemTypeName); // Type-specific
        
        // Extract simple type name (e.g., "ProductData" from "ref ProductData")
        int lastSpace = itemTypeName.LastIndexOf(" ");
        if (lastSpace != -1)
        {
            string simpleTypeName = itemTypeName.Substring(lastSpace + 1, itemTypeName.Length() - lastSpace - 1);
            propertyPatterns.Insert("_input" + simpleTypeName);
        }
        
        // Also try without "Data" suffix if present
        if (itemTypeName.IndexOf("Data") != -1)
        {
            string withoutData = itemTypeName;
            withoutData.Replace("Data", "");
            propertyPatterns.Insert("_input" + withoutData);
        }
        
        // Try each pattern
        foreach (string pattern : propertyPatterns)
        {
                bool success = EnScript.SetClassVar(component, pattern, 0, item);
            
            // SetClassVar might return false even when it works, so verify by reading back
            Class verifyValue;
            EnScript.GetClassVar(component, pattern, 0, verifyValue);
            if (verifyValue)
            {
                return;
            }
        }
        
        DZLog().LogWarning("ForEachBinding: Could not find suitable _input property for type: " + itemTypeName);
    }
    
    // Clear all created components
    protected void ClearAllComponents()
    {
        
        foreach (DZlrComponentBase comp : m_CreatedComponents)
        {
            if (comp)
            {
                // Remove from widget tree
                if (comp.GetRoot())
                {
                    comp.GetRoot().Unlink();
                }
                
                // Remove from parent's child components
                if (m_ParentComponent)
                {
                    int index = m_ParentComponent.m_ChildComponents.Find(comp);
                    if (index != -1)
                    {
                        m_ParentComponent.m_ChildComponents.Remove(index);
                    }
                }
                
                // Destroy component
                comp.Destroy();
                delete comp;
            }
        }
        
        m_CreatedComponents.Clear();
    }
    
    // Get created components (for manual event subscription)
    array<ref DZlrComponentBase> GetComponents()
    {
        return m_CreatedComponents;
    }
    
    // Cleanup
    void ~ForEachBinding()
    {
        ClearAllComponents();
    }
}
