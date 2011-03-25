#!/bin/sh
autoreconf -W all -i && ./configure --enable-gtest --enable-developer
