# Repository Guidelines

## Project Structure

Public APIs live in `include/dephy_config/`, portable implementation lives in
`src/`, Zephyr module metadata lives in `zephyr/`, Linux tests live in `tests/`,
and helper scripts live in `scripts/`.

## Development Model

Keep this module product-agnostic. Products own schema, defaults, and
validation; this module only stores fixed-size records by key.

## Commands

- `make -f Makefile.linux`: build the Linux test binary.
- `make -f Makefile.linux test`: run the Linux unit test.
- `scripts/test_config_store.sh`: run the module test script.

## Style

Use C11, four-space indentation, snake_case symbols, and small public APIs.
