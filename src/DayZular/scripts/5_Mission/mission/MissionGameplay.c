// DayZular Framework - Mission Integration
// Bootstrap the framework when mission starts on client

modded class MissionGameplay
{
    protected bool m_DZlrMissionLoaded = false;

    override void OnMissionStart()
    {
        super.OnMissionStart();
        
        AppModule.Bootstrap();
        
        m_DZlrMissionLoaded = false;
        
        // Invoke mission start event for all subscribed services
        GetDayZularEvents().OnMissionStart.Invoke();
    }
    
    override void OnMissionFinish()
    {
        // Invoke mission finish event for all subscribed services
        GetDayZularEvents().OnMissionFinish.Invoke();
        
        AppModule.Shutdown();
        
        super.OnMissionFinish();
    }
    
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);
        
		// MissionBase no longer exposes OnMissionLoaded, so detect once when loading completes
		if (!m_DZlrMissionLoaded && !g_Game.IsLoading())
		{
			m_DZlrMissionLoaded = true;
			HandleMissionLoaded();
		}
		
        // Invoke update event for subscribed services
        GetDayZularEvents().OnUpdate.Invoke(timeslice);
        
        // Update logger for config reload
        DZLog().Update(timeslice);
    }

	protected void HandleMissionLoaded()
	{
		GetDayZularEvents().OnMissionLoaded.Invoke();
	}
}
