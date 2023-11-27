## Error function
function errorCode()
{ 
    ## Video_input must be provided as first parameter before executing...
    if [ "$1" == "--h" ]; then
        echo "usage: PHODS benchmarking for different optimization cases [options] *VideoInput* *FPS*"
        echo "  options:"
        echo "      -v, Provide video_file required for frame construction and PHODS testing"
        echo "      -f, Provide FPS settings"
        echo "      -m [name], Builds PHODS motion estimation with/without any -loop/-Ox optimization"
        echo "      -r [name], Runs estimator and measures execution time for basic PHODS test case..."
        echo "      -b [name], Starts benchmarking with [name] provided, either with or without -Ox..."
        echo "  name:" 
        echo "      Supported mainly for PHODS algorithm... Reserved for future use..."
        echo "      -phods, for basic make(-m) and run(-r) executions..."
        echo "      -phodsO0/1/2, for choosing benchmark with pre-defined blocks sizes, with/out optimizations..."
        echo "         -> Note: You can change blocksize_array cases in phods_push.sh file..."
        echo -e "\u00A9 2023 DimGeorgako EmbeddedSystems..."
        
        echo ""
        echo ""
        read -p "View README.md for more information (y/n): " choice
        if [ "$choice" == "y" ]; then
            cat README.md
        fi
        exit
    fi
}

function creatingFrames()
{
    video_value=$1
    fps_value=$2
    maxfps=$3

    # Check if the file exists
    if [ -e "$video_value" ]; then
        echo "Video file exists: $video_value"
        echo ""
    else
        echo "Video file does not exist: $video_value"
        exit
    fi

    # Check if the value is a valid integer
    if [[ "$fps_value" =~ ^[0-9]+$ ]]; then
        echo "FPS value: $fps_value"
    else
        echo "Invalid FPS value...: $fps_value"
        echo "usage: -h for help..."
        exit
    fi

    ## Video into frames with fps provided
    while [ "$fps_value" -gt "$maxfps" ]; do
        echo "Limit frames created to a maximum of 10 fps...(^C to exit)"
        read -p "  Errno: Please provide lower FPS input: " fps_value
    done

    echo ""
    directory="./get_frames"

    if [ ! -d "$directory" ]; then
        mkdir -p "$directory"
        echo "Directory didn't exist... Created and re-build properly..."
        echo ""
    fi
    cd $directory
    ## rm frame*.png
    touch frame1.png
    cd ../

    echo ""
    echo "Re-creating frames for given FPS:$fps_value..."
    echo ""
    ffmpeg -i $video_value -vf "fps=$fps_value" ./get_frames/frame%d.png
    echo "" 
    echo "Frames created successfully..."
    echo ""
}


## Convert frames from .png file into QCIF formatted arrays: Runs .py script that uses OpenCV library
## The .py code also creates "images.h" header file that converts directly the frames into QCIF format
function createQCIF_images()
{
    echo ""
    if [ ! -d "./qcif_frames" ]; then
        mkdir -p "./qcif_frames"
        echo "Directory didn't exist... Created and re-build properly..."
        echo ""
    fi
    "$PYTHON_EXECUTABLE" "$(pwd)/frames_to_qcif.py"
    echo ""
}
