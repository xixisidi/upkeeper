#!/bin/sh
if [ -f Makefile ]
then
    make clean
fi
LD_RUN_PATH="/home/y/lib64" LDFLAGS=-L/home/y/lib64 CPPFLAGS=-I/home/y/include  ./configure --prefix=/home/y --enable-rpath=/home/y/lib64
make
rm -rf test-results
mkdir test-results
/home/y/bin/prove --timer --formatter TAP::Formatter::JUnit adm/y/ptest >test-results/bogus.xml
