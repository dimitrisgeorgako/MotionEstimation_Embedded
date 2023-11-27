#!/bin/bash

## Link to phods main function files
source phods_run.sh
source requirements.sh

## Check errorcode
errorCode $1

## Main parameters provided and settings configured
## Get the value after -v for videoInput & -f for fps parameter input
while getopts ":v:f:m:r:b:h:" opt; do
    case $opt in
        v)
            video_file="$OPTARG"
            no_make=1
            ;;
        f)
            fps_value="$OPTARG"
            ;;
        m)  
            if [ "$OPTARG" == "phods" ]; then
                make_phods0=1
            fi  
            ;;
        r)  
            if [ "$OPTARG" == "phods" ]; then
                run_phods0=1
            fi              
            ;;
        b)  
            if [ "$OPTARG" == "phodsO0" ]; then 
                bench_phods0=1
            fi
            if [ "$OPTARG" == "phodsO1" ]; then 
                bench_phods_opt=1
            fi
            if [ "$OPTARG" == "phodsO2" ]; then 
                bench_phods_opt_reuse=1
            fi
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            exit 1
            ;;
        :)
            echo "Option -$OPTARG requires an argument." >&2
            exit 1
            ;;
    esac
done

shift $((OPTIND-1))

echo ""

if [[ "$video_file" != "" && "$fps_value" != "" ]]; then
    creatingFrames $video_file $fps_value $maxfps
    createQCIF_images
else
    if [[ "$make_phods0" == "" && "$run_phods0" == "" && "$bench_phods0" == "" && "$bench_phods_opt" == ""  && "$bench_phods_opt_reuse" == "" ]]; then
        echo "VideoFile or FPS settings is missing..."
        echo "Check --h for help usage..."
        exit 1
    fi
fi

if [ "$make_phods0" == "1" ]; then
    echo "Building PHODS block matching algorithm for motion estimation"
    echo "With/without -O3 optimization flags, for benchmarking the process..."
    echo ""
    install_packages
    make clean
    make
    echo ""
    echo "Compiled and Linked successfully..."
fi

if [ "$run_phods0" == "1"  ]; then
    echo "PHODS0 test case..."
    ./phodsO0 -phodsO0  ## Default block size: PHODS algorithm Test case 
fi

## Run benchmark cases for different block sizes and save timings in a .txt file properly
if [ "$bench_phods0" == "1" ]; then
    echo "-O0 optimizations below:" >> times.txt
    for blocksizes in "${block_size_array[@]}"; do
        ./phodsO0 -phodsO0 $blocksizes
    done
elif [ "$bench_phods_opt" == "1" ]; then
    echo "-O1 optimizations below:" >> times.txt
    for blocksizes in "${block_size_array[@]}"; do
        ./phodsO0 -phodsO1 $blocksizes
    done
elif [ "$bench_phods_opt_reuse" == "1" ]; then
    echo "-O2 optimizations below:" >> times.txt
    for blocksizes in "${block_size_array[@]}"; do
        ./phodsO0 -phodsO2 $blocksizes
    done
fi

## Request user if he wants to view motion_vectors in a specific QCIF frame
if [[ "$make_phods0" != "1" && "$no_make" != "1" ]]; then
    decision=0
    while [ "$decision" == "0" ]; do
        read -p "Do you wish to visualize Motion Vectors (Vx,Vy) for latest block size value... (y/n): " decision
        if [ "$decision" == "y" ]; then
            "$PYTHON_EXECUTABLE" "$(pwd)/motvect_plotting.py" ${block_size_array[-1]}
            echo "Plotting results exited..."
        elif [ "$decision" == "n" ]; then
            echo "Exiting..."
        else 
            decision=0
        fi
    done
fi