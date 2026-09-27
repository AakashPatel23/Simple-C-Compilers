# SCC — Simple C Compiler (Lex/Yacc)

A compiler for a subset of C, built with **Lex/Flex** (lexer) and **Yacc/Bison** (parser). It compiles `.c` source files directly to **x86-64 GNU-syntax assembly** (`.s`), targeting the Linux System V ABI.

- `simple.l` — lexer (tokenizer)
- `simple.y` — grammar + code generator (emits assembly directly from grammar actions)
- `Makefile` — builds the `scc` binary
- `tests/` — sample `.c` programs and a test runner (`testall`) that checks `scc`'s output against real `gcc`

## Requirements

- `flex` (or `lex`)
- `bison` (or `yacc`)
- `gcc`
- **A Linux x86-64 environment** to assemble/run/test the generated `.s` files

## Building

From the project root:

```bash
make
```

This runs `lex`/`yacc` to generate `lex.yy.c` and `y.tab.c`, then compiles them into the `scc` binary.

To clean build artifacts:

```bash
make clean
```

## Running

`scc` takes a single `.c` file and produces a `.s` assembly file of the same name:

```bash
./scc tests/fact.c        # produces tests/fact.s
```

To turn that into a runnable binary and execute it:

```bash
gcc -static -o fact tests/fact.s
./fact
```

(Drop `-static` if your system doesn't support static linking, e.g. many macOS/Linux setups without glibc-static installed.)

## Testing

The test suite lives in `tests/` and is driven by the `testall` script, which:

1. Rebuilds `scc` (`make clean && make` in the project root).
2. For each sample program in `tests/*.c`:
   - Compiles it with `scc` to produce `<name>.s`.
   - Assembles/links `<name>.s` with `gcc -static` to produce `<name>.scc`.
   - Compiles the same `.c` file with the system `gcc` to produce `<name>.gcc` (the reference).
   - Runs both binaries and diffs their output.
   - Records `PASS`/`FAIL` in `tests/results.txt`.
3. Prints a summary (`Passed: N / total`).

Run it with:

```bash
cd tests
./testall
```

Results are written to `tests/results.txt`. Intermediate artifacts (`*.s`, `*.scc`, `*.gcc`, `*.out*`) are cleaned up by `make clean` from the project root.

### Running a single test manually

```bash
./scc tests/queens.c
gcc -static -o tests/queens.scc tests/queens.s
gcc -o tests/queens.gcc tests/queens.c
./tests/queens.scc > out.scc
./tests/queens.gcc > out.gcc
diff out.scc out.gcc
```
