{
    'targets': [
    {
      'target_name': 'opencl',
      'defines': [
        'VERSION=0.3.0',
        'NOCL_REALEASE_DRIVER_ISSUES'
      ],
      'sources': [
        'src/addon.cpp',
        'src/common.cpp',
        'src/context.cpp',
        'src/device.cpp',
        'src/event.cpp',
        'src/platform.cpp',
        'src/types.cpp',
        'src/program.cpp',
        'src/kernel.cpp',
        'src/memobj.cpp',
        'src/sampler.cpp',
        'src/commandqueue.cpp',
        'src/pipe.cpp',
        'src/manager.cpp',
        'src/svm.cpp'
      ],
      'include_dirs' : [
        "<!(node -e \"require('nan')\")",
      ],
      'conditions': [
        ['OS=="mac"', {
          'include_dirs' : [
            "<!(echo $OPENCL_HEADER)",
           ],
          'make_global_settings': [
            ['CC', '/usr/bin/clang'],
            ['CXX', '/usr/bin/clang++'],
          ],
          "xcode_settings": {
             'OTHER_CPLUSPLUSFLAGS' : ['-std=c++1y','-stdlib=libc++', ' -Wall'],
             'OTHER_LDFLAGS': ['-stdlib=libc++'],
              'MACOSX_DEPLOYMENT_TARGET': '10.9'
          },
          'libraries': ['-framework OpenCL'],
        }],
        ['OS=="linux"', {
          'variables' : {
            # AMD APP SDK
            'OPENCL_SDK' : '/opt/AMDAPPSDK-3.0-0-Beta',
            'OPENCL_SDK_INCLUDE' : '<(OPENCL_SDK)/include',
            'OPENCL_SDK_LIB' : '<(OPENCL_SDK)/lib/x86_64',
          },
          'include_dirs' : [
            "<(OPENCL_SDK_INCLUDE)",
          ],
          'libraries': ['-lGL', '-lOpenCL','-L<(OPENCL_SDK_LIB) -lOpenCL'],
          'cflags': ['-std=c++11' ' -Wall']
        }],
        ['OS=="win"', {
          'variables' :
            {
              # AMD APP SDK
              'AMD_OPENCL_SDK' : '<!(echo %AMDAPPSDKROOT%)',
              'AMD_OPENCL_SDK_INCLUDE' : '<(AMD_OPENCL_SDK)\\include',
              'AMD_OPENCL_SDK_LIB' : '<(AMD_OPENCL_SDK)\\lib\\x86_64',

              # Intel OpenCL SDK
              'INTEL_OPENCL_SDK' : '<!(echo %INTELOCLSDKROOT%)',
              'INTEL_OPENCL_SDK_INCLUDE' : '<(INTEL_OPENCL_SDK)\\include',
              'INTEL_OPENCL_SDK_LIB' : '<(INTEL_OPENCL_SDK)\\lib\\x64',
            },
            'include_dirs' : [
              "<(AMD_OPENCL_SDK_INCLUDE)", "<(INTEL_OPENCL_SDK_INCLUDE)","<!(echo %OPENCL_HEADER%)",
            ],
            'library_dirs' : [
              "<(AMD_OPENCL_SDK_LIB)", "<(INTEL_OPENCL_SDK_LIB)",
            ],
            'defines' : [
              'WIN32_LEAN_AND_MEAN',
              'VC_EXTRALEAN',
            ],
            'msvs_settings' : {
              'VCCLCompilerTool' : {
                'AdditionalOptions' : ['/O2','/Oy','/GL','/GF','/Gm-','/EHsc','/MT','/GS','/Gy','/GR-','/Gd']
              },
              'VCLinkerTool' : {
                'AdditionalOptions' : ['/OPT:REF','/OPT:ICF','/LTCG']
              },
            },
            'cflags' : [
              '/O2','/Oy','/GL','/GF','/Gm-','/EHsc','/MT','/GS','/Gy','/GR-','/Gd'
            ],
            'ldflags' : [
              '/OPT:REF','/OPT:ICF','/LTCG'
            ],
            'libraries': ['OpenCL.lib'],
          },
       ],
    ]
  }]
}
