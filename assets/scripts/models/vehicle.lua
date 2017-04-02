function createVehicle()

    local Model = {}

    Model.IsCastShadows = true;
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
    
    Model.Physics = {}
    Model.Physics.Chassis = {}
    Model.Physics.Chassis.Restitution = 0.0;    -- bounciness
    Model.Physics.Chassis.Friction = 0.4;
    Model.Physics.Wheels = {}
    Model.Physics.Wheels.Restitution = 0.0;
    Model.Physics.Wheels.Friction = 1.0;
    Model.Physics.Wheels.AnisotropicFriction = {}
    Model.Physics.Wheels.AnisotropicFriction.x = 0.5;   -- latitude friction
    Model.Physics.Wheels.AnisotropicFriction.y = 0.05;  -- longtitude friction
    Model.Physics.Wheels.AnisotropicFriction.z = 0.05;  -- longtitude friction
    Model.Physics.Wheels.RollingFriction = 0.00081;
    Model.Physics.Wheels.Front = {}
    Model.Physics.Wheels.Front.LimitSpringParams = false;
    Model.Physics.Wheels.Front.Mass = 2.0;
    Model.Physics.Wheels.Front.MinSteer = -45.0;
    Model.Physics.Wheels.Front.MaxSteer = 45.0;
    Model.Physics.Wheels.Front.SpringStiffness = 950.0;
    Model.Physics.Wheels.Front.SpringDamping = 50.0;
    Model.Physics.Wheels.Rear = {}
    Model.Physics.Wheels.Rear.LimitSpringParams = false;
    Model.Physics.Wheels.Rear.Mass = 2.0;
    Model.Physics.Wheels.Rear.SpringStiffness = 950.0;
    Model.Physics.Wheels.Rear.SpringDamping = 50.0;

    return Model;

end