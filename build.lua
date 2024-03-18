local F = require "F"

var "builddir" ".build"

--------------------------------------------------------------------
section "Compilation"
--------------------------------------------------------------------

local cc = "clang"
local ar = "ar"

local cflags = {
    "-std=gnu2x",
    "-O3",
    "-g",
    "-Werror -Wall -Wextra -Weverything",

    "-DUSE_MALLOC",

    "-Isrc",

    "-Wno-unsafe-buffer-usage",
    "-Wno-declaration-after-statement",
}

local sanitifer_flags = {
    "-fno-omit-frame-pointer -fno-optimize-sibling-calls",
    "-fno-sanitize-recover=all",    -- let's crash on undefined behaviour
    "-fsanitize=address",
    "-fsanitize=undefined",
    "-fsanitize=float-divide-by-zero",
    "-fsanitize=unsigned-integer-overflow", -- TBD
    "-fsanitize=implicit-conversion",
    "-fsanitize=local-bounds",
    "-fsanitize=float-cast-overflow",
    "-fsanitize=nullability-arg",
    "-fsanitize=nullability-assign",
    "-fsanitize=nullability-return",
}

file "compile_flags.txt" {
    F.unlines(cflags),
}

local ldflags = {
    "-fsanitize=address",
    "-fsanitize=undefined",
}

local run_options = {
    "export ASAN_OPTIONS=check_initialization_order=1:detect_stack_use_after_return=1:detect_leaks=1;",
    "export UBSAN_OPTIONS=print_stacktrace=1;",
}

rule "compile" {
    description = "CC $in",
    command = { cc, cflags, sanitifer_flags, "-MD -MF $depfile", "-c $in -o $out" },
    depfile = "$out.d",
}

rule "archive" {
    description = "AR $out",
    command = { ar, "-crs $out $in" },
}

rule "link" {
    description = "LD $out",
    command = { cc, ldflags, "$in -o $out" },
}

local lib = build "$builddir/lib/std.a" { "archive",
    ls "src/*.c"
    : map(function(src)
        return build("$builddir/obj"/src:splitext()..".o") { "compile", src }
    end)
}

--------------------------------------------------------------------
section "Tests"
--------------------------------------------------------------------

rule "test" {
    description = "TEST $in",
    command = {
        run_options,
        "$in &> $out || ( cat $out; false )",
    },
}

local tests = ls "tests/*.c"
: map(function(test)
    return build("$builddir"/test:splitext()) { "link",
        build("$builddir/obj"/test:splitext()..".o") { "compile", test },
        lib,
    }
end)

local test_results = tests
: map(function(test)
    return build(test..".log") { "test", test }
end)

--------------------------------------------------------------------
section "Documentation"
--------------------------------------------------------------------

rule "doc_macro.md" {
    description = "DOC $in",
    command = [[echo '@doc "$in"' > $out]],
}

rule "ypp.md" {
    description = "YPP $in",
    command = "ypp --MD --MF $depfile $in -o $out",
    depfile = "$out.d",
}

rule "pandoc.md" {
    description = "PANDOC $out",
    command = "pandoc -t gfm $in -o $out",
}

local h_to_gfm = pipe { "doc_macro.md", "ypp.md", "pandoc.md" }

local doc = ls "src/*.h"
: map(function(header)
    return h_to_gfm("doc"/header:basename():splitext()..".md") { header }
end)

--------------------------------------------------------------------
section "Shortcuts"
--------------------------------------------------------------------

phony "test" { test_results }
phony "doc" { doc }
phony "all" "test doc"

default { "test" }
