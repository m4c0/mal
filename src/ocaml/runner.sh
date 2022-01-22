#!/bin/bash

set -ex

DIR=$(dirname $0)
eval $(opam env)
cd $DIR && dune exe ./$1.exe
