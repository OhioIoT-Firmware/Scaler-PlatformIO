#!/bin/bash


rm -f *.zip


zip -r code.zip .gitignore src/ lib/ modules/ -x src/credentials.h
