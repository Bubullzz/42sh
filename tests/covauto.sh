#!/bin/sh

cd ..
make check
cd src/
gcovr --html --html-nested --output=report.html
