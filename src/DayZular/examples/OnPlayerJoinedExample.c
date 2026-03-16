// DayZular Framework - OnPlayerJoined Example
// Shows how to use OnPlayerJoined to send config to clients

class MyConfigModule : DayZularModule
{
    ref MyModuleConfig m_Config;
    
    override void OnRegister()
    {
        super.OnRegister();
        
        // Register client RPC handler
        if (IsClient())
        {
            RegisterRPC("MyMod", "ReceiveConfig", DayZularRPCType.Client);
        }
    }
    
    override void OnMissionStart()
    {
        super.OnMissionStart();
        
        // Load config on server
        if (IsServer())
        {
            m_Config = MyModuleConfig.Load();
            DZLog().LogInfo("Config loaded: " + m_Config.ToString());
        }
    }
    
    override void OnPlayerJoined(PlayerBase player, PlayerIdentity identity)
    {
        super.OnPlayerJoined(player, identity);
        
        // THIS IS THE PERFECT PLACE to send config to client!
        // Called once per player on first connection
        if (IsServer())
        {
            DZLog().LogInfo("Sending config to new player: " + identity.GetName());
            
            // Send config to client
            SendRPC("MyMod", "ReceiveConfig", new Param1<MyModuleConfig>(m_Config), true, identity);
            
            // You can also send other initial data here:
            // - Player stats
            // - Server settings
            // - Available features
            // - Custom data
        }
    }
    
    override void OnPlayerReady(PlayerBase player, PlayerIdentity identity)
    {
        super.OnPlayerReady(player, identity);
        
        // Player is fully spawned and ready
        // Use this for data that requires player to be in-world
        if (IsServer())
        {
            DZLog().LogInfo("Player fully ready: " + identity.GetName());
            
            // Send additional data that requires player to be spawned
            // - Nearby entities
            // - Territory info
            // - Active quests
        }
    }
    
    // Client-side RPC handler
    void ReceiveConfig(PlayerIdentity sender, ParamsReadContext ctx)
    {
        Param1<MyModuleConfig> data;
        if (!ctx.Read(data))
        {
            DZLog().LogError("Failed to read config from server");
            return;
        }
        
        m_Config = data.param1;
        DZLog().LogInfo("Config received from server!");
        
        // Now you can use the config on client
        // - Update UI
        // - Configure client-side behavior
        // - Enable/disable features
    }
}
