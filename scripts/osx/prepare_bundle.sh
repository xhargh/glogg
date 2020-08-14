#!/bin/bash
macdeployqt ./output/klogg-io.app -always-overwrite -verbose=2
python ../3rdparty/macdeployqtfix/macdeployqtfix.py ./output/klogg-io.app/Contents/MacOS/klogg-io $(brew --prefix qt5)
