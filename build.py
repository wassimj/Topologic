#!/usr/bin/env python3

# script to wrap invocation of cmake generation, build, install


import logging
logger = logging.getLogger(__name__)
log = logger.info

import sys, os

import argparse
import subprocess, shlex


DRY_RUN = False
VERBOSE = False


def syscmd(cmd, args=None):
    def cmd_to_argv(cmd):
        lex = shlex.shlex(cmd, posix=True)
        lex.whitespace_split = True
        lex.escape = []
        return list(lex)

    cmd_argv = cmd_to_argv(cmd)
    log(f"running subprocess: {repr(cmd_argv)}")
    if DRY_RUN: return
    proc = subprocess.run([*cmd_argv], check=True)


def _cmd_gen_add_extra_args(cmd, args):
    extras = args.extra_cmake_args if args.extra_cmake_args else []
    for x in extras:
        cmd = cmd + " " + x
    if "TOPOLOGIC_EXTRA_CMAKE_ARGS" in os.environ:
        cmd = cmd + " " + os.environ['TOPOLOGIC_EXTRA_CMAKE_ARGS']
    return cmd


def _cmd_build_add_target(cmd, args):
    if args.build_target:
        cmd = cmd + " " + f"--target '{args.build_target}'"
    return cmd


def _cmd_build_add_extra_args(cmd, args):
    if VERBOSE:
        cmd = cmd + " -v"
    return cmd


def _cmd_install(args):
    if not args.install_to: raise ValueError()
    cmd = f"cmake --install '{args.build_dir}' --prefix '{args.install_to}'"
    if args.install_component:
        cmd = cmd + " " + f"--component '{args.install_component}'"
    return cmd


def build_win32(args):
    # letting cmake decide the generator on windows
    cmd = f"cmake -S '{args.source_dir}' -B '{args.build_dir}' -DCMAKE_BUILD_TYPE='{args.build_type}' -DCMAKE_CONFIGURATION_TYPES='{args.build_type}'"
    cmd = _cmd_gen_add_extra_args(cmd, args)
    syscmd(cmd)
    cmd = f"cmake --build '{args.build_dir}' --config '{args.build_type}'"
    cmd = _cmd_build_add_target(cmd, args)
    cmd = _cmd_build_add_extra_args(cmd, args)
    syscmd(cmd)
    return


def build_linux(args):
    # use ninja on linux
    cmd = f"cmake -S '{args.source_dir}' -G Ninja -B '{args.build_dir}' -DCMAKE_BUILD_TYPE='{args.build_type}'"
    cmd = _cmd_gen_add_extra_args(cmd, args)
    syscmd(cmd)
    cmd = f"cmake --build '{args.build_dir}'"
    cmd = _cmd_build_add_target(cmd, args)
    cmd = _cmd_build_add_extra_args(cmd, args)
    syscmd(cmd)
    return


def build_darwin(args):
    # use ninja on moacos
    cmd = f"cmake -S '{args.source_dir}' -G Ninja -B '{args.build_dir}' -DCMAKE_BUILD_TYPE='{args.build_type}'"
    cmd = _cmd_gen_add_extra_args(cmd, args)
    syscmd(cmd)
    cmd = f"cmake --build '{args.build_dir}'"
    cmd = _cmd_build_add_target(cmd, args)
    cmd = _cmd_build_add_extra_args(cmd, args)
    syscmd(cmd)
    return


def main(args):
    build_funcs = {}
    build_funcs["win32"] = build_win32
    build_funcs["linux"] = build_linux
    build_funcs["darwin"] = build_darwin

    if not args.target_platform: args.target_platform = sys.platform
    args.target_platform = args.target_platform.lower()
    if args.target_platform not in build_funcs.keys():
        raise NotImplementedError(f"target platform not supported: {args.target_platform}")


    args.build_type = args.build_type.lower()
    if args.build_type not in ["debug", "release"]:
        raise ValueError(f"invalid build type: {args.build_type}")
    if args.build_type == "debug": args.build_type = "Debug"
    if args.build_type == "release": args.build_type = "Release"


    global DRY_RUN, VERBOSE
    if args.dry_run: DRY_RUN = True
    if args.verbose: VERBOSE = True

    build_funcs[args.target_platform](args)
    if args.install_to:
        cmd_install = _cmd_install(args)
        syscmd(cmd_install)


def script():
    logformat = "%(levelname)s:%(module)s:%(funcName)s: %(message)s"
    logging.basicConfig(level=logging.INFO, format=logformat)

    p = argparse.ArgumentParser()
    p.add_argument("--target-platform", metavar="<name>")
    p.add_argument("--build-type", required=True, metavar="<debug|release>")
    p.add_argument("--source-dir", required=True, metavar="<path>")
    p.add_argument("--build-dir", required=True, metavar="<path>")
    p.add_argument("--build-target", required=False, metavar="<name>")
    p.add_argument("--install-to", required=False, metavar="<path>")
    p.add_argument("--install-component", required=False, metavar="<name>")
    p.add_argument("--dry-run", action='store_true')
    p.add_argument("--verbose", action='store_true')
    p.add_argument('--extra-cmake-args', nargs=argparse.REMAINDER, metavar="<args>")
    args = p.parse_args()

    main(args)


if __name__ == '__main__':
    script()
