Main = {}
Main.OriginalPathData = "./";
Main.OriginalPathCommon = "./";

Window = {}
Window.IsFullScreen = "No";
Window.FSAA = "0";
Window.Resolution = "1280 x 1024";
Window.VSync = "No"
Window.VSyncInterval = "1"

-- executed from native code
function main()

    parseFile("vehicle.lua");
    parseFile("Speedway.lua");
    parseFile("Dam.lua");
    parseFile("Citytrack.lua");
    parseFile("Nutopia.lua");
    parseFile("Scene.lua");

--~     Terrain = createSpeedway();
    Terrain = createDam();
--~     Terrain = createCitytrack();
--~     Terrain = createNutopia();
    Model = createVehicle();
    Scene = createScene();
    
    Main.Character = "beryl";
--~     Main.Character = "dodgy";
--~     Main.Character = "hemp";
--~     Main.Character = "marl";
--~     Main.Character = "crayz";
--~     Main.Character = "driverx";
--~     Main.Character = "armadillo";
    
    initScene();
end

-- executed from native code
function create()
    initTerrain();
    initModel();
    
    initMisc();
end

-- executed from native code
function clear()
    clearScene();
end
