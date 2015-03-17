# Node OpenCL
 
[![Build Status](http://pub-ci.ioweb.fr/api/badge/github.com/ioweb-fr/node-opencl/status.svg?branch=master)](http://pub-ci.ioweb.fr/github.com/ioweb-fr/node-opencl)

## Abstract

This is an early implementation of Node.JS bindings to OpenCL supporting all features of OpenCL up to the latests specification available on Khronos.org.

This implementation is different from node-webcl in the sense that it is close to OpenCL C host methods. A WebCL object model would be available later by simply wrapping the low level methods of node-opencl.

The test/ folder will contain as many unit tests testing all features of OpenCL. These tests are not based on Khronos conformance tests, though they should be similar. The goals of these tests is to check OpenCL drivers to discover their bugs on various platforms and, if possible, implement remedies while companies correct their drivers.

## Current state

The implementation is almost OpenCL 1.2 compliant. We are currently working on OpenCL 2.0 compatibility.

## Tested configurations

This project has been succesfully tested on the following platforms  :

- OSX 10.10 with Intel Iris GPU, Native OpenCL implementation (OpenCL 1.2), Node 0.12
- Ubuntu 14.10 with Intel CPU, AMD APP SDK 3 (OpenCL 2.0), Node 0.12 (Dockerfile is available in docker folder)
- Windows 7.1 with Intel CPU, AMD APP SDK 3 (OpenCL 2.0), Node 0.12


# Installation

Except specific platform installation guides below, you only need to install the package through

```npm install```

To run unit tests, you will need mocha as a global package. Then you can simply call 

```npm test```

## Mac OS X

You should only need : 

- XCode Developers tools (for C++ bindings compilation)
- NodeJS >= 0.10
- OpenCL 1.2 is natively included

## Linux / Docker 

The easiest way to get the projet running with an Intel CPU or an AMD CPU/GPU/APU is by using AMD APP SDK.

You can use the included Dockerfile for an example with Ubuntu. 

This dockerfile is based on the following guide : http://streamcomputing.eu/blog/2011-06-24/install-opencl-on-debianubuntu-orderly/

## Windows

You need : 

- Visual Studio 2013
- NodeJS >= 0.10
- AMD APP SDK

Please note that while node-gyp should work for compilation, it is possible that another DLL will be used on dynamic linking, leading to errors.

You need to update your path in order to to get AMD APP SDK's OpenCL.dll first in path resolution order.

# Usage

For now you can simply require this project and call native-like functions on the global object returned. 

The API is very close to the low-level one, although there are minor changes when it comes to lengths and, of course, pointers.

A full documentation and a WebCL wrapper might be available soon. For now, you can find examples API calls in the examples folder.

# Troubleshooting

## Drivers issues

Depending on your OpenCL implementation, your OS and the OpenCL implementation you're using, you will get different behaviours that do not 
always correspond to the one given in Khronos specifications. For now we do not correct them, but we have put warnings when running tests that trigger
those behaviours so you can run them to check if it is a known issue. 

## Int 64

Javascript does not support 64 bits integers. OpenCL returns some int64, mainly in getInfo functions. To resolve this, we instead return a data structure
that changes depending on what kind of value is returned : 

- Instead of a 64 bits integer representing nanoseconds time, we return an array with [milliseconds, nanoseconds]
- Instead of a 64 bits integer representing bytes, we return an array with [megabytes, bytes]

## Differences between Node-OpenCL and WebCL

### Raw data type

Buffers are used mostly everywhere in Node.JS. WebCL natively uses TypedArray, but Node OpenCL uses buffers since
they provide a lot more features and copying them into a TyoedArray is hence not needed.

# Contributions Guidelines

Pull requests are welcome ! When you do a PR on this project, you need to respect the following rules : 

- Your JS files must be compliant with the eslint rules
- All tests should pass on as much platforms as possible

For this last step we do understand that it can be pretty difficult so you can do a PR that do not respect this last rule and we will provide you
the results of tests on platforms listed in the Tested Configurations part.

# Getting involved

This project has two goals :

- Providing a low level wrapper for OpenCL APIs
- Allow users to check for bugs related to drivers on their platforms, by running unit tests.

For the first part, we are quite done.

However, for the second, this is much more difficult and you can help us in two ways :

- Reporting issues with drivers that are not listed in unit tests
- Provide a CI project build on a different platform, so we can test new builds with your platform easily

# Licence

This project is under a BSD 2-Clause licence.
