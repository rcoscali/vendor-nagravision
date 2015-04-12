#!/bin/bash

echo "** running decryptTest ..."
./decryptTest
ret=$?

test $ret && echo "*** ALL TESTS PASSED" || echo "*** SOME TESTS FAILED"

