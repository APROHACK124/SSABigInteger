nvcc -c cuda_SSA.cu -o cuda_SSA.o
nvcc -c cuda_NTT.cu -o cuda_NTT.o
g++ -std=c++11 -c bigInteger.cpp -o bigInteger.o
g++ -std=c++11 -c modular.cpp -o modular.o
g++ -std=c++11 -c stress.cpp -o stress.o
g++ -std=c++11 -c run.cpp -o run.o
nvcc cuda_SSA.o cuda_NTT.o bigInteger.o modular.o stress.o run.o -o run