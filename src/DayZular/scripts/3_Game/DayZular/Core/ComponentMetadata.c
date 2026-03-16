// DayZular Framework - Component Metadata
// Stores component configuration (replaces Angular @Component decorator)

class ComponentMetadata
{
    string selector;        // Component selector name
    string templatePath;    // Path to layout file
    ref map<string, string> inputs;   // Map of input name -> property name (e.g., "product" -> "_inputProduct")
    ref map<string, string> outputs;  // Map of output name -> property name (e.g., "onPurchase" -> "_outputOnPurchase")
    
    void ComponentMetadata()
    {
        selector = "";
        templatePath = "";
        inputs = new map<string, string>();
        outputs = new map<string, string>();
    }
}
