#!/bin/sh
mkdir -p m4
mkdir -p config/
touch config/config.rpath
autoreconf -i -s
