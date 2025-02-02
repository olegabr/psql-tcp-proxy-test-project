#!/bin/bash

if [[ $# -eq 0 ]]; then
   ./psql_proxy ${PROXY_HOST} ${PROXY_PORT} ${TARGET_HOST} ${TARGET_PORT} ${QUERY_LOG_FILE_PATH}
elif [[ "$1" == "psql-proxy" ]]; then
   ./psql_proxy ${PROXY_HOST} ${PROXY_PORT} ${TARGET_HOST} ${TARGET_PORT} ${QUERY_LOG_FILE_PATH}
elif [[ "$1" == "echo" ]]; then
    ./echo ${ECHO_HOST} ${ECHO_PORT}
elif [[ "$1" == "tcp-proxy" ]]; then
   ./tcp_proxy ${TCP_PROXY_HOST} ${TCP_PROXY_PORT} ${TCP_TARGET_HOST} ${TCP_TARGET_PORT}
elif [[ "$1" == "init-db" ]]; then
    echo "init-db"
    /usr/local/bin/sysbench \
        --db-driver=pgsql \
        --oltp-table-size=10000 \
        --oltp-tables-count=24 \
        --threads=1 \
        --pgsql-host=${POSTGRES_HOST} \
        --pgsql-port=${TARGET_PORT} \
        --pgsql-user=${POSTGRES_USER} \
        --pgsql-password=${POSTGRES_PASSWORD} \
        --pgsql-db=${POSTGRES_DB} \
        /usr/local/share/sysbench/tests/include/oltp_legacy/parallel_prepare.lua \
        run || true && \
    lighttpd -D -f /etc/lighttpd/lighttpd.conf
elif [[ "$1" == "dry-run" ]]; then
    echo "dry-run" && \
    /usr/local/bin/sysbench \
        --db-driver=pgsql \
        --report-interval=2 \
        --oltp-table-size=10000 \
        --oltp-tables-count=24 \
        --threads=${TEST_THREADS_COUNT} \
        --time=${TEST_PERIOD_SECONDS} \
        --pgsql-host=${POSTGRES_HOST} \
        --pgsql-port=${TARGET_PORT} \
        --pgsql-user=${POSTGRES_USER} \
        --pgsql-password=${POSTGRES_PASSWORD} \
        --pgsql-db=${POSTGRES_DB} \
        --pgsql-sslmode=disable \
        /usr/local/share/sysbench/tests/include/oltp_legacy/select.lua \
        run && \
    lighttpd -D -f /etc/lighttpd/lighttpd.conf
elif [[ "$1" == "test-run" ]]; then
    echo "test-run" && \
    /usr/local/bin/sysbench \
        --db-driver=pgsql \
        --report-interval=2 \
        --oltp-table-size=10000 \
        --oltp-tables-count=24 \
        --threads=${TEST_THREADS_COUNT} \
        --time=${TEST_PERIOD_SECONDS} \
        --pgsql-host=${POSTGRES_HOST} \
        --pgsql-port=${PROXY_PORT} \
        --pgsql-user=${POSTGRES_USER} \
        --pgsql-password=${POSTGRES_PASSWORD} \
        --pgsql-db=${POSTGRES_DB} \
        --pgsql-sslmode=disable \
        /usr/local/share/sysbench/tests/include/oltp_legacy/select.lua \
        run && \
    lighttpd -D -f /etc/lighttpd/lighttpd.conf
elif [[ "$1" == "lighttpd" ]]; then
    echo "lighttpd"
    lighttpd -D -f /etc/lighttpd/lighttpd.conf
else
   echo "Unknown argument"
fi
