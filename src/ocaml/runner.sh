#!/bin/bash

DIR=$(dirname $0)
eval $(opam env)
cd $DIR && dune $*
