* [-] preprocessor
    * [x] #include directives
    * [ ] keep track of previously included files to avoid redefinitions
    * [ ] include as submodule
* [-] render image with compute shader
    * [x] render a texture to a full-screen quad
    * [x] pass uniforms to shader to animate it
    * [x] pass structed uniform buffers
    * [-] ray tracing time
        * [-] light
            * [x] perspective
            * [x] camera lookat
            * [x] do a bounce
            * [x] do a 'flect
            * [ ] properly sample depth for reflective surfaces
            * [ ] do a 'fract
            * [ ] depth of field
            * [ ] emission
            * [ ] wavelength-based instead of rgb?
        * [-] acceleration time ! 
            * [x] acquire randomness
            * [-] auxiliary textures
                * [x] depth buffer
                * [ ] frame blending
                    * [ ] use pixel object tags to determine balance between trace 
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
