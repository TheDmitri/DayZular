// DayZular Framework - Observable Collection
// Reactive array collection that notifies on changes
// Independent from DabsFramework - uses DZ prefix to avoid conflicts

// Collection change actions
enum DZNotifyCollectionChangedAction
{
    DZ_Insert,      // Item added at end
    DZ_InsertAt,    // Item inserted at index
    DZ_Remove,      // Item removed
    DZ_Replace,     // Item replaced
    DZ_Move,        // Item moved
    DZ_Swap,        // Items swapped
    DZ_Clear        // All items removed
}

// Collection change event arguments
class DZCollectionChangedEventArgs
{
    ref DZObservableCollectionBase Source;
    DZNotifyCollectionChangedAction ChangedAction;
    int ChangedIndex;
    ref Param ChangedValue;
    
    void DZCollectionChangedEventArgs(DZObservableCollectionBase source, DZNotifyCollectionChangedAction action, int index, Param value)
    {
        Source = source;
        ChangedAction = action;
        ChangedIndex = index;
        ChangedValue = value;
    }
}

// Swap arguments
class DZCollectionSwapArgs: Param2<int, int>
{
    void DZCollectionSwapArgs(int p1, int p2)
    {
        param1 = p1;
        param2 = p2;
    }
}

// Base class for observable collections
class DZObservableCollectionBase
{
    protected ref ScriptInvoker m_OnCollectionChanged;
    protected DZlrComponentBase m_Component;
    
    void DZObservableCollectionBase(DZlrComponentBase component)
    {
        m_Component = component;
        m_OnCollectionChanged = new ScriptInvoker();
    }
    
    protected void CollectionChanged(DZCollectionChangedEventArgs args)
    {
        m_OnCollectionChanged.Invoke(args);
        
        // Trigger component update
        if (m_Component)
        {
            m_Component.MarkForCheck();
        }
    }
    
    ScriptInvoker GetInvoker()
    {
        return m_OnCollectionChanged;
    }
    
    int Count()
    {
        return 0;
    }
}

/*
Example usage:

class MyComponent extends DZlrComponentBase
{
    ref DZObservableCollection<string> items;
    
    void MyComponent()
    {
        items = new DZObservableCollection<string>(this);
        items.Insert("Item 1");
        items.Insert("Item 2");
    }
    
    override void OnInit()
    {
        items.GetInvoker().Insert(OnItemsChanged);
    }
    
    void OnItemsChanged(DZCollectionChangedEventArgs args)
    {
         DZLog().LogDebug("Collection changed: " + args.ChangedAction.ToString());
    }
}
*/

class DZObservableCollection<Class TValue>: DZObservableCollectionBase
{
    protected ref array<ref TValue> m_Data;
    
    void DZObservableCollection(DZlrComponentBase component)
    {
        m_Data = new array<ref TValue>();
    }
    
    void ~DZObservableCollection()
    {
        if (m_Data)
        {
            m_Data.Clear();
        }
    }
    
    // Insert item at end
    int Insert(TValue value)
    {
        int index = m_Data.Insert(value);
        if (index != -1)
        {
            CollectionChanged(new DZCollectionChangedEventArgs(this, DZNotifyCollectionChangedAction.DZ_Insert, index, new Param1<TValue>(value)));
        }
        return index;
    }
    
    // Insert item at specific index
    int InsertAt(TValue value, int index)
    {
        int new_index = m_Data.InsertAt(value, index);
        CollectionChanged(new DZCollectionChangedEventArgs(this, DZNotifyCollectionChangedAction.DZ_InsertAt, index, new Param1<TValue>(value)));
        return new_index;
    }
    
    // Insert all items from another array
    void InsertAll(notnull array<TValue> from)
    {
        for (int i = 0; i < from.Count(); i++)
        {
            Insert(from[i]);
        }
    }
    
    // Remove item at index (fast, doesn't preserve order)
    void Remove(int index)
    {
        if (index >= 0 && index < m_Data.Count())
        {
            TValue value = m_Data.Get(index);
            CollectionChanged(new DZCollectionChangedEventArgs(this, DZNotifyCollectionChangedAction.DZ_Remove, index, new Param1<TValue>(value)));
            m_Data.Remove(index);
        }
    }
    
    // Remove item at index (preserves order)
    void RemoveOrdered(int index)
    {
        if (index >= 0 && index < m_Data.Count())
        {
            TValue value = m_Data.Get(index);
            CollectionChanged(new DZCollectionChangedEventArgs(this, DZNotifyCollectionChangedAction.DZ_Remove, index, new Param1<TValue>(value)));
            m_Data.RemoveOrdered(index);
        }
    }
    
    // Remove range of items
    void RemoveRange(int start, int end)
    {
        for (int i = start; i < end; i++)
        {
            if (start < m_Data.Count())
            {
                TValue value = m_Data.Get(start);
                CollectionChanged(new DZCollectionChangedEventArgs(this, DZNotifyCollectionChangedAction.DZ_Remove, start, new Param1<TValue>(value)));
                m_Data.RemoveOrdered(start);
            }
        }
    }
    
    // Remove specific item
    void RemoveItem(TValue value)
    {
        int index = m_Data.Find(value);
        if (index != -1)
        {
            Remove(index);
        }
    }
    
    // Replace item at index
    void Set(int index, TValue value)
    {
        if (index >= 0 && index < m_Data.Count())
        {
            CollectionChanged(new DZCollectionChangedEventArgs(this, DZNotifyCollectionChangedAction.DZ_Replace, index, new Param1<TValue>(value)));
            m_Data.Set(index, value);
        }
    }
    
    // Move item to new index
    int MoveIndex(int index, int moveIndex)
    {
        if (index >= 0 && index < m_Data.Count())
        {
            TValue value = m_Data.Get(index);
            int new_index = m_Data.MoveIndex(index, moveIndex);
            CollectionChanged(new DZCollectionChangedEventArgs(this, DZNotifyCollectionChangedAction.DZ_Move, new_index, new Param1<TValue>(value)));
            return new_index;
        }
        return -1;
    }
    
    // Swap two items
    void SwapItems(int itemA, int itemB)
    {
        if (itemA >= 0 && itemA < m_Data.Count() && itemB >= 0 && itemB < m_Data.Count())
        {
            m_Data.SwapItems(itemA, itemB);
            CollectionChanged(new DZCollectionChangedEventArgs(this, DZNotifyCollectionChangedAction.DZ_Swap, -1, new DZCollectionSwapArgs(itemA, itemB)));
        }
    }
    
    // Clear all items
    void Clear()
    {
        m_Data.Clear();
        CollectionChanged(new DZCollectionChangedEventArgs(this, DZNotifyCollectionChangedAction.DZ_Clear, -1, null));
    }
    
    // Get item at index
    TValue Get(int index)
    {
        return m_Data.Get(index);
    }
    
    // Get count
    override int Count()
    {
        return m_Data.Count();
    }
    
    // Get underlying array
    array<ref TValue> GetArray()
    {
        return m_Data;
    }
    
    // Find item index
    int Find(TValue value)
    {
        return m_Data.Find(value);
    }
}
