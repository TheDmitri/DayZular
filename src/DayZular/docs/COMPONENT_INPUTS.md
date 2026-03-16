# DayZular Component Inputs Pattern

## Overview

DayZular supports Angular-style `@Input` properties for passing data to child components. The **preferred pattern** is using the `_input` prefix for auto-detection by `ForEachBinding`. You can also pass data objects via setter methods.

## Pattern Comparison

### ❌ Old Pattern (Multiple Setters)

```cpp
// Parent Component
ProductCardComponent card = new ProductCardComponent();
card.InitComponent(null);

// Multiple setter calls - verbose and error-prone
card.SetProductName(product.name);
card.SetProductDescription(product.description);
card.SetProductPrice(product.price);
card.SetImagePath(product.imagePath);
```

```cpp
// Child Component
class ProductCardComponent extends ComponentBase
{
    protected string m_ProductName;
    protected string m_ProductDescription;
    protected int m_ProductPrice;
    protected string m_ImagePath;
    
    void SetProductName(string name) { m_ProductName = name; }
    void SetProductDescription(string desc) { m_ProductDescription = desc; }
    void SetProductPrice(int price) { m_ProductPrice = price; }
    void SetImagePath(string path) { m_ImagePath = path; }
}
```

### ✅ Preferred Pattern (`_input` Prefix Auto-Detection)

Properties prefixed with `_input` are automatically detected by `ForEachBinding` and set from collection items:

```c
class ProductCardComponent extends ComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("product-card", ProductCardComponent);
    
    // Auto-detected input — ForEachBinding sets this from the collection item
    ref ProductData _inputProduct;
    
    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "product-card";
        m_Metadata.templatePath = "MyMod/gui/layouts/product_card.layout";
    }
    
    string GetProductName()
    {
        if (_inputProduct)
            return _inputProduct.name;
        return "Unknown";
    }
    
    string GetPriceDisplay()
    {
        if (_inputProduct)
            return _inputProduct.price.ToString() + " TC";
        return "0 TC";
    }
}
```

The parent uses `ForEachBinding` in the layout to repeat child components:

```xml
WrapSpacerWidgetClass products_list {
    scriptclass "ComponentWidgetBinding"
    {
        ScriptParamsClass {
            Component_ForEach "{{ _inputProducts }}"
            Component_Template "product-card"
            Component_ItemProperty "_inputProduct"
        }
    }
}
```

### Alternative: Manual Setter Pattern (Input Object)

```c
class ProductCardComponent extends ComponentBase
{
    protected ref ProductData m_ProductData;
    
    void SetProductData(ProductData data)
    {
        m_ProductData = data;
        OnChanges();
        MarkForCheck();
    }
    
    string GetProductName()
    {
        if (m_ProductData)
            return m_ProductData.name;
        return "Unknown";
    }
}
```

## Benefits

### 1. **Single Responsibility**
The component receives one cohesive data object instead of scattered properties.

### 2. **Type Safety**
The data structure is defined once in a class, ensuring consistency.

### 3. **Easier Refactoring**
Add/remove fields in `ProductData` without changing component interface.

### 4. **Less Boilerplate**
One setter instead of many.

### 5. **Angular-Like**
Matches Angular's `@Input()` decorator pattern:

```typescript
// Angular
@Component({...})
export class ProductCardComponent {
  @Input() product: ProductData;
}
```

```cpp
// DayZular
class ProductCardComponent extends ComponentBase
{
    protected ref ProductData m_ProductData;
    
    void SetProductData(ProductData data)
    {
        m_ProductData = data;
        OnChanges();
        MarkForCheck();
    }
}
```

## Data Object Pattern

### Define Your Data Class

```cpp
class ProductData
{
    string name;
    string description;
    int price;
    string imagePath;
    
    void ProductData(string _name, string _desc, int _price, string _image = "")
    {
        name = _name;
        description = _desc;
        price = _price;
        imagePath = _image;
    }
}
```

### Component Receives Data Object

```cpp
class ProductCardComponent extends ComponentBase
{
    protected ref ProductData m_ProductData;
    
    void SetProductData(ProductData data)
    {
        m_ProductData = data;
        OnChanges();
        MarkForCheck();
    }
    
    ProductData GetProductData()
    {
        return m_ProductData;
    }
}
```

### Parent Passes Data Object

With the `_input` prefix pattern, `ForEachBinding` handles this automatically. For manual creation:

```c
class BrowseListingPageComponent extends ComponentBase
{
    void RenderProductCards()
    {
        // Subscribe to static ScriptInvoker
        ProductCardComponent.OnPurchase.Insert(OnProductPurchased);
        
        for (int i = 0; i < m_Products.Count(); i++)
        {
            ProductData product = m_Products.Get(i);
            
            ProductCardComponent card = new ProductCardComponent();
            card.InitComponent(null);
            card.SetProductData(product);
            
            products_container.AddChild(card.GetRoot());
        }
    }
    
    void OnProductPurchased(ProductCardComponent card, int price)
    {
        DZLog().LogInfo("Purchased for: " + price);
    }
}
```

## Binding to Input Data

Use getter methods or nested property access in layout-driven bindings:

```xml
<!-- Direct nested property access -->
TextWidgetClass product_name {
    scriptclass "TextBinding"
    {
        ScriptParamsClass {
            Binding_Expression "{{ _inputProduct.name }}"
        }
    }
}

<!-- Via getter method -->
TextWidgetClass product_price {
    scriptclass "TextBinding"
    {
        ScriptParamsClass {
            Binding_Expression "{{ GetPriceDisplay() }}"
        }
    }
}
```

Getter methods access input data:
```c
string GetPriceDisplay()
{
    if (_inputProduct)
        return _inputProduct.price.ToString() + " TC";
    return "0 TC";
}
```

## Multiple Inputs

Components can have multiple input objects:

```cpp
class OrderCardComponent extends ComponentBase
{
    protected ref OrderData m_Order;
    protected ref UserData m_Seller;
    protected ref UserData m_Buyer;
    
    void SetOrder(OrderData order)
    {
        m_Order = order;
        OnChanges();
        MarkForCheck();
    }
    
    void SetSeller(UserData seller)
    {
        m_Seller = seller;
        OnChanges();
        MarkForCheck();
    }
    
    void SetBuyer(UserData buyer)
    {
        m_Buyer = buyer;
        OnChanges();
        MarkForCheck();
    }
}
```

## Change Detection

Always call `OnChanges()` and `MarkForCheck()` when input data changes:

```cpp
void SetProductData(ProductData data)
{
    m_ProductData = data;
    OnChanges();     // Trigger lifecycle hook
    MarkForCheck();  // Update all bindings
}
```

This ensures:
- Bindings are updated with new data
- Lifecycle hooks are triggered
- UI reflects the new state

## Best Practices

### ✅ DO

- Use data objects for related properties
- Call `OnChanges()` and `MarkForCheck()` in setters
- Provide null-safe getters
- Use descriptive data class names

### ❌ DON'T

- Mix input objects with individual setters
- Forget to call `MarkForCheck()` after setting input
- Mutate input data directly (create new objects instead)
- Use primitive types when an object makes sense

## Example: Complete Component (v2.0)

```c
class ProductData
{
    string name;
    string description;
    int price;
    
    void ProductData(string _name, string _desc, int _price)
    {
        name = _name;
        description = _desc;
        price = _price;
    }
}

class ProductCardComponent extends ComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("product-card", ProductCardComponent);
    
    // Static ScriptInvoker for parent communication
    static ref ScriptInvoker OnPurchase = new ScriptInvoker();
    
    // Auto-detected input
    ref ProductData _inputProduct;
    
    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "product-card";
        m_Metadata.templatePath = "MyMod/gui/layouts/product_card.layout";
    }
    
    override void OnInit()
    {
        DZLog().LogDebug("ProductCard initialized: " + _inputProduct.name);
    }
    
    string GetPriceDisplay()
    {
        if (_inputProduct)
            return _inputProduct.price.ToString() + " TC";
        return "0 TC";
    }
    
    void OnBuyClick()
    {
        if (_inputProduct)
            OnPurchase.Invoke(this, _inputProduct.price);
    }
}
```

---

**DayZular Framework v2.0** — By TheDmitri
