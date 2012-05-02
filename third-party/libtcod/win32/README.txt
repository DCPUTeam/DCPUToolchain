It is not possible to easily compile LibTCOD on Windows due to the
extensive dependencies; these dependencies cause issues with ensuring
that it can be compiled both under GCC on Windows and under Visual
Studio, so for Windows we just link against the provided libraries.