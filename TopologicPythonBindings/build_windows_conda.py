
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
