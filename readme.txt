=== The Project ===

gifmethat is a gif decoder written in C.
It was written following mostly the article by commandlinefanatic at
https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art011.
Please note that I, the author, don't have a lot of experience with programming
in C so the code might not comply with standards and conventions and might
absolutely be suboptimal. PRs are hence highly appreciated.
Also, note that the decoder isn't feature-complete and doesn't aim to be. I
want this to be a tiny cute library that is enough to decode most gifs but
doesn't need to be able to handle them all. Namely, it only supports gif version
97a with a global color table (and no animations). To my understanding though,
most gifs don't use these features.

=== Build Instructions ===

To build the whole thing, consider the `build.sh` script at the project's root.
You will need `gcc` and `ar` to build the library. Alternatively, just figure it
out, it's only a header and its implementation.
When using the build script, object files will be at `target/` and the static
library at `target/static/libgifmethat.a`

=== Tests ===

You will notice a cute `cats.gif` at the project's root. The test application
located at `test/main.c` will decode it and save it as `cats.ppm`.
The test application can be run using the `test.sh` script which requires `gcc`.
Or just figure it out again, it's just a single C file that includes the
`gifmethat.h` file using a relative path.

=== TODO ===

    1. With every read, check if enough data was left in the file
    2. Properly parse graphic control extension
    3. Parse other extensions (comment e. g. could even be added to the Gif
    struct)
    4. Make error reporting more consistent
    5. refactor uncompress method

