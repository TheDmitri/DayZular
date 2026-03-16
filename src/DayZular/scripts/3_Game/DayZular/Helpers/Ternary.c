// DayZular Framework - Ternary Helper
// Provides ternary-like functionality for DayZ's Enforce Script
// Improved version inspired by DabsFramework but with better type support

// ============================================================================
// GENERIC TERNARY (for reference types)
// ============================================================================

class DZTernary<Class T>
{
    private void DZTernary();
    private void ~DZTernary();
    
    // Generic ternary for any class type
    static T If(bool condition, T trueValue, T falseValue)
    {
        if (condition)
        {
            return trueValue;
        }
        return falseValue;
    }
}

// ============================================================================
// PRIMITIVE TYPE TERNARIES (better performance, no boxing)
// ============================================================================

class DZTernaryBool
{
    private void DZTernaryBool();
    private void ~DZTernaryBool();
    
    static bool If(bool condition, bool trueValue, bool falseValue)
    {
        if (condition)
        {
            return trueValue;
        }
        return falseValue;
    }
}

class DZTernaryInt
{
    private void DZTernaryInt();
    private void ~DZTernaryInt();
    
    static int If(bool condition, int trueValue, int falseValue)
    {
        if (condition)
        {
            return trueValue;
        }
        return falseValue;
    }
}

class DZTernaryFloat
{
    private void DZTernaryFloat();
    private void ~DZTernaryFloat();
    
    static float If(bool condition, float trueValue, float falseValue)
    {
        if (condition)
        {
            return trueValue;
        }
        return falseValue;
    }
}

class DZTernaryString
{
    private void DZTernaryString();
    private void ~DZTernaryString();
    
    static string If(bool condition, string trueValue, string falseValue)
    {
        if (condition)
        {
            return trueValue;
        }
        return falseValue;
    }
}

class DZTernaryVector
{
    private void DZTernaryVector();
    private void ~DZTernaryVector();
    
    static vector If(bool condition, vector trueValue, vector falseValue)
    {
        if (condition)
        {
            return trueValue;
        }
        return falseValue;
    }
}

// ============================================================================
// CONVENIENCE FUNCTIONS (global scope for easy access)
// ============================================================================

// Boolean ternary
bool TernaryBool(bool condition, bool trueValue, bool falseValue)
{
    if (condition) return trueValue;
    return falseValue;
}

// Integer ternary
int TernaryInt(bool condition, int trueValue, int falseValue)
{
    if (condition) return trueValue;
    return falseValue;
}

// Float ternary
float TernaryFloat(bool condition, float trueValue, float falseValue)
{
    if (condition) return trueValue;
    return falseValue;
}

// String ternary
string TernaryString(bool condition, string trueValue, string falseValue)
{
    if (condition) return trueValue;
    return falseValue;
}

// Vector ternary
vector TernaryVector(bool condition, vector trueValue, vector falseValue)
{
    if (condition) return trueValue;
    return falseValue;
}

// ============================================================================
// NULL-COALESCING HELPERS (like ?? operator)
// ============================================================================

class DZCoalesce<Class T>
{
    private void DZCoalesce();
    private void ~DZCoalesce();
    
    // Returns first non-null value
    static T Value(T value, T defaultValue)
    {
        if (value)
        {
            return value;
        }
        return defaultValue;
    }
}

// Note: Generic functions not supported in global scope
// Use DZCoalesce<T>.Value() instead for generic types

// String-specific coalescing (handles empty strings too)
string CoalesceString(string value, string defaultValue)
{
    if (value && value != "")
    {
        return value;
    }
    return defaultValue;
}

// Multiple string coalescing (returns first non-empty)
string CoalesceStrings(array<string> values, string defaultValue = "")
{
    if (values)
    {
        for (int i = 0; i < values.Count(); i++)
        {
            if (values[i] && values[i] != "")
            {
                return values[i];
            }
        }
    }
    return defaultValue;
}

// ============================================================================
// USAGE EXAMPLES
// ============================================================================

/*
// Generic (for classes)
Widget widget = DZTernary<Widget>.If(condition, widgetA, widgetB);

// Primitives (better performance)
int value = DZTernaryInt.If(health > 0, health, 0);
string text = DZTernaryString.If(isOnline, "Online", "Offline");
bool result = DZTernaryBool.If(hasPermission, true, false);
float speed = DZTernaryFloat.If(isSprinting, 5.0, 2.0);

// Convenience functions (shorter syntax)
int damage = TernaryInt(isCritical, 100, 50);
string status = TernaryString(isAlive, "Alive", "Dead");

// Null coalescing (use class method for generic types)
Widget myWidget = DZCoalesce<Widget>.Value(optionalWidget, defaultWidget);
DZlrComponentBase comp = DZCoalesce<DZlrComponentBase>.Value(customComp, defaultComp);

// String coalescing (handles empty strings)
string displayName = CoalesceString(playerName, "Guest");
string status = CoalesceString(customStatus, "Offline");

// Multiple string coalescing (first non-empty wins)
array<string> names = new array<string>();
names.Insert(nickname);
names.Insert(username);
names.Insert(characterName);
string finalName = CoalesceStrings(names, "Unknown Player");

// Real-world examples:
class MyComponent extends DZlrComponentBase
{
    void UpdateUI()
    {
        // Instead of:
        string status;
        if (m_IsOnline)
        {
            status = "Connected";
        }
        else
        {
            status = "Disconnected";
        }
        
        // Use:
        string status = TernaryString(m_IsOnline, "Connected", "Disconnected");
        
        // Or:
        int health = TernaryInt(m_Player, m_Player.GetHealth(), 0);
        
        // Null coalescing:
        Widget widget = DZCoalesce<Widget>.Value(m_OptionalWidget, m_DefaultWidget);
    }
}
*/
