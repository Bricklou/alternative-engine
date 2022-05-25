#pragma once

// Just to
#ifndef ALTE_API
  #include <alte/types/time.hpp>
#endif

namespace AltE {

  constexpr Time::Time() : _microseconds(0) {}

  constexpr float Time::as_seconds() const {
    return static_cast<float>(_microseconds) / 1000000.0f;
  }

  constexpr uint32_t Time::as_millis() const {
    return static_cast<uint32_t>(_microseconds) / 1000;
  }

  constexpr uint64_t Time::as_micros() const { return _microseconds; }

  constexpr Time::Time(int64_t microseconds) : _microseconds(microseconds) {}

  constexpr Time seconds(float amount) {
    return Time(static_cast<int64_t>(amount * 1000000.0f));
  }

  constexpr Time milliseconds(int32_t amount) {
    return Time(static_cast<int64_t>(amount * 1000));
  }

  constexpr Time microseconds(int64_t amount) {
    return Time(static_cast<int64_t>(amount));
  }

  constexpr bool operator==(Time &left, Time &right) {
    return left.as_micros() == right.as_micros();
  }

  constexpr bool operator!=(Time &left, Time &right) {
    return left.as_micros() != right.as_micros();
  }

  constexpr bool operator<(Time &left, Time &right) {
    return left.as_micros() < right.as_micros();
  }

  constexpr bool operator>(Time &left, Time &right) {
    return left.as_micros() > right.as_micros();
  }

  constexpr bool operator<=(Time left, Time right) {
    return left.as_micros() <= right.as_micros();
  }

  constexpr bool operator>=(Time left, Time right) {
    return left.as_micros() >= right.as_micros();
  }

  constexpr Time operator-(Time &right) {
    return microseconds(-right.as_micros());
  }

  constexpr Time operator+(Time left, Time right) {
    return microseconds(left.as_micros() + right.as_micros());
  }

  constexpr Time &operator+=(Time &left, Time right) {
    return left = left + right;
  }

  constexpr Time operator-(Time left, Time right) {
    return microseconds(left.as_micros() - right.as_micros());
  }

  constexpr Time &operator-=(Time &left, Time right) {
    return left = left - right;
  }

  constexpr Time operator*(Time left, float right) {
    return seconds(left.as_seconds() * right);
  }

  constexpr Time operator*(Time left, uint64_t right) {
    return microseconds(left.as_micros() * right);
  }

  constexpr Time operator*(float left, Time right) { return right * left; }

  constexpr Time operator*(uint64_t left, Time right) { return right * left; }

  constexpr Time &operator*=(Time &left, float right) {
    return left = left * right;
  }

  constexpr Time &operator*=(Time &left, uint64_t right) {
    return left = left * right;
  }

  constexpr Time operator/(Time left, float right) {
    return seconds(left.as_seconds() / right);
  }

  constexpr Time operator/(Time &left, uint64_t right) {
    return microseconds(left.as_micros() / right);
  }

  constexpr Time &operator/=(Time &left, float right) {
    return left = left / right;
  }

  constexpr Time &operator/=(Time &left, uint64_t right) {
    return left = left / right;
  }

  constexpr float operator/(Time left, Time right) {
    return left.as_seconds() / right.as_seconds();
  }

  constexpr Time operator%(Time left, Time right) {
    return microseconds(left.as_micros() % right.as_micros());
  }

  constexpr Time &operator%=(Time &left, Time right) {
    return left = left % right;
  }

  // Note: the 'inline' keyword here is technically not required, but VS2019
  // fails to compile with a bogus "multiple definition" error if not explicitly
  // used.
  inline constexpr Time Time::Zero;
} // namespace AltE