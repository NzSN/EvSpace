{
  "targets": [
    {
    "target_name": "evaln",
    "sources": [
      "module.cc",

      "base.cc",

      "poly.cc",
      "../evals/poly.cc",

      "sort.cc",
      "../evals/sort.cc"],
    "cflags!": ["-fno-exceptions"],
    "cflags_cc!": ["-fno-exceptions"],
    "include_dirs": [
      "<!@(node -p \"require('node-addon-api').include\")"
    ]
    }
  ]
}
