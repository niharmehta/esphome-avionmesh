# Contributing

## Dependencies

| What | Where declared | Pin |
| --- | --- | --- |
| ESPHome >= 2026.4.0 | `avionmesh.yaml` (`min_version`) | minimum only |
| ESP32 / ESP-IDF, MQTT, JSON, BLE, web_server | `components/avionmesh/__init__.py` (`DEPENDENCIES`/`AUTO_LOAD`) | via ESPHome |
| avionmesh-cpp (+ recsrmesh-cpp inside it) | `__init__.py` `lib_deps` | unpinned git — pin before tagging a release |
| ArduinoJson | via ESPHome `json` component | via ESPHome |
| Mbed TLS | via ESP-IDF | via ESP-IDF |
| GoogleTest (host tests) | `tests/CMakeLists.txt` FetchContent | v1.14.0 |
| ArduinoJson (host tests) | `tests/CMakeLists.txt` FetchContent fallback | v7.4.2 |
| Python tool (`tools/avion_import.py`) | stdlib only | — |

## Setup

```sh
python3 -m venv .venv && .venv/bin/pip install esphome ruff pre-commit
pre-commit install
cp examples/avionmesh.example.yaml avionmesh.yaml  # then edit + create secrets.yaml
git clone https://github.com/oyvindkinsey/avionmesh-cpp ../avionmesh-cpp  # for host tests
```

## Checks (run before opening a PR)

```sh
make format       # clang-format (Google, 100 cols, 4-space) + ruff format
make format-check # verify formatting
make lint         # ruff check
make test         # host GoogleTests via CMake/ctest
make build        # esphome compile avionmesh.yaml
```

CI (`.github/workflows/ci.yml`) runs format-check, host tests, and an
ESPHome compile of `examples/avionmesh.example.yaml` on every PR.

Generated files `components/avionmesh/web_*.h` are produced by
`__init__.py` at compile time from `web.html`/`web.css`/`web.js` — edit the
sources, never the generated headers.
