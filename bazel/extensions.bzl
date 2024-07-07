load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")


def _ext_impl(ctx):
    new_git_repository(
        name = "proxy",
        remote = "https://github.com/microsoft/proxy.git",
        branch = "main",
        build_file_content =
        """
load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")

filegroup(
    name = "proxy_srcs",
    srcs = glob(["**"]),
)

cmake(
    name = "proxy",
    lib_source = ":proxy_srcs",
    out_include_dir = "include",
    out_headers_only = True,
    generate_args = ["-DBUILD_TESTING=off"],
    visibility = ["//visibility:public"],
)
        """)

ext = module_extension(
   implementation = _ext_impl
)
