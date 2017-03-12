-- This file defines the resources that should be loaded
-- into the engine and when they should be loaded.
--
-- This has one function inside the scope which is `addResource`
-- that is defined as the following:
--
-- add_(Name, Scope, Type, location)
--
-- where:
--  - Name, Unique string that can be used to retrieve the resource from the
--          resource manager
--  - Type, is the type of resource
--  - location, is a path to the resource itself
--  - Scope, is a flag saying which states should have this available to it
--
-- Scope is defined by ResourceScope
-- Type is defined by ResourceType

addResource("Texture::Background", "media/Textures/optionsMenu.png",
            ResourceType.Texture,
            ResourceScope.All)

addResource("Texture::Debug", "media/Textures/debug.png",
            ResourceType.Texture,
            ResourceScope.All)

addResource("Texture::Dropdown", "media/Textures/dropdown.png",
            ResourceType.Texture, ResourceScope.All)

addResource("Texture::Slider", "media/Textures/sliderbg.png",
            ResourceType.Texture, ResourceScope.All)

addResource("Texture::SliderButton", "media/Textures/sliderbutton.png",
            ResourceType.Texture, ResourceScope.All)

addResource("Texture::m1_test.png", "media/Textures/m1_test.png",
            ResourceType.Texture,
            ResourceScope.Master)

addResource("Texture::m2_test.png", "media/Textures/m2_test.png",
            ResourceType.Texture,
            ResourceScope.Master)

addResource("Texture::m3_test.png", "media/Textures/m3_test.png",
            ResourceType.Texture,
            ResourceScope.Master)

addResource("Texture::m4_test.png", "media/Textures/m4_test.png",
            ResourceType.Texture,
            ResourceScope.Master)

addResource("Font::Dejavu", "media/Fonts/DejaVuSansMono.ttf",
            ResourceType.Font, ResourceScope.All)

addResource("Program::GUI", "shaders/GUI/GUI.vs,shaders/GUI/GUI.fs",
            ResourceType.Program, ResourceScope.All)

addResource("Program::Font", "shaders/GUI/Font.vs,shaders/GUI/Font.fs",
            ResourceType.Program, ResourceScope.All)

addResource("Program::ColorRect", "shaders/ColorRect.vs,shaders/ColorRect.fs",
            ResourceType.Program, ResourceScope.All)

addResource("Program::Clear", "shaders/Framebuffer.vs,shaders/Utils/Clear.fs",
            ResourceType.Program, ResourceScope.All)

addResource("Program::Copy", "shaders/Framebuffer.vs,shaders/Utils/Copy.fs",
            ResourceType.Program, ResourceScope.All)

addResource("Program::Shadow", "shaders/Shadow.fs,shaders/Shadow.vs",
            ResourceType.Program, ResourceScope.Master)

addResource("Program::Model", "shaders/Model.fs,shaders/Model.vs",
            ResourceType.Program, ResourceScope.Master)

-- addResource("Mesh::Box", "media/models/test.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("PhysicsMesh::Box", "media/models/test.bullet",
--             ResourceType.PhysicsMesh, ResourceScope.Master)

addResource("Mesh::Spider", "media/models/spider.dae",
            ResourceType.Mesh, ResourceScope.Master)

addResource("PhysicsMesh::Spider", "media/models/spider.bullet",
            ResourceType.PhysicsMesh, ResourceScope.Master)

-- addResource("Mesh::Abdomen", "media/models/spider/abdomin.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::AbdomenInner", "media/models/spider/abdomin_inner.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::Coxa", "media/models/spider/coxa.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::Femur", "media/models/spider/femur.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::FemurTibiaInner", "media/models/spider/femur_tibia_inner.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::Head", "media/models/spider/head.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::HeadDisk", "media/models/spider/head_disk.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::HeadInner", "media/models/spider/head_inner.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::Hip", "media/models/spider/hip.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::Joint", "media/models/spider/joint.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::JointRot", "media/models/spider/joint_rot.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::Metatarsus", "media/models/spider/metatarsus.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::Neck", "media/models/spider/neck.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::Patella", "media/models/spider/patella.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::PatellaInner", "media/models/spider/patella_inner.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::SternumBack", "media/models/spider/sternum_back.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::SternumFront", "media/models/spider/sternum_front.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::TarsusBot", "media/models/spider/tarsus_bot.dae",
--            ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::TarsusTop", "media/models/spider/tarsus_top.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::Thorax", "media/models/spider/thorax.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::Tibia", "media/models/spider/tibia.dae",
--             ResourceType.Mesh, ResourceScope.Master)

-- addResource("Mesh::Trochanter", "media/models/spider/trochanter.dae",
--             ResourceType.Mesh, ResourceScope.Master)
