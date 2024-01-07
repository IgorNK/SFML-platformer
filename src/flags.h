#pragma once
#include <bitset>
#include <cstdint>
template <typename EnumT> class Flags {
  static_assert(std::is_enum_v<EnumT>,
                "Flags can only be specialized for enum types");
  using UnderlyingT =
      typename std::make_unsigned_t<typename std::underlying_type_t<EnumT>>;

public:
  Flags &set(EnumT e, bool value = true) noexcept {
    bits_.set(underlying(e), value);
    return *this;
  }

  Flags &reset(EnumT e) noexcept {
    set(e, false);
    return *this;
  }

  Flags &reset() noexcept {
    bits_.reset();
    return *this;
  }

  [[nodiscard]] bool all() const noexcept { return bits_.all(); }

  [[nodiscard]] bool any() const noexcept { return bits_.any(); }

  [[nodiscard]] bool none() const noexcept { return bits_.none(); }

  [[nodiscard]] std::size_t count() const noexcept { return bits_.count(); }

  constexpr bool operator[](EnumT e) const { return bits_[underlying(e)]; }

private:
  static constexpr UnderlyingT underlying(EnumT e) {
    return static_cast<UnderlyingT>(e);
  }

private:
  std::bitset<underlying(EnumT::size)> bits_;
};

enum class StaticCollisionDirection : std::uint8_t {
  top,
  right,
  bottom,
  left,
  size
};

using EStaticCollisionDirection = Flags<StaticCollisionDirection>;
