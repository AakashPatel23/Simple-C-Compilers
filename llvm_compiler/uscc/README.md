# uscc — University Simple C Compiler

A compiler for USC (University Simple C), a small C-like language, built
on the LLVM 3.5 framework. It implements a complete pipeline: recursive
descent parsing, semantic analysis with a scoped symbol table, IR
lowering to LLVM bitcode, SSA construction, several classic optimization
passes, and Chaitin-Briggs graph-coloring register allocation.

Originally developed by Sanjay Madhav as course starting code; this
repository is my own completed implementation of every
compiler stage built on top of it.

## Language features

USC supports: functions with typed arguments and return values (`int`,
`char`, arrays), local and global variables, `if`/`while`/`for`/
`do-while`/`switch`-`case`-`default` control flow, `break`/`continue`,
compound-assignment operators, and array indexing with bounds-check
warnings.

## Pipeline stages

- **Parsing** (`scan/`, `parse/ParseStmt.cpp`, `parse/ParseExpr.cpp`):
  hand-written lexer (Flex) and recursive-descent parser producing an AST.
- **Semantic analysis** (`parse/Symbols.{h,cpp}`, `parse/Parse.cpp`,
  `parse/ASTExpr.cpp`): scoped symbol table, type checking, and
  diagnostics (identifier shadowing, unused return values,
  out-of-bounds array indices).
- **IR lowering** (`parse/ASTEmit.cpp`, `parse/Emitter.{h,cpp}`): emits
  LLVM IR from the AST.
- **SSA construction** (`opt/SSABuilder.{h,cpp}`, `opt/RedundantPhi.cpp`,
  `opt/Graph.{h,cpp}`): on-the-fly SSA form via the Braun et al.
  algorithm, plus redundant-phi elimination.
- **Optimizations** (`opt/ConstantOps.cpp`, `opt/ConstantBranch.cpp`,
  `opt/DeadBlocks.cpp`, `opt/LICM.cpp`, `opt/AvailableExpressions.{h,cpp}`,
  `opt/CSE.cpp`, `opt/DCE.cpp`, `opt/Liveness.{h,cpp}`): constant folding
  and branch simplification, dead-block removal, loop-invariant code
  motion, available-expression analysis, common-subexpression
  elimination, liveness analysis, and dead-code elimination.
- **Register allocation** (`opt/RegAlloc.cpp`): Chaitin-Briggs
  graph-coloring register allocator, emitting x86 assembly.

## Building and running

### 1. Clone

```
git clone https://github.com/AakashPatel23/Simple-C-Compilers.git
cd Simple-C-Compilers/llvm_compiler/uscc
```

Everything below is run from `uscc/` (this directory), not the repo root.

### 2. Install LLVM 3.5

This compiler links against LLVM 3.5's libraries, which predate modern
package managers — you'll need to build it from source:

```
curl -LO https://releases.llvm.org/3.5.0/llvm-3.5.0.src.tar.xz
tar xf llvm-3.5.0.src.tar.xz
cd llvm-3.5.0.src
./configure --enable-optimized
make -j$(nproc)
export PATH="$PWD/Release/bin:$PATH"   # so llvm-config is on PATH
cd -
```

You'll also need a C++ compiler with `-fno-rtti` support (any recent
g++/clang++ works).

### 3. Build uscc

```
make
```

This builds `bin/uscc`.

### 4. Run it

```
bin/uscc -O -p -s tests/cse/quicksort.usc -o quicksort.s
```

or write your own `.usc` file (see [Example](#example) below) and run:

```
bin/uscc [OPTIONS] <input.usc>
```

Key flags: `-a` (print AST), `-l` (print symbol table), `-p` (print LLVM
IR), `-O` (run optimization passes), `-ae`/`-cse`/`-rpr` (available
expressions / CSE / redundant-phi removal), `-dce`/`-liveness` (dead-code
elimination / liveness analysis), `-s` (emit x86 assembly, with
`--num-colors` controlling the register allocator's graph coloring), `-b`
(force bitcode output), `-o` (output file).

## Example

```c
// gcd.usc — Euclid's algorithm
int gcd(int a, int b) {
    while (b != 0) {
        int t;
        t = b;
        b = a - (a / b) * b; // a % b
        a = t;
    }
    return a;
}

int main() {
    printf("gcd(48, 18) = %d\n", gcd(48, 18));
    return 0;
}
```

```
$ uscc -O -p -s gcd.usc -o gcd.s
```

runs `gcd.usc` through parsing, semantic analysis, SSA construction, the
optimization passes, and Chaitin-Briggs register allocation, printing the
optimized LLVM IR (`-p`) and emitting x86 assembly (`-s`) to `gcd.s`.

## Tests

`tests/` is organized by pipeline stage: `parsing/`, `semantic_analysis/`,
`ir_lowering/`, `cse/`, `ssa/`, `optimizations/`, `register_allocation/`.
Each subdirectory has its own Python 2 `unittest` runner (e.g.
`tests/cse/testAE.py`) and expects a built `bin/uscc` two directories up
(i.e. run `make` from `uscc/` first). Run a suite with:

```
cd tests/cse
python2 testAE.py
```

## Status

**Not yet build-verified** — this tree hasn't been compiled and run end
to end, because no LLVM 3.5 toolchain was available in the environment it
was assembled in. To verify, on a machine with LLVM 3.5 installed:

1. `make` from `uscc/` (see [Building and running](#building-and-running));
   confirm `bin/uscc` builds without errors.
2. Run each `tests/<phase>/test*.py` with Python 2.
