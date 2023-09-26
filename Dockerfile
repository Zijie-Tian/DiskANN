# 使用Ubuntu作为基础镜像
FROM ubuntu:22.04

# 维护者信息
LABEL maintainer="ztecube@gmail.com"

# 避免在构建过程中出现交互式对话框
ENV DEBIAN_FRONTEND noninteractive

# 更新软件包列表并安装依赖
RUN apt-get update && \
    apt-get install -y wget bzip2 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# 更新软件包列表并安装所需的包
RUN apt-get update && \
    apt-get install -y \
        cmake \
        libjemalloc-dev \
        libsnappy-dev \
        libgflags-dev \
        pkg-config \
        swig \
        libboost-all-dev \
        libtbb-dev \
        libmkl-full-dev \
        libisal-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# 更新软件包列表并安装libgoogle-perftools-dev和clang-format
RUN apt-get update && \
    apt-get install -y \
        libgoogle-perftools-dev \
        clang-format && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# 添加新的仓库以获取gcc-9
RUN apt-get update && \
    apt-get install -y software-properties-common && \
    add-apt-repository ppa:ubuntu-toolchain-r/test && \
    apt-get update

# 安装MKL库。
RUN apt-get update && \
    apt-get install -y wget && \
    wget https://registrationcenter-download.intel.com/akdlm/irc_nas/18487/l_BaseKit_p_2022.1.2.146.sh && \
    sh l_BaseKit_p_2022.1.2.146.sh -a --components intel.oneapi.lin.mkl.devel --action install --eula accept -s && \
    rm l_BaseKit_p_2022.1.2.146.sh
    
# 安装gcc-9
RUN apt-get install -y gcc-9 g++-9

RUN apt-get install -y libcunit1-dev libssl-dev gdb

RUN apt-get install -y libncurses5-dev libaio-dev

RUN apt-get install -y nasm

# 配置默认编译器为gcc-9
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 90 --slave /usr/bin/g++ g++ /usr/bin/g++-9


# 设置Miniconda的版本和位置
ENV MINICONDA_VERSION latest
ENV MINICONDA_URL https://repo.anaconda.com/miniconda/Miniconda3-$MINICONDA_VERSION-Linux-x86_64.sh
ENV MINICONDA_HOME /opt/miniconda

# 下载并安装Miniconda
RUN wget $MINICONDA_URL -O miniconda.sh && \
    bash miniconda.sh -b -p $MINICONDA_HOME && \
    rm miniconda.sh

# 将Miniconda的bin目录加入到PATH
ENV PATH $MINICONDA_HOME/bin:$PATH

# 创建一个新的conda环境
RUN conda create -y --name diskann python=3.8.10

# 将新环境添加到bashrc以便激活
RUN echo "source activate diskann" > ~/.bashrc
ENV PATH /opt/miniconda/envs/diskann/bin:$PATH

# 安装meson和elftools
RUN conda install -c conda-forge meson pyelftools

# 运行任何其它所需的命令
# ...

# 设置默认shell
SHELL ["/bin/bash", "-c"]

# 验证安装是否成功
RUN conda --version
