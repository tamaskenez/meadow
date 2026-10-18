#!/bin/bash -e

use_deployer=0
for arg in "$@"; do
	if [[ "$arg" == "--use-deployer" ]]; then
		use_deployer=1
	fi
done

# Keep in sync with CMAKE_CXX_STANDARD in CMakeLists.txt
UNAME_S=$(uname -s)
if [[ $UNAME_S == "Linux" ]]; then
	cppstd=gnu26
elif [[ $UNAME_S == "Darwin" ]]; then
	cppstd=gnu23
else
	cppstd=23
fi

deployer_flags=()
if [[ $use_deployer -eq 1 ]]; then
	deployer_flags=(--deployer=full_deploy --deployer-folder=id)
fi

# Local recipe, see conan/abseil/conandata.yml
conan export conan/abseil --version 20261017.0-git.017286b

for build_type in Debug Release; do
	conan install conanfile.txt \
		-b missing \
		-pr:b default \
		-of id/cmake \
		"${deployer_flags[@]}" \
		-s build_type=${build_type} \
		-s compiler.cppstd=${cppstd}
done
