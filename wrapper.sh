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
  log_info "Stage #1 Compiling..."
  local build_dir=build

  rm -rf ${build_dir}
  mkdir ${build_dir}
  cp Makefile ./${build_dir}
  cp *.c ./${build_dir}
  if ! make --directory=${build_dir} debug  ; then
    log_error "Failed to compile."
    return 1
  fi

  local test_dir=tests
  local executable=os_lab1
  local test_genrator=generator
  if [ $# == 1 ] && [ $1 == -gen ] ; then
    rm -rf ${test_dir}
    mkdir ${test_dir}
    cp run.sh ./${test_dir}
    log_info "Stage #2 Test generating..."
    if ! python3 ${test_genrator}.py ${test_dir} ; then
      log_error "Failed to generate tests."
      return 1
    fi
  else 
  log_info "Stage #2 Test generating skipped (-gen to generate)"
  fi

  log_info "Stage #3 Checking..."
  
    if ! ./${test_dir}/run.sh ; then
      log_error "Failed to run test"
      return 1
    fi
    
    
  



}

main $@