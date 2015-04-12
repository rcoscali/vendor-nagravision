#!/bin/bash

echo "** running checkRecordCrypto ..."
./checkRecordCrypto
ret=$?

echo "** running onSaveRights ..."
./onSaveRights
ret=$(( $ret & $? ))

echo "** running onCheckRightsStatus ..."
./onCheckRightsStatus
ret=$(( $ret & $? ))

echo "** running dbTest ..."
./dbTest
ret=$(( $ret & $? ))

test $ret && echo "*** ALL TESTS PASSED" || echo "*** SOME TESTS FAILED"

