/**
 * RangeMapper - Threshold-based value mapping utility for DayZular
 * 
 * Maps numeric ranges to values using threshold logic. Given an input value,
 * returns the value associated with the highest threshold that is <= the input.
 * 
 * Syntax follows Enforce Script generics pattern with Class TKey, Class TValue.
 * Supports fluent method chaining for declarative configuration.
 * 
 * Example:
 * @code
 * RangeMapper<int, string> states = new RangeMapper<int, string>();
 * states.Insert(0, "Gathering materials...")
 *       .Insert(25, "Assembling components...")
 *       .Insert(50, "Finishing touches...")
 *       .Insert(75, "Almost complete...")
 *       .SetDefault("Done");
 * 
 * string state = states.GetValue(craftProgress); // Returns appropriate message
 * @endcode
 */
class RangeMapper<Class TKey, Class TValue>
{
    protected ref map<TKey, TValue> m_Thresholds;
    protected TValue m_DefaultValue;
    protected bool m_HasDefault;
    
    void RangeMapper()
    {
        m_Thresholds = new map<TKey, TValue>();
        m_HasDefault = false;
    }
    
    /*!brief Add a threshold mapping.
     * @param threshold The minimum value for this range (inclusive)
     * @param value The value to return when input >= threshold
     * @return This RangeMapper for method chaining
     */
    RangeMapper<TKey, TValue> Insert(TKey threshold, TValue value)
    {
        m_Thresholds.Set(threshold, value);
        return this;
    }
    
    /*!brief Set the default value returned when no threshold matches.
     * @param value The default value
     * @return This RangeMapper for method chaining
     */
    RangeMapper<TKey, TValue> SetDefault(TValue value)
    {
        m_DefaultValue = value;
        m_HasDefault = true;
        return this;
    }
    
    /*!brief Get the mapped value for an input.
     * Finds the highest threshold <= input and returns its associated value.
     * @param input The value to evaluate against thresholds
     * @return The mapped value, or default if no threshold matches
     */
    TValue GetValue(TKey input)
    {
        // Find highest threshold <= input
        TValue lastMatch = m_DefaultValue;
        
        if (m_Thresholds.Count() == 0)
        {
            return lastMatch;
        }
        
        // Get all keys and find the best match
        array<TKey> keys = m_Thresholds.GetKeyArray();
        
        // Sort keys ascending (bubble sort for generic types)
        for (int i = 0; i < keys.Count(); i++)
        {
            for (int j = 0; j < keys.Count() - i - 1; j++)
            {
                if (keys[j] > keys[j + 1])
                {
                    TKey temp = keys[j];
                    keys[j] = keys[j + 1];
                    keys[j + 1] = temp;
                }
            }
        }
        
        foreach (TKey threshold : keys)
        {
            if (input >= threshold)
            {
                lastMatch = m_Thresholds.Get(threshold);
            }
            else
            {
                break; // Keys are sorted, no more matches possible
            }
        }
        
        return lastMatch;
    }
}
