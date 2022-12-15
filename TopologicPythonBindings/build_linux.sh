#!/bin/sh
python -m pip wheel . -w dist -v && \
python repair_wheel_linux.py --wheel dist/topologic-*.whl
