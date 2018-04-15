function createVehicle()

    local Model = {}

    Model.ShadowLightDistance = 40.0;
    Model.ShadowLightOuterAngle = 90.0;
    Model.ShadowLightInnerAngle = 80.0;
    
    Model.IsAttenuateExcludeBox = false;
    
    
    Model.Sfx = {}
    Model.Sfx.ReferenceDist = 30.0;
    Model.Sfx.MaxDist = 100.0;
    
    Model.Mesh = {}
    Model.Mesh.IsTangents = false;
    Model.Mesh.IsBB = false;

    Model.Material = {}
    Model.Material.SubMaterials = {}
    
    Model.Material.IsOverrideSubMaterials = true;
    
--~     Model.Material.SingleSubMaterial = "Test/SpotDiffuse";
--~     Model.Material.SingleSubMaterial = "Test/PointDiffuseLR";
    Model.Material.SingleSubMaterial = "Test/Multi_2/PointDiffuseLR";
    Model.Material.SingleSubMaterialNoAtt = "Test/Multi_2/PointDiffuseLRNoAtt";
    Model.Material.SingleSubMaterialExclude = "Test/Multi_2/PointDiffuseLRExclude";
    Model.Material.SingleSubMaterialAI = "Test/PointDiffuseSimple";
    Model.Material.SingleSubMaterialAIFog = "Test/PointDiffuseSimpleFog";
--~     Model.Material.SingleSubMaterial = "Test/NormalDebug";

    Model.Material.SingleAmbient = {}
    Model.Material.SingleAmbient.r = 0.5;
    Model.Material.SingleAmbient.g = 0.5;
    Model.Material.SingleAmbient.b = 0.5;
    Model.Material.SingleDiffuse = {}
    Model.Material.SingleDiffuse.r = 0.6;
    Model.Material.SingleDiffuse.g = 0.6;
    Model.Material.SingleDiffuse.b = 0.6;
    Model.Material.SingleSpecular = {}
    Model.Material.SingleSpecular.r = 1.0;
    Model.Material.SingleSpecular.g = 1.0;
    Model.Material.SingleSpecular.b = 1.0;

    return Model;

end