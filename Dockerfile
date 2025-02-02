# @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
# @copyright MIT
# @brief Dockerfile with with CMake, Google Test, Doxygen, Sysbench and C++ build tools

FROM ubuntu:18.04

# General C++ build packages
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    git \
    build-essential \
    curl \
    ca-certificates \
    cmake && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists

# Install Google Test dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    googletest \
    lcov && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists

# Build GTest library
RUN cd /usr/src/googletest && \
    cmake . && \
    cmake --build . --target install

#  Doxygen support
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    graphviz \
    doxygen && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists

# Install sysbench dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    make \
    automake \
    libtool \
    libaio-dev \
    pkg-config \
    libpq-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists

# Build sysbench
RUN git clone https://github.com/olegabr/sysbench.git && \
    cd sysbench && \
    git checkout 1.0.20-pgsql-sslmode && \
    ./autogen.sh && \
    ./configure --with-pgsql --without-mysql && \
    make -j && \
    make install && \
    cd .. && \
    rm -rf sysbench

# serve static html files for docs and test coverage results
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    lighttpd && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists

# Set the working directory to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY src ./src
COPY tests ./tests
COPY CMakeModules ./CMakeModules
COPY CMakeLists.txt .

# Build project coverage info
RUN cmake -DCMAKE_BUILD_TYPE=Debug . && \
    make -j io_test_coverage

# Build project and test to ensure it works
RUN cmake . && \
    make -j && \
    ./io_test

COPY cppreference-doxygen-web.tag.xml .

# Build project docs
RUN make -j doxygen

# serve static html files for docs and test coverage results
RUN ln -s /app/doxygen /var/www/html/doxygen && \
    ln -s /app/io_test_coverage /var/www/html/io_test_coverage


ENV ECHO_HOST=0.0.0.0
ENV ECHO_PORT=1234

ENV TCP_PROXY_HOST=0.0.0.0
ENV TCP_PROXY_PORT=1235
ENV TCP_TARGET_HOST=0.0.0.0
ENV TCP_TARGET_PORT=5432

ENV PROXY_HOST=0.0.0.0
ENV PROXY_PORT=1235
ENV TARGET_HOST=0.0.0.0
ENV TARGET_PORT=5432
ENV QUERY_LOG_FILE_PATH=/tmp/query.log

ENV TEST_THREADS_COUNT=64
ENV TEST_PERIOD_SECONDS=60

COPY docker-entrypoint.sh .

ENTRYPOINT [ "./docker-entrypoint.sh" ]
