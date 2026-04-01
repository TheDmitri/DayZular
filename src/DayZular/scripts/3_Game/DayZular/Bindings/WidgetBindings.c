// DayZular Framework - Unified Widget Bindings
// Single binding class per widget type with BOTH data and events
// Uses ternary helpers for cleaner code

// Base class for all widget bindings
class WidgetBindingBase : Managed
{
    protected DZlrComponentBase m_Component;
    protected ref ScriptedWidgetEventHandler m_Handler;
    protected ref PropertyBinding m_DataBinding;
    
    void SetComponent(DZlrComponentBase component)
    {
        m_Component = component;
    }
    
    DZlrComponentBase GetComponent()
    {
        return m_Component;
    }
    
    ScriptedWidgetEventHandler GetHandler()
    {
        return m_Handler;
    }
    
    PropertyBinding GetDataBinding()
    {
        return m_DataBinding;
    }
    
    // Override this in child classes to create data binding
    PropertyBinding CreateDataBinding(Widget widget)
    {
        return null;
    }
    
    // Helper to invoke component methods
    protected bool InvokeMethod(string methodName)
    {
        if (!m_Component || methodName == "")
        {
            return false;
        }
        
        bool result = false;
        if (g_Script.CallFunction(m_Component, methodName, result, null))
        {
            return result;
        }
        
        return false;
    }
}

// ============================================================================
// TEXT BINDING
// ============================================================================

class TextBinding : WidgetBindingBase
{
    // Visible in Workbench
    reference string Binding_Expression;
    
    override PropertyBinding CreateDataBinding(Widget widget)
    {
        if (Binding_Expression && Binding_Expression != "")
        {
            m_DataBinding = new TextPropertyBinding(widget, "text", Binding_Expression);
            return m_DataBinding;
        }
        return null;
    }
}

// ============================================================================
// IMAGE BINDING
// ============================================================================

class ImageBinding : WidgetBindingBase
{
    // Visible in Workbench
    reference string Binding_Expression;
    
    override PropertyBinding CreateDataBinding(Widget widget)
    {
        if (Binding_Expression && Binding_Expression != "")
        {
            m_DataBinding = new ImagePropertyBinding(widget, "image", Binding_Expression);
            return m_DataBinding;
        }
        return null;
    }
}

// ============================================================================
// PROGRESS BAR BINDING
// ============================================================================

class ProgressBinding : WidgetBindingBase
{
    // Visible in Workbench
    reference string Binding_Expression;
    
    override PropertyBinding CreateDataBinding(Widget widget)
    {
        if (Binding_Expression && Binding_Expression != "")
        {
            m_DataBinding = new ProgressBarPropertyBinding(widget, "progress", Binding_Expression);
            return m_DataBinding;
        }
        return null;
    }
}

// ============================================================================
// BUTTON BINDING
// ============================================================================

class ButtonBinding : WidgetBindingBase
{
    // Visible in Workbench
    reference string Binding_Text;
    reference string OnClick;
    reference string OnDoubleClick;
    reference string OnMouseEnter;
    reference string OnMouseLeave;
    
    void ButtonBinding()
    {
        m_Handler = new ButtonBindingHandler(this);
    }
    
    override PropertyBinding CreateDataBinding(Widget widget)
    {
        if (Binding_Text && Binding_Text != "")
        {
            m_DataBinding = new ButtonTextPropertyBinding(widget, "text", Binding_Text);
            return m_DataBinding;
        }
        return null;
    }
    
    bool HandleOnClick(Widget w, int x, int y, int button)
    {
        if (OnClick && OnClick != "")
        {
            return InvokeMethod(OnClick);
        }
        return TernaryBool(m_Component != null, m_Component.OnClick(w, x, y, button), false);
    }
    
    bool HandleOnDoubleClick(Widget w, int x, int y, int button)
    {
        if (OnDoubleClick && OnDoubleClick != "")
        {
            return InvokeMethod(OnDoubleClick);
        }
        return TernaryBool(m_Component != null, m_Component.OnDoubleClick(w, x, y, button), false);
    }
    
    bool HandleOnMouseEnter(Widget w, int x, int y)
    {
        if (OnMouseEnter && OnMouseEnter != "")
        {
            return InvokeMethod(OnMouseEnter);
        }
        return TernaryBool(m_Component != null, m_Component.OnMouseEnter(w, x, y), false);
    }
    
    bool HandleOnMouseLeave(Widget w, Widget enterW, int x, int y)
    {
        if (OnMouseLeave && OnMouseLeave != "")
        {
            return InvokeMethod(OnMouseLeave);
        }
        return TernaryBool(m_Component != null, m_Component.OnMouseLeave(w, enterW, x, y), false);
    }
}

class ButtonBindingHandler : ScriptedWidgetEventHandler
{
    protected ButtonBinding m_Binding;
    
    void ButtonBindingHandler(ButtonBinding binding)
    {
        m_Binding = binding;
    }
    
    override bool OnClick(Widget w, int x, int y, int button)
    {
        return TernaryBool(m_Binding != null, m_Binding.HandleOnClick(w, x, y, button), false);
    }
    
    override bool OnDoubleClick(Widget w, int x, int y, int button)
    {
        return TernaryBool(m_Binding != null, m_Binding.HandleOnDoubleClick(w, x, y, button), false);
    }
    
    override bool OnMouseEnter(Widget w, int x, int y)
    {
        return TernaryBool(m_Binding != null, m_Binding.HandleOnMouseEnter(w, x, y), false);
    }
    
    override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
    {
        return TernaryBool(m_Binding != null, m_Binding.HandleOnMouseLeave(w, enterW, x, y), false);
    }
}

// ============================================================================
// COMBOBOX BINDING (Data + Events)
// ============================================================================

class ComboBoxBinding : WidgetBindingBase
{
    // Data Binding
    reference string Binding_Items;
    reference string Binding_Selection;
    
    // Event Binding
    reference string OnChange;
    
    void ComboBoxBinding()
    {
        m_Handler = new ComboBoxBindingHandler(this);
    }
    
    override PropertyBinding CreateDataBinding(Widget widget)
    {
        if (Binding_Items && Binding_Items != "")
        {
            m_DataBinding = new ComboBoxPropertyBinding(widget, "items", Binding_Items, CoalesceString(Binding_Selection, ""));
            return m_DataBinding;
        }
        return null;
    }
    
    bool HandleOnChange(Widget w, int x, int y, bool finished)
    {
        if (OnChange && OnChange != "")
        {
            return InvokeMethod(OnChange);
        }
        return TernaryBool(m_Component != null, m_Component.OnChange(w, x, y, finished), false);
    }
}

class ComboBoxBindingHandler : ScriptedWidgetEventHandler
{
    protected ComboBoxBinding m_Binding;
    
    void ComboBoxBindingHandler(ComboBoxBinding binding)
    {
        m_Binding = binding;
    }
    
    override bool OnChange(Widget w, int x, int y, bool finished)
    {
        return TernaryBool(m_Binding != null, m_Binding.HandleOnChange(w, x, y, finished), false);
    }
}

// ============================================================================
// TEXTLISTBOX BINDING (Data + Events)
// ============================================================================

class TextListboxBinding : WidgetBindingBase
{
    // Data Binding
    reference string Binding_Items;
    reference string Binding_Selection;
    
    // Event Binding
    reference string OnItemSelected;
    reference string OnDoubleClick;
    
    void TextListboxBinding()
    {
        m_Handler = new TextListboxBindingHandler(this);
    }
    
    override PropertyBinding CreateDataBinding(Widget widget)
    {
        if (Binding_Items && Binding_Items != "")
        {
            m_DataBinding = new TextListboxPropertyBinding(widget, "items", Binding_Items, CoalesceString(Binding_Selection, ""));
            return m_DataBinding;
        }
        return null;
    }
    
    bool HandleOnClick(Widget w, int x, int y, int button)
    {
        if (OnItemSelected && OnItemSelected != "")
        {
            return InvokeMethod(OnItemSelected);
        }
        return TernaryBool(m_Component != null, m_Component.OnClick(w, x, y, button), false);
    }
    
    bool HandleOnDoubleClick(Widget w, int x, int y, int button)
    {
        if (OnDoubleClick && OnDoubleClick != "")
        {
            return InvokeMethod(OnDoubleClick);
        }
        return TernaryBool(m_Component != null, m_Component.OnDoubleClick(w, x, y, button), false);
    }
}

class TextListboxBindingHandler : ScriptedWidgetEventHandler
{
    protected TextListboxBinding m_Binding;
    
    void TextListboxBindingHandler(TextListboxBinding binding)
    {
        m_Binding = binding;
    }
    
    override bool OnClick(Widget w, int x, int y, int button)
    {
        return TernaryBool(m_Binding != null, m_Binding.HandleOnClick(w, x, y, button), false);
    }
    
    override bool OnDoubleClick(Widget w, int x, int y, int button)
    {
        return TernaryBool(m_Binding != null, m_Binding.HandleOnDoubleClick(w, x, y, button), false);
    }
}

// ============================================================================
// ITEMPREVIEW BINDING
// ============================================================================

class ItemPreviewBinding : WidgetBindingBase
{
    // Visible in Workbench
    reference string Binding_Item;
    reference int Enable_Rotation;
    reference int Enable_Scale;
    
    // Internal state for rotation and scale
    protected int m_RotX;
    protected int m_RotY;
    protected vector m_Orientation;
    protected float m_Scale;
    
    override PropertyBinding CreateDataBinding(Widget widget)
    {
        if (Binding_Item && Binding_Item != "")
        {
            m_DataBinding = new ItemPreviewPropertyBinding(widget, "item", Binding_Item);
            
            // Initialize rotation and scale if enabled
            if (Enable_Rotation == 1 || Enable_Scale == 1)
            {
                m_Orientation = vector.Zero;
                m_Scale = 0;
                m_Handler = new ItemPreviewEventHandler(this, widget);
            }
            
            return m_DataBinding;
        }
        return null;
    }
    
    bool IsRotationEnabled()
    {
        return Enable_Rotation == 1;
    }
    
    bool IsScaleEnabled()
    {
        return Enable_Scale == 1;
    }
    
    void UpdateRotation(int mouse_x, int mouse_y, bool is_dragging)
    {
        if (!IsRotationEnabled())
            return;
        
        ItemPreviewWidget preview = ItemPreviewWidget.Cast(m_DataBinding.GetWidget());
        if (!preview)
            return;
            
        vector orientation = m_Orientation;
        orientation[0] = orientation[0] + (m_RotY - mouse_y);
        orientation[1] = orientation[1] - (m_RotX - mouse_x);
        
        preview.SetModelOrientation(orientation);
        
        if (!is_dragging)
            m_Orientation = orientation;
    }
    
    void UpdateScale(int wheel, ItemPreviewWidget preview)
    {
        if (!IsScaleEnabled() || !preview)
            return;
            
        float w, h, x, y;
        preview.GetPos(x, y);
        preview.GetSize(w, h);
        
        float oldW = w;
        float oldH = h;
        
        m_Scale = wheel;
        w = w + (m_Scale / 4);
        h = h + (m_Scale / 4);
        
        if (w > 0.5 && w < 3)
        {
            // Calculate size difference to keep centered
            float deltaW = w - oldW;
            float deltaH = h - oldH;
            
            preview.SetSize(w, h);
            
            // Adjust position to keep centered
            float new_x = x - (deltaW / 2);
            float new_y = y - (deltaH / 2);
            preview.SetPos(new_x, new_y);
        }
    }
    
    void SetRotationStart(int x, int y)
    {
        m_RotX = x;
        m_RotY = y;
    }
}

// Event handler for ItemPreview rotation and scale
class ItemPreviewEventHandler : ScriptedWidgetEventHandler
{
    protected ItemPreviewBinding m_Binding;
    protected ItemPreviewWidget m_Preview;
    
    void ItemPreviewEventHandler(ItemPreviewBinding binding, Widget widget)
    {
        m_Binding = binding;
        m_Preview = ItemPreviewWidget.Cast(widget);
        
        if (m_Preview)
        {
            m_Preview.SetHandler(this);
        }
    }
    
    override bool OnMouseButtonDown(Widget w, int x, int y, int button)
    {
        if (!m_Binding || !m_Binding.IsRotationEnabled() || w != m_Preview)
            return false;
            
        GetGame().GetDragQueue().Call(m_Binding, "UpdateRotation");
        
        int mouse_x, mouse_y;
        GetMousePos(mouse_x, mouse_y);
        m_Binding.SetRotationStart(mouse_x, mouse_y);
        
        return true;
    }
    
    override bool OnMouseWheel(Widget w, int x, int y, int wheel)
    {
        if (!m_Binding || !m_Binding.IsScaleEnabled() || w != m_Preview)
            return false;
            
        m_Binding.UpdateScale(wheel, m_Preview);
        return true;
    }
}

// ============================================================================
// PLAYERPREVIEW BINDING
// ============================================================================

class PlayerPreviewBinding : WidgetBindingBase
{
    // Visible in Workbench
    reference string Binding_Player;
    
    override PropertyBinding CreateDataBinding(Widget widget)
    {
        if (Binding_Player && Binding_Player != "")
        {
            m_DataBinding = new PlayerPreviewPropertyBinding(widget, "player", Binding_Player);
            return m_DataBinding;
        }
        return null;
    }
}

// ============================================================================
// EDITBOX BINDING (Data + Events)
// ============================================================================

class EditBoxBinding : WidgetBindingBase
{
    // Data Binding
    reference string Binding_Expression;
    reference int Two_Way_Binding;
    
    // Form Control Integration
    reference string Form_Control_Name;  // Name of form control to bind to
    reference string Form_Validators;    // Comma-separated validators: "required,min:1,max:999"
    
    // Event Binding
    reference string OnChange;
    reference string OnFocus;
    reference string OnFocusLost;
    
    // Form control reference
    protected ref FormControl m_FormControl;
    protected Widget m_Widget;
    
    void EditBoxBinding()
    {
        m_Handler = new EditBoxBindingHandler(this);
    }
    
    override PropertyBinding CreateDataBinding(Widget widget)
    {
        m_Widget = widget;
        
        // Option 3: Layout-driven form creation
        // If Validators are specified but no Form_Control_Name, auto-create both FormGroup and FormControl
        if (Form_Validators && Form_Validators != "" && m_Component)
        {
            AutoCreateFormControl();
        }
        // Auto-discover and bind FormControl if Form_Control_Name is specified
        else if (Form_Control_Name && Form_Control_Name != "" && m_Component)
        {
            AutoBindFormControl();
            
            // If form not found yet, try again after component initialization
            if (!m_FormControl)
            {
                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RetryAutoBindFormControl, 100, false);
            }
        }
        
        if (Binding_Expression && Binding_Expression != "")
        {
            m_DataBinding = new EditBoxPropertyBinding(widget, "text", Binding_Expression, Two_Way_Binding == 1);
            return m_DataBinding;
        }
        return null;
    }
    
    // Option 3: Auto-create FormGroup and FormControl from layout
    protected void AutoCreateFormControl()
    {
        if (!m_Component || !Form_Validators || Form_Validators == "")
            return;
        
         DZLog().LogDebug("EditBoxBinding: Auto-creating FormControl with validators: " + Form_Validators);
        
        // Get or auto-create FormGroup
        FormGroup formGroup = GetOrCreateFormGroup();
        if (!formGroup)
        {
            DZLog().LogWarning("EditBoxBinding: Could not get or create FormGroup");
            return;
        }
        
        // Generate control name from binding expression if not specified
        string controlName = Form_Control_Name;
        if (controlName == "" && Binding_Expression)
        {
            // Extract property name from {{ property_name }}
            controlName = Binding_Expression;
            controlName.Replace("{{", "");
            controlName.Replace("}}", "");
            controlName = controlName.Trim();
        }
        
        if (controlName == "")
        {
            DZLog().LogWarning("EditBoxBinding: Cannot auto-create FormControl without name");
            return;
        }
        
        // Check if control already exists
        FormControl existingControl = formGroup.GetControl(controlName);
        if (existingControl)
        {
            SetFormControl(existingControl);
            return;
        }
        
        // Get initial value from component property
        string initialValue = "";
        if (Binding_Expression && m_Component)
        {
            EnScript.GetClassVar(m_Component, controlName, 0, initialValue);
        }
        
        // Create FormControl with validators from string
        FormControl control = FormBuilder.FromString(controlName, initialValue, Form_Validators);
        formGroup.AddControl(controlName, control);
        
        SetFormControl(control);
        AutoBindErrorLabel();
        
         DZLog().LogDebug("EditBoxBinding: Auto-created FormControl: " + controlName);
    }
    
    // Get existing FormGroup or auto-create one
    protected FormGroup GetOrCreateFormGroup()
    {
        // Try to find existing FormGroup
        FormGroup formGroup;
        EnScript.GetClassVar(m_Component, "m_ListingForm", 0, formGroup);
        if (formGroup) return formGroup;
        
        EnScript.GetClassVar(m_Component, "m_ListItemForm", 0, formGroup);
        if (formGroup) return formGroup;
        
        EnScript.GetClassVar(m_Component, "m_Form", 0, formGroup);
        if (formGroup) return formGroup;
        
        EnScript.GetClassVar(m_Component, "m_FormGroup", 0, formGroup);
        if (formGroup) return formGroup;
        
        // Auto-create FormGroup on component
        formGroup = new FormGroup("autoForm");
        
        // Try to set on most common property names
        if (EnScript.SetClassVar(m_Component, "m_ListingForm", 0, formGroup))
        {
             DZLog().LogDebug("EditBoxBinding: Auto-created m_ListingForm");
            return formGroup;
        }
        if (EnScript.SetClassVar(m_Component, "m_Form", 0, formGroup))
        {
             DZLog().LogDebug("EditBoxBinding: Auto-created m_Form");
            return formGroup;
        }
        if (EnScript.SetClassVar(m_Component, "m_FormGroup", 0, formGroup))
        {
             DZLog().LogDebug("EditBoxBinding: Auto-created m_FormGroup");
            return formGroup;
        }
        
        // Store reference in binding as fallback
        return formGroup;
    }
    
    // Retry auto-binding after component initialization
    protected void RetryAutoBindFormControl()
    {
        if (!m_FormControl && Form_Control_Name && Form_Control_Name != "" && m_Component)
        {
            AutoBindFormControl();
        }
    }
    
    // Auto-discover FormControl from component's FormGroup
    protected void AutoBindFormControl()
    {
        if (!m_Component || !Form_Control_Name || Form_Control_Name == "")
            return;
            
         DZLog().LogDebug("EditBoxBinding: Auto-binding FormControl: " + Form_Control_Name);
        
        // Try to find FormGroup in component
        FormGroup formGroup;
        EnScript.GetClassVar(m_Component, "m_ListingForm", 0, formGroup);
        
        if (!formGroup)
        {
            EnScript.GetClassVar(m_Component, "m_ListItemForm", 0, formGroup);
        }
        
        if (!formGroup)
        {
            // Try other common form group names
            EnScript.GetClassVar(m_Component, "m_Form", 0, formGroup);
        }
        
        if (!formGroup)
        {
            EnScript.GetClassVar(m_Component, "m_FormGroup", 0, formGroup);
        }
        
        if (formGroup)
        {
            FormControl control = formGroup.GetControl(Form_Control_Name);
            if (control)
            {
                SetFormControl(control);
                
                // Also bind error label if it exists
                AutoBindErrorLabel();
                
                 DZLog().LogDebug("EditBoxBinding: Bound FormControl: " + Form_Control_Name);
            }
            else
            {
                DZLog().LogWarning("EditBoxBinding: FormControl not found: " + Form_Control_Name);
            }
        }
        else
        {
            DZLog().LogWarning("EditBoxBinding: FormGroup not found in component");
        }
    }
    
    // Auto-discover and bind error label
    protected void AutoBindErrorLabel()
    {
        if (!m_FormControl || !m_Component)
            return;
            
        // Try to find error label widget with naming convention: {controlName}_error
        string errorLabelName = Form_Control_Name + "_error";
        Widget root = m_Component.GetRoot();
        
        if (root)
        {
            Widget errorWidget = root.FindAnyWidget(errorLabelName);
            if (errorWidget)
            {
                TextWidget errorLabel = TextWidget.Cast(errorWidget);
                if (errorLabel)
                {
                    m_FormControl.BindErrorLabel(errorLabel);
                     DZLog().LogDebug("EditBoxBinding: Bound error label: " + errorLabelName);
                }
            }
        }
    }
    
    // Set form control for validation
    void SetFormControl(FormControl control)
    {
        m_FormControl = control;
        
        if (m_FormControl && m_Widget)
        {
            // Bind widget for visual feedback
            m_FormControl.BindToWidget(m_Widget);
            
            // Parse and add validators if specified
            if (Form_Validators && Form_Validators != "")
            {
                ParseAndAddValidators();
            }
        }
    }
    
    // Parse validator string and add validators
    protected void ParseAndAddValidators()
    {
        if (!m_FormControl || !Form_Validators)
            return;
            
        array<string> validatorList = new array<string>();
        Form_Validators.Split(",", validatorList);
        
        foreach (string validatorDef : validatorList)
        {
            validatorDef.Trim();
            
            // Check for validators with parameters (e.g., "min:1")
            int colonIndex = validatorDef.IndexOf(":");
            string validatorName = validatorDef;
            string validatorParam = "";
            
            if (colonIndex != -1)
            {
                validatorName = validatorDef.Substring(0, colonIndex);
                validatorParam = validatorDef.Substring(colonIndex + 1, validatorDef.Length() - colonIndex - 1);
                validatorName.Trim();
                validatorParam.Trim();
            }
            
            // Add validator based on name
            if (validatorName == "required")
            {
                m_FormControl.AddValidator(new RequiredValidator());
            }
            else if (validatorName == "min")
            {
                m_FormControl.AddValidator(new MinValidator(validatorParam.ToFloat()));
            }
            else if (validatorName == "max")
            {
                m_FormControl.AddValidator(new MaxValidator(validatorParam.ToFloat()));
            }
            else if (validatorName == "minlength")
            {
                m_FormControl.AddValidator(new MinLengthValidator(validatorParam.ToInt()));
            }
            else if (validatorName == "maxlength")
            {
                m_FormControl.AddValidator(new MaxLengthValidator(validatorParam.ToInt()));
            }
            else if (validatorName == "numeric" || validatorName == "integer" || validatorName == "alpha" || validatorName == "alphanumeric")
            {
                m_FormControl.AddValidator(new PatternValidator(validatorName, "Invalid format"));
            }
            else if (validatorName == "email")
            {
                m_FormControl.AddValidator(new EmailValidator());
            }
        }
    }
    
    bool HandleOnChange(Widget w, int x, int y, bool finished)
    {
        EditBoxWidget editBox = EditBoxWidget.Cast(w);
        string text = "";
        if (editBox)
        {
            text = editBox.GetText();
        }
        
        // Update form control value if bound
        if (m_FormControl)
        {
            m_FormControl.SetValue(text, false);
        }
        
        // Two-way binding: write back to component property
        if (Two_Way_Binding == 1 && Binding_Expression && m_Component)
        {
            string propertyName = Binding_Expression;
            propertyName.Replace("{{", "");
            propertyName.Replace("}}", "");
            propertyName = propertyName.Trim();
            
            // Try to set the property value on the component
            EnScript.SetClassVar(m_Component, propertyName, 0, text);
        }
        
        // Call component method with text value if OnChange is specified
        if (OnChange && OnChange != "" && m_Component)
        {
            bool result;
            if (g_Script.CallFunction(m_Component, OnChange, result, text))
            {
                return result;
            }
        }
        
        // Fall back to component's OnChange override
        if (m_Component)
        {
            return m_Component.OnChange(w, x, y, finished);
        }
        return false;
    }
    
    bool HandleOnFocus(Widget w, int x, int y)
    {
        if (OnFocus && OnFocus != "")
        {
            return InvokeMethod(OnFocus);
        }
        return TernaryBool(m_Component != null, m_Component.OnFocus(w, x, y), false);
    }
    
    bool HandleOnFocusLost(Widget w, int x, int y)
    {
        // Mark as touched when focus is lost
        if (m_FormControl)
        {
            m_FormControl.MarkAsTouched();
        }
        
        if (OnFocusLost && OnFocusLost != "")
        {
            return InvokeMethod(OnFocusLost);
        }
        return TernaryBool(m_Component != null, m_Component.OnFocusLost(w, x, y), false);
    }
    
    FormControl GetFormControl()
    {
        return m_FormControl;
    }
}

class EditBoxBindingHandler : ScriptedWidgetEventHandler
{
    protected EditBoxBinding m_Binding;
    
    void EditBoxBindingHandler(EditBoxBinding binding)
    {
        m_Binding = binding;
    }
    
    override bool OnChange(Widget w, int x, int y, bool finished)
    {
        return TernaryBool(m_Binding != null, m_Binding.HandleOnChange(w, x, y, finished), false);
    }
    
    override bool OnFocus(Widget w, int x, int y)
    {
        return TernaryBool(m_Binding != null, m_Binding.HandleOnFocus(w, x, y), false);
    }
    
    override bool OnFocusLost(Widget w, int x, int y)
    {
        return TernaryBool(m_Binding != null, m_Binding.HandleOnFocusLost(w, x, y), false);
    }
}

// ============================================================================
// CHECKBOX BINDING (Data + Events)
// ============================================================================

class CheckBoxBinding : WidgetBindingBase
{
    // Data Binding
    reference string Binding_Checked;
    
    // Event Binding
    reference string OnToggle;
    
    void CheckBoxBinding()
    {
        m_Handler = new CheckBoxBindingHandler(this);
    }
    
    override PropertyBinding CreateDataBinding(Widget widget)
    {
        if (Binding_Checked && Binding_Checked != "")
        {
            m_DataBinding = new CheckBoxPropertyBinding(widget, "checked", Binding_Checked);
            return m_DataBinding;
        }
        return null;
    }
    
    bool HandleOnClick(Widget w, int x, int y, int button)
    {
        if (OnToggle && OnToggle != "")
        {
            return InvokeMethod(OnToggle);
        }
        return TernaryBool(m_Component != null, m_Component.OnClick(w, x, y, button), false);
    }
}

class CheckBoxBindingHandler : ScriptedWidgetEventHandler
{
    protected CheckBoxBinding m_Binding;
    
    void CheckBoxBindingHandler(CheckBoxBinding binding)
    {
        m_Binding = binding;
    }
    
    override bool OnClick(Widget w, int x, int y, int button)
    {
        return TernaryBool(m_Binding != null, m_Binding.HandleOnClick(w, x, y, button), false);
    }
}

// ============================================================================
// SLIDER BINDING (Data + Events)
// ============================================================================

class SliderBinding : WidgetBindingBase
{
    // Data Binding
    reference string Binding_Value;
    
    // Event Binding
    reference string OnValueChanged;
    
    void SliderBinding()
    {
        m_Handler = new SliderBindingHandler(this);
    }
    
    override PropertyBinding CreateDataBinding(Widget widget)
    {
        if (Binding_Value && Binding_Value != "")
        {
            m_DataBinding = new SliderPropertyBinding(widget, "value", Binding_Value);
            return m_DataBinding;
        }
        return null;
    }
    
    bool HandleOnChange(Widget w, int x, int y, bool finished)
    {
        if (OnValueChanged && OnValueChanged != "")
        {
            return InvokeMethod(OnValueChanged);
        }
        return TernaryBool(m_Component != null, m_Component.OnChange(w, x, y, finished), false);
    }
}

class SliderBindingHandler : ScriptedWidgetEventHandler
{
    protected SliderBinding m_Binding;
    
    void SliderBindingHandler(SliderBinding binding)
    {
        m_Binding = binding;
    }
    
    override bool OnChange(Widget w, int x, int y, bool finished)
    {
        return TernaryBool(m_Binding != null, m_Binding.HandleOnChange(w, x, y, finished), false);
    }
}

/*
Usage - Single binding class per widget!

// Text (data only)
<TextWidgetClass name {
  scriptclass "TextBinding"
  {
    Binding_Expression "{{ _inputName }}"
  }
}

// Button (events only)
<ButtonWidgetClass submit {
  scriptclass "ButtonBinding"
  {
    OnClick "OnSubmitClick"
    OnMouseEnter "OnButtonHover"
  }
}

// ComboBox (data + events)
<XComboBoxWidgetClass category {
  scriptclass "ComboBoxBinding"
  {
    Binding_Items "{{ _inputCategories }}"
    Binding_Selection "{{ _inputSelectedIndex }}"
    OnChange "OnCategoryChanged"
  }
}

// EditBox (data + events)
<EditBoxWidgetClass search {
  scriptclass "EditBoxBinding"
  {
    Binding_Expression "{{ _inputSearchText }}"
    Two_Way_Binding 1
    OnChange "OnSearchChanged"
    OnFocus "OnSearchFocused"
  }
}

ONE CLASS PER WIDGET TYPE - SIMPLE!
*/
