# DayZular Framework v2.0

**An Angular-inspired UI component framework for DayZ modding**

Build reactive, data-driven UIs using familiar web development patterns: components, data binding, pipes, services, observables, and form validation.

**Author:** TheDmitri
**Development Note:** Developed using AI assistance orchestrated by the author.
**Version:** 2.0.0  
**License:** See LICENSE.md  
**Dependencies:** None (pure DayZ game scripts)

---

## Philosophy

DayZular brings modern web framework patterns to DayZ modding. Inspired by Angular, React, and Vue, it provides a familiar development experience for those coming from web development backgrounds.

**This framework is NOT intended to replace Community Framework or DabsFramework.** Instead, it represents a personal exploration of framework design and offers an alternative approach focused on UI components and reactive data binding. See [CREDITS.md](CREDITS.md) for acknowledgments and detailed philosophy.

### Why DayZular?

- **Familiar Patterns** - If you know Angular/React, you'll feel at home
- **Zero Dependencies** - No CF or Dabs required (but works alongside them)
- **Reactive UI** - Automatic updates when data changes
- **Type-Safe** - Leverage Enforce Script's type system
- **Production Ready** - Optimized for performance with minimal logging overhead

---

## Features

- **Component System** — Lifecycle hooks, metadata, auto-registration, nested components
- **Property Bindings** — `{{ propertyName }}`, `{{ method() }}`, `{{ nested.property }}`, pipes `{{ value | uppercase }}`
- **Event Bindings** — `(click)="OnButtonClick"` in layout XML
- **ForEach Bindings** — Repeat child components from observable collections
- **Component Bindings** — Dynamic child component instantiation from selectors
- **Pipes** — Transform bound values: `uppercase`, `lowercase`, `number`, `currency`, `percent`, `json`, etc.
- **Services** — Singleton dependency injection via `ServiceRegistry`
- **Observables** — `ObservableInt`, `ObservableFloat`, `ObservableString`, `ObservableBool`, `DZObservableCollection<T>`
- **Form Validation** — `FormControl`, `FormGroup`, `FormBuilder`, built-in validators (required, min, max, range, pattern, email)
- **Logging** — `DZLog()` singleton with log levels (Debug, Info, Warn, Error) and optional file output

---

## Quick Start

### 1. Create a Component

```c
class MyComponent extends ComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("my-component", MyComponent);

    string title = "Hello DayZular!";
    int counter = 0;

    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "my-component";
        m_Metadata.templatePath = "MyMod/gui/layouts/my_component.layout";
    }

    override void OnInit()
    {
        DZLog().LogInfo("MyComponent initialized");
    }

    void OnButtonClick()
    {
        counter++;
        MarkForCheck();
    }

    string GetCounterDisplay()
    {
        return "Count: " + counter.ToString();
    }
}
```

### 2. Create a Layout (XML)

```xml
TextWidgetClass title_label {
    text "{{ title }}"
}
TextWidgetClass counter_label {
    text "{{ GetCounterDisplay() }}"
}
ButtonWidgetClass my_button {
    scriptclass "EventBinding"
    {
        ScriptParamsClass {
            Event_Click "OnButtonClick"
        }
    }
}
```

### 3. Show a Menu

```c
// In your mission or mod code:
ComponentMenu menu = new ComponentMenu();
menu.SetUseMouse(true);

MyComponent comp = new MyComponent();
comp.InitComponent(null);
menu.SetRootComponent(comp);
menu.ShowMenu();
```

---

## Bindings

| Syntax | Description |
|--------|-------------|
| `{{ property }}` | One-way text binding |
| `{{ method() }}` | Method call binding |
| `{{ value \| pipe }}` | Pipe transform |
| `{{ obj.nested }}` | Nested property access |
| `(click)="Handler"` | Event binding |
| `Component_Selector` | Child component binding |
| `Component_ForEach` | ForEach repeater binding |

---

## Built-in Pipes

`uppercase`, `lowercase`, `capitalize`, `number`, `currency`, `percent`, `eur`, `usd`, `default`, `json`

---

## Logging

```c
DZLog().LogDebug("verbose detail");
DZLog().LogInfo("general info");
DZLog().LogWarning("something unexpected");
DZLog().LogError("critical failure");

// Configure
DZLog().SetLogLevel(DayZularLogLevel.Debug);
DZLog().EnableFileLogging(); // writes to $profile:DayZular/Logging/Logs/
```

---

## Form Validation

```c
FormGroup form = new FormGroup("myForm");
form.AddControl("quantity", FormBuilder.Integer("quantity", "1", 1, 999));
form.AddControl("price", FormBuilder.Integer("price", "100", 1, 999999));

if (form.Validate())
{
    map<string, string> values = form.GetValues();
}
```

Layout integration with `EditBoxBinding`:
```xml
EditBoxWidgetClass price_input {
    scriptclass "EditBoxBinding"
    {
        ScriptParamsClass {
            Validators "required,integer,min:1,max:999999"
            Form_Control_Name "price"
        }
    }
}
```

---

## Services

```c
// Register
ServiceRegistry.GetInstance().RegisterService(MyService, new MyService());

// Retrieve
MyService svc = MyService.Cast(ServiceRegistry.GetInstance().GetService(MyService));
```

---

## Project Structure

```
DayZular/
├── config.cpp                  # Mod metadata
├── scripts/
│   ├── Common/                 # Shared preprocessor defines
│   ├── 3_Game/
│   │   ├── Constants/          # Path constants
│   │   └── DayZular/
│   │       ├── Bindings/       # Property, ForEach, Component, Widget bindings
│   │       ├── Core/           # ComponentBase, ComponentMenu, ComponentRegistry, AppModule
│   │       ├── Forms/          # FormControl, FormGroup, FormBuilder, Validators
│   │       ├── Observables/    # Observable types and collections
│   │       ├── Pipes/          # Built-in pipes and PipeRegistry
│   │       └── Services/       # ServiceRegistry, LoggerService
│   ├── 4_World/                # Logger module placeholders
│   └── 5_Mission/              # Mission bootstrap (client + server)
└── docs/                       # Documentation
```

---

## Documentation

- **[FEATURES.md](FEATURES.md)** - Complete feature guide with examples
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Technical architecture and design patterns
- **[CONFIGURATION.md](CONFIGURATION.md)** - Configuration and production setup
- **[QUICKSTART_SERVICES.md](QUICKSTART_SERVICES.md)** - Service system quick start
- **[CREDITS.md](CREDITS.md)** - Acknowledgments and framework philosophy

---

## Companion Mod: DayZular_Examples

A separate `DayZular_Examples` mod will be available soon demonstrating all framework features. Load it alongside DayZular to see the framework in action.

---

## Credits

DayZular was inspired by and built upon knowledge gained from:

- **[Community Framework (CF)](https://github.com/Arkensor/DayZ-CommunityFramework)** - Module system, RPC patterns, logging infrastructure
- **[DabsFramework](https://github.com/InclementDab/DayZ-Dabs-Framework)** - MVC architecture, reflection, property binding techniques

Special thanks to **InclementDab** and the **CF Team** for their groundbreaking work in the DayZ modding community. See [CREDITS.md](CREDITS.md) for detailed acknowledgments.

---

## Contributing

Contributions, bug reports, and feature requests are welcome! Please:
- Report issues on GitHub
- Follow existing code style
- Test changes thoroughly
- Update documentation as needed

---

## License

See [LICENSE.md](LICENSE.md) for details.

---

**Built with ❤️ for the DayZ modding community**
