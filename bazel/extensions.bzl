load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")


def _ext_impl(ctx):
    ###########################################################################
    #                                  Proxy                                  #
    ###########################################################################
    new_git_repository(
        name = "proxy",
        remote = "https://github.com/microsoft/proxy.git",
        branch = "main",
        build_file_content =
        """
filegroup(
    name = "proxy-srcs",
    srcs = ["proxy.h"],
)

cc_library(
    name = "proxy",
    hdrs = [":proxy-srcs"],
    include_prefix = "proxy",
    visibility = ["//visibility:public"],
)
        """)

    ###########################################################################
    #                                Rapidcheck                               #
    ###########################################################################
    new_git_repository(
        name = "rapidcheck",
        remote = "https://github.com/emil-e/rapidcheck.git",
        branch = "master",
        build_file_content =
        """
load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")

filegroup(
    name = "rapidcheck_src",
    srcs = glob(["**"]),
)

cmake(
    name = "rapidcheck",
    lib_source = "rapidcheck_src",
    out_static_libs = ["librapidcheck.a"],
    out_include_dir = "include",
    generate_args = ["-DRC_ENABLE_GTEST=ON"],
    visibility = ["//visibility:public"],
)
        """)

ext = module_extension(
   implementation = _ext_impl
)
