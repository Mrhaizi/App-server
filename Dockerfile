FROM ubuntu:22.04

# 设置时区
ENV TZ=Asia/Shanghai
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
# ENV http_proxy=http://127.0.0.1:7890
# ENV https_proxy=http://127.0.0.1:7890
# ENV no_proxy=localhost,127.0.0.1
# 安装必要的依赖
RUN apt-get update --fix-missing && apt-get install -y \
    openssh-server \
    neovim \
    build-essential \
    cmake \
    git \
    wget \
    qtbase5-dev \
    qtdeclarative5-dev \
    qtmultimedia5-dev \
    qt5-qmake \
    libgl1-mesa-dev \
    && apt-get clean

# 安装 MySQL Connector
RUN wget https://dev.mysql.com/get/Downloads/Connector-C++/libmysqlcppconn10_9.0.0-1ubuntu22.04_amd64.deb \
    && wget https://dev.mysql.com/get/Downloads/Connector-C++/libmysqlcppconnx2_9.0.0-1ubuntu22.04_amd64.deb \
    && wget https://dev.mysql.com/get/Downloads/Connector-C++/libmysqlcppconn10-dbgsym_9.0.0-1ubuntu22.04_amd64.deb \
    && wget https://dev.mysql.com/get/Downloads/Connector-C++/libmysqlcppconn-dev_9.0.0-1ubuntu22.04_amd64.deb \
    && dpkg -i libmysqlcppconn10_9.0.0-1ubuntu22.04_amd64.deb \
    && dpkg -i libmysqlcppconnx2_9.0.0-1ubuntu22.04_amd64.deb \
    && dpkg -i libmysqlcppconn-dev_9.0.0-1ubuntu22.04_amd64.deb \
    && dpkg -i libmysqlcppconn10-dbgsym_9.0.0-1ubuntu22.04_amd64.deb \
    && rm -f libmysqlcppconn10_9.0.0-1ubuntu22.04_amd64.deb \
    && rm -f libmysqlcppconnx2_9.0.0-1ubuntu22.04_amd64.deb \
    && rm -f libmysqlcppconn-dev_9.0.0-1ubuntu22.04_amd64.deb \
    && rm -f libmysqlcppconn10-dbgsym_9.0.0-1ubuntu22.04_amd64.deb

# 设置 Qt 环境变量
ENV QTDIR=/usr/lib/qt5
ENV PATH=$QTDIR/bin:$PATH

# 复制项目并编译
COPY . /app
WORKDIR /app

RUN cmake . && make

# 指定容器启动时的默认命令
CMD ["/bin/bash"]
