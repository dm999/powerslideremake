#ifndef ENUMS
#define ENUMS

enum CameraPositions
{
    CameraPosition_Bumper       = 0,
    CameraPosition_ChassisA     = 1,
    CameraPosition_ChassisB     = 2,
    CameraPosition_ChassisC     = 3
};

enum GameMode
{
    ModeMenu,
    ModeMenuChampionship,
    ModeMenuTimetrial,
    ModeMenuMulti,          // after connection happen
    ModeRaceSingle,
    ModeRaceChampionship,
    ModeRaceTimetrial,
    ModeRaceMulti,
};

enum SinglePlayerMenuStates // multiplayer menus too but before connection happen
{
    State_SingleMulti,
    State_Multi,
    State_MultiConnect,
    State_MultiCreateRoom,
    State_MultiJoinRoom,
    State_SingleType,
    State_Difficulty,
    State_Track,
    State_Car,
    State_Character,
    State_Options,  // == options: graphics
    State_Options_Input,
    State_Options_Sound,
    State_Options_Race,
    State_Options_Scores,
    State_Options_Name,
    State_Options_Trophies,
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
    HumanVehicle    = 2,
    AIVehicle       = 4
};

enum TransmissionType
{
    trManual    = 0,
    trAuto      = 1
};

enum InputType
{
    itKeyboard  = 0,
    itMouse     = 1
};

#endif