Relevant headers:
#include <bcc/bcc_elf.h>
#include <bcc/bcc_syms.h>

bpftrace.{h,cpp}:
- `symbol_table_cache_`:
  - mapping from file path (to ELF file) to `map<Addr, ElfSymbol>`
  - used when `user_symbol_cache_type` is `per_program`
  - the `map<Addr, ElfSymbol>` construct is populated by iterating over
    all symbols in the file (woah!?)
- `pid_sym_`:
  - map<Pid, SymCache>
- `exe_sym_`:
  - mapping from path (to ELF file) to (Pid, SymCache)

get_symbol_table_for_elf()
get_func_symbols_from_file():
  - retrieves symbols from an ELF file, taking into account debug symbols

VENDOR_GTEST shows how to vendor sources from GitHub

Build the thing:
- prerequisites:
  - on Fedora: bcc-devel cereal-devel llvm-devel gtest-devel gmock-devel
    maybe but probably not: lldb-devel
    (see
    https://github.com/bpftrace/bpftrace/blob/master/docker/Dockerfile.fedora
    for full list)

rm -rf build/
mkdir build/
cd build/
cmake -DCMAKE_BUILD_TYPE=Release -DSTATIC_LINKING=0 -DUSE_BLAZESYM=1 ../
make VERBOSE=1 -j
