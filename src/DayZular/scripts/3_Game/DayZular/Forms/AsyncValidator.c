// DayZular Framework - Async Validator
// For server-side or delayed validation

class AsyncValidator extends FormValidator
{
    protected bool m_Pending = false;
    protected bool m_Valid = false;
    protected string m_ErrorMessage;
    protected string m_ErrorKey;
    
    void AsyncValidator(string errorKey = "async", string errorMessage = "Validation failed")
    {
        m_ErrorKey = errorKey;
        m_ErrorMessage = errorMessage;
    }
    
    override ValidationError Validate(string value)
    {
        if (m_Pending)
            return new ValidationError("pending", "Validating...");
            
        if (!m_Valid)
            return new ValidationError(m_ErrorKey, m_ErrorMessage);
            
        return null;
    }
    
    // Start async validation
    void StartValidation(string value)
    {
        m_Pending = true;
        m_Valid = false;
        
        // Override this in derived classes to implement actual async logic
        ValidateAsync(value);
    }
    
    // Override this to implement async validation logic
    protected void ValidateAsync(string value)
    {
        // Example: Check server-side
        // This would typically use RPC or REST API
        
        // For now, simulate async with delayed callback
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(OnValidationComplete, 1000, false, true);
    }
    
    // Called when async validation completes
    protected void OnValidationComplete(bool isValid)
    {
        m_Pending = false;
        m_Valid = isValid;
    }
    
    // Getters
    bool IsPending() { return m_Pending; }
    bool IsValid() { return m_Valid; }
}

