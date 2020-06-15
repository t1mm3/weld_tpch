#!/bin/env bash


# Install rust using:
# curl https://sh.rustup.rs -sSf | sh

# Later use:
# source $HOME/.cargo/env

export BASE=/export/scratch2/home/tgubner/weld_tpch
export N=32

# LLVM60
if [[ ! -e $BASE/llvm60/is_installed ]]; then
	echo "Installing LLVM6"
	mkdir -p $BASE/llvm60/src
	git clone -b release/6.x https://github.com/llvm/llvm-project.git $BASE/llvm60/src  || exit 1
	mkdir -p $BASE/llvm60/install
	mkdir -p $BASE/llvm60/build

	cd $BASE/llvm60/build

	cmake -DLLVM_ENABLE_PROJECTS="clang;libcxx;libcxxabi" -DCMAKE_INSTALL_PREFIX=$BASE/llvm60/install -DCMAKE_BUILD_TYPE=Release $BASE/llvm60/src/llvm || exit 1
	make -j$N || exit 1
	make install || exit 1

	cd $BASE/llvm60/install/bin
	ln -s clang++ clang++-6.0
	mkdir $BASE/llvm60/is_installed
fi

export PATH=$BASE/llvm60/install/bin:$PATH
echo "" > $BASE/env.sh
echo "export LLVM60_BIN=$BASE/llvm60/install/bin" >> $BASE/env.sh


# Get Weld
if [[ ! -e $BASE/weld/is_installed ]]; then
	git clone https://www.github.com/weld-project/weld $BASE/weld || exit 1
	cd $BASE/weld
	cargo build --release || exit 1
	cargo test || exit 1

	mkdir -p $BASE/weld/install
	mkdir -p $BASE/weld/install/include
	mkdir -p $BASE/weld/install/lib

	cp -rf $BASE/weld/weld-capi/weld.h $BASE/weld/install/include
	cp -rf $BASE/weld/target/release/libweld.so $BASE/weld/install/lib

	mkdir $BASE/weld/is_installed
fi

export WELD_HOME=$BASE/weld
echo "export WELD_HOME=$BASE/weld" >> $BASE/env.sh
echo "export WELD_LIB=$BASE/weld/install/lib" >> $BASE/env.sh


if [[ ! -e $BASE/tbb/is_installed ]]; then
	echo "Installing TBB"
	if [ -d "$BASE/tbb/src" ]; then
		git  --work-tree=$BASE/tbb/src --git-dir=$BASE/tbb/src/.git pull origin master
	else
		mkdir -p $BASE/tbb/src || exit 1
		git clone https://github.com/wjakob/tbb.git $BASE/tbb/src || exit 1
	fi
	mkdir -p $BASE/tbb/build || exit 1
	cd $BASE/tbb/build || exit 1
	cmake -DCMAKE_INSTALL_PREFIX=$BASE/tbb/install -DCMAKE_BUILD_TYPE=Release $BASE/tbb/src || exit 1
	make -j$N || exit 1
	make install || exit 1
	mkdir $BASE/tbb/is_installed
fi

if [[ ! -e $BASE/weld_tpch/is_installed ]]; then
	echo "Installing weld_tpch"
	if [ -d "$BASE/weld_tpch/src" ]; then
		git  --work-tree=$BASE/weld_tpch/src --git-dir=$BASE/weld_tpch/src/.git pull origin master
	else
		mkdir -p $BASE/weld_tpch/src || exit 1
		git clone git@github.com:t1mm3/weld_tpch.git $BASE/weld_tpch/src || exit 1
	fi
	mkdir -p $BASE/weld_tpch/build || exit 1
	cd $BASE/weld_tpch/build || exit 1
	cmake -DCMAKE_CXX_FLAGS="-Wno-error=deprecated-copy" -DCMAKE_INSTALL_PREFIX=$BASE/weld_tpch/install -DCMAKE_BUILD_TYPE=Release $BASE/weld_tpch/src -DTBB_ROOT_DIR=$BASE/tbb/install -DTBBROOT=$BASE/tbb/install -DTBB_INSTALL_DIR=$BASE/tbb/install -DTBB_LIBRARY=$BASE/tbb/install/lib -DWELD_HOME:STRING=$WELD_HOME -DWELD_INCLUDE:STRING=$WELD_HOME/install/include -DWELD_LIBRARY:STRING=$WELD_HOME/install/lib || exit 1
	make -j$N || exit 1
	make install || exit 1

	mkdir -p $BASE/weld_tpch/debug || exit 1
	cd $BASE/weld_tpch/debug || exit 1
	cmake -DCMAKE_CXX_FLAGS="-Wno-error=deprecated-copy" -DCMAKE_BUILD_TYPE=Debug $BASE/weld_tpch/src -DTBB_ROOT_DIR=$BASE/tbb/install -DTBBROOT=$BASE/tbb/install -DTBB_INSTALL_DIR=$BASE/tbb/install -DTBB_LIBRARY=$BASE/tbb/install/lib -DWELD_HOME:STRING=$WELD_HOME -DWELD_INCLUDE:STRING=$WELD_HOME/install/include -DWELD_LIBRARY:STRING=$WELD_HOME/install/lib || exit 1
	make -j$N || exit 1

	mkdir $BASE/weld_tpch/is_installed
fi


echo "Please update your environment variables"
echo "source $BASE/env.sh"
