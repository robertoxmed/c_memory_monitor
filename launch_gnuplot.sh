#! /bin/bash

echo "---------------------------------"
echo "     Graphic: Execution Time     "     
echo "---------------------------------"

./plot/execution_scheduler.sh
./plot/execution_wrapper.sh


echo "--------------------------------"
echo "     Graphic: Miss Cache L1     "     
echo "--------------------------------"

./plot/missL1_scheduler.sh
./plot/missL1_wrapper.sh

echo "--------------------------------"
echo "     Graphic: Miss Cache L2     "     
echo "--------------------------------"

./plot/missL2_scheduler.sh
./plot/missL2_wrapper.sh

echo "--------------------------------"
echo "     Graphic: Miss Cache L3     "     
echo "--------------------------------"

./plot/missL3_scheduler.sh
./plot/missL3_wrapper.sh


echo "------------------"
echo "     Cleaning     "
echo "------------------"

./clean_gnuplot.sh     