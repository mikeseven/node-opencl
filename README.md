# Node OpenCL (TypeScript)

This is a fork of mikeseven's node-opencl project from:
https://github.com/mikeseven/node-opencl (by https://github.com/mikeseven)

I've taken the initiative to renovate the existing code for my own purposes and now I'm sharing the result in hope that it will be useful for anyone else who is lacking typescript support with `node-opencl`. 

This project contains full typings coverage of OpenCL 1.0-2.0 (and OpenCL 2.1, 2.2 ready to be implemented) specification (based on Khronos specification) with node-opencl specifics taken into account. The package also contains couple of minor bugfixes suggested by community PRs in original repo and couple of mine aesthetic interface changes / additions and changes related to management of deprecated OpenCL apis (basically I enabled the use of deprecated APIs if they were still supported by OpenCL implementation). Which I think may block mikeseven from merging current changes into `node-opencl`. Nevertheless in order to not partition the community with redundant library versions I'm open to kill this repo in event of merge with `node-opencl`.

One more thing, the typings are handcrafted, I've attempted to make the typings as verbose and type-safe as possible, this means that there is still room for improvement for both typings and the OpenCL bindings implementation itself (I'm open for both issues and PRs).

Currently there are just couple of v1.0-v2.0 functions that are not implemented (most of them are deprecated in OpenCL 1.2 - 2.0 or not needed at all due to the way `node-opencl` was implemented). OpenCL v2.1 and v2.2 are not implemented at all so again issues and PRs are welcome.

And the last thing I wanted to mention. Due to hardware nature of OpenCL segfaults, BSODs, freezes and computer restarts are a normal thing in OpenCL world. So in order to compensate the risk I've implemented extra measures for type-safety. Basically I've used nominal types everywhere where possible (search for type discrimination). I.e. types which basically allow to distinguish two similar atomic (or non-atomic) type, example:

```typescript
export type Kilos<T> = T & { readonly discriminator: unique symbol };
export type Pounds<T> = T & { readonly discriminator: unique symbol };

export interface MetricWeight {
    value: Kilos<number>
}

export interface ImperialWeight {
    value: Pounds<number>
}

const wm: MetricWeight = { value: 0 as Kilos<number> }
const wi: ImperialWeight = { value: 0 as Pounds<number> }

wm.value = wi.value;                  // Gives compiler error
wi.value = wi.value * 2;              // Gives compiler error
wm.value = wi.value * 2;              // Gives compiler error
const we: MetricWeight = { value: 0 } // Gives compiler error
```

It improves the TypeSafety keeps notion of original type but comes with additional strain of casting `as Kilos<number>` here and there, though it shouldn't be a problem due to risk compensation (I guess).

--------------------

# Node OpenCL

[![Build Status](http://pub-ci.ioweb.fr/api/badge/github.com/ioweb-fr/node-opencl/status.svg?branch=master)](http://pub-ci.ioweb.fr/github.com/ioweb-fr/node-opencl)

## Abstract

This is an early implementation of Node.JS bindings to OpenCL supporting all features of OpenCL up to the latests specification available on Khronos.org.

This implementation is different from node-webcl in the sense that it is close to OpenCL C host methods. A WebCL object model would be available later by simply wrapping the low level methods of node-opencl.

The test/ folder will contain as many unit tests testing all features of OpenCL. These tests are not based on Khronos conformance tests, though they should be similar. The goals of these tests is to check OpenCL drivers to discover their bugs on various platforms and, if possible, implement remedies while companies correct their drivers.

## Current state

The implementation is compliant to OpenCL 1.1, 1.2 and 2.0 in terms of method support. Presence of these methods depend on the version of OpenCL supported by your driver.

Compliant to node.js 4.x architecture and NAN 2.x. Due to the extensive changes in v8, we don't support earlier versions of node.js.

## Tested configurations

This project has been succesfully tested on the following platforms  :

- OSX 10.10 & 10.11 x64 with Intel Iris GPU and NVidia GPU, Native OpenCL implementation (OpenCL 1.2), Node 4.x
- Ubuntu 14.10 & 15.04 x64 with Intel CPU, AMD APP SDK 3 (OpenCL 2.0), Node 4.x (Dockerfile is available in docker folder)
- Windows 8.1 & 10 x64 with Intel and AMD GPU, AMD APU, AMD APP SDK 3 (OpenCL 2.0), Node 4.x

We only support 64-bit operating systems, though 32-bit OS should work too.

# Installation

Dependencies:
- npm and node-gyp
- C++11 compatible compiler
- OpenCL headers and library (e.g. the AMD APP SDK)

Except specific platform installation guides below, you only need to install the package through

```npm install```

To run unit tests, you will need mocha as a global package. Then you can simply call

```npm test```

## Mac OS X

You should only need :

- XCode Developers tools (for C++ bindings compilation)
- NodeJS >= 4.x
- OpenCL 1.2 is natively included

## Linux / Docker

The easiest way to get the projet running with an Intel CPU or an AMD CPU/GPU/APU is by using AMD APP SDK.

You can use the included Dockerfile for an example with Ubuntu.

This dockerfile is based on the following guide : http://streamcomputing.eu/blog/2011-06-24/install-opencl-on-debianubuntu-orderly/

## Windows

You need at least:

- Visual Studio 2013
- NodeJS >= 4.x
- AMD APP SDK 3.0 for OpenCL runtime, or similar from other vendors eg Intel, NVidia...

Please note that while node-gyp should work for compilation, it is possible that another DLL will be used on dynamic linking, leading to errors.

You need to update your path in order to to get AMD APP SDK's OpenCL.dll first in path resolution order.

# Usage

For now you can simply require this project and call native-like functions on the global object returned.

The API is very close to the low-level one, although there are minor changes when it comes to lengths and, of course, pointers.

# Troubleshooting

## Drivers issues

Depending on your OS and the OpenCL implementation you're using, you will get different behaviours that do not
always correspond to the one given in Khronos specifications. For now we do not correct them, but we have put warnings when running tests that trigger
those behaviours so you can run them to check if it is a known issue.

## Int 64

Javascript does not support 64 bits integers however OpenCL returns Int64 values for mem-size related functions like `getDeviceInfo`, `getKernelWorkGroupInfo` and time-related function  `getEventProfilingInfo`. In order to deal with the limitation for `getDeviceInfo`, `getKernelWorkGroupInfo` would return the amounts in kilobytes. For `getEventProfilingInfo` we return an array of two 32 bit integers that form the resulting Int64 value, if necessary the resulting value can be gathered together using formula `value = (hi << 32) | lo` however in absolute majority of cases only lower 32 bits of 64 bit integer are required.

## Differences between Node-OpenCL and WebCL

### OpenCL support

- WebCL (and node-webcl) was designed for OpenCL 1.1 with few extensions from OpenCL 1.2.
- node-opencl is designed to support all versions of OpenCL, currently up to OpenCL 2.0
- WebCL follows an object-oriented model of OpenCL specification
- node-opencl follows the C nature of OpenCL specification. It is thus a one-to-one low-level wrapper of OpenCL. This allows better control of native resources, quick upgrade when OpenCL specification changes. Importantly, it allows developers to create higher-level APIs that fit their needs, relying on an implementation close to OpenCL driver.

### User events

By adding true instead of creating a cl.Event (as in webcl) to any enqueueXXX() methods, the enqueueXXX() returns a cl.Event that can be used to coordinate calls, profiling etc...

### Javascript Array not supported

- due to changes in v8, we don't support Javascript arrays for OpenCL buffers
- only node.js Buffer and Javascript ArrayBuffer/TypedArrays are supported for OpenCL buffers.

### Raw data type

Due to changes in Chrome v8 embedded in node 4.x, buffers can only be ArrayBuffer or Buffer, or derivative. Using Javascript Array will throw exceptions.

# Contributions Guidelines

Pull requests are welcome! When you do a PR on this project, you need to respect the following rules :

- Your JS files must be compliant with the eslint rules
- All tests should pass on as much platforms as possible

# Licence

This project is under a BSD 3-Clause licence.
