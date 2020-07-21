BUILD_DIR=build
HEADER_IMPLEMENTATION_FILES=thirdparty/cpp-base64/base64.cpp
SQLITE_DIR_PATH=thirdparty/sqlite3

.PHONY: help
help:
	echo "not implemented yet"

.PHONY: clean
clean:
	mkdir -p ${BUILD_DIR}
	rm -rf ${BUILD_DIR}/*

.PHONY: update_submodules
update_submodules:
	git submodule update --init

.PHONY: build_all
build_all: update_submodules clean build_sqlite build_app

.PHONY: build_sqlite
build_sqlite:
	gcc -Wall -Wextra -c -std=c99 -o ${BUILD_DIR}/sqlite3.so ${SQLITE_DIR_PATH}/sqlite3.c

.PHONY: build_app
build_app:
	g++ -Wall -Wextra -std=c++17 -o build/email-db ${BUILD_DIR}/sqlite3.so ${HEADER_IMPLEMENTATION_FILES} email-db-main.cpp -pthread -ldl

.PHONY: clean_test_dir
clean_test_dir:
	mkdir -p ${BUILD_DIR}/test
	rm -rf ${BUILD_DIR}/test/*

.PHONY: test
test: clean_test_dir
	g++ -Wall -Wextra -std=c++17 string_util_test.cpp -o ${BUILD_DIR}/test/string_util_test
	${BUILD_DIR}/test/string_util_test
