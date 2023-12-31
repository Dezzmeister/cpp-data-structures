# Data Structures

Implementations of cool data structures.

## Tests

To run tests: 
```sh
make test
```

To run tests with valgrind:
```sh
make memtest
```

To run tests with inverted `expect` and `fail_test` macros:
```sh
make invtest
```

In this mode, every test should fail. If a test does not fail, then it is a good indication that
nothing is actually being tested there (given that every test passes with `make test`).

### Patterns

To run only tests matching a specific name, set the `PATTERN` environment variable. Only tests having
`${PATTERN}` as a substring in either the suite or case names will be run. Example:

```sh
PATTERN=radix make test
```

This will only run radix trie tests, because "radix" is a substring of "radix trie".

## Developing

I use [YouCompleteMe](https://github.com/ycm-core/YouCompleteMe) for code completion with 
[Bear](https://github.com/rizsotto/Bear/tree/master) to generate a compilation database. 
To generate the compilation database for yourself, install Bear and run

```sh
bear -- make <something>
```
