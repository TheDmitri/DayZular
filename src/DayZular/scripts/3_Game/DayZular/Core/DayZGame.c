// DayZular Framework - Game Hook
// Hooks into DayZGame.OnRPC to route DayZular RPCs to the manager

modded class DayZGame
{
    override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
    {
        // Check if this is a DayZular RPC
        if (rpc_type == DAYZULAR_RPC_UNTARGETED || rpc_type == DAYZULAR_RPC_TARGETED)
        {
            DZLog().LogDebug("DayZGame: RPC received type " + rpc_type);
            if (GetDayZularRPCManager().OnRPC(sender, target, ctx))
                return;
        }
        
        super.OnRPC(sender, target, rpc_type, ctx);
    }
}
