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
    
    // Widget references (found by name convention)
    protected Widget m_DropdownContent;
    protected TextWidget m_ValueText;
    protected Widget m_ResetBtn;
    
    void DropdownBinding()
    {
        m_Handler = new DropdownBindingHandler(this);
    }
    
    override PropertyBinding CreateDataBinding(Widget widget)
    {
        // Find child widgets by naming convention
        if (widget)
        {
            m_DropdownContent = widget.FindAnyWidget("dropdownContent");
            m_ValueText = TextWidget.Cast(widget.FindAnyWidget("dropdownValue"));
            m_ResetBtn = widget.FindAnyWidget("dropdownReset");
            
            // Hide content initially
            if (m_DropdownContent)
            {
                m_DropdownContent.Show(false);
            }
        }
        
        // Create bindings for items and value
        if (Binding_Items && Binding_Items != "")
        {
            m_DataBinding = new DropdownPropertyBinding(widget, "items", Binding_Items, Binding_Value);
            return m_DataBinding;
        }
        
        return null;
    }
    
    // Toggle dropdown visibility
    void ToggleDropdown()
    {
        if (m_DropdownContent)
        {
            bool isVisible = m_DropdownContent.IsVisible();
            m_DropdownContent.Show(!isVisible);
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
    
    void DropdownBindingHandler(DropdownBinding binding)
    {
        m_Binding = binding;
    }
    
    override bool OnClick(Widget w, int x, int y, int button)
    {
        string widgetName = w.GetName();
        
        // Check for toggle button
        if (widgetName == "dropdownToggle" || widgetName.Contains("Toggle"))
        {
            if (m_Binding)
            {
                m_Binding.ToggleDropdown();
                return true;
            }
        }
        
        // Check for item click (items should have "dropdownItem" prefix)
        if (widgetName.Contains("dropdownItem"))
        {
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
        // Find content container
        Widget content = parent.FindAnyWidget("dropdownContent");
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
