* [x] basic opengl initialisation
* [x] shader pre-processor
    * [x] ppp.py
    * [x] read root shaders
    * [x] read include shaders
    * [x] write processed shaders to bin/res/shader/
    * [x] compile processed shaders
* [-] render image with compute shader
    * [x] render a texture to a full-screen quad
    * [x] pass uniforms to texture to animate it
    * [-] ray tracing time
        * [x] perspective
        * [-] depth
            * [x] acquire value
            * [ ] depth texture
        * [ ] acquire randomness
        * [ ] acceleration time ! 
            * [ ] auxiliary textures: g buffer
            * [ ] frame blending
    * [ ] maybe do some fractals
        * [ ] mandelbrot
        * [ ] julia
        * [ ] trongle
* [ ] output frame to a file
    * [ ] detect input keydown s
    * [ ] get timestamp
    * [ ] create and write to file (maybe with `stb_image.h`?)
* [ ] command line arguments
    * [ ] help
    * [ ] window dimensions
* [ ] scene definition
