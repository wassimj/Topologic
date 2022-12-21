#!/bin/sh
python -m pip wheel . -w dist -v && \
python repair_wheel_macos.py --wheel dist/topologic-*.whl

# note: glob is used for the wheel file name, but it is expected to be 1 wheel file