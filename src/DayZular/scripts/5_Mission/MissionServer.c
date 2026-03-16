// DayZular Framework - Server Mission Integration
// Bootstrap the framework when mission starts on server

modded class MissionServer extends MissionBase
{
	protected ref array<string> m_DZlrJoinedPlayers = {};
	protected bool m_DZlrMissionLoaded = false;

	void MissionServer()
	{
		AppModule.Bootstrap();
	}
	
	override void OnMissionStart()
	{
		super.OnMissionStart();
		
		m_DZlrMissionLoaded = false;
		
		// Invoke mission start event for all subscribed services
		GetDayZularEvents().OnMissionStart.Invoke();
	}
	
	override void OnMissionFinish()
	{
		// Invoke mission finish event for all subscribed services
		GetDayZularEvents().OnMissionFinish.Invoke();
		
		super.OnMissionFinish();
	}
	
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		
		// DayZ vanilla MissionBase no longer exposes OnMissionLoaded, so we hook it manually once loading is done
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
	
	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);
		
		// Invoke event for subscribed services
		GetDayZularEvents().OnInvokeConnect.Invoke(player, identity);
		
		// Track first-time joins
		if (m_DZlrJoinedPlayers.Find(identity.GetId()) == -1)
		{
			OnPlayerJoined(player, identity);
			m_DZlrJoinedPlayers.Insert(identity.GetId());
		}
	}
	
	override void InvokeOnDisconnect(PlayerBase player)
	{
		super.InvokeOnDisconnect(player);
		
		GetDayZularEvents().OnInvokeDisconnect.Invoke(player);
	}
	
	override void OnClientReadyEvent(PlayerIdentity identity, PlayerBase player)
	{
		super.OnClientReadyEvent(identity, player);
		
		// Invoke player ready event for subscribed services
		GetDayZularEvents().OnPlayerReady.Invoke(player, identity);
		
		// Flush any queued RPC messages for this player
		GetDayZularRPCManager().FlushQueuedMessages(identity);
	}
	
	override void OnClientReconnectEvent(PlayerIdentity identity, PlayerBase player)
	{
		super.OnClientReconnectEvent(identity, player);
		
		GetDayZularEvents().OnPlayerReconnect.Invoke(player, identity);
	}
	
	override void OnClientRespawnEvent(PlayerIdentity identity, PlayerBase player)
	{
		super.OnClientRespawnEvent(identity, player);
		
		GetDayZularEvents().OnPlayerRespawn.Invoke(player, identity);
	}
	
	override void OnClientDisconnectedEvent(PlayerIdentity identity, PlayerBase player, int logoutTime, bool authFailed)
	{
		super.OnClientDisconnectedEvent(identity, player, logoutTime, authFailed);
		
		// Invoke disconnected event for subscribed services
		GetDayZularEvents().OnPlayerDisconnected.Invoke(player, identity, logoutTime, authFailed);
		
		// Clean up tracking
		if (m_DZlrJoinedPlayers.Find(identity.GetId()) != -1)
		{
			m_DZlrJoinedPlayers.Remove(m_DZlrJoinedPlayers.Find(identity.GetId()));
		}
	}
	
	override PlayerBase OnClientNewEvent(PlayerIdentity identity, vector pos, ParamsReadContext ctx)
	{
		PlayerBase player = super.OnClientNewEvent(identity, pos, ctx);
		
		GetDayZularEvents().OnClientNew.Invoke(player, identity, pos, ctx);
		
		return player;
	}
	
	override void OnClientPrepareEvent(PlayerIdentity identity, out bool useDB, out vector pos, out float yaw, out int preloadTimeout)
	{
		// Invoke event before vanilla processing
		GetDayZularEvents().OnClientPrepare.Invoke(identity, useDB, pos, yaw, preloadTimeout);
		
		super.OnClientPrepareEvent(identity, useDB, pos, yaw, preloadTimeout);
	}

	// OnPlayerJoined - Called when a player joins the server (first connection)
	// Invokes OnPlayerJoined event for all subscribed services
	void OnPlayerJoined(PlayerBase player, PlayerIdentity identity)
	{
		GetDayZularEvents().OnPlayerJoined.Invoke(player, identity);
	}
};
