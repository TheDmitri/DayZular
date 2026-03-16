# DayZular Framework v2.0

An **Angular-inspired UI component framework** for DayZ modding. Build reactive, data-driven UIs using familiar patterns: **components, bindings, pipes, services, observables, and form validation**.

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
- **Logging** — `DZLog()` singleton with log levels and optional file output

---

## Quick Example

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

    void OnButtonClick()
    {
        counter++;
        MarkForCheck();
    }
}
```

```xml
<TextWidgetClass title_label {
    text "{{ title }}"
}
<ButtonWidgetClass my_button {
    scriptclass "EventBinding"
    {
        ScriptParamsClass {
            Event_Click "OnButtonClick"
        }
    }
}
```

---

## Installation

1. Subscribe to this mod
2. Add `DayZular` as a dependency in your mod's `config.cpp`
3. Start building reactive UIs!

---

## Documentation & Examples

**GitHub Repository:** https://github.com/TheDmitri/DayZular  
**Example Mod:** Search for `DayZular_Examples` on the Workshop (coming soon eventually)

---

## Dependencies

- Game
- World
- Mission
- DZ_Data

**No external mod dependencies required.**

---

## License

Open source on GitHub under custom license. **No republishing on Steam Workshop.**
See LICENSE file in the repository for full terms.

---

## Credits

**Author:** TheDmitri  
**Version:** 2.0
