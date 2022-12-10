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
    State_Options_Credits_Video,
    State_Options_About_Video,
    State_Options_Trophies_Video,
    State_StartingGrid,
    State_Podium,
    State_Leaderboard,          //championship
    State_FinishChampionship,   //championship
    State_ExitGame
};

enum AIStrength
{
    Easy    = 0,
    Medium  = 1,
    Hard    = 2,
    Insane  = 3,
    UltraInsane = 4 // insane with 9 gears auto gearbox
};

enum VehicleType
{
    HumanVehicle    = 2,
    AIVehicle       = 4
};

enum TransmissionType
{
    trManual        = 0,
    trAuto          = 1,
    trAutoAdvanced  = 2 // 9 gears gear box
};

enum InputType
{
    itKeyboard  = 0,
    itMouse     = 1
};

enum InputKeyMapping
{
    kmLeft = 0,
    kmRight = 1,
    kmUp = 2,
    kmDown = 3,

    kmGearUp = 4,
    kmGearDown = 5,

    kmView = 6,

    kmGhost = 7,

    kmHandBreak = 8,

    //cheats
    kmBurn = 9,
    kmBomb = 10,
    kmNitro = 11,
    kmDropCam = 12,
    kmSticky = 13,
    kmSpider = 14,
    kmICBM = 15,
    kmApollo = 16,
    kmLunar = 17,
    kmJump = 18,
    kmHover = 19,
    kmGlider = 20,

    kmEmpty = 21
};

#endif