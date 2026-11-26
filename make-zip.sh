#!/bin/bash


rm -r *.tar.gz


tar -czf code_xxx.tar.gz --exclude='src/credentials.h' .gitignore platformio.ini src/ lib/ modules/
