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
            ResourceType.Texture, ResourceScope.All)

addResource("Texture::Dropdown", "media/Textures/dropdown.png",
            ResourceType.Texture, ResourceScope.All)

addResource("Texture::Slider", "media/Textures/sliderbg.png",
            ResourceType.Texture, ResourceScope.All)

addResource("Texture::SliderButton", "media/Textures/sliderbutton.png",
            ResourceType.Texture, ResourceScope.All)

addResource("Font::Dejavu", "media/Fonts/DejaVuSansMono.ttf",
            ResourceType.Font, ResourceScope.All)

addResource("Program::GUI", "shaders/GUI/GUI.vsfs",
            ResourceType.Program, ResourceScope.All)

addResource("Program::ColorRect", "shaders/GUI/ColorRect.vsfs",
            ResourceType.Program, ResourceScope.All)

addResource("Program::Clear", "shaders/Framebuffer.vs,shaders/Utils/Clear.fs",
            ResourceType.Program, ResourceScope.All)

addResource("Program::Copy", "shaders/Framebuffer.vs,shaders/Utils/Copy.fs",
            ResourceType.Program, ResourceScope.All)