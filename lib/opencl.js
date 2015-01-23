"use strict";

var cl = require('bindings')('opencl.node');

module.exports = cl;

global.WebCLPlatform=cl.WebCLPlatform=function (_) { this._ = _; }
global.WebCLDevice=cl.WebCLDevice=function (_) { this._ = _; }
global.WebCLContext=cl.WebCLContext=function (_) { this._ = _; }
global.WebCLCommandQueue=cl.WebCLCommandQueue=function (_) { this._ = _; }
global.WebCLMemObject=cl.WebCLMemObject=function (_) { this._ = _; }
global.WebCLEvent=cl.WebCLEvent=function (_) { this._ = _; }
global.WebCLProgram=cl.WebCLProgram=function (_) { this._ = _; }
global.WebCLKernel=cl.WebCLKernel=function (_) { this._ = _; }
global.WebCLSampler=cl.WebCLSampler=function (_) { this._ = _; }

/********************************************************************************************************/
// errors
cl.ErrorDesc=function(err)
{
  switch (err) {
    case cl.SUCCESS:                            return "Success!";
    case cl.DEVICE_NOT_FOUND:                   return "Device not found.";
    case cl.DEVICE_NOT_AVAILABLE:               return "Device not available";
    case cl.COMPILER_NOT_AVAILABLE:             return "Compiler not available";
    case cl.MEM_OBJECT_ALLOCATION_FAILURE:      return "Memory object allocation failure";
    case cl.OUT_OF_RESOURCES:                   return "Out of resources";
    case cl.OUT_OF_HOST_MEMORY:                 return "Out of host memory";
    case cl.PROFILING_INFO_NOT_AVAILABLE:       return "Profiling information not available";
    case cl.MEM_COPY_OVERLAP:                   return "Memory copy overlap";
    case cl.IMAGE_FORMAT_MISMATCH:              return "Image format mismatch";
    case cl.IMAGE_FORMAT_NOT_SUPPORTED:         return "Image format not supported";
    case cl.BUILD_PROGRAM_FAILURE:              return "Program build failure";
    case cl.MAP_FAILURE:                        return "Map failure";
    case cl.MISALIGNED_SUB_BUFFER_OFFSET:       return "Misaligned sub-buffer offset";
    case cl.EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: return "Execution status error for events in wait list";
    case cl.COMPILE_PROGRAM_FAILURE:            return "Compile program failure";
    case cl.LINKER_NOT_AVAILABLE:               return "Linker not available";
    case cl.LINK_PROGRAM_FAILURE:               return "Link program failure";
    case cl.DEVICE_PARTITION_FAILED:            return "Device partition failed";
    case cl.KERNEL_ARG_INFO_NOT_AVAILABLE:      return "Kernel argument info not available";

    case cl.INVALID_VALUE:                      return "Invalid value";
    case cl.INVALID_DEVICE_TYPE:                return "Invalid device type";
    case cl.INVALID_PLATFORM:                   return "Invalid platform";
    case cl.INVALID_DEVICE:                     return "Invalid device";
    case cl.INVALID_CONTEXT:                    return "Invalid context";
    case cl.INVALID_QUEUE_PROPERTIES:           return "Invalid queue properties";
    case cl.INVALID_COMMAND_QUEUE:              return "Invalid command queue";
    case cl.INVALID_HOST_PTR:                   return "Invalid host pointer";
    case cl.INVALID_MEM_OBJECT:                 return "Invalid memory object";
    case cl.INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "Invalid image format descriptor";
    case cl.INVALID_IMAGE_SIZE:                 return "Invalid image size";
    case cl.INVALID_SAMPLER:                    return "Invalid sampler";
    case cl.INVALID_BINARY:                     return "Invalid binary";
    case cl.INVALID_BUILD_OPTIONS:              return "Invalid build options";
    case cl.INVALID_PROGRAM:                    return "Invalid program";
    case cl.INVALID_PROGRAM_EXECUTABLE:         return "Invalid program executable";
    case cl.INVALID_KERNEL_NAME:                return "Invalid kernel name";
    case cl.INVALID_KERNEL_DEFINITION:          return "Invalid kernel definition";
    case cl.INVALID_KERNEL:                     return "Invalid kernel";
    case cl.INVALID_ARG_INDEX:                  return "Invalid argument index";
    case cl.INVALID_ARG_VALUE:                  return "Invalid argument value";
    case cl.INVALID_ARG_SIZE:                   return "Invalid argument size";
    case cl.INVALID_KERNEL_ARGS:                return "Invalid kernel arguments";
    case cl.INVALID_WORK_DIMENSION:             return "Invalid work dimension";
    case cl.INVALID_WORK_GROUP_SIZE:            return "Invalid work group size";
    case cl.INVALID_WORK_ITEM_SIZE:             return "Invalid work item size";
    case cl.INVALID_GLOBAL_OFFSET:              return "Invalid global offset";
    case cl.INVALID_EVENT_WAIT_LIST:            return "Invalid event wait list";
    case cl.INVALID_EVENT:                      return "Invalid event";
    case cl.INVALID_OPERATION:                  return "Invalid operation";
    case cl.INVALID_GL_OBJECT:                  return "Invalid OpenGL object";
    case cl.INVALID_BUFFER_SIZE:                return "Invalid buffer size";
    case cl.INVALID_MIP_LEVEL:                  return "Invalid mip-map level";
    case cl.INVALID_GLOBAL_WORK_SIZE:           return "Invalid global work size";
    case cl.INVALID_PROPERTY:                   return "Invalid property";
    case cl.INVALID_IMAGE_DESCRIPTOR:           return "Invalid image descriptor";
    case cl.INVALID_COMPILER_OPTIONS:           return "Invalid compiler options";
    case cl.INVALID_LINKER_OPTIONS:             return "Invalid linker options";
    case cl.INVALID_DEVICE_PARTITION_COUNT:     return "Invalid device partition count";
  }
  return "Unknown";
}

/********************************************************************************************************/

// /* Platform API */
// cl.GetPlatformIDs();

// cl.GetPlatformInfo(platform,
//                   param_name);

// /* Device APIs */
// cl.GetDeviceIDs(platform,
//                device_type,
//                devices);

// cl.GetDeviceInfo(device,
//                 param_name);

// if(cl.CL_VERSION_1_2) {
//   cl.CreateSubDevices(in_device,
//                    properties,
//                    out_devices);

//   cl.RetainDevice(device);

//   cl.ReleaseDevice(device);
// }

// /* Context APIs  */
// cl.CreateContext(properties,
//                 devices,
//                 callback,
//                 user_data);

// cl.CreateContextFromType(properties,
//                         device_type,
//                         callback,
//                         user_data);

// cl.RetainContext(context);

// cl.ReleaseContext(context);

// cl.GetContextInfo(context,
//                  param_name);

// /* Command Queue APIs */
// cl.CreateCommandQueue(context,
//                      device,
//                      properties);

// cl.RetainCommandQueue(command_queue);

// cl.ReleaseCommandQueue(command_queue);

// cl.GetCommandQueueInfo(command_queue,
//                       param_name);

// /* Memory Object APIs */
// cl.CreateBuffer(context,
//                flags,
//                size,
//                host_ptr);

// cl.CreateSubBuffer(buffer,
//                   flags,
//                   buffer_create_type,
//                   buffer_create_info);

// if(cl.CL_VERSION_1_2) {
//   cl.CreateImage(context,
//               flags,
//               image_format,
//               image_desc,
//               host_ptr);
// }

// cl.RetainMemObject(memobj);

// cl.ReleaseMemObject(memobj);

// cl.GetSupportedImageFormats(context,
//                            flags,
//                            image_type);

// cl.GetMemObjectInfo(memobj,
//                    param_name);

// cl.GetImageInfo(image,
//                param_name);

// cl.SetMemObjectDestructorCallback(memobj,
//                                  callback,
//                                  user_data);

// /* Sampler APIs */
// cl.CreateSampler(context,
//                 normalized_coords,
//                 addressing_mode,
//                 filter_mode);

// cl.RetainSampler(sampler);

// cl.ReleaseSampler(sampler);

// cl.GetSamplerInfo(sampler,
//                  param_name);

// /* Program Object APIs  */
// cl.CreateProgramWithSource(context,
//                           source);

// cl.CreateProgramWithBinary(context,
//                           device_list,
//                           binaries,
//                           binary_status);

// if(cl.CL_VERSION_1_2) {
//   cl.CreateProgramWithBuiltInKernels(context,
//                                   device_list,
//                                   kernel_names);
// }

// cl.RetainProgram(program);

// cl.ReleaseProgram(program);

// cl.BuildProgram(program,
//                device_list,
//                options,
//                callback,
//                user_data);

// if(cl.CL_VERSION_1_2) {
//   cl.CompileProgram(program,
//                    device_list,
//                    options,
//                    input_headers,
//                    header_include_names,
//                    callback,
//                    user_data);

//   cl.LinkProgram(context,
//                 device_list,
//                 options,
//                 input_programs,
//                 callback,
//                 user_data);

//   cl.UnloadPlatformCompiler(platform);
// }

// cl.GetProgramInfo(program,
//                  param_name);

// cl.GetProgramBuildInfo(program,
//                       device,
//                       param_name);

// /* Kernel Object APIs */
// cl.CreateKernel(program,
//                kernel_name);

// cl.CreateKernelsInProgram(program,
//                          kernels);

// cl.RetainKernel(kernel);

// cl.ReleaseKernel(kernel);

// cl.SetKernelArg(kernel,
//                arg_index,
//                arg_size,
//                arg_value);

// cl.GetKernelInfo(kernel,
//                 param_name);

// if(cl.CL_VERSION_1_2)
//   cl.GetKernelArgInfo(kernel,
//                    arg_indx,
//                    param_name);
// }

// cl.GetKernelWorkGroupInfo(kernel,
//                          device,
//                          param_name);

// /* Event Object APIs */
// cl.WaitForEvents(event_list );

// cl.GetEventInfo(event,
//                param_name);

// cl.CreateUserEvent(context);

// cl.RetainEvent(event);

// cl.ReleaseEvent(event);

// cl.SetUserEventStatus(event,
//                      execution_status);

// cl.SetEventCallback( event,
//                     command_exec_callback_type,
//                     callback,
//                     user_data);

// /* Profiling APIs */
// cl.GetEventProfilingInfo(event,
//                         param_name);

// /* Flush and Finish APIs */
// cl.Flush(command_queue);

// cl.Finish(command_queue);

// /* Enqueued Commands APIs */
// cl.EnqueueReadBuffer(command_queue,
//                     buffer,
//                     blocking_read,
//                     offset,
//                     size,
//                     ptr,
//                     event_wait_list,
//                     event);

// cl.EnqueueReadBufferRect(command_queue,
//                         buffer,
//                         blocking_read,
//                         buffer_offset,
//                         host_offset,
//                         region,
//                         buffer_row_pitch,
//                         buffer_slice_pitch,
//                         host_row_pitch,
//                         host_slice_pitch,
//                         ptr,
//                         event_wait_list,
//                         event);

// cl.EnqueueWriteBuffer(command_queue,
//                      buffer,
//                      blocking_write,
//                      offset,
//                      size,
//                      ptr,
//                      event_wait_list,
//                      event);

// cl.EnqueueWriteBufferRect(command_queue,
//                          buffer,
//                          blocking_write,
//                          buffer_offset,
//                          host_offset,
//                          region,
//                          buffer_row_pitch,
//                          buffer_slice_pitch,
//                          host_row_pitch,
//                          host_slice_pitch,
//                          ptr,
//                          event_wait_list,
//                          event);

// if(c.CL_VERSION_1_2) {
//   cl.EnqueueFillBuffer(command_queue,
//                       buffer,
//                       pattern,
//                       pattern_size,
//                       offset,
//                       size,
//                       event_wait_list,
//                       event);
// }

// cl.EnqueueCopyBuffer(command_queue,
//                     src_buffer,
//                     dst_buffer,
//                     src_offset,
//                     dst_offset,
//                     size,
//                     event_wait_list,
//                     event);

// cl.EnqueueCopyBufferRect(command_queue,
//                         src_buffer,
//                         dst_buffer,
//                         src_origin,
//                         dst_origin,
//                         region,
//                         src_row_pitch,
//                         src_slice_pitch,
//                         dst_row_pitch,
//                         dst_slice_pitch,
//                         event_wait_list,
//                         event);

// cl.EnqueueReadImage(command_queue,
//                    image,
//                    blocking_read,
//                    origin[3],
//                    region[3],
//                    row_pitch,
//                    slice_pitch,
//                    ptr,
//                    event_wait_list,
//                    event);

// cl.EnqueueWriteImage(command_queue,
//                     image,
//                     blocking_write,
//                     origin[3],
//                     region[3],
//                     input_row_pitch,
//                     input_slice_pitch,
//                     ptr,
//                     event_wait_list,
//                     event);

// if(cl.CL_VERSION_1_2) {
//   cl.EnqueueFillImage(command_queue,
//                    image,
//                    fill_color,
//                    origin[3],
//                    region[3],
//                    event_wait_list,
//                    event);
// }

// cl.EnqueueCopyImage(command_queue,
//                    src_image,
//                    dst_image,
//                    src_origin[3],
//                    dst_origin[3],
//                    region[3],
//                    event_wait_list,
//                    event);

// cl.EnqueueCopyImageToBuffer(command_queue,
//                            src_image,
//                            dst_buffer,
//                            src_origin[3],
//                            region[3],
//                            dst_offset,
//                            event_wait_list,
//                            event);

// cl.EnqueueCopyBufferToImage(command_queue,
//                            src_buffer,
//                            dst_image,
//                            src_offset,
//                            dst_origin[3],
//                            region[3],
//                            event_wait_list,
//                            event);

// cl.EnqueueMapBuffer(command_queue,
//                    buffer,
//                    blocking_map,
//                    map_flags,
//                    offset,
//                    size,
//                    event_wait_list,
//                    event);

// cl.EnqueueMapImage(command_queue,
//                   image,
//                   blocking_map,
//                   map_flags,
//                   origin[3],
//                   region[3],
//                   event_wait_list,
//                   event);

// cl.EnqueueUnmapMemObject(command_queue,
//                         memobj,
//                         mapped_ptr,
//                         event_wait_list,
//                         event);

// if(cl.CL_VERSION_1_2) {
//   cl.EnqueueMigrateMemObjects(command_queue,
//                            mem_objects,
//                            flags,
//                            event_wait_list,
//                            event);
// }

// cl.EnqueueNDRangeKernel(command_queue,
//                        kernel,
//                        work_dim,
//                        global_work_offset,
//                        global_work_size,
//                        local_work_size,
//                        event_wait_list,
//                        event);

// cl.EnqueueTask(command_queue,
//               kernel,
//               event_wait_list,
//               event);

// cl.EnqueueNativeKernel(command_queue,
//                       callback,
//                       args,
//                       mem_list,
//                       event_wait_list,
//                       event);

// if(cl.CL_VERSION_1_2) {
//   cl.EnqueueMarkerWithWaitList(command_queue,
//                             event_wait_list,
//                             event);

//   cl.EnqueueBarrierWithWaitList(command_queue,
//                              event_wait_list,
//                              event);
// }

// // Deprecated OpenCL 1.1 APIs
// cl.CreateImage2D(context,
//                 flags,
//                 image_format,
//                 image_width,
//                 image_height,
//                 image_row_pitch,
//                 host_ptr);

// cl.CreateImage3D(context,
//                 flags,
//                 image_format,
//                 image_width,
//                 image_height,
//                 image_depth,
//                 image_row_pitch,
//                 image_slice_pitch,
//                 host_ptr);

// cl.EnqueueMarker(command_queue,
//                 event);

// cl.EnqueueWaitForEvents(command_queue,
//                        event_list);

// cl.EnqueueBarrier(command_queue);

// cl.UnloadCompiler();

/********************************************************************************************************/
/* Extensions */
/********************************************************************************************************/

/* GL */
// cl.CreateEventFromGLsyncKHR(context,
//                            cl_GLsync);

