function createScene()

    local Scene = {}

    Scene.ShadowMapSize = 1024;

    Scene.Lights = Terrain.Lights;
    Scene.TerrainScene = Terrain.Scene;
    Scene.Gravity = 59.81;
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
    
    
    Scene.IsCamToAI = false;
    
    Scene.Mirror = {}
    Scene.Mirror.Enable = true;
    Scene.Mirror.RTTSize = {}
    Scene.Mirror.RTTSize.w = 256;
    Scene.Mirror.RTTSize.h = 128;
    Scene.Mirror.FOV = 90.0;

    return Scene;
end
