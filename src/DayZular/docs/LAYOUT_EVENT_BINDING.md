# DayZular Layout-Driven Event Binding

## Overview

DayZular now supports **layout-driven event binding**, allowing you to specify event handlers directly in layout files instead of writing `OnClick` override code. This feature is inspired by DabsFramework's `Relay_Command` and Angular's template event binding.

## How It Works

### The `reference` Keyword

In Enforce Script, using the `reference` keyword on class members makes them **visible and editable in Workbench's layout editor**. This is how DabsFramework creates custom fields like `Binding_Name` and `Relay_Command`.

```c
class ComponentEventBinding : Managed
{
    // These fields appear in Workbench layout editor!
    reference string OnClick;
    reference string OnRightClick;
    reference string OnDoubleClick;
    reference string OnMouseEnter;
    reference string OnMouseLeave;
    reference string OnFocus;
    reference string OnFocusLost;
    reference string OnChange;
}
```

For simple button events, use the `ButtonBinding` scriptclass instead (see below).

### ComponentEventBinding Class

The `ComponentEventBinding` class is a `Managed` class that:
1. Reads event handler method names from layout `ScriptParamsClass`
2. Intercepts widget events (OnClick, OnDoubleClick, etc.)
3. Dynamically invokes the specified method on the component using `g_Script.CallFunction`
4. Falls back to the component's override methods if no layout handler is specified

## Usage

### 1. In Your Layout File

For buttons, use `scriptclass "ButtonBinding"` which provides `OnClick`, `OnRightClick`, `OnDoubleClick`, `OnMouseEnter`, and `OnMouseLeave` fields:

```
ButtonWidgetClass buy_button {
  scriptclass "ButtonBinding"
  {
    ScriptParamsClass {
      OnClick "OnBuyClick"
      OnRightClick "OnBuyContextMenu"
    }
  }
}
```

> **Note:** DayZ `ButtonWidget.OnClick` fires for the left mouse button only. `OnRightClick` is dispatched through `OnMouseButtonDown` when the right button is pressed.

For other widgets needing event handling, `ComponentEventBinding` is attached automatically by `ComponentBase` during `InitComponent()` to any widget with the appropriate scriptclass.

### 2. In Your Component Class

Simply define the method - no need to override `OnClick`:

```c
class ProductCardComponent extends ComponentBase
{
    static ref ScriptInvoker OnPurchase = new ScriptInvoker();
    ref ProductData _inputProduct;
    
    // Layout calls this method when button is clicked
    void OnBuyClick()
    {
        DZLog().LogDebug("Buy button clicked!");
        OnPurchase.Invoke(this, _inputProduct.price);
    }
}
```

## Available Event Handlers

You can bind any of these events in your layout:

| Layout Field | Widget Event | Method Signature |
|--------------|--------------|------------------|
| `OnClick` | Button click (left) | `void MethodName()` or `bool MethodName(ClickEventArgs args)` |
| `OnRightClick` | Button click (right) | `void MethodName()` or `bool MethodName(ClickEventArgs args)` |
| `OnDoubleClick` | Double click | `void MethodName()` or `bool MethodName(ClickEventArgs args)` |
| `OnMouseEnter` | Mouse enter | `void MethodName()` or `bool MethodName(MouseEventArgs args)` |
| `OnMouseLeave` | Mouse leave | `void MethodName()` or `bool MethodName(MouseEventArgs args)` |
| `OnFocus` | Widget focused | `void MethodName()` or `bool MethodName(MouseEventArgs args)` |
| `OnFocusLost` | Widget lost focus | `void MethodName()` or `bool MethodName(MouseEventArgs args)` |
| `OnChange` | Value changed | `void MethodName()` or `bool MethodName(ChangeEventArgs args)` |

## Event Arguments

If your method needs event details, accept an `EventArgs` parameter:

```cpp
void OnBuyClick(ClickEventArgs args)
{
    Print("Clicked at: " + args.x + ", " + args.y);
    Print("Button: " + args.button);
    Print("Widget: " + args.widget.GetName());
}
```

### EventArgs Classes

- **EventArgs** - Base class with `widget`, `x`, `y`
- **ClickEventArgs** - Adds `button` (mouse button index)
- **MouseEventArgs** - Same as EventArgs
- **ChangeEventArgs** - Adds `finished` (bool)

## Comparison with DabsFramework

### DabsFramework (MVC Pattern)
```
// Layout
ButtonWidgetClass buy_button {
  scriptclass "ViewBinding"
  {
    ScriptParamsClass {
      Relay_Command "OnBuyClickEvent"
    }
  }
}

// Controller
void OnBuyClickEvent()
{
    // Handle click
}
```

### DayZular (Component Pattern)
```
// Layout
ButtonWidgetClass buy_button {
  scriptclass "ButtonBinding"
  {
    ScriptParamsClass {
      OnClick "OnBuyClick"
    }
  }
}

// Component
void OnBuyClick()
{
    // Handle click
}
```

## Benefits

✅ **Declarative** - Layout self-documents event handling
✅ **Less Boilerplate** - No giant `OnClick` switch statements
✅ **Designer-Friendly** - Non-programmers can wire events
✅ **Maintainable** - Easy to see what triggers what
✅ **Scalable** - Works great with many buttons
✅ **Flexible** - Can still override for complex cases

## Hybrid Approach

You can use both layout-driven and override-driven event handling:

```c
class MyComponent extends ComponentBase
{
    // Layout-driven (simple cases)
    void OnBuyClick()
    {
        DZLog().LogDebug("Buy clicked!");
    }
    
    // Override-driven (complex cases needing position/button info)
    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == special_widget && button == MouseState.RIGHT)
        {
            ShowContextMenu(x, y);
            return true;
        }
        return false; // Let ButtonBinding handle other clicks
    }
}
```

## How ComponentBinding is Attached

`ComponentBase` automatically scans all widgets in the component tree during `InitComponent()` and connects event bindings:

```c
// In ComponentBase.InitComponent()
AttachEventBindings(); // Recursively finds and connects event binding widgets
```

## Example: Navigation Component

**Layout:**
```
ButtonWidgetClass browse_button {
  scriptclass "ButtonBinding"
  {
    ScriptParamsClass {
      OnClick "OnBrowseClick"
    }
  }
}
```

**Component:**
```c
class NavigationComponent extends ComponentBase
{
    static ref ScriptInvoker OnNavigate = new ScriptInvoker();
    
    void OnBrowseClick()
    {
        OnNavigate.Invoke(this, "browse");
    }
    
    void OnListItemClick()
    {
        OnNavigate.Invoke(this, "list");
    }
}
```

## Creating Custom Layout Fields

To create your own custom layout fields (like `Binding_Name` in DabsFramework):

1. Create a class extending `ScriptedWidgetEventHandler` or `ScriptedViewBase`
2. Use `reference` keyword for fields you want visible in Workbench
3. Access them via `ScriptParamsClass` in layouts

```c
class MyCustomBinding : Managed
{
    reference string CustomField;  // Visible in Workbench!
    reference bool EnableFeature;  // Also visible!
    reference int MaxValue;        // Also visible!
}
```

Then in layout:
```
WidgetClass my_widget {
  scriptclass "MyCustomBinding"
  {
    ScriptParamsClass {
      CustomField "MyValue"
      EnableFeature 1
      MaxValue 100
    }
  }
}
```

## Notes

- Method names are case-sensitive
- If method not found, a warning is printed to console
- `ButtonBinding` handles click events for buttons
- `ComponentEventBinding` handles general widget events (attached automatically)
- Return `true` from handler to stop event propagation
- Layout-driven handlers are called before component overrides

---

**DayZular Framework v2.0** — By TheDmitri
