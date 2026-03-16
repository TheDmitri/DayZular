/**
 * DZStream - Stream-like operations for Enforce Script arrays
 * 
 * Provides functional-style operations on arrays without lambdas.
 * Use predicate/consumer delegate classes for operations.
 * 
 * Example:
 * @code
 * // Define a predicate
 * class ItemTypePredicate : DZPredicate<EntityAI>
 * {
 *     string m_Type;
 *     void ItemTypePredicate(string type) { m_Type = type; }
 *     override bool Test(EntityAI item) { return item && item.GetType() == m_Type; }
 * }
 * 
 * // Use
 * bool hasItem = DZStream.From(items)
 *     .AnyMatch(new ItemTypePredicate("CombatKnife"));
 * 
 * // Find first matching
 * EntityAI knife = DZStream.From(items)
 *     .Filter(new ItemTypePredicate("CombatKnife"))
 *     .FindFirst();
 * @endcode
 */
class DZStream<Class T>
{
    protected ref array<T> m_Source;
    
    void DZStream(array<T> source)
    {
        m_Source = source;
        if (!m_Source)
            m_Source = new array<T>();
    }
    
    static DZStream<T> From(array<T> source)
    {
        return new DZStream<T>(source);
    }
    
    /*!brief Filter elements matching predicate */
    DZStream<T> Filter(DZPredicate<T> predicate)
    {
        array<T> filtered = new array<T>();
        foreach (T item : m_Source)
        {
            if (predicate.Test(item))
                filtered.Insert(item);
        }
        return new DZStream<T>(filtered);
    }
    
    /*!brief Transform elements using mapper function */
    DZStream<T> Map(DZFunction<T, T> mapper)
    {
        array<T> mapped = new array<T>();
        foreach (T item : m_Source)
        {
            mapped.Insert(mapper.Apply(item));
        }
        return new DZStream<T>(mapped);
    }
    
    /*!brief Returns first element, or null if empty */
    T FindFirst()
    {
        if (m_Source.Count() > 0)
            return m_Source[0];
        return null;
    }
    
    /*!brief Returns first matching element, or null */
    T FindFirst(DZPredicate<T> predicate)
    {
        foreach (T item : m_Source)
        {
            if (predicate.Test(item))
                return item;
        }
        return null;
    }
    
    /*!brief Check if any element matches */
    bool AnyMatch(DZPredicate<T> predicate)
    {
        foreach (T item : m_Source)
        {
            if (predicate.Test(item))
                return true;
        }
        return false;
    }
    
    /*!brief Check if all elements match */
    bool AllMatch(DZPredicate<T> predicate)
    {
        if (m_Source.Count() == 0)
            return true;
            
        foreach (T item : m_Source)
        {
            if (!predicate.Test(item))
                return false;
        }
        return true;
    }
    
    /*!brief Count elements matching predicate */
    int Count(DZPredicate<T> predicate)
    {
        int count = 0;
        foreach (T item : m_Source)
        {
            if (predicate.Test(item))
                count++;
        }
        return count;
    }
    
    /*!brief Count all elements */
    int Count()
    {
        return m_Source.Count();
    }
    
    /*!brief Execute action for each element */
    void ForEach(DZConsumer<T> action)
    {
        foreach (T item : m_Source)
        {
            action.Accept(item);
        }
    }
    
    /*!brief Check if stream has elements */
    bool IsEmpty()
    {
        return m_Source.Count() == 0;
    }
    
    /*!brief Get underlying array */
    array<T> ToArray()
    {
        return m_Source;
    }
}

/*!brief Predicate interface for filter/match operations */
class DZPredicate<Class T>
{
    bool Test(T item)
    {
        return true;
    }
}

/*!brief Consumer interface for forEach operations */
class DZConsumer<Class T>
{
    void Accept(T item)
    {
    }
}

/*!brief Function interface for map operations (T input -> R output) */
class DZFunction<Class T, Class R>
{
    R Apply(T input)
    {
        return null;
    }
}
