#!/bin/sh

which lcov 1>/dev/null 2>&1
if [ $? != 0 ]
then
    echo "You need to have lcov installed in order to generate the test coverage report"
    exit 1
fi

if [ ! $DIST_ARCHIVES ]
then
    echo "You need to provide the archive name by running DIST_ARCHIVES=\"ARCHIVE_NAME\" $0"
    exit 1
fi

tar zxf $DIST_ARCHIVES
srcdir=${DIST_ARCHIVES/.tar.gz}
cd $srcdir

# Reconfigure with gcov support
CXXFLAGS="-g -O0 --coverage" CFLAGS="-g -O0 --coverage" ./autogen.sh --disable-shared

# Generate gcov output
${MAKE}

# Generate html report
lcov --base-directory . --directory . --zerocounters -q
${MAKE} check
lcov --base-directory . --directory . -c -o libbash_test.info
lcov --remove libbash_test.info "/usr*" -o libbash_test.info # remove output for external libraries
rm -rf ../test_coverage
genhtml -o ../test_coverage -t "libbash test coverage" --num-spaces 4 libbash_test.info

# Clean work space
cd .. && rm -rf $srcdir
