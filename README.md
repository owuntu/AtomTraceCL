# AtomTraceCL
A GPU ray tracer. Currently using OpenCL 2.0.
Known issue:
- There is a GPU memory bottle neck. It cannot load too much scene data into it, which shouldn't happen, since the same data can be render in OpenGL. Probably there is some setting need to be done in OpenCL.
