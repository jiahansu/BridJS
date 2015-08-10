{
  "targets": [
    {
      'target_name': 'bridjs',
      'sources': [
        'src/array_struct_v8.cc',
        'src/bridjs_module.cc',
        'src/dyncall_v8.cc',
        'src/dyncall_v8_utils.cc',
        'src/dynload_v8.cc',
        'src/native_function_v8.cc',
        'src/pointer_v8.cc',
        'src/struct_v8.cc',
        'src/union_struct_v8.cc',
        'src/dyncallback_v8.cc',
        'src/bridjs_module.cc',
        'src/test.cc',
      ],
      'include_dirs': [
        'deps/dyncall-0.8/include',
      ],
      "msbuild_settings": {
                "Link": {
                    "ImageHasSafeExceptionHandlers": "false"
                }
            },
      'conditions': [
        ['OS=="win"', {
            'conditions': [
                      [ 'target_arch=="ia32"', {
                        'libraries': [
                            '../deps/dyncall-0.8/win32_ia32/libdyncall_s.lib',
                            '../deps/dyncall-0.8/win32_ia32/libdyncallback_s.lib',
                            '../deps/dyncall-0.8/win32_ia32/libdynload_s.lib',
                        ]
                      } ],
                      [ 'target_arch=="x64"', {
                        'libraries': [
                            '../deps/dyncall-0.8/win32_x64/libdyncall_s.lib',
                            '../deps/dyncall-0.8/win32_x64/libdyncallback_s.lib',
                            '../deps/dyncall-0.8/win32_x64/libdynload_s.lib',
                        ]
                      } ],
            ]
        }],
        ['OS=="linux"', {
            'conditions': [
                      [ 'target_arch=="ia32"', {
                        'libraries': [
                            '../deps/dyncall-0.8/linux_ia32/libdyncall_s.a',
                            '../deps/dyncall-0.8/linux_ia32/libdyncallback_s.a',
                            '../deps/dyncall-0.8/linux_ia32/libdynload_s.a',
                        ]
                      } ],
                      [ 'target_arch=="x64"', {
                        'libraries': [
                            '../deps/dyncall-0.8/linux_x64/libdyncall_s.a',
                            '../deps/dyncall-0.8/linux_x64/libdyncallback_s.a',
                            '../deps/dyncall-0.8/linux_x64/libdynload_s.a',
                        ]
                      } ],
            ]
        }],
        ['OS=="mac"', {
                        'libraries': [
                            '../deps/dyncall-0.8/darwin/libdyncall_s.a',
                            '../deps/dyncall-0.8/darwin/libdyncallback_s.a',
                            '../deps/dyncall-0.8/darwin/libdynload_s.a',
                        ],
                        'xcode_settings': {
                        'OTHER_CFLAGS': [
                            "-std=c++11", "-stdlib=libc++", "-O3", "-fexceptions", "-fPIC", "-mmacosx-version-min=10.9"
                      ]},                
      },]
      ],
      "cflags": ["-std=c++11","-O3", "-s", "-fPIC"],
      "cflags_cc": ["-std=c++11", "-O3", "-s", "-fexceptions", "-fPIC"]
    }
  ]
}
