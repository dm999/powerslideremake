Main = {}
Main.LevelLogDetailed = false;

Window = {}
Window.IsFullScreen = "No";
Window.FSAA = "0";
Window.Resolution = "1280 x 1024";
Window.VSync = "No"
Window.VSyncInterval = "1"

-- executed from native code
function main()

    parseFile("vehicle.lua");
    parseFile("Scene.lua");
    
    Terrain = {}
    Terrain.Mesh = {}
    Terrain.Mesh.IsTangents = false;
    Terrain.Mesh.IsBB = false;
    
    Terrain.Material = {}
    Terrain.Material.IsMaskLights = false;
    Terrain.Material.SingleSubMaterialNoShadow = "Test/DiffuseSimpleNoShadow";
    Terrain.Material.SingleSubMaterialNoShadowFog = "Test/DiffuseSimpleNoShadowFog";
    Terrain.Material.SingleSubMaterial = "Test/DiffuseSimpleShadowSoft";
    Terrain.Material.SingleSubMaterialArray = "Test/DiffuseSimpleShadowSoftArray";
    Terrain.Material.SingleSubMaterialFog = "Test/DiffuseSimpleShadowSoftFog";
    Terrain.Material.IsOverrideDefaultTextureWithTransparentForSubMaterials = true;
    Terrain.Material.DefaultTextureName = "defaulttexture.tex";
    
    Terrain.Scene = {}
    
    Model = createVehicle();
    Scene = createScene();
    
end
