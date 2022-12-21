# script wrapping the invocation of repair_wheel_windows.py, to be able to use glob to find the actual wheel file on windows
# this script is supposed to be used when building in a conda env, it looks for dlls in $CONDA_PREFIX/Library/bin
# note: glob is used for the wheel file name, but it is expected to be 1 wheel file

import os, sys
import glob
import subprocess


if __name__ == '__main__':
    subprocess.run([sys.executable, "-m", "pip", "wheel", ".",
                    "-w", "dist", "-v"],
                   check=True)

    wheel = glob.glob(os.path.join(os.curdir, "dist", "topologic-*"))[0]

    subprocess.run([sys.executable, "repair_wheel_windows.py",
                    "--wheel", wheel,
                    "--dll-dir", os.environ["CONDA_PREFIX"] + "\\Library\\bin"],
                   check=True)
