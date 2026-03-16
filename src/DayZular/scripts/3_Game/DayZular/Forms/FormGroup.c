// DayZular Framework - Form Group
// Collection of form controls with group-level validation

class FormGroup
{
    protected ref map<string, ref FormControl> m_Controls;
    protected ref array<ref FormGroupValidator> m_GroupValidators;
    protected bool m_Valid;
    protected string m_Name;
    
    void FormGroup(string name)
    {
        m_Name = name;
        m_Valid = true;
        m_Controls = new map<string, ref FormControl>();
        m_GroupValidators = new array<ref FormGroupValidator>();
    }
    
    // Add control
    void AddControl(string name, FormControl control)
    {
        if (control)
            m_Controls.Insert(name, control);
    }
    
    // Remove control
    void RemoveControl(string name)
    {
        m_Controls.Remove(name);
    }
    
    // Get control
    FormControl GetControl(string name)
    {
        return m_Controls.Get(name);
    }
    
    // Add group-level validator
    void AddGroupValidator(FormGroupValidator validator)
    {
        if (validator)
            m_GroupValidators.Insert(validator);
    }
    
    // Validate entire form
    bool Validate()
    {
        m_Valid = true;
        
        Print("FormGroup: Validating form: " + m_Name);
        
        // Validate each control
        foreach (string name, FormControl control : m_Controls)
        {
            control.Validate();
            if (!control.IsValid())
            {
                m_Valid = false;
                Print("FormGroup: Control '" + name + "' is invalid: " + control.GetFirstError());
            }
        }
        
        // Run group-level validators
        foreach (FormGroupValidator validator : m_GroupValidators)
        {
            if (!validator.Validate(this))
            {
                m_Valid = false;
                Print("FormGroup: Group validation failed");
            }
        }
        
        Print("FormGroup: Validation result: " + m_Valid);
        return m_Valid;
    }
    
    // Mark all controls as touched (useful for showing all errors on submit)
    void MarkAllAsTouched()
    {
        foreach (string name, FormControl control : m_Controls)
        {
            control.MarkAsTouched();
        }
    }
    
    // Get all values as map
    map<string, string> GetValues()
    {
        map<string, string> values = new map<string, string>();
        foreach (string name, FormControl control : m_Controls)
        {
            values.Insert(name, control.GetValue());
        }
        return values;
    }
    
    // Get value of specific control
    string GetValue(string controlName)
    {
        FormControl control = GetControl(controlName);
        if (control)
            return control.GetValue();
        return "";
    }
    
    // Set value of specific control
    void SetValue(string controlName, string value, bool markAsTouched)
    {
        FormControl control = GetControl(controlName);
        if (control)
            control.SetValue(value, markAsTouched);
    }
    
    // Reset entire form
    void Reset()
    {
        foreach (string name, FormControl control : m_Controls)
        {
            control.Reset("");
        }
    }
    
    // Check if form is dirty (any control modified)
    bool IsDirty()
    {
        foreach (string name, FormControl control : m_Controls)
        {
            if (control.IsDirty())
                return true;
        }
        return false;
    }
    
    // Check if form is pristine (no controls modified)
    bool IsPristine()
    {
        return !IsDirty();
    }
    
    // Check if form is touched (any control touched)
    bool IsTouched()
    {
        foreach (string name, FormControl control : m_Controls)
        {
            if (control.IsTouched())
                return true;
        }
        return false;
    }
    
    // Get all errors from all controls
    map<string, string> GetAllErrors()
    {
        map<string, string> allErrors = new map<string, string>();
        foreach (string name, FormControl control : m_Controls)
        {
            if (!control.IsValid())
            {
                map<string, string> controlErrors = control.GetErrors();
                for (int i = 0; i < controlErrors.Count(); i++)
                {
                    string errorKey = controlErrors.GetKey(i);
                    string errorMsg = controlErrors.GetElement(i);
                    allErrors.Insert(name + "." + errorKey, errorMsg);
                }
            }
        }
        return allErrors;
    }
    
    // Get all errors as formatted string (convenience method for binding)
    string GetAllErrorsAsString()
    {
        map<string, string> allErrors = GetAllErrors();
        if (allErrors.Count() == 0)
            return "";
        
        string result = "";
        for (int i = 0; i < allErrors.Count(); i++)
        {
            if (i > 0)
                result = result + " | ";
            result = result + allErrors.GetElement(i);
        }
        return result;
    }
    
    // Getters
    bool IsValid() { return m_Valid; }
    string GetName() { return m_Name; }
    int GetControlCount() { return m_Controls.Count(); }
}

// Base class for group-level validators
class FormGroupValidator
{
    bool Validate(FormGroup formGroup)
    {
        return true;
    }
}
