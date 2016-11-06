function createDam()

    local Terrain = {}
    
    Terrain.Scene = {}
    Terrain.Scene.IsDebugLights = true;
    
    Terrain.Scene.AI = {}
    Terrain.Scene.AI.Easy = {}
    Terrain.Scene.AI.Easy.SpeedCoeff = 0.6;
    Terrain.Scene.AI.Easy.LateralStabilization = 0.2;
    Terrain.Scene.AI.Medium = {}
    Terrain.Scene.AI.Medium.SpeedCoeff = 0.7;
    Terrain.Scene.AI.Medium.LateralStabilization = 0.7;
    Terrain.Scene.AI.Hard = {}
    Terrain.Scene.AI.Hard.SpeedCoeff = 0.9;
    Terrain.Scene.AI.Hard.LateralStabilization = 0.8;
    Terrain.Scene.AI.Insane = {}
    Terrain.Scene.AI.Insane.SpeedCoeff = 1.0;
    Terrain.Scene.AI.Insane.LateralStabilization = 1.0;

    Terrain.Mesh = {}
    Terrain.Mesh.IsTangents = false;
    Terrain.Mesh.IsBB = false;

    Terrain.Mesh.TrackName = "dam";
    Terrain.Mesh.FileName = "dam.de2";
--~     Terrain.Mesh.TrackName = "desert";
--~     Terrain.Mesh.FileName = "deserttrack.de2";
--~     Terrain.Mesh.TrackName = "mf";
--~     Terrain.Mesh.FileName = "mf.de2";
--~     Terrain.Mesh.TrackName = "mineshaft";
--~     Terrain.Mesh.FileName = "mineshaft.de2";
--~     Terrain.Mesh.TrackName = "stunt";
--~     Terrain.Mesh.FileName = "stunt.de2";
--~     Terrain.Mesh.TrackName = "alpine";
--~     Terrain.Mesh.FileName = "alpine.de2";
--~     Terrain.Mesh.TrackName = "luge";
--~     Terrain.Mesh.FileName = "luge.de2";
--~     Terrain.Mesh.TrackName = "foxnhound1";
--~     Terrain.Mesh.FileName = "fnh.de2";
--~     Terrain.Mesh.TrackName = "foxnhound2";
--~     Terrain.Mesh.FileName = "fnh2.de2";
    
    Terrain.Material = {}
    Terrain.Material.IsOverrideMaterial = false;
    Terrain.Material.Scale = 1.0;
    
    --try to keep original texture
    --won`t keep if Terrain.Material.IsOverrideMaterial == true
    Terrain.Material.IsOverrideSubMaterials = true;
    Terrain.Material.IsMaskLights = false;
    
    --only if Terrain.Material.IsOverrideSubMaterials == true
--~     Terrain.Material.SingleSubMaterial = "Test/SpotDiffuseShadowSmoothReciever";
--~     Terrain.Material.SingleSubMaterial = "Test/SpotDiffuseShadowReciever";
--~     Terrain.Material.SingleSubMaterial = "Test/SpotDiffuse";
--~     Terrain.Material.SingleSubMaterial = "Test/PointDiffuse";
--~     Terrain.Material.SingleSubMaterial = "Test/PointDiffuseLR";
--~     Terrain.Material.SingleSubMaterial = "Test/Multi_2/PointDiffuseLR";
--~     Terrain.Material.SingleSubMaterial = "Test/Multi_2/PointDiffuseLRShadowSoft";
--~     Terrain.Material.SingleSubMaterial = "Test/Multi_2/PointDiffuseLRShadowSoftLightmap";
--~     Terrain.Material.SingleSubMaterial = "Test/Multi_2/PointDiffuseLRShadowSoftNoNormal";
        Terrain.Material.SingleSubMaterial = "Test/DiffuseSimpleShadowSoft";
--~         Terrain.Material.SingleSubMaterial = "Test/DiffuseSimpleShadowSoftDebug";
--~     Terrain.Material.SingleSubMaterial = "Test/NormalDebug";
--~         Terrain.Material.SingleSubMaterial = "Test/DebugTerrain";

        Terrain.Material.SingleSubMaterialArray = "Test/DiffuseSimpleShadowSoftArray";

    Terrain.Material.IsOverrideDefaultTextureWithTransparentForSubMaterials = true;
    Terrain.Material.DefaultTextureName = "defaulttexture.tex";

    return Terrain;

end
