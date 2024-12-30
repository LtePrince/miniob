build:
	@sudo bash build.sh

env:
	@sudo bash build.sh init

run: build
	@rm -rf ./miniob
#	@./build/bin/observer -f ../build/etc/observer.ini -P cli
	@./build_debug/bin/observer -f ../build_debug/etc/observer.ini -P cli

debug: build
	@rm -rf ./miniob
	@gdb ./build_debug/bin/observer -f ../build_debug/etc/observer.ini -p 6789

rebuild:
	@sudo rm -rf ./build
	@sudo rm -rf ./build_debug

clean:
	@rm -rf ./miniob
	@rm -rf ./observer.log.*

gen_parser:
	@cd src/observer/sql/parser && ./gen_parser.sh

.PHONY: build env run gen_parser