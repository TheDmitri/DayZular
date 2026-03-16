// DayZular Framework - Form Validators
// Angular-inspired validation system

// Base validator interface
class FormValidator
{
    ValidationError Validate(string value)
    {
        return null;
    }
}

// Required validator - field must not be empty
class RequiredValidator extends FormValidator
{
    override ValidationError Validate(string value)
    {
        if (value == "" || !value)
            return new ValidationError("required", "This field is required");
        return null;
    }
}

// Min validator - numeric value must be at least min
class MinValidator extends FormValidator
{
    protected float m_Min;
    
    void MinValidator(float min)
    {
        m_Min = min;
    }
    
    override ValidationError Validate(string value)
    {
        if (value == "" || !value)
            return null; // Let RequiredValidator handle empty values
            
        float numValue = value.ToFloat();
        if (numValue < m_Min)
            return new ValidationError("min", "Value must be at least " + m_Min.ToString());
        return null;
    }
}

// Max validator - numeric value cannot exceed max
class MaxValidator extends FormValidator
{
    protected float m_Max;
    
    void MaxValidator(float max)
    {
        m_Max = max;
    }
    
    override ValidationError Validate(string value)
    {
        if (value == "" || !value)
            return null; // Let RequiredValidator handle empty values
            
        float numValue = value.ToFloat();
        if (numValue > m_Max)
            return new ValidationError("max", "Value cannot exceed " + m_Max.ToString());
        return null;
    }
}

// MinLength validator - string must have minimum length
class MinLengthValidator extends FormValidator
{
    protected int m_MinLength;
    
    void MinLengthValidator(int minLength)
    {
        m_MinLength = minLength;
    }
    
    override ValidationError Validate(string value)
    {
        if (value == "" || !value)
            return null;
            
        if (value.Length() < m_MinLength)
            return new ValidationError("minlength", "Must be at least " + m_MinLength.ToString() + " characters");
        return null;
    }
}

// MaxLength validator - string cannot exceed maximum length
class MaxLengthValidator extends FormValidator
{
    protected int m_MaxLength;
    
    void MaxLengthValidator(int maxLength)
    {
        m_MaxLength = maxLength;
    }
    
    override ValidationError Validate(string value)
    {
        if (value == "" || !value)
            return null;
            
        if (value.Length() > m_MaxLength)
            return new ValidationError("maxlength", "Cannot exceed " + m_MaxLength.ToString() + " characters");
        return null;
    }
}

// Pattern validator - validates against simple patterns
class PatternValidator extends FormValidator
{
    protected string m_Pattern;
    protected string m_ErrorMessage;
    
    void PatternValidator(string pattern, string errorMessage)
    {
        m_Pattern = pattern;
        m_ErrorMessage = errorMessage;
    }
    
    override ValidationError Validate(string value)
    {
        if (value == "" || !value)
            return null;
            
        bool valid = true;
        
        // Built-in patterns
        if (m_Pattern == "numeric")
        {
            valid = IsNumeric(value);
        }
        else if (m_Pattern == "alphanumeric")
        {
            valid = IsAlphanumeric(value);
        }
        else if (m_Pattern == "alpha")
        {
            valid = IsAlpha(value);
        }
        else if (m_Pattern == "integer")
        {
            valid = IsInteger(value);
        }
        
        if (!valid)
            return new ValidationError("pattern", m_ErrorMessage);
            
        return null;
    }
    
    protected bool IsNumeric(string value)
    {
        bool hasDecimal = false;
        for (int i = 0; i < value.Length(); i++)
        {
            string char = value.Substring(i, 1);
            
            // Allow negative sign at start
            if (i == 0 && char == "-")
                continue;
                
            // Allow one decimal point
            if (char == ".")
            {
                if (hasDecimal)
                    return false;
                hasDecimal = true;
                continue;
            }
            
            if (char < "0" || char > "9")
                return false;
        }
        return true;
    }
    
    protected bool IsInteger(string value)
    {
        for (int i = 0; i < value.Length(); i++)
        {
            string char = value.Substring(i, 1);
            
            // Allow negative sign at start
            if (i == 0 && char == "-")
                continue;
                
            if (char < "0" || char > "9")
                return false;
        }
        return true;
    }
    
    protected bool IsAlphanumeric(string value)
    {
        for (int i = 0; i < value.Length(); i++)
        {
            string char = value.Substring(i, 1);
            bool isDigit = (char >= "0" && char <= "9");
            bool isUpper = (char >= "A" && char <= "Z");
            bool isLower = (char >= "a" && char <= "z");
            
            if (!isDigit && !isUpper && !isLower)
                return false;
        }
        return true;
    }
    
    protected bool IsAlpha(string value)
    {
        for (int i = 0; i < value.Length(); i++)
        {
            string char = value.Substring(i, 1);
            bool isUpper = (char >= "A" && char <= "Z");
            bool isLower = (char >= "a" && char <= "z");
            
            if (!isUpper && !isLower)
                return false;
        }
        return true;
    }
}

// Email validator
class EmailValidator extends FormValidator
{
    override ValidationError Validate(string value)
    {
        if (value == "" || !value)
            return null;
            
        // Simple email validation: must contain @ and .
        int atIndex = value.IndexOf("@");
        int dotIndex = value.LastIndexOf(".");
        
        if (atIndex == -1 || dotIndex == -1 || atIndex > dotIndex || atIndex == 0 || dotIndex == value.Length() - 1)
            return new ValidationError("email", "Invalid email format");
            
        return null;
    }
}

// Range validator - value must be within range
class RangeValidator extends FormValidator
{
    protected float m_Min;
    protected float m_Max;
    
    void RangeValidator(float min, float max)
    {
        m_Min = min;
        m_Max = max;
    }
    
    override ValidationError Validate(string value)
    {
        if (value == "" || !value)
            return null;
            
        float numValue = value.ToFloat();
        if (numValue < m_Min || numValue > m_Max)
            return new ValidationError("range", "Value must be between " + m_Min.ToString() + " and " + m_Max.ToString());
            
        return null;
    }
}

// Custom validator - uses a custom validation function
class CustomValidator extends FormValidator
{
    protected string m_ErrorKey;
    protected string m_ErrorMessage;
    
    void CustomValidator(string errorKey, string errorMessage)
    {
        m_ErrorKey = errorKey;
        m_ErrorMessage = errorMessage;
    }
    
    // Override this in derived classes
    override ValidationError Validate(string value)
    {
        return null;
    }
}
