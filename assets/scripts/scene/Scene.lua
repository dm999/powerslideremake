function createScene()

    local Scene = {}

    Scene.ShadowMapSize = 1024;

    Scene.Camera = {}
    Scene.Camera.AspectRatio = 3.0 / 3.0;
    Scene.Camera.FOV = 95.0;

    Scene.Lights = Terrain.Lights;
    Scene.TerrainScene = Terrain.Scene;
    Scene.Gravity = 59.81;
    Scene.AIOpponents = 1; -- not more than 11
    Scene.AIStrength = 3;   -- 0 easy, 1 medium, 2 hard, 3 insane
    Terrain.Mesh.IsBB = false;
    Terrain.Mesh.IsBS = false;
    Terrain.Scene.IsDebugLights = false;
    Terrain.Scene.IsDebugLightsSizeAsRange = false;
    Terrain.Scene.IsDebugExclusion = false;
    Terrain.Scene.IsDebugAI = false;
    Terrain.Scene.IsDebugLLT = false;
    Terrain.Scene.IsDebugPhysics = false;
--~     Terrain.Material.IsOverrideSubMaterials = true;
--~     Terrain.Material.SingleSubMaterial = "Test/Multi_2/PointDiffuseLRShadow";
--~     Terrain.Material.SingleSubMaterial = "Test/Multi_2/PointDiffuseLRShadowSoft";
    Terrain.Scene.IsAdjustNormals = true;
    Terrain.Scene.AdjustNormalsAngleThreshold = math.cos(math.rad(80.0));--average lower than
    
    
    Scene.IsDisableMouse = true;
    Scene.IsCamToAI = false;
    
    Scene.SoundMasterGain = 0.0;
    
    Scene.Mirror = {}
    Scene.Mirror.Enable = true;
    Scene.Mirror.RTTSize = {}
    Scene.Mirror.RTTSize.w = 256;
    Scene.Mirror.RTTSize.h = 128;
    Scene.Mirror.FOV = 90.0;
    
    Scene.Multiplayer = {}
    Scene.Multiplayer.Enabled = false;
    Scene.Multiplayer.IsMaster = true;
--~     Scene.Multiplayer.IsMaster = false;
    Scene.Multiplayer.RoomName = "Powerslide";
    Scene.Multiplayer.UserName = "DM";
--~     Scene.Multiplayer.UserName = "DM2";
    Scene.Multiplayer.ServerIP = "95.213.199.37";
--~     Scene.Multiplayer.ServerIP = "127.0.0.1";
    Scene.Multiplayer.ServerIP = "195.19.158.25";
    Scene.Multiplayer.ServerPort = 8800;
    Scene.Multiplayer.PlayersLimits = 2; -- for master only
    Scene.Multiplayer.BroadcastInterval = 150; --in ms; 50 for fast; 150 for slow

    return Scene;
end
