// DayZular Framework - Form Builder
// Helper class for building forms with fluent API

class FormBuilder
{
    // Create a form control with validators from string (layout-driven)
    // Format: "required,integer,min:1,max:999"
    static FormControl FromString(string name, string initialValue, string validatorString)
    {
        FormControl control = new FormControl(name, initialValue);
        
        if (validatorString && validatorString != "")
        {
            ParseValidators(control, validatorString);
        }
        
        return control;
    }
    
    // Parse validator string and add to control
    protected static void ParseValidators(FormControl control, string validatorString)
    {
        array<string> validatorList = new array<string>();
        validatorString.Split(",", validatorList);
        
        foreach (string validatorDef : validatorList)
        {
            validatorDef.Trim();
            if (validatorDef == "") continue;
            
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
                control.AddValidator(new RequiredValidator());
            }
            else if (validatorName == "min")
            {
                control.AddValidator(new MinValidator(validatorParam.ToFloat()));
            }
            else if (validatorName == "max")
            {
                control.AddValidator(new MaxValidator(validatorParam.ToFloat()));
            }
            else if (validatorName == "range")
            {
                // Range format: "range:1:100"
                array<string> rangeParts = new array<string>();
                validatorParam.Split(":", rangeParts);
                if (rangeParts.Count() >= 2)
                {
                    control.AddValidator(new RangeValidator(rangeParts[0].ToFloat(), rangeParts[1].ToFloat()));
                }
            }
            else if (validatorName == "minlength")
            {
                control.AddValidator(new MinLengthValidator(validatorParam.ToInt()));
            }
            else if (validatorName == "maxlength")
            {
                control.AddValidator(new MaxLengthValidator(validatorParam.ToInt()));
            }
            else if (validatorName == "numeric")
            {
                control.AddValidator(new PatternValidator("numeric", "Must be a number"));
            }
            else if (validatorName == "integer")
            {
                control.AddValidator(new PatternValidator("integer", "Must be a whole number"));
            }
            else if (validatorName == "alpha" || validatorName == "alphanumeric" || validatorName == "email")
            {
                control.AddValidator(new PatternValidator(validatorName, "Invalid format"));
            }
        }
    }

    // Create a form control with validators
    static FormControl Control(string name, string initialValue)
    {
        return new FormControl(name, initialValue);
    }
    
    // Create a required control
    static FormControl Required(string name, string initialValue)
    {
        FormControl control = new FormControl(name, initialValue);
        control.AddValidator(new RequiredValidator());
        return control;
    }
    
    // Create a numeric control with range
    static FormControl Numeric(string name, string initialValue, float min, float max)
    {
        FormControl control = new FormControl(name, initialValue);
        control.AddValidator(new RequiredValidator());
        control.AddValidator(new PatternValidator("numeric", "Must be a number"));
        control.AddValidator(new MinValidator(min));
        control.AddValidator(new MaxValidator(max));
        return control;
    }
    
    // Create an integer control with range
    static FormControl Integer(string name, string initialValue, int min, int max)
    {
        FormControl control = new FormControl(name, initialValue);
        control.AddValidator(new RequiredValidator());
        control.AddValidator(new PatternValidator("integer", "Must be a whole number"));
        control.AddValidator(new MinValidator(min));
        control.AddValidator(new MaxValidator(max));
        return control;
    }
    
    // Create a text control with length constraints
    static FormControl Text(string name, string initialValue, int minLength, int maxLength)
    {
        FormControl control = new FormControl(name, initialValue);
        if (minLength > 0)
        {
            control.AddValidator(new RequiredValidator());
            control.AddValidator(new MinLengthValidator(minLength));
        }
        if (maxLength < 999)
            control.AddValidator(new MaxLengthValidator(maxLength));
        return control;
    }
    
    // Create an email control
    static FormControl Email(string name, string initialValue)
    {
        FormControl control = new FormControl(name, initialValue);
        control.AddValidator(new RequiredValidator());
        control.AddValidator(new EmailValidator());
        return control;
    }
    
    // Compose multiple validators
    static array<ref FormValidator> Compose(array<ref FormValidator> validators)
    {
        return validators;
    }
}

// Validators helper class for common validator combinations
class Validators
{
    // Required validator
    static RequiredValidator Required()
    {
        return new RequiredValidator();
    }
    
    // Min validator
    static MinValidator Min(float min)
    {
        return new MinValidator(min);
    }
    
    // Max validator
    static MaxValidator Max(float max)
    {
        return new MaxValidator(max);
    }
    
    // Range validator
    static RangeValidator Range(float min, float max)
    {
        return new RangeValidator(min, max);
    }
    
    // MinLength validator
    static MinLengthValidator MinLength(int length)
    {
        return new MinLengthValidator(length);
    }
    
    // MaxLength validator
    static MaxLengthValidator MaxLength(int length)
    {
        return new MaxLengthValidator(length);
    }
    
    // Pattern validator
    static PatternValidator Pattern(string pattern, string errorMessage)
    {
        return new PatternValidator(pattern, errorMessage);
    }
    
    // Email validator
    static EmailValidator Email()
    {
        return new EmailValidator();
    }
    
    // Compose multiple validators
    static array<ref FormValidator> Compose(array<ref FormValidator> validators)
    {
        return validators;
    }
}
