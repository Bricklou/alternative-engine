#pragma once

#include <alte/export.hpp>
#include <cstdint>

namespace AltE {
  /**
   * @brief Represents a time value
   */
  class ALTE_API Time {
    public:
      /**
       * @brief Default constructor
       *
       * Set the time value to zero.
       */
      constexpr Time();

      /**
       * @brief Return the time value as a number of seconds
       *
       * @return Time in seconds
       * @see as_millis, as_micros
       */
      constexpr float as_seconds() const;

      /**
       * @brief Return the time value as a number of milliseconds
       *
       * @return Time in milliseconds
       * @see as_seconds, as_micros
       */
      constexpr uint32_t as_millis() const;

      /**
       * @brief Return the time value as a number of microseconds
       *
       * @return Time in microseconds
       * @see as_seconds, as_millis
       */
      constexpr uint64_t as_micros() const;

      /// Predefined "zero" time value
      static const Time Zero;

    private:
      friend constexpr ALTE_API Time seconds(float);
      friend constexpr ALTE_API Time milliseconds(int32_t);
      friend constexpr ALTE_API Time microseconds(int64_t);

      /**
       * @brief Construct from a number of microseconds
       *
       * This function is internal. To construct time values, use seconds(),
       * milliseconds or  microseconds() instead.
       *
       * @param microseconds Number of microseconds
       */
      constexpr Time(int64_t microseconds);

      /// Time value stored as microseconds
      int64_t _microseconds;
  };

  /**
   * @link #Time
   * @brief Construct a time value from a number of seconds
   *
   * @param amount Number of seconds
   * @return Time value constructed from the amount of seconds
   * @see Time::as_millis, Time::as_micros
   */
  [[nodiscard]] constexpr ALTE_API Time seconds(float amount);

  /**
   * @link #Time
   * @brief Construct a time value from a number of milliseconds
   *
   * @param amount Number of milliseconds
   * @return Time value constructed from the amount of milliseconds
   * @see Time::as_seconds, Time::as_micros
   */
  [[nodiscard]] constexpr ALTE_API Time milliseconds(int32_t amount);

  /**
   * @link #Time
   * @brief Construct a time value from a number of microseconds
   *
   * @param amount Number of microseconds
   * @return Time value constructed from the amount of microseconds
   * @see Time::as_seconds, Time::as_millis
   */
  [[nodiscard]] constexpr ALTE_API Time microseconds(int64_t amount);

  /**
   * @link #Time
   * @brief Overload of == operator to compare two time values
   *
   * @param left Left operand (a time)
   * @param right Right operand (a time)
   * @return True if the two time values are equal
   */
  [[nodiscard]] constexpr ALTE_API bool operator==(Time left, Time right);

  /**
   * @link #Time
   * @brief Overload of != operator to compare two time values
   *
   * @param left Left operand (a time)
   * @param right Right operand (a time)
   * @return True if the two time values are not equal
   */
  [[nodiscard]] constexpr ALTE_API bool operator!=(Time left, Time right);

  /**
   * @link #Time
   * @brief Overload of < operator to compare two time values
   *
   * @param left Left operand (a time)
   * @param right Right operand (a time)
   * @return True if the \a left is lesser \a right
   */
  [[nodiscard]] constexpr ALTE_API bool operator<(Time left, Time right);

  /**
   * @link #Time
   * @brief Overload of > operator to compare two time values
   *
   * @param left Left operand (a time)
   * @param right Right operand (a time)
   * @return True if the \a left is greater \a right
   */
  [[nodiscard]] constexpr ALTE_API bool operator>(Time left, Time right);

  /**
   * @link #Time
   * @brief Overload of <= operator to compare two time values
   *
   * @param left Left operand (a time)
   * @param right Right operand (a time)
   * @return True if the \a left is lesser or equal \a right
   */
  [[nodiscard]] constexpr ALTE_API bool operator<=(Time left, Time right);

  /**
   * @link #Time
   * @brief Overload of >= operator to compare two time values
   *
   * @param left Left operand (a time)
   * @param right Right operand (a time)
   * @return True if the \a left is greater or equal \a right
   */
  [[nodiscard]] constexpr ALTE_API bool operator>=(Time left, Time right);

  /**
   * @link #Time
   * @brief Overload of unary - operator to negate a time valie
   *
   * @param time Time value to negate
   * @return Negated time value
   */
  [[nodiscard]] constexpr ALTE_API Time operator-(Time time);

  /**
   * @link #Time
   * @brief Overload of binary + operator to add two time values
   *
   * @param left Left operand (a time)
   * @param right Right operand (a time)
   * @return Sum of the two time values
   */
  [[nodiscard]] constexpr ALTE_API Time operator+(Time left, Time right);

  /**
   * @link #Time
   * @brief Overload of binary - operator to subtract two time values
   *
   * @param left Left operand (a time)
   * @param right Right operand (a time)
   * @return Difference of the two time values
   */
  [[nodiscard]] constexpr ALTE_API Time operator-(Time left, Time right);

  /**
   * @link #Time
   * @brief Overload of binary += operator to add a time value to another
   *
   * @param left Left operand (a time)
   * @param right Right operand (a scalar)
   * @return Sum of the two time values
   */
  [[nodiscard]] constexpr ALTE_API Time operator+=(Time &left, float right);

  /**
   * @link #Time
   * @brief Overload of binary -= operator to subtract a time value from another
   *
   * @param left Left operand (a time)
   * @param right Right operand (a scalar)
   * @return Difference of the two time values
   */
  [[nodiscard]] constexpr ALTE_API Time operator-=(Time &left, float right);

  /**
   * @link #Time
   * @brief Overload of binary * operator to scale a time value
   *
   * @param left Left operand (a time)
   * @param right Right operand (a number)
   * @return \a left multiplied by \a right
   */
  [[nodiscard]] constexpr ALTE_API Time operator*(Time left, float right);

  /**
   * @link #Time
   * @brief Overload of binary * operator to scale a time value
   *
   * @param left Left operand (a time)
   * @param right Right operand (a number)
   * @return \a left multiplied by \a right
   */
  [[nodiscard]] constexpr ALTE_API Time operator*(Time left, int64_t right);

  /**
   * @link #Time
   * @brief Overload of binary * operator to scale a time value
   *
   * @param left Left operand (a number)
   * @param right Right operand (a time)
   * @return \a left multiplied by \a right
   */
  [[nodiscard]] constexpr ALTE_API Time operator*(int64_t left, Time right);

  /**
   * @link #Time
   * @brief Overload of binary * operator to scale a time value
   *
   * @param left Left operand (a number)
   * @param right Right operand (a time)
   * @return \a left multiplied by \a right
   */
  [[nodiscard]] constexpr ALTE_API Time operator*(float left, Time right);

  /**
   * @link #Time
   * @brief Overload of binary *= operator to scale a time value
   *
   * @param left Left operand (a time)
   * @param right Right operand (a number)
   * @return \a left multiplied by \a right
   */
  [[nodiscard]] constexpr ALTE_API Time &operator*=(Time &left, float right);

  /**
   * @link #Time
   * @brief Overload of binary *= operator to scale a time value
   *
   * @param left Left operand (a time)
   * @param right Right operand (a number)
   * @return \a left multiplied by \a right
   */
  [[nodiscard]] constexpr ALTE_API Time &operator*=(Time &left, int64_t right);

  /**
   * @link #Time
   * @brief Overload of binary / operator to divide a time value by a scalar
   *
   * @param left Left operand (a time)
   * @param right Right operand (a scalar)
   * @return Quotient of the two time values
   */
  [[nodiscard]] constexpr ALTE_API Time operator/(Time left, float right);

  /**
   * @link #Time
   * @brief Overload of binary /= operator to divide a time value by a scalar
   *
   * @param left Left operand (a time)
   * @param right Right operand (a scalar)
   * @return Quotient of the two time values
   */
  [[nodiscard]] constexpr ALTE_API Time &operator/=(Time &left, float right);

  /**
   * @link #Time
   * @brief Overload of binary / operator to divide a time value by a scalar
   *
   * @param left Left operand (a time)
   * @param right Right operand (a scalar)
   * @return Quotient of the two time values
   */
  [[nodiscard]] constexpr ALTE_API Time operator/(Time left, int64_t right);

  /**
   * @link #Time
   * @brief Overload of binary /= operator to divide a time value by a scalar
   *
   * @param left Left operand (a time)
   * @param right Right operand (a scalar)
   * @return Quotient of the two time values
   */
  [[nodiscard]] constexpr ALTE_API Time &operator/=(Time &left, int64_t right);

  /**
   * @link #Time
   * @brief Overload of binary / operator to compute the ratio of two time
   * values
   *
   * @param left Left operand (a time)
   * @param right Right operand (a time)
   * @return  \a left divided by \a right
   */
  [[nodiscard]] constexpr ALTE_API float operator/(Time left, Time right);

  /**
   * @link #Time
   * @brief Overload of binary %  operator to compute remainder of a time value
   * values
   *
   * @param left Left operand (a time)
   * @param right Right operand (a time)
   * @return  \a left modulo by \a right
   */
  [[nodiscard]] constexpr ALTE_API Time operator%(Time left, Time right);

  /**
   * @link #Time
   * @brief Overload of binary %= operator to compute remainder of a time value
   * values
   *
   * @param left Left operand (a time)
   * @param right Right operand (a time)
   * @return  \a left modulo by \a right
   */
  [[nodiscard]] constexpr ALTE_API Time &operator%=(Time &left, Time right);

} // namespace AltE

#include <alte/types/time.inl>