# pi_thread

## usage with CLion

 - verify you are using Makefiles in CLion: Settings > Build, Execution, Deployment > CMake > Generator > Unix Makefiels
 - right-click `CMakeLists.txt`
 - select "Load CMake project"
 - build, run, debug using the CLion tools (e.g. Shift+F10 or click run button, etc.)


## usage without CLion (cmake commandline)

 - create cmake directory `mkdir build`
 - `cd build`
 - run cmake `cmake ..`
 - build with `make`
 - run from command line `./pi_thread`
