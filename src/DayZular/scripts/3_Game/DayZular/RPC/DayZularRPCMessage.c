// DayZular Framework - RPC Message
// Wrapper around ScriptRPC for sending messages

class DayZularRPCMessage: ScriptRPC
{
    int m_RPCID;
    Object m_Target;
    
    // Write network ID helper
    void WriteNetworkID(Object object)
    {
        int low, high;
        object.GetNetworkID(low, high);
        Write(low);
        Write(high);
    }
    
    // Send the RPC (like WebSocket.send())
    void Send(bool guaranteed, PlayerIdentity recipient)
    {
        SendTo(m_Target, guaranteed, recipient);
    }
    
    // Send to specific target
    void SendTo(Object target, bool guaranteed, PlayerIdentity recipient)
    {
        if (!m_RPCID)
        {
            DZLog().LogError("DayZularRPCMessage: Invalid RPC ID");
            return;
        }

        if (target)
            Send(target, DAYZULAR_RPC_TARGETED, guaranteed, recipient);
        else
            Send(NULL, DAYZULAR_RPC_UNTARGETED, guaranteed, recipient);
    }
    
    // Factory method to create RPC message
    static DayZularRPCMessage Create(int rpcID)
    {
        DayZularRPCMessage rpc = new DayZularRPCMessage();
        if (!rpcID)
        {
            DZLog().LogError("DayZularRPCMessage: Cannot create RPC with ID 0");
        }
        else
        {
            rpc.m_RPCID = rpcID;
        }
        rpc.Write(rpcID);
        return rpc;
    }
}
