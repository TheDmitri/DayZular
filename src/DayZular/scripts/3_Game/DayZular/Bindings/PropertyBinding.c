// DayZular Framework - Property Binding System

// Base binding class
class PropertyBinding
{
    protected Widget m_Widget;
    protected string m_PropertyName;
    protected string m_Expression;
    
    void PropertyBinding(Widget widget, string propertyName, string expression)
    {
        m_Widget = widget;
        m_PropertyName = propertyName;
        m_Expression = expression;
    }
    
    void Update(DZlrComponentBase component)
    {
        // Override in child classes
    }
    
    protected string EvaluateExpression(DZlrComponentBase component, string expr)
    {
        // Expression evaluator
        // Supports: 
        // - {{ propertyName }}
        // - {{ propertyName | pipe }}
        // - {{ propertyName ?? defaultValue }}
        // - Static strings with interpolation: "x{{ count }} items"
        // - Pure static strings (no {{ }})
        
        // Check if it's a static string (no binding expression)
        if (!expr.Contains("{{"))
        {
            return expr;
        }
        
        // Process string interpolation - evaluate each {{ }} block and preserve surrounding text
        string result = expr;
        int maxIterations = 10;
        int iterations = 0;
        
        while (result.Contains("{{") && iterations < maxIterations)
        {
            int startIdx = result.IndexOf("{{");
            int endIdx = result.IndexOf("}}");
            
            if (endIdx == -1 || endIdx < startIdx)
                break; // Malformed expression
            
            // Get the parts
            string before = result.Substring(0, startIdx);
            string bindingExpr = result.Substring(startIdx + 2, endIdx - startIdx - 2); // {{ and }}
            bindingExpr = bindingExpr.Trim();
            string after = "";
            if (endIdx + 2 < result.Length())
            {
                after = result.Substring(endIdx + 2, result.Length() - endIdx - 2);
            }
            
            // Evaluate the binding expression
            string bindingValue = EvaluateBindingExpression(component, bindingExpr);
            
            // Reconstruct result
            result = before + bindingValue + after;
            iterations++;
        }
        
        return result;
    }
    
    // Evaluate a single binding expression (without {{ }} wrappers)
    protected string EvaluateBindingExpression(DZlrComponentBase component, string expr)
    {
        expr = expr.Trim();
        
        // Check for ternary operator: condition ? trueValue : falseValue
        if (expr.IndexOf("?") != -1 && expr.IndexOf(":") != -1)
        {
            return EvaluateTernary(component, expr);
        }
        
        // Check for null coalescing operator: property ?? default
        array<string> coalesceParts = new array<string>();
        expr.Split("??", coalesceParts);
        
        string propertyExpression = coalesceParts[0];
        propertyExpression = propertyExpression.Trim();
        
        string defaultValue = "";
        if (coalesceParts.Count() > 1)
        {
            defaultValue = coalesceParts[1];
            defaultValue = defaultValue.Trim();
        }
        
        // Check for pipe usage: property | pipe
        array<string> pipeParts = new array<string>();
        propertyExpression.Split("|", pipeParts);
        
        string propertyName = pipeParts[0];
        propertyName = propertyName.Trim();
        
        string value = GetPropertyValue(component, propertyName);
        
        // Apply null coalescing if value is empty
        if (value == "" && defaultValue != "")
        {
            value = defaultValue;
        }
        
        // Apply pipes if present
        if (pipeParts.Count() > 1)
        {
            for (int i = 1; i < pipeParts.Count(); i++) {
                string pipeExpr = pipeParts[i];
                pipeExpr = pipeExpr.Trim();
                
                // Parse pipe name and arguments: pipeName:arg1:arg2
                array<string> pipeComponents = new array<string>();
                pipeExpr.Split(":", pipeComponents);
                
                string pipeName = pipeComponents[0];
                pipeName = pipeName.Trim();
                
                // Extract arguments
                array<string> pipeArgs = new array<string>();
                for (int j = 1; j < pipeComponents.Count(); j++) {
                    string arg = pipeComponents[j];
                    arg = arg.Trim();
                    // Remove quotes if present
                    arg.Replace("'", "");
                    arg.Replace("\"", "");
                    pipeArgs.Insert(arg);
                }
                
                DZPipe pipe = PipeRegistry.GetPipe(pipeName);
                if (pipe)
                {
                    value = pipe.Transform(value, pipeArgs);
                }
                else
                {
                    DZLog().LogWarning("Pipe '" + pipeName + "' not found");
                }
            }
        }
        
        return value;
    }
    
    protected string GetPropertyValue(DZlrComponentBase component, string propertyName)
    {
        
        // Check if it's nested property/method access (contains .) FIRST
        // This handles _inputItem.GetDisplayName() and _inputItem.name
        if (propertyName.Contains("."))
        {
            return EvaluateNestedProperty(component, propertyName);
        }
        
        // Check if it's a direct method call on component (ends with ())
        if (propertyName.Contains("()"))
        {
            return EvaluateMethodCall(component, propertyName);
        }
        
        // Resolve the variable's actual type using typename reflection,
        // then call GetClassVar with the correctly typed out-parameter.
        // EnScript.GetClassVar may return false for subclass variables but
        // still populates the out-parameter correctly.
        typename componentType = component.Type();
        typename varType;
        for (int i = 0; i < componentType.GetVariableCount(); i++) {
            if (componentType.GetVariableName(i) == propertyName)
            {
                varType = componentType.GetVariableType(i);
                break;
            }
        }
        
        if (varType)
        {
            if (varType == string)
            {
                string stringValue;
                EnScript.GetClassVar(component, propertyName, 0, stringValue);
                return stringValue;
            }
            else if (varType == int)
            {
                int intValue;
                EnScript.GetClassVar(component, propertyName, 0, intValue);
                return intValue.ToString();
            }
            else if (varType == float)
            {
                float floatValue;
                EnScript.GetClassVar(component, propertyName, 0, floatValue);
                return floatValue.ToString();
            }
            else if (varType == bool)
            {
                bool boolValue;
                EnScript.GetClassVar(component, propertyName, 0, boolValue);
                return boolValue.ToString();
            }
            else
            {
                Class classValue;
                EnScript.GetClassVar(component, propertyName, 0, classValue);
                if (classValue)
                    return classValue.ToString();
                return "";
            }
        }
        
        return "";
    }
    
    // Evaluate method call: GetHealth()
    protected string EvaluateMethodCall(DZlrComponentBase component, string methodExpr)
    {
        // Remove ()
        string methodName = methodExpr;
        methodName.Replace("()", "");
        methodName = methodName.Trim();
        
        // Try float first (most common for health/percentage values)
        float floatResult;
        if (g_Script.CallFunction(component, methodName, floatResult, null) && floatResult != 0)
        {
            return floatResult.ToString();
        }
        
        // Try int
        int intResult;
        if (g_Script.CallFunction(component, methodName, intResult, null) && intResult != 0)
        {
            return intResult.ToString();
        }
        
        // Try string (check non-empty to avoid matching numeric methods)
        string stringResult;
        if (g_Script.CallFunction(component, methodName, stringResult, null) && stringResult != "")
        {
            return stringResult;
        }
        
        // Try bool last (accept any result)
        bool boolResult;
        if (g_Script.CallFunction(component, methodName, boolResult, null))
        {
            return boolResult.ToString();
        }
        
        DZLog().LogWarning("Method '" + methodName + "' not found or returned void");
        return "";
    }
    
    // Evaluate ternary expression: condition ? trueValue : falseValue
    protected string EvaluateTernary(DZlrComponentBase component, string ternaryExpr)
    {
        // Split by ? to get condition and rest
        array<string> ternaryParts = new array<string>();
        ternaryExpr.Split("?", ternaryParts);
        
        if (ternaryParts.Count() < 2)
            return "";
        
        string conditionStr = ternaryParts[0].Trim();
        
        // The rest should be "trueValue : falseValue"
        string trueFalsePart = ternaryParts[1];
        for (int i = 2; i < ternaryParts.Count(); i++) {
            trueFalsePart = trueFalsePart + "?" + ternaryParts[i];
        }
        
        // Split by : to separate true and false values
        array<string> valueParts = new array<string>();
        trueFalsePart.Split(":", valueParts);
        
        if (valueParts.Count() < 2)
            return "";
        
        string trueValueExpr = valueParts[0].Trim();
        string falseValueExpr = valueParts[1].Trim();
        
        // Check for pipes in the result (they come after the false value)
        string pipesToApply = "";
        if (valueParts.Count() > 2)
        {
            // Reconstruct the pipes part (everything after first falseValue)
            falseValueExpr = valueParts[1].Trim();
            pipesToApply = valueParts[2];
            for (int j = 3; j < valueParts.Count(); j++) {
                pipesToApply = pipesToApply + ":" + valueParts[j];
            }
            pipesToApply = pipesToApply.Trim();
        }
        
        // Evaluate condition
        bool conditionResult = EvaluateCondition(component, conditionStr);
        
        // Select appropriate value expression
        string selectedValue;
        if (conditionResult)
            selectedValue = trueValueExpr;
        else
            selectedValue = falseValueExpr;
        
        // Evaluate the selected value
        string result = EvaluateValueExpression(component, selectedValue);
        
        // Apply pipes if present (after the false value)
        if (pipesToApply != "")
        {
            result = ApplyPipes(result, pipesToApply);
        }
        
        return result;
    }
    
    // Evaluate condition for ternary (can be bool property or method)
    protected bool EvaluateCondition(DZlrComponentBase component, string conditionExpr)
    {
        // Try as bool property or method
        string conditionValue = GetPropertyValue(component, conditionExpr);
        
        // Convert to bool - "true" or "1" means true, anything else is false
        conditionValue.ToLower();
        if (conditionValue == "true" || conditionValue == "1")
            return true;
        
        return false;
    }
    
    // Evaluate a value expression (property, method, or literal string)
    protected string EvaluateValueExpression(DZlrComponentBase component, string valueExpr)
    {
        // Check if it's a string literal (starts and ends with same quote type)
        int firstQuote = valueExpr.IndexOf("'");
        int lastQuote = valueExpr.LastIndexOf("'");
        int firstDblQuote = valueExpr.IndexOf("\"");
        int lastDblQuote = valueExpr.LastIndexOf("\"");
        int exprLen = valueExpr.Length();
        bool isSingleQuoted = (firstQuote == 0 && lastQuote == exprLen - 1 && firstQuote != -1);
        bool isDoubleQuoted = (firstDblQuote == 0 && lastDblQuote == exprLen - 1 && firstDblQuote != -1);
        if (isSingleQuoted || isDoubleQuoted)
        {
            // Remove quotes
            valueExpr.Replace("'", "");
            valueExpr.Replace("\"", "");
            return valueExpr;
        }
        
        // Otherwise treat as property or method
        return GetPropertyValue(component, valueExpr);
    }
    
    // Apply pipes to a value (extracted for reuse)
    protected string ApplyPipes(string value, string pipeExpression)
    {
        // Parse multiple pipes: pipe1 | pipe2
        array<string> pipeParts = new array<string>();
        pipeExpression.Split("|", pipeParts);
        
        foreach (string pipeExpr : pipeParts) {
            string pipeExprTrimmed = pipeExpr;
            pipeExprTrimmed = pipeExprTrimmed.Trim();
            
            // Parse pipe name and arguments: pipeName:arg1:arg2
            array<string> pipeComponents = new array<string>();
            pipeExprTrimmed.Split(":", pipeComponents);
            
            string pipeName = pipeComponents[0];
            pipeName = pipeName.Trim();
            
            // Extract arguments
            array<string> pipeArgs = new array<string>();
            for (int j = 1; j < pipeComponents.Count(); j++)
            {
                string arg = pipeComponents[j];
                arg = arg.Trim();
                // Remove quotes if present
                arg.Replace("'", "");
                arg.Replace("\"", "");
                pipeArgs.Insert(arg);
            }
            
            DZPipe pipe = PipeRegistry.GetPipe(pipeName);
            if (pipe)
            {
                value = pipe.Transform(value, pipeArgs);
            }
            else
            {
                DZLog().LogWarning("Pipe '" + pipeName + "' not found in ternary");
            }
        }
        
        return value;
    }
    
    // Evaluate nested property: _inputProduct.name or _inputItem.GetDisplayName()
    protected string EvaluateNestedProperty(DZlrComponentBase component, string propertyPath)
    {
        array<string> parts = new array<string>();
        propertyPath.Split(".", parts);
        
        if (parts.Count() == 0)
            return "";
        
        
        // Get first property (try as string first, then as Class)
        string stringValue = "";
        EnScript.GetClassVar(component, parts[0], 0, stringValue);
        if (stringValue != "")
        {
            // First property is a string, can't navigate further
            if (parts.Count() == 1)
                return stringValue;
            return "";
        }
        
        // Try as Class/object
        Class currentValue;
        EnScript.GetClassVar(component, parts[0], 0, currentValue);
        
        if (!currentValue)
        {
            return "";
        }
        
        // Navigate through nested properties
        for (int i = 1; i < parts.Count(); i++) {
            string part = parts[i];
            
            // Check if this part is a method call (ends with ())
            if (part.Contains("()"))
            {
                return EvaluateMethodOnObject(currentValue, part);
            }
            
            // Try string first
            string nestedString = "";
            EnScript.GetClassVar(currentValue, part, 0, nestedString);
            if (nestedString != "")
            {
                return nestedString;
            }
            
            // Try int
            int nestedInt;
            EnScript.GetClassVar(currentValue, part, 0, nestedInt);
            if (nestedInt != 0)
            {
                return nestedInt.ToString();
            }
            
            // Try as object for further navigation
            Class nextValue;
            EnScript.GetClassVar(currentValue, part, 0, nextValue);
            if (nextValue)
            {
                currentValue = nextValue;
            }
            else
            {
                return "";
            }
        }
        
        if (currentValue)
        {
            return currentValue.ToString();
        }
        
        return "";
    }
    
    // Evaluate method call on an arbitrary object (not just DZlrComponentBase)
    protected string EvaluateMethodOnObject(Class target, string methodExpr)
    {
        // Remove ()
        string methodName = methodExpr;
        methodName.Replace("()", "");
        methodName = methodName.Trim();
        
        // Try string first (most common for display names)
        string stringResult;
        if (g_Script.CallFunction(target, methodName, stringResult, null) && stringResult != "")
        {
            return stringResult;
        }
        
        // Try int
        int intResult;
        if (g_Script.CallFunction(target, methodName, intResult, null))
        {
            return intResult.ToString();
        }
        
        // Try float
        float floatResult;
        if (g_Script.CallFunction(target, methodName, floatResult, null) && floatResult != 0)
        {
            return floatResult.ToString();
        }
        
        // Try bool last
        bool boolResult;
        if (g_Script.CallFunction(target, methodName, boolResult, null))
        {
            return boolResult.ToString();
        }
        
        DZLog().LogWarning("Method '" + methodName + "' not found or returned void");
        return "";
    }
}

// Text property binding (internal implementation)
class TextPropertyBinding extends PropertyBinding
{
    protected string m_BindingExpression; // Stored separately from widget text
    
    void TextPropertyBinding(Widget widget, string propertyName, string expression)
    {
        m_Widget = widget;
        m_PropertyName = propertyName;
        m_Expression = expression;
        m_BindingExpression = expression;
    }
    
    
    override void Update(DZlrComponentBase component)
    {
        string value = EvaluateExpression(component, m_BindingExpression);
        
        TextWidget textWidget = TextWidget.Cast(m_Widget);
        if (textWidget)
        {
            textWidget.SetText(value);
        }
    }
}

// Widget property binding ([property]="expression")
class WidgetPropertyBinding extends PropertyBinding
{
    void WidgetPropertyBinding(Widget widget, string propertyName, string expression)
    {
        m_Widget = widget;
        m_PropertyName = propertyName;
        m_Expression = expression;
    }
    
    override void Update(DZlrComponentBase component)
    {
        Class value;
        if (EnScript.GetClassVar(component, m_Expression, 0, value))
        {
            // Set widget property
            EnScript.SetClassVar(m_Widget, m_PropertyName, 0, value);
        }
    }
}

// Progress bar property binding
class ProgressBarPropertyBinding extends PropertyBinding
{
    protected string m_BindingExpression;
    
    void ProgressBarPropertyBinding(Widget widget, string propertyName, string expression)
    {
        m_Widget = widget;
        m_PropertyName = propertyName;
        m_Expression = expression;
        m_BindingExpression = expression;
        
        ProgressBarWidget progressBar = ProgressBarWidget.Cast(widget);
        if (progressBar)
        {
            progressBar.SetCurrent(0);
        }
    }
    
    override void Update(DZlrComponentBase component)
    {
        ProgressBarWidget progressBar = ProgressBarWidget.Cast(m_Widget);
        if (progressBar)
        {
            string strValue = EvaluateExpression(component, m_BindingExpression);
            int intValue = strValue.ToInt();
            float floatValue = intValue * 1.0;
            Print("[ProgressBarPropertyBinding] Expression: " + m_BindingExpression + " strValue: " + strValue + " intValue: " + intValue + " Setting current: " + floatValue);
            progressBar.SetCurrent(floatValue);
        }
        else
        {
            DZLog().LogWarning("ProgressBarPropertyBinding - Widget is not a ProgressBarWidget");
        }
    }
}

