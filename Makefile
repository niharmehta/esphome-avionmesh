.PHONY: help format format-check lint test build clean

CXX_SRC := components/avionmesh tests
ESPHOME ?= .venv/bin/esphome

help:
	@echo "  make format        Format C++ (clang-format) and Python (ruff)"
	@echo "  make format-check  Check formatting without modifying files"
	@echo "  make lint          Run ruff on Python sources"
	@echo "  make test          Configure, build and run host tests (ctest)"
	@echo "  make build         Compile firmware with ESPHome (avionmesh.yaml)"
	@echo "  make clean         Remove tests/build"

CXX_FILES := $(shell find $(CXX_SRC) -path tests/build -prune -o \( -name '*.cpp' -o -name '*.h' \) -print | grep -vE 'web_(content|script|style)\.h')

format:
	clang-format -i $(CXX_FILES)
	ruff format components tools

format-check:
	clang-format --dry-run --Werror $(CXX_FILES)
	ruff format --check components tools

lint:
	ruff check components tools

test:
	cmake -S tests -B tests/build
	cmake --build tests/build -j
	cd tests/build && ctest --output-on-failure

build:
	$(ESPHOME) compile avionmesh.yaml

clean:
	rm -rf tests/build
