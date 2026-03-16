// DayZular Framework - Component Menu System
// Integrates with DayZ's UIManager for proper menu handling with mouse/keyboard

// UIScriptedMenu wrapper for DayZular components
class UIComponentMenu: UIScriptedMenu
{
    protected ComponentMenu m_ComponentMenu;
    
    void UIComponentMenu(ComponentMenu componentMenu)
    {
        m_ComponentMenu = componentMenu;
    }
    
    override Widget Init()
    {
        if (m_ComponentMenu)
        {
            return m_ComponentMenu.GetRoot();
        }
        return null;
    }
    
    override Widget GetLayoutRoot()
    {
        if (m_ComponentMenu)
        {
            return m_ComponentMenu.GetRoot();
        }
        return null;
    }
    
    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (m_ComponentMenu && m_ComponentMenu.GetRootComponent())
        {
            return m_ComponentMenu.GetRootComponent().OnClick(w, x, y, button);
        }
        return false;
    }
    
    override bool OnMouseEnter(Widget w, int x, int y)
    {
        if (m_ComponentMenu && m_ComponentMenu.GetRootComponent())
        {
            return m_ComponentMenu.GetRootComponent().OnMouseEnter(w, x, y);
        }
        return false;
    }
    
    override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
    {
        if (m_ComponentMenu && m_ComponentMenu.GetRootComponent())
        {
            return m_ComponentMenu.GetRootComponent().OnMouseLeave(w, enterW, x, y);
        }
        return false;
    }
    
    override bool UseMouse()
    {
        if (m_ComponentMenu)
        {
            return m_ComponentMenu.UseMouse();
        }
        return true;
    }
    
    override bool UseKeyboard()
    {
        if (m_ComponentMenu)
        {
            return m_ComponentMenu.UseKeyboard();
        }
        return false;
    }
    
    ComponentMenu GetComponentMenu()
    {
        return m_ComponentMenu;
    }
}

// Component-based menu system
class ComponentMenu
{
    static ref map<UIScriptedMenu, ComponentMenu> s_AllMenus = new map<UIScriptedMenu, ComponentMenu>();
    
    protected ref UIComponentMenu m_UIMenu;
    protected ref DZlrComponentBase m_RootComponent;
    protected bool m_UseMouse = true;
    protected bool m_UseKeyboard = false;
    protected bool m_IsShown = false;
    
    void ComponentMenu()
    {
        m_UIMenu = new UIComponentMenu(this);
        
        if (!s_AllMenus)
        {
            s_AllMenus = new map<UIScriptedMenu, ComponentMenu>();
        }
        s_AllMenus.Set(m_UIMenu, this);
        
        Print("ComponentMenu created");
    }
    
    void ~ComponentMenu()
    {
        Print("ComponentMenu destructor called");
        
        // Cleanup
        if (m_UIMenu)
        {
            s_AllMenus.Remove(m_UIMenu);
            HideMenu();
        }
        
        if (m_RootComponent)
        {
            delete m_RootComponent;
        }
    }
    
    // Set the root component for this menu
    void SetRootComponent(DZlrComponentBase component)
    {
        m_RootComponent = component;
        
        // Hide the widget by default until ShowMenu is called
        if (m_RootComponent && m_RootComponent.GetRoot())
        {
            m_RootComponent.GetRoot().Show(false);
        }
        
        // If menu is already shown, show the new component immediately
        if (m_IsShown && m_RootComponent && m_RootComponent.GetRoot())
        {
            m_RootComponent.GetRoot().Show(true);
            m_RootComponent.GetRoot().Update();
        }
    }
    
    // Show the menu using UIManager
    void ShowMenu()
    {
        if (!m_UIMenu || !m_RootComponent)
        {
            DZLog().LogError("Cannot show menu - UIMenu or RootComponent is null");
            return;
        }
        
        if (m_IsShown)
        {
            if (m_RootComponent.GetRoot())
            {
                m_RootComponent.GetRoot().Show(true);
            }
            return;
        }
        
        Print("Showing menu");
        
        // Disable player control (freeze player)
        GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
        
        // Hide HUD
        GetGame().GetMission().GetHud().Show(false);
        
        // Optional: Add blur effect
        PPEffects.SetBlurMenu(0.5);
        
        // Show through UIManager
        g_Game.GetUIManager().ShowScriptedMenu(m_UIMenu, g_Game.GetUIManager().GetMenu());
        
        // Handle mouse cursor
        if (m_UseMouse)
        {
            g_Game.GetInput().ChangeGameFocus(1, INPUT_DEVICE_MOUSE);
            GetGame().GetUIManager().ShowUICursor(true);
        }
        
        // Handle keyboard
        if (m_UseKeyboard)
        {
            g_Game.GetInput().ChangeGameFocus(1, INPUT_DEVICE_KEYBOARD);
        }
        
        // Show the root widget
        if (m_RootComponent.GetRoot())
        {
            m_RootComponent.GetRoot().Show(true);
        }
        
        m_IsShown = true;
        
        // Call later to ensure cursor is shown (like DabsFramework)
        g_Game.GetCallQueue(CALL_CATEGORY_GUI).CallLater(GetGame().GetMission().RefreshCrosshairVisibility, 2, false);
    }
    
    // Hide the menu
    void HideMenu()
    {
        if (!m_UIMenu || !m_IsShown)
        {
            return;
        }
        
        Print("Hiding menu");
        
        // Hide the root widget
        if (m_RootComponent && m_RootComponent.GetRoot())
        {
            m_RootComponent.GetRoot().Show(false);
        }
        
        // Hide through UIManager
        g_Game.GetUIManager().HideScriptedMenu(m_UIMenu);
        
        // Re-enable player control
        GetGame().GetMission().PlayerControlEnable(false);
        
        // Show HUD
        GetGame().GetMission().GetHud().Show(true);
        
        // Remove blur effect
        PPEffects.SetBlurMenu(0.0);
        
        // Restore input focus
        if (m_UseMouse)
        {
            g_Game.GetInput().ChangeGameFocus(-1, INPUT_DEVICE_MOUSE);
            GetGame().GetUIManager().ShowUICursor(false);
        }
        
        if (m_UseKeyboard)
        {
            g_Game.GetInput().ChangeGameFocus(-1, INPUT_DEVICE_KEYBOARD);
        }
        
        // Refresh crosshair
        g_Game.GetCallQueue(CALL_CATEGORY_GUI).CallLater(GetGame().GetMission().RefreshCrosshairVisibility, 0, false);
        
        m_IsShown = false;
    }
    
    // Toggle menu visibility
    void ToggleMenu()
    {
        if (IsVisible())
        {
            HideMenu();
        }
        else
        {
            ShowMenu();
        }
    }
    
    // Check if menu is visible
    bool IsVisible()
    {
        return m_IsShown;
    }
    
    // Close menu (with cleanup)
    void Close()
    {
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(CloseInternal, 0, false);
    }
    
    private void CloseInternal()
    {
        if (CanClose())
        {
            delete this;
        }
    }
    
    // Override to control close behavior
    bool CanClose()
    {
        return true;
    }
    
    // Configuration
    void SetUseMouse(bool useMouse)
    {
        m_UseMouse = useMouse;
    }
    
    void SetUseKeyboard(bool useKeyboard)
    {
        m_UseKeyboard = useKeyboard;
    }
    
    bool UseMouse()
    {
        return m_UseMouse;
    }
    
    bool UseKeyboard()
    {
        return m_UseKeyboard;
    }
    
    // Getters
    Widget GetRoot()
    {
        if (m_RootComponent)
        {
            return m_RootComponent.GetRoot();
        }
        return null;
    }
    
    DZlrComponentBase GetRootComponent()
    {
        return m_RootComponent;
    }
    
    UIComponentMenu GetUIMenu()
    {
        return m_UIMenu;
    }
    
    // Static helper
    static ComponentMenu GetMenuFromUI(UIScriptedMenu uiMenu)
    {
        if (s_AllMenus && s_AllMenus.Contains(uiMenu))
        {
            return s_AllMenus.Get(uiMenu);
        }
        return null;
    }
}
