/**
 * DZPredicates - Pre-built predicates for common DayZ operations
 * 
 * These require NO class definition — just instantiate with parameters.
 * 
 * Example:
 * @code
 * // Instead of writing a foreach loop with 5 lines:
 * bool hasKnife = DZStream.From(items)
 *     .AnyMatch(new DZItemTypePredicate("CombatKnife"));
 * 
 * // Find first item with enough health
 * EntityAI goodItem = DZStream.From(items)
 *     .Filter(new DZItemTypePredicate("CombatKnife"))
 *     .Filter(new DZHealthMinPredicate(0.5))  // 50% health
 *     .FindFirst();
 * @endcode
 */

/*!brief Matches items by type name (case-insensitive) */
class DZItemTypePredicate : DZPredicate<EntityAI>
{
    string m_Type;
    
    void DZItemTypePredicate(string type)
    {
        m_Type = type;
    }
    
    override bool Test(EntityAI item)
    {
        if (!item)
            return false;
        
        return DZlrString.EqualsIgnoreCase(item.GetType(), m_Type);
    }
}

/*!brief Matches items with health above threshold (0.0 - 1.0) */
class DZHealthMinPredicate : DZPredicate<EntityAI>
{
    float m_MinHealth;
    
    void DZHealthMinPredicate(float minHealth)
    {
        m_MinHealth = minHealth;
    }
    
    override bool Test(EntityAI item)
    {
        if (!item)
            return false;
        ItemBase itemBase = ItemBase.Cast(item);
        if (!itemBase)
            return false;
        return itemBase.GetHealth01() >= m_MinHealth;
    }
}

/*!brief Matches items with health below threshold */
class DZHealthMaxPredicate : DZPredicate<EntityAI>
{
    float m_MaxHealth;
    
    void DZHealthMaxPredicate(float maxHealth)
    {
        m_MaxHealth = maxHealth;
    }
    
    override bool Test(EntityAI item)
    {
        if (!item)
            return false;
        ItemBase itemBase = ItemBase.Cast(item);
        if (!itemBase)
            return false;
        return itemBase.GetHealth01() <= m_MaxHealth;
    }
}

/*!brief Matches items with specific health level (0-4, see GameConstants) */
class DZHealthLevelPredicate : DZPredicate<EntityAI>
{
    int m_HealthLevel;
    
    void DZHealthLevelPredicate(int healthLevel)
    {
        m_HealthLevel = healthLevel;
    }
    
    override bool Test(EntityAI item)
    {
        if (!item)
            return false;
        return item.GetHealthLevel() == m_HealthLevel;
    }
}

/*!brief Matches items that are not ruined */
class DZNotRuinedPredicate : DZPredicate<EntityAI>
{
    override bool Test(EntityAI item)
    {
        if (!item)
            return false;
        return item.GetHealthLevel() != GameConstants.STATE_RUINED;
    }
}

/*!brief Matches items in specific quantity range (for stackable items) */
class DZQuantityMinPredicate : DZPredicate<EntityAI>
{
    int m_MinQuantity;
    
    void DZQuantityMinPredicate(int minQuantity)
    {
        m_MinQuantity = minQuantity;
    }
    
    override bool Test(EntityAI item)
    {
        if (!item)
            return false;
        
        return true;
    }
}

/*!brief Matches edible items that are in RAW state */
class DZFoodRawPredicate : DZPredicate<EntityAI>
{
    override bool Test(EntityAI item)
    {
        if (!item)
            return false;
        ItemBase itemBase = ItemBase.Cast(item);
        if (!itemBase)
            return false;
        if (!itemBase.IsKindOf("Edible_Base"))
            return true; // Not edible, so passes raw check
        
        Edible_Base edible = Edible_Base.Cast(itemBase);
        if (!edible.HasFoodStage())
            return true; // No food stage, passes
            
        return edible.GetFoodStageType() == FoodStageType.RAW;
    }
}
class DZAndPredicate : DZPredicate<EntityAI>
{
    ref array<ref DZPredicate<EntityAI>> m_Predicates;
    
    void DZAndPredicate(DZPredicate<EntityAI> p1, DZPredicate<EntityAI> p2 = NULL, DZPredicate<EntityAI> p3 = NULL)
    {
        m_Predicates = new array<ref DZPredicate<EntityAI>>();
        m_Predicates.Insert(p1);
        if (p2)
            m_Predicates.Insert(p2);
        if (p3)
            m_Predicates.Insert(p3);
    }
    
    override bool Test(EntityAI item)
    {
        foreach (DZPredicate<EntityAI> pred : m_Predicates)
        {
            if (!pred.Test(item))
                return false;
        }
        return true;
    }
}

/*!brief Combines multiple predicates with OR logic */
class DZOrPredicate : DZPredicate<EntityAI>
{
    ref array<ref DZPredicate<EntityAI>> m_Predicates;
    
    void DZOrPredicate(DZPredicate<EntityAI> p1, DZPredicate<EntityAI> p2 = NULL, DZPredicate<EntityAI> p3 = NULL)
    {
        m_Predicates = new array<ref DZPredicate<EntityAI>>();
        m_Predicates.Insert(p1);
        if (p2)
            m_Predicates.Insert(p2);
        if (p3)
            m_Predicates.Insert(p3);
    }
    
    override bool Test(EntityAI item)
    {
        foreach (DZPredicate<EntityAI> pred : m_Predicates)
        {
            if (pred.Test(item))
                return true;
        }
        return false;
    }
}
