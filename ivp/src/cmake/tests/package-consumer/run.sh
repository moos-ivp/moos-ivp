#!/usr/bin/env bash

# Build, install, and consume a non-GUI IVP package in an isolated directory.
set -euo pipefail

script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
repo_root=$(cd "${script_dir}/../../../../.." && pwd)

moos_dir=${MOOS_DIR:-"${repo_root}/build/MOOS/MOOSCore"}
moosgeodesy_dir=${MOOSGeodesy_DIR:-"${repo_root}/build/MOOS/MOOSGeodesy"}

if [[ ! -f "${moos_dir}/MOOSConfig.cmake" ]]; then
  echo "MOOSConfig.cmake not found under MOOS_DIR: ${moos_dir}" >&2
  exit 1
fi

if [[ ! -f "${moosgeodesy_dir}/MOOSGeodesyConfig.cmake" ]]; then
  echo "MOOSGeodesyConfig.cmake not found under MOOSGeodesy_DIR: ${moosgeodesy_dir}" >&2
  exit 1
fi

test_dir=$(mktemp -d "${TMPDIR:-/tmp}/ivp-package-consumer.XXXXXX")
if [[ "${IVP_PACKAGE_TEST_KEEP:-0}" != "1" ]]; then
  trap 'rm -rf "${test_dir}"' EXIT
fi

ivp_build_dir="${test_dir}/ivp-build"
install_prefix="${test_dir}/ivp-prefix"
consumer_build_dir="${test_dir}/consumer-build"

cmake -S "${repo_root}/ivp/src" -B "${ivp_build_dir}" \
  -DIVP_BUILD_GUI_CODE=OFF \
  -DIVP_LIB_DIRECTORY="${ivp_build_dir}/lib" \
  -DIVP_INC_DIRECTORY="${ivp_build_dir}/include" \
  -DIVP_BIN_DIRECTORY="${ivp_build_dir}/bin" \
  -DMOOS_DIR="${moos_dir}" \
  -DMOOSGeodesy_DIR="${moosgeodesy_dir}"
cmake --build "${ivp_build_dir}" --parallel
cmake --install "${ivp_build_dir}" --prefix "${install_prefix}"

cmake -S "${script_dir}" -B "${consumer_build_dir}" \
  -DCMAKE_PREFIX_PATH="${install_prefix}" \
  -DMOOS_DIR="${moos_dir}" \
  -DMOOSGeodesy_DIR="${moosgeodesy_dir}"
cmake --build "${consumer_build_dir}" --parallel
"${consumer_build_dir}/ivp_package_consumer"

echo "IVP package-consumer test passed."
if [[ "${IVP_PACKAGE_TEST_KEEP:-0}" == "1" ]]; then
  echo "Test artifacts retained in: ${test_dir}"
fi
