#!/bin/bash -e

if [[ $(uname -s) == "Darwin" ]]; then
	cppstd=gnu23
else
	cppstd=23
fi

for build_type in Debug Release; do
	conan install conanfile.txt \
		-b missing \
		-pr:b default \
		-of id/cmake \
		-s build_type=Debug \
		-s compiler.cppstd=${cppstd}
done
