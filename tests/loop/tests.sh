#!/bin/sh

for entry in *.test; do
    run_test $entry
done
