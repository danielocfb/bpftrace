#include <sstream>

#include <bcc/bcc_syms.h>

#include "blazesym_int.h"
#include "config.h"
#include "ksyms.h"
#include "utils.h"

namespace bpftrace {
Ksyms::Ksyms(const Config &config) : config_(config)
{
}

Ksyms::~Ksyms()
{
  if (ksyms_)
    bcc_free_symcache(ksyms_, -1);

#ifdef USE_BLAZESYM
  if (symbolizer_)
    blaze_symbolizer_free(symbolizer_);
#endif
}

std::string Ksyms::resolve_bcc(uint64_t addr, bool show_offset)
{
  struct bcc_symbol ksym;
  std::ostringstream symbol;

  if (!ksyms_)
    ksyms_ = bcc_symcache_new(-1, nullptr);

  if (bcc_symcache_resolve(ksyms_, addr, &ksym) == 0) {
    symbol << ksym.name;
    if (show_offset)
      symbol << "+" << ksym.offset;
  } else {
    symbol << reinterpret_cast<void *>(addr);
  }

  return symbol.str();
}

#ifdef USE_BLAZESYM
std::optional<std::string> Ksyms::resolve_blazesym_int(uint64_t addr,
                                                       bool show_offset)
{
  if (symbolizer_ == nullptr) {
    symbolizer_ = blaze_symbolizer_new();
    return std::nullopt;
  }

  std::ostringstream symbol;
  BLAZESYM_OPT blaze_symbolize_src_kernel src = {
    .type_size = sizeof(src),
    // Use default system-wide kallsyms file.
    .kallsyms = NULL,
    // TODO: We should eventually support using debug symbols in the
    //       kernel image, if found on the file system.
    .debug_syms = false,
  };
  uint64_t addrs[1] = { addr };

  const blaze_syms *syms = blaze_symbolize_kernel_abs_addrs(
      symbolizer_, &src, addrs, ARRAY_SIZE(addrs));
  if (syms == NULL)
    return std::nullopt;

  const blaze_sym *sym = &syms->syms[0];
  symbol << sym->name;
  if (show_offset) {
    auto offset = addr - sym->addr;
    symbol << "+" << offset;
  }
  blaze_syms_free(syms);
  return symbol.str();
}

std::string Ksyms::resolve_blazesym(uint64_t addr, bool show_offset)
{
  if (auto sym = resolve_blazesym_int(addr, show_offset)) {
    return *sym;
  } else {
    std::ostringstream symbol;
    symbol << reinterpret_cast<void *>(addr);
    return symbol.str();
  }
}
#endif

std::string Ksyms::resolve(uint64_t addr, bool show_offset)
{
#ifdef USE_BLAZESYM
  if (config_.get(ConfigKeyBool::use_blazesym))
    return resolve_blazesym(addr, show_offset);
  else
#endif
    return resolve_bcc(addr, show_offset);
}
} // namespace bpftrace
