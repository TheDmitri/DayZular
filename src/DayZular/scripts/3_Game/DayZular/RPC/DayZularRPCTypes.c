// DayZular Framework - RPC Types and Constants
// Core type definitions and constants used across the RPC system

// RPC IDs for DayZular framework
const int DAYZULAR_RPC_UNTARGETED = 1684234857;  // "DayZular".Hash()
const int DAYZULAR_RPC_TARGETED = 1684234858;    // "DayZular".Hash() + 1

enum DayZularRPCType
{
    Server,
    Client,
    Both
}
