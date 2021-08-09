* [-] preprocessor
    * [x] #include directives
    * [ ] keep track of previously included files to avoid redefinitions
* [-] render image with compute shader
    * [x] render a texture to a full-screen quad
    * [x] pass uniforms to shader to animate it
    * [x] pass structed uniform buffers
    * [-] ray tracing time
        * [x] perspective
        * [x] camera lookat
        * [x] do a bounce
        * [ ] do a 'flect
        * [ ] do a 'fract
        * [-] depth
            * [x] acquire value
        * [ ] acquire randomness
        * [ ] acceleration time ! 
            * [ ] auxiliary textures
                * [ ] depth buffer
                * [ ] frame blending
    * [ ] maybe do some fractals
        * [ ] mandelbrot
        * [ ] julia
        * [ ] trongle
* [ ] resizable window
* [ ] output frame to a file
    * [ ] detect input keydown s
    * [ ] get timestamp
    * [ ] create and write to file (maybe with `stb_image.h`?)
* [ ] command line arguments
    * [ ] help
    * [ ] window dimensions
* [ ] scene definition
