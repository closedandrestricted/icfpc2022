# icfpc2022
ICFPC 2022

How to check new solution.
./run_cmake
cd release
make -j 16
./build/icfpc -mode adjust -solution XXX
./build/icfpc -mode update -solution XXX
cd ..
./submit.py
