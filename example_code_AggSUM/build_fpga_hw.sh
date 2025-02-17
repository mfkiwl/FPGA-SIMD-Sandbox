#!/bin/bash

# # run this by
# qsub -l nodes=1:fpga_compile:ppn=2 -d . build_fpga_emu.sh -l walltime=23:00:00
# # monitor job via
# watch -n 1 qstat -n -1

# Initial Setup
source /data/intel_fpga/devcloudLoginToolSetup.sh
tools_setup -t S10OAPI
export PATH=/glob/intel-python/python2/bin:$PATH

cd ~/FPGA-SIMD-Sandbox/example_code_AggSUM 

# Running project in FPGA Hardware Mode (this takes approximately 1 hour)
printf "\\n%s\\n" "Running in FPGA Hardware compile Mode:"
make hw
# error_check
