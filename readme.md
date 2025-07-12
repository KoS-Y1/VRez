# Vulkan Ray Tracer

*Used as a TODO list for now*

## File System
~~+ Verify if it's a valid file path~~

~~## Meshes~~
+ ~~Mesh Loader~~
+ ~~Add Mesh Instance class, which has a model matrix handling~~
  + ~~Transformation~~
    + ~~Translation~~
    + ~~Rotation~~
    + ~~Scaling~~

## Lighting
+ Light
  + Spot light
  + Enviroment light
  + Parallel light
+ Light with transformation
  + Translation
  + Rotation
  + Scale?
+ Basic ray tracing(make sure models' normal working)
  + Basic rendering(e.g. phong..)
  + Shadow
    + Soft shadow
    + Antialiasing

## Texture
+ Texture loader
+ Different kind of textures with corresponding shaders
  + Basic texture mapping
  + Normal mapping...

## Shader Compiler
+ Parse to get the descrirptor set layout and push constant info with a given glsl file
~~~~ c++
read all file path
for each file
parse descriptor set layout/push constant layout
merge same descriptor set/push constant
compile file to spirv

~~~~
+ Reading from shader file, ~~then compile the shader~~, and return a pipeline config
+ In VulkanState map<filename, shared_ptr pipeline>(still need a function to decide either pointer to compute/graphics pipeline)
+ ~~Mesh instance owns a reference to a shared pipeline (shared_ptr to pipeline), and just need to update push constant/descriptor per draw to reuse the pipeline~~
+ In VulkanState, each frame, update descriptor sets/push constant
+ Feature to compile until shader header(#include <name.glsl> in shader file)
+ VulkanPipeline will directly create descriptor set layout and pipeline with the parsed descriptor set layout and push constant info 
~~~~ c++
pass all file path of the shaders to ShaderCompiler
Get all spirv code, descriptor layout info, and push constant
For each spirv code, create shader module and shader stage
Create pipeline layout with descriptor layout info and push constant
~~~~


## RAII
  + Avoid to use raw pointer, use unique_ptr, shared_ptr or weak_ptr if possible