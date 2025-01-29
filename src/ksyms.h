#pragma once

#include <cstdint>
#include <string>

typedef struct blaze_symbolizer blaze_symbolizer;

namespace bpftrace {
class Config;

class Ksyms {
public:
  Ksyms(const Config &config);
  ~Ksyms();

  Ksyms(Ksyms &) = delete;
  Ksyms &operator=(const Ksyms &) = delete;

  std::string resolve(uint64_t addr, bool show_offset);

private:
  const Config &config_;
  void *ksyms_{ nullptr };

#ifdef USE_BLAZESYM
  blaze_symbolizer *symbolizer_{ nullptr };

  std::optional<std::string> resolve_blazesym_int(uint64_t addr,
                                                  bool show_offset);
  std::string resolve_blazesym(uint64_t addr, bool show_offset);
#endif

  std::string resolve_bcc(uint64_t addr, bool show_offset);
};
} // namespace bpftrace
