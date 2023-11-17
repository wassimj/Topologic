import os, sys, platform
import glob
import subprocess
import sysconfig


def find_wheel(pa_dir, name):
    # construct a glob pattern for the wheel file name using sysconfig.get_platform() as the abi tag
    py_version = f"{sys.version_info.major}{sys.version_info.minor}"
    #edit by jake ferrer
    #on mac osx 13(ventura) sys.get_platform() returns 'macosx_11_0_arm64'
    #converting the numbers into wildcard makes it work
    #abi_tag = sysconfig.get_platform()
    #abi_tag = abi_tag.replace("-", "_")
    #abi_tag = abi_tag.replace(".", "_")
    #wheel_pattern = f"{name}-*-*{py_version}-*{abi_tag}.whl"
    wheel_pattern = f"{name}-*-*{py_version}-macosx_*.whl"
    path = os.path.join(pa_dir, wheel_pattern)
    wheels = glob.glob(path)
    if len(wheels) == 0:
        raise FileNotFoundError(f"{wheel_pattern}")
    if len(wheels) > 1:
        raise Exception(f"found more than 1 file for pattern: {wheel_pattern}")

    return wheels[0]


if __name__ == '__main__':
    args = [sys.executable, "-m", "pip", "wheel", ".",
                    "-w", "dist", "-v"]
    subprocess.run(args, check=True)
    
    #edit os.curdir gives "." causing glob matching problems
    #!!! this should be tested on other mac as I dont know if this change breaks anything
    #wheel = find_wheel(os.path.join(os.curdir, "dist"), "topologic")
    path = os.getcwd();
    wheel = find_wheel(os.path.join(path, "dist"), "topologic")

    subprocess.run([sys.executable, "repair_wheel_macos.py",
                    "--wheel", wheel],
                   check=True)
