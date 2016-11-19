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
    
--~     Main.Track = "desert track";
--~     Main.Track = "speedway track";
    Main.Track = "dam";
--~     Main.Track = "mountain forest track";
--~     Main.Track = "mineshaft";
--~     Main.Track = "alpine track";
--~     Main.Track = "city track";
--~     Main.Track = "nutopia track";
--~     Main.Track = "stunt track";
--~     Main.Track = "luge track";
--~     Main.Track = "Foxnhound2 track";
--~     Main.Track = "Foxnhound1 track";

    
    Main.Character = "frantic";
--~     Main.Character = "cyber";
--~     Main.Character = "radiation";
--~     Main.Character = "zig";
--~     Main.Character = "zag";
--~     Main.Character = "beryl";
--~     Main.Character = "stig";
    
--~     Main.Character = "dodgy";
--~     Main.Character = "pepper";
--~     Main.Character = "colonel";
--~     Main.Character = "furnace";
--~     Main.Character = "pablo";
--~     Main.Character = "dwayne";
--~     Main.Character = "massie";

--~     Main.Character = "jean";
--~     Main.Character = "hemp";
--~     Main.Character = "hardy";
--~     Main.Character = "bill";
--~     Main.Character = "upjohn";
--~     Main.Character = "bendito";

--~     Main.Character = "marl";
--~     Main.Character = "yukio";
--~     Main.Character = "bimbeau";
--~     Main.Character = "miki";
--~     Main.Character = "rosie";
--~     Main.Character = "cinnamon";

--~     Main.Character = "crayz";
--~     Main.Character = "mad";
--~     Main.Character = "max";

--~     Main.Character = "driverx";
--~     Main.Character = "chuck";
--~     Main.Character = "punch";
    
--~     Main.Character = "mach";
--~     Main.Character = "armadillo";
--~     Main.Character = "alter";
--~     Main.Character = "komodo";
--~     Main.Character = "panther";
--~     Main.Character = "volt";
--~     Main.Character = "razer";
--~     Main.Character = "argon";
    
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
