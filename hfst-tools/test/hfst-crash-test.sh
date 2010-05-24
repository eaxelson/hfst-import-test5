#! /bin/bash

$*
error_code=$?

if test $error_code == 1; then
exit 0
fi;

exit $error_code
