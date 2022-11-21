FROM gitpod/workspace-full

RUN sudo apt-get update && sudo apt-get install -y valgrind libcunit1 libcunit1-doc libcunit1-dev clang
run pip install matplotlib

