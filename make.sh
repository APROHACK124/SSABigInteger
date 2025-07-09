nvcc -c cuda_SSA.cu -o cuda_SSA.o
nvcc -c cuda_NTT.cu -o cuda_NTT.o
g++ -c bigInteger.cpp -o bigInteger.o
g++ -c modular.cpp -o modular.o
g++ -c stress.cpp -o stress.o
g++ -c run.cpp -o run