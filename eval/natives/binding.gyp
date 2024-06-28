{
  "targets": [
    {
    "target_name": "polyfunc",
    "sources": ["poly.cc"],
    "cflags!": ["-fno-exceptions", "-O2"],
    "cflags_cc!": ["-fno-exceptions", "-O2"],
    "include_dirs": [
      "<!@(node -p \"require('node-addon-api').include\")"
    ]
    }
  ]
}
