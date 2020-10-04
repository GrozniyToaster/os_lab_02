#! /usr/bin/env bash

set -o errexit
set -o nounset


function log_info()
{
  log_ "info" "$@"
}

function log_error()
{
  log_ "error" "$@"
}

function log_()
{
  local type=$1
  local message=$2
  local date_str=; date_str=$(date +'%Y-%m-%d %H:%M:%S')
  echo "[${type}] [${date_str}] ${message}"
}

function main()
{
  
  local build_dir=build


  local test_dir=tests
  local executable=os_lab1
  
  
  for test_file in $( ls ./tests/*.src ) ; do
    log_info "${test_file}"
    if ! ./${build_dir}/${executable} < ./${test_file}; then
      log_error "Failed to run test"
      return 1
    fi
    
    
  done



}

main $@