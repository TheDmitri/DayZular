# DayZular Framework v2.0 — API Reference

Detailed API documentation for the DayZular framework. For a quick-start guide, see the root `README.md`.

---

## ComponentBase

Base class for all DayZular components. Handles lifecycle, widget management, and binding updates.

### Overridable Methods

| Method | Description |
|--------|-------------|
| `void RegisterMetadata()` | Set `m_Metadata.selector` and `m_Metadata.templatePath` |
| `void LoadChildWidgets()` | Call `AutoLoadWidgets()` here to auto-bind widgets by name |
| `void OnInit()` | Called after component is fully initialized |
| `void OnDestroy()` | Called before component is destroyed |
| `void OnChanges()` | Called when input properties change |

### Public Methods

| Method | Description |
|--------|-------------|
| `void InitComponent(Widget root)` | Initialize the component with an optional root widget |
| `void MarkForCheck()` | Trigger UI update (re-evaluate all bindings) |
| `void AutoLoadWidgets()` | Auto-assign widget member variables by matching widget names |
| `Widget GetRoot()` | Get the root widget |
| `void Destroy()` | Destroy the component and its widget tree |

### Auto-Registration

Register components so they can be instantiated by selector in `ComponentBinding` and `ForEachBinding`:

```c
class MyComponent extends ComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("my-component", MyComponent);
    // ...
}
```

### Input Properties

Properties prefixed with `_input` are automatically detected as inputs and set by `ForEachBinding`:

```c
class ProductCardComponent extends ComponentBase
{
    // Auto-detected input — ForEachBinding sets this from the collection item
    ref ProductData _inputProduct;
}
```

### Events (Static ScriptInvoker Pattern)

Use static `ScriptInvoker` for parent-child communication:

```c
class ProductCardComponent extends ComponentBase
{
    static ref ScriptInvoker OnPurchase = new ScriptInvoker();

    void OnBuyClick()
    {
        OnPurchase.Invoke(this, _inputProduct.price);
    }
}

// Parent subscribes directly
class ShopComponent extends ComponentBase
{
    override void OnInit()
    {
        ProductCardComponent.OnPurchase.Insert(OnProductPurchased);
    }

    void OnProductPurchased(ProductCardComponent card, int price)
    {
        DZLog().LogInfo("Purchased for: " + price);
    }
}
```

---

## Layout-Driven Bindings (scriptclass)

Bindings are declared in layout XML via `scriptclass`. No manual `CreateBindings()` needed.

### Text Binding

```xml
TextWidgetClass my_label {
    scriptclass "TextBinding"
    {
        ScriptParamsClass {
            Binding_Expression "{{ propertyName }}"
        }
    }
}
```

### Image Binding

```xml
ImageWidgetClass my_image {
    scriptclass "ImageBinding"
    {
        ScriptParamsClass {
            Binding_Expression "{{ imagePath }}"
        }
    }
}
```

### Progress Bar Binding

```xml
ProgressBarWidgetClass health_bar {
    scriptclass "ProgressBinding"
    {
        ScriptParamsClass {
            Binding_Expression "{{ healthPercent }}"
        }
    }
}
```

### Button Binding (Events)

```xml
ButtonWidgetClass buy_button {
    scriptclass "ButtonBinding"
    {
        ScriptParamsClass {
            OnClick "OnBuyClick"
        }
    }
}
```

### EditBox Binding (Data + Validation)

```xml
EditBoxWidgetClass price_input {
    scriptclass "EditBoxBinding"
    {
        ScriptParamsClass {
            Binding_Expression "{{ _inputPrice }}"
            Two_Way_Binding 1
            Validators "required,integer,min:1,max:999999"
            Form_Control_Name "price"
        }
    }
}
```

### Component Binding (Child Component)

```xml
FrameWidgetClass child_container {
    scriptclass "ComponentWidgetBinding"
    {
        ScriptParamsClass {
            Component_Selector "{{ currentPageSelector }}"
        }
    }
}
```

### ForEach Binding (Repeater)

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

### Other Widget Bindings

- **ComboBoxBinding** — `Binding_Items`, `Binding_Selection`, `OnSelectionChange`
- **TextListboxBinding** — `Binding_Items`, `Binding_Selection`, `OnSelectionChange`
- **CheckBoxBinding** — `Binding_Checked`, `OnCheckChanged`
- **SliderBinding** — `Binding_Value`, `OnSliderChange`
- **ItemPreviewBinding** — `Binding_Item`
- **PlayerPreviewBinding** — `Binding_Player`

---

## Binding Expressions

| Syntax | Example | Description |
|--------|---------|-------------|
| `{{ property }}` | `{{ title }}` | Read a component property |
| `{{ method() }}` | `{{ GetPriceDisplay() }}` | Call a component method |
| `{{ obj.nested }}` | `{{ _inputProduct.name }}` | Nested property access |
| `{{ value \| pipe }}` | `{{ price \| currency }}` | Pipe transform |
| `{{ value \| pipe:arg }}` | `{{ text \| truncate:20 }}` | Pipe with arguments |

---

## Pipes (DZPipe)

Base class is `DZPipe`. Register custom pipes via `PipeRegistry`.

### Built-in Pipes (11)

| Pipe | Description | Example |
|------|-------------|---------|
| `uppercase` | Convert to UPPERCASE | `{{ name \| uppercase }}` |
| `lowercase` | Convert to lowercase | `{{ name \| lowercase }}` |
| `number` | Format number with decimals | `{{ value \| number:2 }}` |
| `percent` | Format as percentage | `{{ ratio \| percent }}` |
| `eur` | Format as EUR currency | `{{ price \| eur }}` |
| `usd` | Format as USD currency | `{{ price \| usd }}` |
| `slice` | Substring | `{{ text \| slice:0:10 }}` |
| `truncate` | Limit length with ellipsis | `{{ text \| truncate:20 }}` |
| `replace` | Replace text | `{{ text \| replace:old:new }}` |
| `default` | Fallback if empty | `{{ value \| default:N/A }}` |
| `json` | Format as JSON string | `{{ data \| json }}` |

### Custom Pipe

```c
class MyPipe extends DZPipe
{
    override string Transform(string value, array<string> args)
    {
        return value + " custom";
    }
}

PipeRegistry.RegisterPipe("myPipe", new MyPipe());
```

---

## Services

### ServiceRegistry

```c
// Register
ServiceRegistry.GetInstance().RegisterService(MyService, new MyService());

// Retrieve
MyService svc = MyService.Cast(ServiceRegistry.GetInstance().GetService(MyService));

// Check
bool exists = ServiceRegistry.GetInstance().HasService(MyService);
```

---

## Observables

| Type | Description |
|------|-------------|
| `ObservableInt` | Reactive integer |
| `ObservableFloat` | Reactive float |
| `ObservableString` | Reactive string |
| `ObservableBool` | Reactive boolean |
| `DZObservableCollection<T>` | Reactive collection with add/remove/change notifications |

### Usage

```c
ref ObservableInt health = new ObservableInt(100);
health.GetInvoker().Insert(OnHealthChanged);
health.Next(75);

void OnHealthChanged(int newValue)
{
    DZLog().LogDebug("Health: " + newValue);
}
```

### Observable Collection

```c
ref DZObservableCollection<string> items = new DZObservableCollection<string>(this);
items.GetInvoker().Insert(OnItemsChanged);
items.Insert("Item 1");
items.Remove(0);
items.Clear();
```

**Operations:** `Insert`, `InsertAt`, `Remove`, `RemoveItem`, `Set`, `SwapItems`, `Clear`, `Get`, `Count`, `Find`

---

## Form Validation

### FormControl & FormGroup

```c
FormGroup form = new FormGroup("myForm");
form.AddControl("quantity", FormBuilder.Integer("quantity", "1", 1, 999));
form.AddControl("price", FormBuilder.Integer("price", "100", 1, 999999));

form.MarkAllAsTouched();
if (form.Validate())
{
    map<string, string> values = form.GetValues();
}
```

### Built-in Validators

`required`, `min`, `max`, `range`, `minLength`, `maxLength`, `pattern` (numeric, integer, alpha, alphanumeric), `email`

### Layout Integration

Validators are parsed from a comma-separated string in `EditBoxBinding`:

```xml
EditBoxWidgetClass age_input {
    scriptclass "EditBoxBinding"
    {
        ScriptParamsClass {
            Validators "required,integer,min:1,max:120"
            Form_Control_Name "age"
        }
    }
}
```

---

## Logging (DZLog)

```c
DZLog().LogDebug("verbose detail");
DZLog().LogInfo("general info");
DZLog().LogWarning("something unexpected");
DZLog().LogError("critical failure");

DZLog().SetLogLevel(DayZularLogLevel.Debug);
DZLog().EnableFileLogging();  // $profile:DayZular/Logging/Logs/
```

---

## Menu System (ComponentMenu)

```c
ComponentMenu menu = new ComponentMenu();
menu.SetUseMouse(true);
menu.SetUseKeyboard(false);

MyComponent comp = new MyComponent();
comp.InitComponent(null);
menu.SetRootComponent(comp);

menu.ShowMenu();
menu.HideMenu();
menu.ToggleMenu();
```

---

## Comparison with Angular

| Angular | DayZular |
|---------|----------|
| `@Component()` | `RegisterMetadata()` + `ComponentAutoRegister` |
| `@Input()` | `_input` prefix auto-detection |
| `@Output()` | Static `ScriptInvoker` |
| `@Injectable()` | `ServiceRegistry` |
| `ngOnInit` | `OnInit()` |
| `ngOnDestroy` | `OnDestroy()` |
| `ngOnChanges` | `OnChanges()` |
| Template interpolation | `{{ expression }}` in layout `Binding_Expression` |
| Pipes | `DZPipe` classes + `PipeRegistry` |
| Observables | `ObservableInt/Float/String/Bool`, `DZObservableCollection<T>` |
| Reactive Forms | `FormControl`, `FormGroup`, `FormBuilder` |

---

## Examples

Examples are in the separate **DayZular_Examples** companion mod. See its `scripts/3_Game/Examples/` folder for:
- **BarterTraderMenuComponent** — Full multi-page UI with navigation
- **ProductCardComponent** — Input auto-detection, static ScriptInvoker
- **ListItemPageComponent** — Form validation integration
- **CounterComponent** — Simple state management
- **HealthBarComponent** — Progress bar bindings
- **ItemListComponent** — Observable collection usage

---

**DayZular Framework v2.0** — By TheDmitri
