// DayZular Framework - Observable System
// Reactive data streams using ScriptInvoker

// Observable class (like RxJS Observable)
// Note: Enforce Script doesn't support template inheritance well, so we use a simpler approach
class ObservableBase
{
    protected ref ScriptInvoker m_OnNext;
    
    void ObservableBase()
    {
        m_OnNext = new ScriptInvoker();
    }
    
    ScriptInvoker GetInvoker()
    {
        return m_OnNext;
    }
}

// Observable for int values
class ObservableInt extends ObservableBase
{
    private int m_Value;
    
    void ObservableInt(int initialValue = 0)
    {
        m_Value = initialValue;
    }
    
    void Next(int value)
    {
        m_Value = value;
        m_OnNext.Invoke(value);
    }
    
    int GetValue()
    {
        return m_Value;
    }
}

// Observable for float values
class ObservableFloat extends ObservableBase
{
    private float m_Value;
    
    void ObservableFloat(float initialValue = 0.0)
    {
        m_Value = initialValue;
    }
    
    void Next(float value)
    {
        m_Value = value;
        m_OnNext.Invoke(value);
    }
    
    float GetValue()
    {
        return m_Value;
    }
}

// Observable for string values
class ObservableString extends ObservableBase
{
    private string m_Value;
    
    void ObservableString(string initialValue = "")
    {
        m_Value = initialValue;
    }
    
    void Next(string value)
    {
        m_Value = value;
        m_OnNext.Invoke(value);
    }
    
    string GetValue()
    {
        return m_Value;
    }
}

// Observable for bool values
class ObservableBool extends ObservableBase
{
    private bool m_Value;
    
    void ObservableBool(bool initialValue = false)
    {
        m_Value = initialValue;
    }
    
    void Next(bool value)
    {
        m_Value = value;
        m_OnNext.Invoke(value);
    }
    
    bool GetValue()
    {
        return m_Value;
    }
}
