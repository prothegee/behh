#ifndef BEHH_PCH_HH
#define BEHH_PCH_HH

#include "config.hh"
#include "export.hh"
#ifdef behh_shared_EXPORTS
#include "export_shared.hh"
#else
#include "export_static.hh"
#endif

#include <array>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

namespace behh {
} // namespace behh

// alias for char
using chr_t = char;

// alias for bool
using bool_t = bool;

// alias for unsigned char
using uchr_t = unsigned char;

// alias for void type
using void_t = void;

// alias for float as in 32 bits
using float32_t = float;

// alias for double as in 64 bits
using float64_t = double;

/// no int128_t & float128_t, for now

// alias for std::vector<unsigned char> as buffer type
using buffer_t = std::vector<unsigned char>;

#endif // BEHH_PCH_HH