# Node OpenCL

This is an early implementation of Node.JS bindings to OpenCL supporting all features of OpenCL up to the latests specification available on Khronos.org.

This implementation is different from node-webcl in the sense that it is close to OpenCL C host methods. A WebCL object model would be available later by simply wrapping the low level methods of node-opencl.

The test/ folder will contain as many unit tests testing all features of OpenCL. These tests are not based on Khronos conformance tests, though they should be similar. The goals of these tests is to check OpenCL drivers to discover their bugs on various platforms and, if possible, implement remedies while companies correct their drivers.
