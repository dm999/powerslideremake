#ifndef ENUMS
#define ENUMS

enum GameMode
{
    ModeMenu,
    ModeMenuMulti,          // after connection happen
    ModeRaceSingle,
    ModeRaceMulti,
};

enum SinglePlayerMenuStates // multiplayer menus too but before connection happen
{
    State_SingleMulti,
    State_Multi,
    State_MultiConnect,
    State_MultiCreateRoom,
    State_MultiJoinRoom,
    State_Difficulty,
    State_Track,
    State_Car,
    State_Character,
    State_StartingGrid,
    State_Podium,
    State_ExitGame
};

enum AIStrength
{
    Easy    = 0,
    Medium  = 1,
    Hard    = 2,
    Insane  = 3
};

enum VehicleType
{
    HumanVehicle = 2,
    AIVehicle = 4
};

enum TransmissionType
{
    trManual,
    trAuto
};

#endif