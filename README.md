# PostgreSQL TCP proxy server

Use `epoll` to proxy requests to `PostgreSQL` and dump all `SQL` requests to a file

## Requirements

Разработать на C++ TCP прокси-сервер для СУБД Postgresql с возможностью логирования
всех SQL запросов, проходящих через него. Документация по сетевому протоколу этой
СУБД доступна на официальном сайте.

Для выполнения тестового задания нужно использовать Berkley sockets (select/poll/epoll).

Прочих зависимостей быть не должно.

Прокси должен уметь обрабатывать большое количество соединений без создания потока
(thread) на каждое соединение. Необходимо распарсить сетевые пакеты, проходящие через
прокси, в которых содержатся SQL запросы, извлечь эти запросы из пакетов и записать их
в файл в виде текста (по одному запросу в строке, структура неважна). Для того, чтобы в
прокси были видны SQL запросы в незашифрованном виде, необходимо отключить SSL
(на клиенте и/или сервере). Должна присутствовать минимальная обработка ошибок, так
же желательны комментарии в тех местах, где возможны ошибки. Приложение не должно
падать на нескольких десятках одновременных соединений, выполняющих запросы к
СУБД без перерыва в течение 5 минут (можно использовать sysbench для тестирования).
Операционная система Linux, компилятор – GCC, так же необходимо создать файл для
сборки проекта с помощью cmake или make.

## Implementation notes

The `C++17` standard is used.

The development was done on the Ubuntu 18 with C++17 as the most recent standard supported.

Some modern CMake configuration can not be also used and workarounds applied instead.

The `no external dependencies` requirement was relaxed for non-network related code to prevent the weel re-invention:
 - The `bipartite_buf` class was borrowed from the [https://github.com/DNedic/lockfree/blob/main/lockfree/spsc/bipartite_buf.hpp](https://github.com/DNedic/lockfree/blob/main/lockfree/spsc/bipartite_buf.hpp) project and adjusted for optional single-threaded usage.
 - `googletest` was used for tests

 > The default log file name is `/tmp/query.log` or `./logs/query.log` if started via `docker compose`.

### Threading issue

 - All requests are handled in a single thread, only the log file is written in another one.
 - Second/third/etc instances of the proxy process can be started simultaneously to utilize more CPU cores. Make sure to provide new log file name for each instance since it is truncated on the program start.
 
## Architecture

> The architecture was deeply influenced by the [boost::asio](https://www.boost.org/doc/libs/release/doc/html/boost_asio.html) library with the `Proactor` pattern changed to the `Reactor` pattern amendment for simplicity.

The [io::bus](./src/io/bus.hpp) derived classes like [io::system::epoll](./src/io/epoll.hpp) implements the `Reactor` pattern and the [io::context](./src/io/context.hpp) class wraps it in a loop.

The [io::ip::tcp::acceptor](./src/io/acceptor.hpp) class is used to accept TCP connections.

And the [io::ip::tcp::socket](./src/io/socket.hpp) object is used for async I/O.

The [io::channel](./src/io/channel.hpp) class implements a `Pipe` pattern to read data from one socket and write it to another one.

The [io::ip::tcp::session_manager](./src/io/session_manager.hpp) and [io::ip::tcp::session_base](./src/io/session_base.hpp) classes are auxiliary base classes to simplify actual server code implementation.

Check the [./src/echo](./src/echo/main.cpp) and [./src/tcp_proxy](./src/tcp_proxy/main.cpp) examples to see it in action.


## Use with docker

### Build docker image

```
docker compose build
```

### Run docker container

```
docker compose up
```

This command will start `echo` service, the `PostgreSQL` server and the `psql_proxy` service connected to it.

The `1235` port would be exposed by the `psql_proxy` service.

The `1234` port would be exposed by the `echo` service.

### The Docker container dry run testing

The `dryrun` service performs the test with the `PostgreSQL` server directly without the `psql_proxy` in between. It can be used to estimate the proxy code performance downgrade compared to the direct connection.

Configure it with `TEST_THREADS_COUNT`, `TEST_PERIOD_SECONDS` and `TARGET_PORT` environment variables.

### The Docker container testing

The `docker-compose.yml` file has the `testrun` service defined. It runs the sysbench test.

Configure it with `TEST_THREADS_COUNT`, `TEST_PERIOD_SECONDS` and `PROXY_PORT` environment variables.

### View documentation

Target your browser on the [http://127.0.0.1:8080/doxygen/html/inherits.html](http://127.0.0.1:8080/doxygen/html/inherits.html) page for the Doxygen docs.

### View test coverage

Target your browser on the [http://127.0.0.1:8080/io_test_coverage/index.html](http://127.0.0.1:8080/io_test_coverage/index.html) page for the test coverage information.


## Use without Docker

### Build

```
cmake .
make -j
```

### Build docs

```
make -j doxygen
```

### Testing

```
sudo apt install googletest lcov
cd /usr/src/googletest
cmake .
make -j
sudo make install
```

```
cd project_root
cmake .
make -j
./io_test
```

### Test coverage

```
cd project_root
cmake .
make -j io_test_coverage
```

Then open the `./io_test_coverage/io/index.html` file in a browser.


### Run the psql client

```
psql 'postgresql://127.0.0.1:1235/sbtest?sslmode=disable' --username=sbtest --password
```

Enter the `password` string as a password.

### Test with sysbench

See [SYSBENCH И POSTGRESQL — ГЕНЕРАЦИЯ ТЕСТОВОЙ НАГРУЗКИ](https://avamk.ru/sysbench-i-postgresql-generatsiya-testovoj-nagruzki.html)

#### Install sysbench

Custom repository is needed to be able to use the `--pgsql-sslmode=disable` option with the `0.4` sysbench version used in the guide above.

```
git clone https://github.com/olegabr/sysbench.git
cd sysbench
git checkout 1.0.20-pgsql-sslmode
./autogen.sh
./configure --with-pgsql --without-mysql
make -j
make install
```

#### Create test data tables

```
/usr/local/bin/sysbench \
--db-driver=pgsql \
--oltp-table-size=10000 \
--oltp-tables-count=24 \
--threads=1 \
--pgsql-host=127.0.0.1 \
--pgsql-port=5432 \
--pgsql-user=sbtest \
--pgsql-password=password \
--pgsql-db=sbtest \
/usr/local/share/sysbench/tests/include/oltp_legacy/parallel_prepare.lua \
run
```

#### Run test

Dry run to postgresql directly:

```
/usr/local/bin/sysbench \
--db-driver=pgsql \
--report-interval=2 \
--oltp-table-size=10000 \
--oltp-tables-count=24 \
--threads=64 \
--time=60 \
--pgsql-host=127.0.0.1 \
--pgsql-port=5432 \
--pgsql-user=sbtest \
--pgsql-password=password \
--pgsql-db=sbtest \
--pgsql-sslmode=disable \
/usr/local/share/sysbench/tests/include/oltp_legacy/select.lua \
run
```

Actual psql_proxy test run:

```
/usr/local/bin/sysbench \
--db-driver=pgsql \
--report-interval=2 \
--oltp-table-size=10000 \
--oltp-tables-count=24 \
--threads=64 \
--time=60 \
--pgsql-host=127.0.0.1 \
--pgsql-port=1235 \
--pgsql-user=sbtest \
--pgsql-password=password \
--pgsql-db=sbtest \
--pgsql-sslmode=disable \
/usr/local/share/sysbench/tests/include/oltp_legacy/select.lua \
run
```