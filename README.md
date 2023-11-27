# Basic optimization techniques on PHODs algorithm for motion estimation, along with visualization of results...

This project contains basic application code for constructing QCIF 176x144 frames from an .mp4 video format and applying a PHODs motion estimation algorithm over video frames, with different optimization cases (such as loop merging and data reuse), ignoring -O3 'gcc' compiler optimization flag (applies -O0:=none/without optimization)
- Basic Information about the usage of the application can be found, if executing:
    *bash phods_push.sh --h*, in which a help message will be provided, as to what aspect of execution you wish to check...
- Motion vectors and QCIF frames are additionally saved to .txt and .png files respectively, along with original video frames... The video input can be changed with replacing the given file.
- Timing is also saved in a respective **times.txt** file to compare the specified optimization over the PHODs algorithm
- Python files are provided to receive plots and break video into frames that provide better understanding of the process... Block size of each frame can be asjusted both for default unoptimized case (change it via B variable in *prog.c*), as well as the block size array can be changed from *requirements.sh*
- images.h file will be created with the actual video formatted QCIF array... The output file from my run session is also inside the repository (it will be overwritten accordingly...)
- Make sure you are working inside the directory that contains all files and executables - Adjust **$PYTHON_EXECUTABLE** (in requirements.sh) to the path that python.exe is located in your system
- Improve the code with more optimizations...
  
   **Start with:**
     **bash phods_push.sh -v RaceCar.mp4 -f 10 : QCIF framing construction with given fps**
     **bash phods_push.sh -m phods             : Compiles and links all files and verifies installation of required PyPi packages**
     **bash phods_push.sh -r phods             : Runs main test case and saves results... Can plot motion vectors for understanding**
