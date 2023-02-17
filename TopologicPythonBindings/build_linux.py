import os, sys
import glob
import subprocess
import sysconfig


def find_wheel(pa_dir, name):
    # construct a glob pattern for the wheel file name using sysconfig.get_platform() as the abi tag
    py_version = f"{sys.version_info.major}{sys.version_info.minor}"
    abi_tag = sysconfig.get_platform().replace("-", "_")
    wheel_pattern = f"{name}-*-*{py_version}-*{abi_tag}.whl"

    wheels = glob.glob(os.path.join(pa_dir, wheel_pattern))
    if len(wheels) == 0:
        raise FileNotFoundError(f"{wheel_pattern}")
    if len(wheels) > 1:
        raise Exception(f"found more than 1 file for pattern: {wheel_pattern}")

    return wheels[0]


if __name__ == '__main__':
    subprocess.run([sys.executable, "-m", "pip", "wheel", ".",
                    "-w", "dist", "-v"],
                   check=True)

    wheel = find_wheel(os.path.join(os.curdir, "dist"), "topologic")

    subprocess.run([sys.executable, "repair_wheel_linux.py",
                    "--wheel", wheel],
                   check=True)
