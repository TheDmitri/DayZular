// DayZular Framework - Form Control
// Represents a single form field with validation

class FormControl
{
    protected string m_Name;
    protected string m_Value;
    protected ref array<ref FormValidator> m_Validators;
    protected ref map<string, string> m_Errors;
    
    // State tracking (Angular-like)
    protected bool m_Pristine;
    protected bool m_Dirty;
    protected bool m_Touched;
    protected bool m_Valid;
    
    // Two-way binding reference
    protected Widget m_BoundWidget;
    protected DZlrComponentBase m_Component;
    
    // Validation UI elements
    protected TextWidget m_ErrorLabel;
    protected int m_DefaultColor;
    protected int m_ErrorColor;
    protected int m_ValidColor;
    
    void FormControl(string name, string initialValue)
    {
        m_Name = name;
        m_Value = initialValue;
        m_Pristine = true;
        m_Dirty = false;
        m_Touched = false;
        m_Valid = true;
        m_DefaultColor = ARGB(255, 255, 255, 255);
        m_ErrorColor = ARGB(255, 220, 80, 80);
        m_ValidColor = ARGB(255, 80, 220, 120);
        m_Validators = new array<ref FormValidator>();
        m_Errors = new map<string, string>();
    }
    
    // Add validator
    void AddValidator(FormValidator validator)
    {
        if (validator)
            m_Validators.Insert(validator);
    }
    
    // Clear all validators
    void ClearValidators()
    {
        m_Validators.Clear();
    }
    
    // Set value and validate
    void SetValue(string value, bool markAsTouched)
    {
        if (value != m_Value)
        {
            m_Value = value;
            m_Dirty = true;
            m_Pristine = false;
        }
        
        if (markAsTouched)
            m_Touched = true;
            
        Validate();
    }
    
    // Mark as touched (user interacted with field)
    void MarkAsTouched()
    {
        m_Touched = true;
        UpdateValidationUI();
    }
    
    // Mark as dirty (value changed)
    void MarkAsDirty()
    {
        m_Dirty = true;
        m_Pristine = false;
    }
    
    // Run all validators
    void Validate()
    {
        m_Errors.Clear();
        m_Valid = true;
        
        foreach (FormValidator validator : m_Validators)
        {
            ValidationError error = validator.Validate(m_Value);
            if (error)
            {
                m_Errors.Insert(error.key, error.message);
                m_Valid = false;
            }
        }
        
        // Update UI
        UpdateValidationUI();
    }
    
    // Bind to widget for visual feedback
    void BindToWidget(Widget widget)
    {
        m_BoundWidget = widget;
        UpdateValidationUI();
    }
    
    // Bind error label for displaying messages
    void BindErrorLabel(TextWidget errorLabel)
    {
        m_ErrorLabel = errorLabel;
        UpdateValidationUI();
    }
    
    // Visual feedback
    void UpdateValidationUI()
    {
        // Update widget color
        if (m_BoundWidget)
        {
            if (!m_Valid && m_Touched)
            {
                m_BoundWidget.SetColor(m_ErrorColor); // Red tint
            }
            else if (m_Valid && m_Dirty)
            {
                m_BoundWidget.SetColor(m_ValidColor); // Green tint
            }
            else
            {
                m_BoundWidget.SetColor(m_DefaultColor); // Normal
            }
        }
        
        // Update error label
        if (m_ErrorLabel)
        {
            if (!m_Valid && m_Touched && m_Errors.Count() > 0)
            {
                // Show first error message
                string firstError = m_Errors.GetElement(0);
                m_ErrorLabel.SetText(firstError);
                m_ErrorLabel.Show(true);
            }
            else
            {
                m_ErrorLabel.SetText("");
                m_ErrorLabel.Show(false);
            }
        }
    }
    
    // Reset to initial state
    void Reset(string value)
    {
        m_Value = value;
        m_Pristine = true;
        m_Dirty = false;
        m_Touched = false;
        m_Valid = true;
        m_Errors.Clear();
        UpdateValidationUI();
    }
    
    // Getters
    bool IsValid() { return m_Valid; }
    bool IsDirty() { return m_Dirty; }
    bool IsTouched() { return m_Touched; }
    bool IsPristine() { return m_Pristine; }
    string GetValue() { return m_Value; }
    string GetName() { return m_Name; }
    map<string, string> GetErrors() { return m_Errors; }
    
    // Get first error message
    string GetFirstError()
    {
        if (m_Errors.Count() > 0)
            return m_Errors.GetElement(0);
        return "";
    }
    
    // Alias for GetFirstError - compatibility with existing code
    string GetErrorMessage()
    {
        return GetFirstError();
    }
    
    // Check if has specific error
    bool HasError(string errorKey)
    {
        return m_Errors.Contains(errorKey);
    }
}
