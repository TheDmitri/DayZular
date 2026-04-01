// DayZular Framework - Dropdown Binding
// Custom dropdown component with toggle, list, and selection binding

class DropdownBinding : WidgetBindingBase
{
    // Data Binding
    reference string Binding_Items;      // Array of items to display
    reference string Binding_Value;      // Currently selected value
    reference string Binding_Placeholder; // Text when no selection
    
    // Event Binding
    reference string OnChange;           // Called when selection changes
    
    // Widget name configuration (optional - defaults to standard names)
    reference string Widget_Content;     // Name of content container (default: "dropdownContent")
    reference string Widget_Value;       // Name of value text widget (default: "dropdownValue")
    reference string Widget_Toggle;      // Name of toggle button (default: "dropdownToggle")
    reference string Widget_Reset;       // Name of reset button (default: "dropdownReset")
    
    // Widget references (found by name convention)
    protected Widget m_DropdownContent;
    protected TextWidget m_ValueText;
    protected Widget m_ResetBtn;
    
    void DropdownBinding()
    {
        m_Handler = new DropdownBindingHandler(this);
        DZLog().LogDebug("[DropdownBinding] Handler created");
    }
    
    override PropertyBinding CreateDataBinding(Widget widget)
    {
        string widgetName = "NULL";
        if (widget) widgetName = widget.GetName();
        DZLog().LogDebug("[DropdownBinding] CreateDataBinding called with widget: " + widgetName);
        
        // Set default widget names if not configured
        if (!Widget_Content || Widget_Content == "") Widget_Content = "dropdownContent";
        if (!Widget_Value || Widget_Value == "") Widget_Value = "dropdownValue";
        if (!Widget_Toggle || Widget_Toggle == "") Widget_Toggle = "dropdownToggle";
        if (!Widget_Reset || Widget_Reset == "") Widget_Reset = "dropdownReset";
        
        DZLog().LogDebug("[DropdownBinding] Looking for widgets: content=" + Widget_Content + ", value=" + Widget_Value + ", toggle=" + Widget_Toggle);
        
        // Find child widgets by configured names (handle numeric suffixes)
        if (widget)
        {
            // Search through children for widgets containing the configured names
            Widget child = widget.GetChildren();
            while (child)
            {
                string childName = child.GetName();
                if (childName.Contains(Widget_Content))
                {
                    m_DropdownContent = child;
                }
                else if (childName.Contains(Widget_Value))
                {
                    m_ValueText = TextWidget.Cast(child);
                }
                else if (childName.Contains(Widget_Reset))
                {
                    m_ResetBtn = child;
                }
                
                // Recursively search children
                if (!m_DropdownContent || !m_ValueText || !m_ResetBtn)
                {
                    SearchWidgetRecursive(child);
                }
                
                child = child.GetSibling();
            }
            
            string contentFound = "NO";
            if (m_DropdownContent) 
            {
                contentFound = "YES";
            }
            string valueFound = "NO";
            if (m_ValueText) 
            {
                valueFound = "YES";
            }
            string resetFound = "NO";
            if (m_ResetBtn) 
            {
                resetFound = "YES";
            }
            DZLog().LogDebug("[DropdownBinding] Widgets found - dropdownContent: " + contentFound + ", dropdownValue: " + valueFound + ", dropdownReset: " + resetFound);
            
            // Hide content initially
            if (m_DropdownContent)
            {
                m_DropdownContent.Show(false);
            }
            
            // Attach handler to toggle button
            Widget toggleBtn = FindWidgetContaining(widget, Widget_Toggle);
            string toggleFound = "NO";
            if (toggleBtn) 
            {
                toggleFound = "YES";
            }
            DZLog().LogDebug("[DropdownBinding] Looking for toggle button, found: " + toggleFound);
            if (toggleBtn && m_Handler)
            {
                toggleBtn.SetHandler(m_Handler);
                DZLog().LogDebug("[DropdownBinding] Handler attached to toggle button");
            }
        }
        
        // Create bindings for items and value
        if (Binding_Items && Binding_Items != "")
        {
            m_DataBinding = new DropdownPropertyBinding(widget, "items", Binding_Items, Binding_Value);
            DZLog().LogDebug("[DropdownBinding] CreateDataBinding - created DropdownPropertyBinding");
            return m_DataBinding;
        }
        
        DZLog().LogDebug("[DropdownBinding] CreateDataBinding completed - no data binding created (Binding_Items empty)");
        return null;
    }
    
    // Helper to search widget tree recursively
    protected void SearchWidgetRecursive(Widget parent)
    {
        Widget child = parent.GetChildren();
        while (child)
        {
            string childName = child.GetName();
            if (!m_DropdownContent && childName.Contains(Widget_Content))
            {
                m_DropdownContent = child;
            }
            else if (!m_ValueText && childName.Contains(Widget_Value))
            {
                m_ValueText = TextWidget.Cast(child);
            }
            else if (!m_ResetBtn && childName.Contains(Widget_Reset))
            {
                m_ResetBtn = child;
            }
            
            if (!m_DropdownContent || !m_ValueText || !m_ResetBtn)
            {
                SearchWidgetRecursive(child);
            }
            
            child = child.GetSibling();
        }
    }
    
    // Helper to find widget containing name
    protected Widget FindWidgetContaining(Widget parent, string nameFragment)
    {
        Widget child = parent.GetChildren();
        while (child)
        {
            string childName = child.GetName();
            if (childName.Contains(nameFragment))
            {
                return child;
            }
            
            Widget found = FindWidgetContaining(child, nameFragment);
            if (found) return found;
            
            child = child.GetSibling();
        }
        return null;
    }
    
    // Toggle dropdown visibility
    void ToggleDropdown()
    {
        // Lazy initialization: if CreateDataBinding wasn't called, find widgets now
        if (!m_DropdownContent)
        {
            // Set defaults if not configured
            if (!Widget_Content || Widget_Content == "") Widget_Content = "dropdownContent";
            
            DZLog().LogDebug("[DropdownBinding] Lazy init: finding " + Widget_Content + "...");
            // Try to find the widget from the handler's widget reference
            if (m_Handler)
            {
                DropdownBindingHandler handler = DropdownBindingHandler.Cast(m_Handler);
                if (handler)
                {
                    Widget widget = handler.GetWidget();
                    if (widget)
                    {
                        string widgetName = widget.GetName();
                        DZLog().LogDebug("[DropdownBinding] Lazy init: found widget " + widgetName);
                        
                        // Traverse up to find the root dropdown widget
                        // Toggle button is inside: dropdownOutline -> dropdownFrame -> dropdownPrefab
                        // dropdownContent is sibling of dropdownFrame, child of dropdownPrefab
                        Widget parent = widget.GetParent();
                        if (parent) parent = parent.GetParent();  // dropdownFrame
                        if (parent) parent = parent.GetParent();  // dropdownPrefab (root)
                        if (parent)
                        {
                            DZLog().LogDebug("[DropdownBinding] Lazy init: searching from parent " + parent.GetName());
                            
                            // Search for dropdownContent (may have numeric suffix like dropdownContent0)
                            Widget child = parent.GetChildren();
                            while (child)
                            {
                                string childName = child.GetName();
                                if (childName.Contains(Widget_Content))
                                {
                                    m_DropdownContent = child;
                                    DZLog().LogDebug("[DropdownBinding] Lazy init: found dropdownContent: " + childName);
                                    break;
                                }
                                child = child.GetSibling();
                            }
                        }
                        else
                        {
                            // Fallback: search from the widget itself
                            m_DropdownContent = FindWidgetContaining(widget, Widget_Content);
                        }
                        
                        if (m_DropdownContent)
                        {
                            DZLog().LogDebug("[DropdownBinding] Lazy init: found dropdownContent!");
                        }
                    }
                }
            }
        }
        
        if (m_DropdownContent)
        {
            bool isVisible = m_DropdownContent.IsVisible();
            string visibleStr = "hidden";
            if (isVisible) 
            {
                visibleStr = "visible";
            }
            string newVisibleStr = "visible";
            if (isVisible) 
            {
                newVisibleStr = "hidden";
            }
            DZLog().LogDebug("[DropdownBinding] ToggleDropdown: currently " + visibleStr + ", toggling to " + newVisibleStr);
            m_DropdownContent.Show(!isVisible);
        }
        else
        {
            DZLog().LogDebug("[DropdownBinding] ToggleDropdown: m_DropdownContent is NULL!");
        }
    }
    
    // Handle item selection
    bool HandleItemSelected(string itemValue)
    {
        // Update component value
        if (m_Component && Binding_Value != "")
        {
            // Set the value on component
            EnScript.SetClassVar(m_Component, Binding_Value, 0, itemValue);
            
            // Update display text
            UpdateDisplayText(itemValue);
            
            // Hide dropdown
            if (m_DropdownContent)
            {
                m_DropdownContent.Show(false);
            }
            
            // Fire change event
            if (OnChange && OnChange != "")
            {
                InvokeMethod(OnChange);
            }
            
            return true;
        }
        return false;
    }
    
    // Handle reset button
    bool HandleReset()
    {
        if (m_Component && Binding_Value != "")
        {
            string empty = "";
            EnScript.SetClassVar(m_Component, Binding_Value, 0, empty);
            UpdateDisplayText("");
            
            if (OnChange && OnChange != "")
            {
                InvokeMethod(OnChange);
            }
            return true;
        }
        return false;
    }
    
    // Update the display text widget
    protected void UpdateDisplayText(string value)
    {
        if (!m_ValueText)
            return;
        
        if (value == "" && Binding_Placeholder && Binding_Placeholder != "")
        {
            m_ValueText.SetText(Binding_Placeholder);
        }
        else
        {
            m_ValueText.SetText(value);
        }
    }
}

// Handler for dropdown widget events
class DropdownBindingHandler : ScriptedWidgetEventHandler
{
    protected DropdownBinding m_Binding;
    protected Widget m_Widget;  // Store reference to the widget
    
    void DropdownBindingHandler(DropdownBinding binding)
    {
        m_Binding = binding;
    }
    
    Widget GetWidget()
    {
        return m_Widget;
    }
    
    override bool OnMouseButtonDown(Widget w, int x, int y, int button)
    {
        m_Widget = w;
        string widgetName = w.GetName();
        DZLog().LogDebug("[DropdownBindingHandler] OnMouseButtonDown: " + widgetName + " button: " + button);
        
        // Handle toggle button (also handle OnClick for regular buttons)
        if (widgetName == "dropdownToggle" || widgetName.Contains("Toggle"))
        {
            DZLog().LogDebug("[DropdownBindingHandler] Toggle button clicked, toggling dropdown");
            if (m_Binding)
            {
                m_Binding.ToggleDropdown();
                return true;
            }
        }
        
        return false;
    }
    
    override bool OnClick(Widget w, int x, int y, int button)
    {
        m_Widget = w;
        string widgetName = w.GetName();
        DZLog().LogDebug("[DropdownBindingHandler] OnClick: " + widgetName);
        
        // Check for toggle button
        if (widgetName == "dropdownToggle" || widgetName.Contains("Toggle"))
        {
            DZLog().LogDebug("[DropdownBindingHandler] Toggle button click event");
            if (m_Binding)
            {
                m_Binding.ToggleDropdown();
                return true;
            }
        }
        
        // Check for item click (items should have "dropdownItem" prefix)
        if (widgetName.Contains("dropdownItem"))
        {
            DZLog().LogDebug("[DropdownBindingHandler] Item clicked: " + widgetName);
            if (m_Binding)
            {
                // Extract value from widget user data or name
                string itemValue = widgetName;
                itemValue.Replace("dropdownItem", "");
                return m_Binding.HandleItemSelected(itemValue);
            }
        }
        
        // Check for reset button
        if (widgetName == "dropdownReset" || widgetName.Contains("Reset"))
        {
            DZLog().LogDebug("[DropdownBindingHandler] Reset button clicked");
            if (m_Binding)
            {
                return m_Binding.HandleReset();
            }
        }
        
        return false;
    }
}

// Property binding for dropdown items
class DropdownPropertyBinding : PropertyBinding
{
    protected string m_ItemsExpression;
    protected string m_ValueExpression;
    protected string m_LastItemsHash;  // Track changes to avoid recreating widgets every frame
    protected string m_LastValue;      // Track value changes
    
    void DropdownPropertyBinding(Widget widget, string propertyName, string expression, string valueExpr)
    {
        m_Widget = widget;
        m_PropertyName = propertyName;
        m_Expression = expression;
        m_ItemsExpression = expression;
        m_ValueExpression = valueExpr;
        m_LastItemsHash = "";
        m_LastValue = "";
    }
    
    override void Update(DZlrComponentBase component)
    {
        if (!component || !m_Widget)
            return;
        
        // Get the actual array reference by evaluating the items expression
        array<string> itemsArray = null;
        if (m_ItemsExpression != "")
        {
            itemsArray = GetArrayProperty(component, m_ItemsExpression);
        }
        
        // Build hash of current items to detect changes
        string currentHash = "";
        if (itemsArray)
        {
            currentHash = itemsArray.Count().ToString();
            int i;
            for (i = 0; i < itemsArray.Count() && i < 10; i++)
            {
                currentHash += itemsArray[i].Hash().ToString();
            }
        }
        
        // Only repopulate if items changed
        if (currentHash != m_LastItemsHash)
        {
            m_LastItemsHash = currentHash;
            PopulateItemsFromArray(m_Widget, itemsArray);
        }
        
        // Update selected value display (check if value changed)
        if (m_ValueExpression != "")
        {
            string currentValue;
            EnScript.GetClassVar(component, m_ValueExpression, 0, currentValue);
            if (currentValue != m_LastValue)
            {
                m_LastValue = currentValue;
                UpdateValueDisplay(m_Widget, currentValue);
            }
        }
    }
    
    // Get array property by reference instead of string conversion
    protected array<string> GetArrayProperty(DZlrComponentBase component, string expression)
    {
        // Remove {{ and }} if present
        string propName = expression;
        propName.Replace("{{", "");
        propName.Replace("}}", "");
        propName.Trim();
        
        array<string> result = null;
        EnScript.GetClassVar(component, propName, 0, result);
        return result;
    }
    
    protected void PopulateItemsFromArray(Widget parent, array<string> items)
    {
        // Find the items container (not dropdownContent which has scroll structure)
        Widget content = parent.FindAnyWidget("dropdownItemsContainer");
        string containerFound = "NO";
        if (content) containerFound = "YES";
        DZLog().LogDebug("[DropdownBinding] PopulateItemsFromArray - dropdownItemsContainer found: " + containerFound);
        if (!content)
            return;
        
        // Clear existing items
        Widget child = content.GetChildren();
        while (child)
        {
            Widget next = child.GetSibling();
            child.Unlink();
            child = next;
        }
        
        // Create widgets from array
        if (items)
        {
            foreach (string item : items)
            {
                if (item != "")
                {
                    CreateItemWidget(content, item);
                }
            }
        }
    }
    
    protected void CreateItemWidget(Widget parent, string itemValue)
    {
        string widgetName = "dropdownItem" + itemValue;
        Widget root = GetGame().GetWorkspace().CreateWidgets("DayZular/gui/DropdownItem.layout", parent);
        if (root)
        {
            root.SetName(widgetName);
            TextWidget label = TextWidget.Cast(root.FindAnyWidget("dropdownItemLabel"));
            if (label)
                label.SetText(itemValue);
        }
    }
    
    protected void UpdateValueDisplay(Widget parent, string value)
    {
        TextWidget valueText = TextWidget.Cast(parent.FindAnyWidget("dropdownValue"));
        if (valueText)
        {
            valueText.SetText(value);
        }
    }
}
