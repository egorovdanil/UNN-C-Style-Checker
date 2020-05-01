export UNN_LLVM_TASK_HOME=$PWD

cd /tmp/
wget https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz /tmp/
cd clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz/
tar -xvf clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz
rm clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz

cd $UNN_LLVM_TASK_HOME
export LLVM_DIR=/tmp/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04/lib/cmake/llvm/
