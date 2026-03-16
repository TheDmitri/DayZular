# DayZular Framework v2.0 - Summary

## Framework Structure

### Core (`scripts/3_Game/DayZular/Core/`)
- **ComponentBase.c** — Base class for all components (lifecycle, widgets, bindings)
- **ComponentMetadata.c** — Component selector and template path configuration
- **ComponentRegistry.c** — Registry for component types + `ComponentAutoRegister` helper
- **ComponentMenu.c** — DayZ UIManager integration for showing/hiding component menus
- **ComponentEventBinding.c** — Layout-driven event binding (`OnClick`, `OnDoubleClick`, etc.)
- **AppModule.c** — Framework bootstrap (registers pipes, logger service)

### Bindings (`scripts/3_Game/DayZular/Bindings/`)
- **PropertyBinding.c** — Base binding class with expression evaluation, pipe support, nested properties
- **PropertyBindingImpl.c** — Concrete binding implementations (Text, Image, Progress, ItemPreview, PlayerPreview)
- **ForEachBinding.c** — Repeater binding that creates child components from collections
- **ComponentBinding.c** — Dynamic child component instantiation from selector expressions
- **WidgetBindings.c** — Layout-driven widget bindings (TextBinding, ImageBinding, ProgressBinding, ButtonBinding, EditBoxBinding, ComboBoxBinding, TextListboxBinding, CheckBoxBinding, SliderBinding, ItemPreviewBinding, PlayerPreviewBinding)
- **WidgetBindings_Component.c** — Layout-driven ComponentWidgetBinding (ForEach + Component selector)

### Pipes (`scripts/3_Game/DayZular/Pipes/`)
- **Pipe.c** — Base `DZPipe` class + `PipeRegistry` + 11 built-in pipes (uppercase, lowercase, number, percent, eur, usd, slice, truncate, replace, default, json)

### Services (`scripts/3_Game/DayZular/Services/`)
- **Service.c** — Base `Service` class + `ServiceRegistry` singleton
- **LoggerService.c** — `DayZularLogger` singleton with log levels and optional file output. Access via `DZLog()`

### Forms (`scripts/3_Game/DayZular/Forms/`)
- **ValidationError.c** — Error representation
- **FormValidator.c** — Built-in validators (Required, Min, Max, Range, MinLength, MaxLength, Pattern, Email)
- **FormControl.c** — Individual field with validation and state tracking (pristine/dirty/touched/valid)
- **FormGroup.c** — Collection of controls with group-level validation
- **FormBuilder.c** — Helper for creating controls with fluent API
- **AsyncValidator.c** — Base class for server-side/delayed validation

### Observables (`scripts/3_Game/DayZular/Observables/`)
- **Observable.c** — `ObservableInt`, `ObservableFloat`, `ObservableString`, `ObservableBool`
- **ObservableCollection.c** — `DZObservableCollection<T>` with change notifications

### Helpers (`scripts/3_Game/DayZular/Helpers/`)
- **Ternary.c** — Ternary helper utilities

### Constants (`scripts/3_Game/Constants/`)
- **Constants.c** — File path constants for logging

### Mission (`scripts/5_Mission/`)
- **MissionGameplay.c** — Client-side bootstrap (`AppModule.Bootstrap()` / `Shutdown()`)
- **MissionServer.c** — Server-side bootstrap

---

## Key Patterns

### Component Creation
```c
class MyComponent extends ComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("my-selector", MyComponent);

    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "my-selector";
        m_Metadata.templatePath = "MyMod/gui/layouts/my_component.layout";
    }

    override void OnInit()
    {
        DZLog().LogInfo("MyComponent initialized");
    }
}
```

### Input Properties (`_input` prefix auto-detection)
```c
ref ProductData _inputProduct;  // Set automatically by ForEachBinding
```

### Events (Static ScriptInvoker)
```c
static ref ScriptInvoker OnPurchase = new ScriptInvoker();

void OnBuyClick()
{
    OnPurchase.Invoke(this, _inputProduct.price);
}
```

### Layout-Driven Bindings (no manual CreateBindings needed)
```xml
TextWidgetClass label { scriptclass "TextBinding" { ScriptParamsClass { Binding_Expression "{{ title }}" } } }
ButtonWidgetClass btn { scriptclass "ButtonBinding" { ScriptParamsClass { OnClick "OnBuyClick" } } }
```

---

## How to Test

1. Load **DayZular** (framework) and **DayZular_Examples** (companion mod - coming soon)
2. Examples will demonstrate: components, bindings, ForEach, navigation, form validation
3. See the `examples/` directory in the framework for code examples

---

## Extending the Framework

### Custom Pipe
```c
class MyPipe extends DZPipe
{
    override string Transform(string value, array<string> args) { return value.ToUpper(); }
}
PipeRegistry.RegisterPipe("myPipe", new MyPipe());
```

### Custom Service
```c
class MyService extends Service { /* ... */ }
ServiceRegistry.GetInstance().RegisterService(MyService, new MyService());
```

---

## Troubleshooting

- **Component not initializing?** — Check `RegisterMetadata()` templatePath, verify layout file exists
- **Bindings not updating?** — Call `MarkForCheck()` after state changes
- **Widget not found?** — Widget name must match variable name, call `AutoLoadWidgets()` in `LoadChildWidgets()`
- **Events not firing?** — Subscribe in `OnInit()`, use static ScriptInvoker pattern

---

## Version History

**v2.0** — Framework refactor for Steam Workshop publication
- Removed CommunityFramework dependency
- Standalone `DayZularLogger` singleton with log levels
- All `Print()` replaced with `DZLog()` log-level calls
- Layout-driven bindings (no manual `CreateBindings()`)
- Form validation system (FormControl, FormGroup, FormBuilder)
- 11 built-in pipes
- Examples moved to separate `DayZular_Examples` companion mod
- ComponentOutput removed in favor of static ScriptInvoker pattern

**v1.0** — Initial release
- Core framework, binding system, pipes, services, observables

---

**DayZular Framework v2.0** — By TheDmitri
