export LD_LIBRARY_PATH=$(pwd)/lib:${LD_LIBRARY_PATH}
nohup ./cmfs > out.log &