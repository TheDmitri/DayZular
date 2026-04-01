// DayZular Framework - Property Binding Implementations
// Internal classes that do the actual binding work

// ============================================================================
// COMBOBOX PROPERTY BINDING
// ============================================================================

class ComboBoxPropertyBinding extends PropertyBinding
{
    protected string m_ItemsExpression;
    protected string m_SelectionExpression;
    
    void ComboBoxPropertyBinding(Widget widget, string propertyName, string expression, string selectionExpr = "")
    {
        m_Widget = widget;
        m_PropertyName = propertyName;
        m_Expression = expression;
        m_ItemsExpression = expression;
        m_SelectionExpression = selectionExpr;
    }
    
    override void Update(DZlrComponentBase component)
    {
        XComboBoxWidget comboBox = XComboBoxWidget.Cast(m_Widget);
        if (!comboBox) return;
        
        // Get items array
        string itemsProperty = m_ItemsExpression;
        itemsProperty.Replace("{{", "");
        itemsProperty.Replace("}}", "");
        itemsProperty = itemsProperty.Trim();
        
        Class itemsValue;
        if (EnScript.GetClassVar(component, itemsProperty, 0, itemsValue))
        {
            array<string> items = array<string>.Cast(itemsValue);
            if (items)
            {
                comboBox.ClearAll();
                for (int i = 0; i < items.Count(); i++)
                {
                    comboBox.AddItem(items[i]);
                }
            }
        }
        
        // Update selection
        if (m_SelectionExpression != "")
        {
            string selectionProperty = m_SelectionExpression;
            selectionProperty.Replace("{{", "");
            selectionProperty.Replace("}}", "");
            selectionProperty = selectionProperty.Trim();
            
            int selectionIndex;
            if (EnScript.GetClassVar(component, selectionProperty, 0, selectionIndex))
            {
                comboBox.SetCurrentItem(selectionIndex);
            }
        }
    }
}

// ============================================================================
// TEXTLISTBOX PROPERTY BINDING
// ============================================================================

class TextListboxPropertyBinding extends PropertyBinding
{
    protected string m_ItemsExpression;
    protected string m_SelectionExpression;
    
    void TextListboxPropertyBinding(Widget widget, string propertyName, string expression, string selectionExpr = "")
    {
        m_Widget = widget;
        m_PropertyName = propertyName;
        m_Expression = expression;
        m_ItemsExpression = expression;
        m_SelectionExpression = selectionExpr;
    }
    
    override void Update(DZlrComponentBase component)
    {
        TextListboxWidget listbox = TextListboxWidget.Cast(m_Widget);
        if (!listbox) return;
        
        // Get items array
        string itemsProperty = m_ItemsExpression;
        itemsProperty.Replace("{{", "");
        itemsProperty.Replace("}}", "");
        itemsProperty = itemsProperty.Trim();
        
        Class itemsValue;
        if (EnScript.GetClassVar(component, itemsProperty, 0, itemsValue))
        {
            array<string> items = array<string>.Cast(itemsValue);
            if (items)
            {
                listbox.ClearItems();
                for (int i = 0; i < items.Count(); i++)
                {
                    listbox.AddItem(items[i], NULL, 0);
                }
            }
        }
        
        // Update selection
        if (m_SelectionExpression != "")
        {
            string selectionProperty = m_SelectionExpression;
            selectionProperty.Replace("{{", "");
            selectionProperty.Replace("}}", "");
            selectionProperty = selectionProperty.Trim();
            
            int selectionIndex;
            if (EnScript.GetClassVar(component, selectionProperty, 0, selectionIndex))
            {
                listbox.SelectRow(selectionIndex);
            }
        }
    }
}

// ============================================================================
// ITEMPREVIEW PROPERTY BINDING
// ============================================================================

class ItemPreviewPropertyBinding extends PropertyBinding
{
    protected string m_BindingExpression;
    
    void ItemPreviewPropertyBinding(Widget widget, string propertyName, string expression)
    {
        m_Widget = widget;
        m_PropertyName = propertyName;
        m_Expression = expression;
        m_BindingExpression = expression;
    }
    
    override void Update(DZlrComponentBase component)
    {
        ItemPreviewWidget preview = ItemPreviewWidget.Cast(m_Widget);
        if (!preview) return;
        
        string itemProperty = m_BindingExpression;
        itemProperty.Replace("{{", "");
        itemProperty.Replace("}}", "");
        itemProperty = itemProperty.Trim();
        
        
        Class itemValue;
        EnScript.GetClassVar(component, itemProperty, 0, itemValue);
        
        
        if (itemValue)
        {
            EntityAI item = EntityAI.Cast(itemValue);
            if (item)
            {
                preview.SetItem(item);
                preview.SetModelOrientation(vector.Zero);
            }
        }
    }
}

// ============================================================================
// PLAYERPREVIEW PROPERTY BINDING
// ============================================================================

class PlayerPreviewPropertyBinding extends PropertyBinding
{
    protected string m_BindingExpression;
    
    void PlayerPreviewPropertyBinding(Widget widget, string propertyName, string expression)
    {
        m_Widget = widget;
        m_PropertyName = propertyName;
        m_Expression = expression;
        m_BindingExpression = expression;
    }
    
    override void Update(DZlrComponentBase component)
    {
        PlayerPreviewWidget preview = PlayerPreviewWidget.Cast(m_Widget);
        if (!preview) return;
        
        string playerProperty = m_BindingExpression;
        playerProperty.Replace("{{", "");
        playerProperty.Replace("}}", "");
        playerProperty = playerProperty.Trim();
        
        Class playerValue;
        if (EnScript.GetClassVar(component, playerProperty, 0, playerValue))
        {
            DayZPlayer player = DayZPlayer.Cast(playerValue);
            if (player)
            {
                preview.SetPlayer(player);
                preview.SetModelOrientation(vector.Zero);
            }
        }
    }
}

// ============================================================================
// EDITBOX PROPERTY BINDING
// ============================================================================

class EditBoxPropertyBinding extends PropertyBinding
{
    protected string m_BindingExpression;
    protected bool m_TwoWay;
    
    void EditBoxPropertyBinding(Widget widget, string propertyName, string expression, bool twoWay = false)
    {
        m_Widget = widget;
        m_PropertyName = propertyName;
        m_Expression = expression;
        m_BindingExpression = expression;
        m_TwoWay = twoWay;
    }
    
    override void Update(DZlrComponentBase component)
    {
        EditBoxWidget editBox = EditBoxWidget.Cast(m_Widget);
        if (!editBox) return;
        
        string propertyName = m_BindingExpression;
        propertyName.Replace("{{", "");
        propertyName.Replace("}}", "");
        propertyName = propertyName.Trim();
        
        string value = GetPropertyValue(component, propertyName);
        editBox.SetText(value);
    }
}

// ============================================================================
// CHECKBOX PROPERTY BINDING
// ============================================================================

class CheckBoxPropertyBinding extends PropertyBinding
{
    protected string m_BindingExpression;
    
    void CheckBoxPropertyBinding(Widget widget, string propertyName, string expression)
    {
        m_Widget = widget;
        m_PropertyName = propertyName;
        m_Expression = expression;
        m_BindingExpression = expression;
    }
    
    override void Update(DZlrComponentBase component)
    {
        CheckBoxWidget checkbox = CheckBoxWidget.Cast(m_Widget);
        if (!checkbox) return;
        
        string propertyName = m_BindingExpression;
        propertyName.Replace("{{", "");
        propertyName.Replace("}}", "");
        propertyName = propertyName.Trim();
        
        bool checked;
        if (EnScript.GetClassVar(component, propertyName, 0, checked))
        {
            checkbox.SetChecked(checked);
        }
    }
}

// ============================================================================
// SLIDER PROPERTY BINDING
// ============================================================================

class SliderPropertyBinding extends PropertyBinding
{
    protected string m_BindingExpression;
    
    void SliderPropertyBinding(Widget widget, string propertyName, string expression)
    {
        m_Widget = widget;
        m_PropertyName = propertyName;
        m_Expression = expression;
        m_BindingExpression = expression;
    }
    
    override void Update(DZlrComponentBase component)
    {
        SliderWidget slider = SliderWidget.Cast(m_Widget);
        if (!slider) return;
        
        string propertyName = m_BindingExpression;
        propertyName.Replace("{{", "");
        propertyName.Replace("}}", "");
        propertyName = propertyName.Trim();
        
        float value;
        if (EnScript.GetClassVar(component, propertyName, 0, value))
        {
            slider.SetCurrent(value);
        }
    }
}

// ============================================================================
// IMAGE PROPERTY BINDING
// ============================================================================

class ImagePropertyBinding extends PropertyBinding
{
    protected string m_BindingExpression;
    
    void ImagePropertyBinding(Widget widget, string propertyName, string expression)
    {
        m_Widget = widget;
        m_PropertyName = propertyName;
        m_Expression = expression;
        m_BindingExpression = expression;
    }
    
    override void Update(DZlrComponentBase component)
    {
        ImageWidget imageWidget = ImageWidget.Cast(m_Widget);
        if (imageWidget)
        {
            string imagePath = EvaluateExpression(component, m_BindingExpression);
            if (imagePath != "")
            {
                imageWidget.LoadImageFile(0, imagePath);
            }
        }
    }
}

// ============================================================================
// BUTTON TEXT PROPERTY BINDING
// ============================================================================

class ButtonTextPropertyBinding extends PropertyBinding
{
    protected string m_BindingExpression;
    
    void ButtonTextPropertyBinding(Widget widget, string propertyName, string expression)
    {
        m_Widget = widget;
        m_PropertyName = propertyName;
        m_Expression = expression;
        m_BindingExpression = expression;
    }
    
    override void Update(DZlrComponentBase component)
    {
        string value = EvaluateExpression(component, m_BindingExpression);
        
        DZLog().LogDebug("[ButtonTextPropertyBinding] Update - Expression: '" + m_BindingExpression + "' -> Value: '" + value + "'");
        
        ButtonWidget buttonWidget = ButtonWidget.Cast(m_Widget);
        if (buttonWidget)
        {
            buttonWidget.SetText(value);
            DZLog().LogDebug("[ButtonTextPropertyBinding] SetText called on button");
        }
        else
        {
            DZLog().LogWarning("[ButtonTextPropertyBinding] Widget is not a ButtonWidget!");
        }
    }
}
