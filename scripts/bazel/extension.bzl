load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def fetch_repos():
    http_archive(
        name = "emsdk",
        # TODO: Remove repo_mapping when emsdk updates to rules_nodejs 5
        repo_mapping = {"@nodejs": "@nodejs_host"},
        sha256 = "a2609fd97580e4e332acbf49b6cc363714982f06cb6970d54c9789df8e91381c",
        strip_prefix = "emsdk-3.1.23/bazel",
        urls = ["https://github.com/emscripten-core/emsdk/archive/refs/tags/3.1.23.tar.gz"],
    )
