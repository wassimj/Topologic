import os, sys
import glob
import subprocess
import sysconfig


def find_wheel(pa_dir, name):
    # construct the wheel file name using the sysconfig vars VERSION and EXT_SUFFIX and using glob
    py_version = sysconfig.get_config_var("VERSION")
    abi_tag = sysconfig.get_config_var('EXT_SUFFIX').split(".")[1]
    wheel_pattern = f"{name}-*-*{py_version}-{abi_tag}.whl"

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