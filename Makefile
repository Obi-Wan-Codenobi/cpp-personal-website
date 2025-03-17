COMPILED_FILE_NAME=apiBuild
MAIN_API_FILE=./api/main.cpp
CERT=cert.pem
KEY=key.pem
# OpenSSL paths for linux 
#OPENSSL_INCLUDE_PATH=/usr/include/openssl
#OPENSSL_LIB_PATH=/usr/lib/x86_64-linux-gnu

# OpenSSL & zlib paths for mac/homebrew
OPENSSL_INCLUDE_PATH=/opt/homebrew/opt/openssl@3/include
OPENSSL_LIB_PATH=/opt/homebrew/opt/openssl@3/lib
ZLIB_INCLUDE_PATH=/opt/homebrew/opt/zlib/include
ZLIB_LIB_PATH=/opt/homebrew/opt/zlib/lib


.PHONY: all
all: compile run

.PHONY: compile
compile:
	@echo "Compiling $(COMPILED_FILE_NAME) ..."
	g++ -std=c++23 -Wall -O3 \
		-I$(OPENSSL_INCLUDE_PATH) -I$(ZLIB_INCLUDE_PATH) \
		-L$(OPENSSL_LIB_PATH) -L$(ZLIB_LIB_PATH) $(MAIN_API_FILE) \
		-o $(COMPILED_FILE_NAME) -lssl -lcrypto -lz

.PHONY: run
run: 
	@echo "Running $(COMPILED_FILE_NAME) ..."
	./$(COMPILED_FILE_NAME)

.PHONY: log
log: compile 
	@echo "Running $(COMPILED_FILE_NAME) with logging ..."
	./$(COMPILED_FILE_NAME) -L 1

.PHONY: cert
cert: 
	@echo "Making key and cert files ..."
	openssl req -x509 -newkey rsa:4096 -keyout $(KEY) -out $(CERT) -days 365

.PHONY: clean
clean: 
	@echo "Deleting files ..."
	rm $(COMPILED_FILE_NAME) $(KEY) $(CERT)

.PHONY: help
help:
	@echo "Makefile commands:"
	@echo "  make                             - Compile api and run"
	@echo "  make log                         - Compile api and run with logging"
	@echo "  make compile                     - Compile main.c"
	@echo "  make cert                        - Make local cert files"
	@echo "  make run                         - Run the api executable"
	@echo "  make clean                       - Remove the executable"
	@echo "  make help                        - Show this help message"
