#!/bin/bash

NUMCPUS=`grep -c processor /proc/cpuinfo`
SCRIPT_PATH=$GEM5_PATH/run_gem5_x86_spec06_benchmark.sh 
BENCHS=(
# "perlbench" # does not work
"bzip2"
"gcc"
"bwaves"
"gamess"
"mcf"
"milc"
"zeusmp"
"gromacs"
"cactusADM"
"leslie3d"
"namd"
"gobmk"
# "dealII" # does not work (infinity loop?)
"soplex"
"povray"
"calculix"
"hmmer"
"sjeng"
"GemsFDTD"
"libquantum"
"h264ref"
# "tonto" # does not work (infinity loop?)
"lbm"
"omnetpp"
"astar"
"wrf"
# "sphinx3" # does not work on Ruby
# "xalancbmk" # dose not work (infinity loop?)
# "specrand_i"
# "specrand_f"
)

function thread_run () {
    start_idx=`expr $1 \* $2`
    num_entries=$2
    for ((i=0;i<$num_entries;i++)); do
        idx=`expr $start_idx + $i`
        if [ ${#BENCHS[@]} -le $idx ]; then
            exit 0
        fi
        echo "AUTORUN for" ${BENCHS[idx]}
        $SCRIPT_PATH ${BENCHS[idx]}
        # sleep 1
    done
}

function multiprocessing () {
    num_thread=$1
    entries=`expr ${#BENCHS[@]} / $num_thread`
    if [ `expr $entries \* $num_thread` -ne ${#BENCHS[@]} ]; then
        entries=$(($entries+1))
    fi
    echo $entries "entries per thread"
    for ((j=0;j<num_thread;j++)); do
        thread_run $j $entries &
    done
    wait
}


# echo $NUMCPUS
# echo ${#BENCHS[@]}
# echo `thread_run 15 2`
multiprocessing 13