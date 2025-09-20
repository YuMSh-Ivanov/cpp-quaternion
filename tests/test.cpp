#include <cmath>
#include <string>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "quaternion.h"

template <>
struct Catch::StringMaker<quaternion>
{
    static std::string convert(const quaternion& q)
    {
        std::string result = std::to_string(q.real());

        result += q.imaginary_x() >= 0 ? " + " : " - ";
        result += std::to_string(std::abs(q.imaginary_x()));
        result += 'i';

        result += q.imaginary_y() >= 0 ? " + " : " - ";
        result += std::to_string(std::abs(q.imaginary_y()));
        result += 'j';

        result += q.imaginary_z() >= 0 ? " + " : " - ";
        result += std::to_string(std::abs(q.imaginary_z()));
        result += 'k';

        return result;
    }
};

TEST_CASE("equality")
{
    const auto [r, x, y, z] = GENERATE(table<double, double, double, double>({
            {-3,   2,    -3.5, 2.5},
            {-2.5, 4.5,  -4,   -4 },
            {-4.5, -4.5, 2,    4  },
            {-0.5, 5,    -1,   2.5},
            {-1.5, -4,   0,    -2 }
    }));

    const quaternion lhs(r, x, y, z);

    REQUIRE(lhs == quaternion(r, x, y, z));
    REQUIRE_FALSE(lhs != quaternion(r, x, y, z));

    REQUIRE_FALSE(lhs == quaternion(r + 1, x, y, z));
    REQUIRE(lhs != quaternion(r + 1, x, y, z));

    REQUIRE_FALSE(lhs == quaternion(r, x + 1, y, z));
    REQUIRE(lhs != quaternion(r, x + 1, y, z));

    REQUIRE_FALSE(lhs == quaternion(r, x, y + 1, z));
    REQUIRE(lhs != quaternion(r, x, y + 1, z));

    REQUIRE_FALSE(lhs == quaternion(r, x, y, z + 1));
    REQUIRE(lhs != quaternion(r, x, y, z + 1));

    REQUIRE_FALSE(lhs == quaternion(r + 1, x - 1, y + 1, z - 1));
    REQUIRE(lhs != quaternion(r + 1, x - 1, y + 1, z - 1));
}

TEST_CASE("default_constructor")
{
    REQUIRE(quaternion() == quaternion(0, 0, 0, 0));
}

TEST_CASE("constructor_of_double")
{
    double value = GENERATE(0, -3, 2, -4.5, -4);
    REQUIRE(quaternion(value) == quaternion(value, 0, 0, 0));
}

TEST_CASE("copy_constructor")
{
    const quaternion arg = GENERATE(
            quaternion(2.5, 5, 1.5, -1),
            quaternion(-4.5, 0, -1.5, 4.5),
            quaternion(2, 5, -1.5, -2.5),
            quaternion(-2, 4, 4.5, -2.5),
            quaternion(-0.5, 4.5, 0, -5));
    const quaternion copy(arg);

    REQUIRE(arg == copy);
}

TEST_CASE("copy_assignment")
{
    auto [lhs, rhs] = GENERATE(table<quaternion, quaternion>({
            {quaternion(1.5,  1, -1,   3.5),  quaternion(-4,   -0.5, 2.5, 2)   },
            {quaternion(1.5,  3, -1.5, 2),    quaternion(0,    4,    2.5, -3)  },
            {quaternion(3,    5, -2,   -4.5), quaternion(-1.5, -3,   -3,  -2.5)},
            {quaternion(-0.5, 4, -3.5, 4),    quaternion(-4,   -0.5, 3.5, -0.5)},
            {quaternion(2.5,  3, 5,    -5),   quaternion(-1,   4.5,  5,   4.5) }
    }));

    lhs = std::as_const(rhs);

    REQUIRE(lhs == rhs);
}

TEST_CASE("prefix_plus")
{
    const auto arg = GENERATE(
            quaternion(1.5, 0, 0.5, -1.5),
            quaternion(4, -2.5, 1.5, 3),
            quaternion(-3.5, 1.5, 3, -4.5),
            quaternion(5, 2.5, 1.5, -2.5),
            quaternion(2.5, -3, 4, -4));

    CAPTURE(arg);

    REQUIRE(+arg == arg);
}

TEST_CASE("prefix_minus")
{
    const auto [arg, result] = GENERATE(table<quaternion, quaternion>({
            {quaternion(0,    2,    3,   1),   quaternion(-0,   -2,  -3,   -1)  },
            {quaternion(3.5,  0,    4,   4),   quaternion(-3.5, -0,  -4,   -4)  },
            {quaternion(-4,   -2.5, -3,  2.5), quaternion(4,    2.5, 3,    -2.5)},
            {quaternion(2,    -3.5, 3.5, 4.5), quaternion(-2,   3.5, -3.5, -4.5)},
            {quaternion(-3.5, -5,   -4,  4.5), quaternion(3.5,  5,   4,    -4.5)}
    }));

    CAPTURE(arg, result);

    REQUIRE(-arg == result);
}

TEST_CASE("conjugate")
{
    const auto [arg, result] = GENERATE(table<quaternion, quaternion>({
            {quaternion(-4.5, -4.5, 3.5,  0),    quaternion(-4.5, 4.5,  -3.5, -0) },
            {quaternion(-2.5, -1,   5,    -4.5), quaternion(-2.5, 1,    -5,   4.5)},
            {quaternion(-4,   -1,   -2.5, 0),    quaternion(-4,   1,    2.5,  -0) },
            {quaternion(1.5,  4,    1,    3),    quaternion(1.5,  -4,   -1,   -3) },
            {quaternion(-3.5, 3.5,  -4.5, -5),   quaternion(-3.5, -3.5, 4.5,  5)  }
    }));

    CAPTURE(arg, result);

    REQUIRE(arg.conjugate() == result);
}

TEST_CASE("increment_and_decrement")
{
    const auto [r, x, y, z] = GENERATE(table<double, double, double, double>({
            {-5,  2.5,  -4.5, -5  },
            {2,   -3.5, 3,    3   },
            {2.5, 0,    -4.5, 0.5 },
            {2.5, 5,    -4,   -0.5},
            {-1,  2.5,  -1.5, -4.5}
    }));
    quaternion q(r, x, y, z);

    SECTION("pre_increment")
    {
        REQUIRE(++q == quaternion(r + 1, x, y, z));
        REQUIRE(q == quaternion(r + 1, x, y, z));
    }
    SECTION("pre_increment_return_value")
    {
        decltype(auto) result = ++q;
        if (&result != &q)
        {
            FAIL("prefix operator++ should return reference to left argument (not copy)");
        }
    }
    SECTION("post_increment")
    {
        REQUIRE(q++ == quaternion(r, x, y, z));
        REQUIRE(q == quaternion(r + 1, x, y, z));
    }
    SECTION("pre_decrement")
    {
        REQUIRE(--q == quaternion(r - 1, x, y, z));
        REQUIRE(q == quaternion(r - 1, x, y, z));
    }
    SECTION("pre_decrement_return_value")
    {
        decltype(auto) result = --q;
        if (&result != &q)
        {
            FAIL("prefix operator-- should return reference to left argument (not copy)");
        }
    }
    SECTION("post_decrement")
    {
        REQUIRE(q-- == quaternion(r, x, y, z));
        REQUIRE(q == quaternion(r - 1, x, y, z));
    }
}

TEST_CASE("norm")
{
    const auto [arg, result] = GENERATE(table<quaternion, double>({
            {quaternion(-2.5, -4,  -3.5, -4.5), 54.75},
            {quaternion(-0.5, 2,   -3.5, 2.5),  22.75},
            {quaternion(1.5,  5,   0.5,  1),    28.5 },
            {quaternion(1,    2.5, -4,   -4.5), 43.5 },
            {quaternion(4.5,  2.5, 4,    -2.5), 48.75}
    }));

    CAPTURE(arg, result);

    REQUIRE(arg.norm() == result);
}

TEST_CASE("add")
{
    const auto [lhs, rhs, result] = GENERATE(table<quaternion, quaternion, quaternion>({
            {quaternion(-4,  1,    -2.5, -3.5), quaternion(2.5,  -0.5, -0.5, 1.5),  quaternion(-1.5, 0.5, -3, -2)  },
            {quaternion(-5,  -3.5, -4.5, 3.5),  quaternion(4,    -0.5, 0.5,  -1.5), quaternion(-1,   -4,  -4, 2)   },
            {quaternion(3,   3.5,  2,    -2.5), quaternion(-1.5, 5,    0,    -2),   quaternion(1.5,  8.5, 2,  -4.5)},
            {quaternion(-5,  -1,   0,    -3),   quaternion(-5,   5,    3,    1),    quaternion(-10,  4,   3,  -2)  },
            {quaternion(3.5, 3.5,  1.5,  -2),   quaternion(1,    4,    -0.5, -2.5), quaternion(4.5,  7.5, 1,  -4.5)}
    }));

    CAPTURE(lhs, rhs, result);

    REQUIRE(lhs + rhs == result);

    quaternion mutable_lhs = lhs;
    mutable_lhs += rhs;
    REQUIRE(mutable_lhs == result);
}

TEST_CASE("subtract")
{
    const auto [lhs, rhs, result] = GENERATE(table<quaternion, quaternion, quaternion>({
            {quaternion(-3,   0.5,  0,    -1.5), quaternion(2,    -1,   4,  1),   quaternion(-5, 1.5, -4,  -2.5)},
            {quaternion(-2,   0,    -2,   -1),   quaternion(0,    -1,   -2, 0),   quaternion(-2, 1,   0,   -1)  },
            {quaternion(-4.5, -2.5, -2,   0.5),  quaternion(-4.5, -5,   -3, 1),   quaternion(0,  2.5, 1,   -0.5)},
            {quaternion(-5,   5,    -5,   0.5),  quaternion(1,    -2.5, 1,  2.5), quaternion(-6, 7.5, -6,  -2)  },
            {quaternion(5,    4,    -0.5, -3.5), quaternion(0,    -5,   -3, 1),   quaternion(5,  9,   2.5, -4.5)}
    }));

    CAPTURE(lhs, rhs, result);

    REQUIRE(lhs - rhs == result);

    quaternion mutable_lhs = lhs;
    mutable_lhs -= rhs;
    REQUIRE(mutable_lhs == result);
}

TEST_CASE("multiply")
{
    const auto [lhs, rhs, result] = GENERATE(table<quaternion, quaternion, quaternion>({
            {quaternion(-5, -5, 4.5,  -2),   quaternion(-1,  -1,  -1.5, -3.5), quaternion(-0.25, -8.75,  -12.5, 31.5)  },
            {quaternion(5,  -4, -2.5, -4.5), quaternion(0.5, 3,   4.5,  -1.5), quaternion(19,    37,     1.75,  -20.25)},
            {quaternion(4,  2,  4.5,  3.5),  quaternion(0,   1,   -0.5, -4),   quaternion(14.25, -12.25, 9.5,   -21.5) },
            {quaternion(2,  -4, -4.5, -3.5), quaternion(-4,  2.5, 0.5,  1.5),  quaternion(9.5,   16,     16.25, 26.25) },
            {quaternion(5,  4,  -3.5, 2),    quaternion(2,   3,   -2.5, -2.5), quaternion(-5.75, 36.75,  -3.5,  -8)    }
    }));

    CAPTURE(lhs, rhs, result);

    REQUIRE(lhs * rhs == result);

    quaternion mutable_lhs = lhs;
    mutable_lhs *= rhs;
    REQUIRE(mutable_lhs == result);
}

TEST_CASE("divide")
{
    const auto [lhs, rhs, result] = GENERATE(table<quaternion, quaternion, quaternion>({
            {quaternion(3,    -3,   3,    -3.5), quaternion(1,    -3,   4.5,  1), quaternion(0.704,    -0.408,  -0.768, -0.064)},
            {quaternion(-3,   -3.5, 0.5,  3.5),  quaternion(1,    -1,   -0.5, 2), quaternion(1.16,     -1.48,   -0.72,  1.16)  },
            {quaternion(-3.5, 4.5,  1,    -3),   quaternion(-1.5, -2.5, -2,   0), quaternion(-0.64,    -0.76,   -1.28,  0.88)  },
            {quaternion(1,    -3,   -2.5, -1.5), quaternion(-2,   0,    2,    1), quaternion(-8.5 / 9, 5.5 / 9, 0,      8. / 9)}
    }));

    CAPTURE(lhs, rhs, result);

    REQUIRE(lhs / rhs == result);

    quaternion mutable_lhs = lhs;
    mutable_lhs /= rhs;
    REQUIRE(mutable_lhs == result);
}

TEST_CASE("implicit_double_conversion")
{
    auto [quaternion_value, double_value] = GENERATE(table<quaternion, double>({
            {quaternion(1,    -4.5, 2.5,  2.5), 3   },
            {quaternion(4,    -2.5, -5,   3.5), -4.5},
            {quaternion(1,    -1.5, 0,    2),   -0.5},
            {quaternion(-4.5, 3,    4.5,  -1),  -2  },
            {quaternion(-4,   0.5,  -3.5, 0.5), 3.5 }
    }));

    REQUIRE_FALSE(quaternion_value == double_value);
    REQUIRE(quaternion_value != double_value);
    REQUIRE(quaternion_value + double_value == double_value + quaternion_value);
    REQUIRE(quaternion_value - double_value == -(double_value - quaternion_value));
    REQUIRE(quaternion_value * double_value == double_value * quaternion_value);
    REQUIRE(quaternion_value / double_value != double_value / quaternion_value);
}

TEST_CASE("prefix_plus_return_value")
{
    quaternion arg(-2.5, -3, -5, -2);
    decltype(auto) result = +arg;
    if (&result == &arg)
    {
        FAIL("prefix operator+ should return copy, not reference");
    }
}

TEST_CASE("assignments_return_values")
{
    auto [lhs, rhs] = GENERATE(table<quaternion, quaternion>({
            {quaternion(-4, -1.5, -3.5, -5),  quaternion(-0.5, 2,    1,    -0.5)},
            {quaternion(-5, -2.5, -2.5, 4),   quaternion(-3,   -2.5, 4.5,  1.5) },
            {quaternion(-5, 3.5,  4,    4.5), quaternion(-2.5, 0,    -2.5, 0.5) },
            {quaternion(3,  1,    -2.5, -3),  quaternion(0.5,  2,    1,    4)   },
            {quaternion(5,  -2.5, 4.5,  4.5), quaternion(3,    -4.5, -1,   -1)  }
    }));

    SECTION("copy_assign")
    {
        if (&(lhs = rhs) != &lhs)
        {
            FAIL("operator= should return reference to left argument");
        }
    }

    SECTION("add_assign")
    {
        if (&(lhs += rhs) != &lhs)
        {
            FAIL("operator+= should return reference to left argument");
        }
    }

    SECTION("subtract_assign")
    {
        if (&(lhs -= rhs) != &lhs)
        {
            FAIL("operator-= should return reference to left argument");
        }
    }

    SECTION("multiply_assign")
    {
        if (&(lhs *= rhs) != &lhs)
        {
            FAIL("operator*= should return reference to left argument");
        }
    }

    SECTION("divide_assign")
    {
        if (&(lhs /= rhs) != &lhs)
        {
            FAIL("operator/= should return reference to left argument");
        }
    }
}

TEST_CASE("assignments_to_itself")
{
    quaternion arg = GENERATE(
            quaternion(-3, 5, 0, 4.5),
            quaternion(-4, -2.5, 1.5, 3),
            quaternion(5, 1, -4.5, 1.5),
            quaternion(3.5, -4.5, -4.5, -2),
            quaternion(5, 4, -5, 1));
    const quaternion copy = arg;

    SECTION("copy_assign")
    {
        arg = arg;
        REQUIRE(arg == copy);
    }

    SECTION("add_assign")
    {
        arg += arg;
        REQUIRE(arg == copy + copy);
    }

    SECTION("subtract_assign")
    {
        arg -= arg;
        REQUIRE(arg == quaternion(0, 0, 0, 0));
    }

    SECTION("multiply_assign")
    {
        arg *= arg;
        REQUIRE(arg == copy * copy);
    }

    SECTION("divide_assign")
    {
        arg /= arg;
        REQUIRE(arg == 1);
    }
}

#ifdef BONUS_VERSION
TEST_CASE("bonus")
{
    REQUIRE(3_i == quaternion(0, 3, 0, 0));
    REQUIRE(-2.5_i == quaternion(0, -2.5, 0, 0));
    REQUIRE(4_j == quaternion(0, 0, 4, 0));
    REQUIRE(1.5_j == quaternion(0, 0, 1.5, 0));
    REQUIRE(1_k == quaternion(0, 0, 0, 1));
    REQUIRE(3.5_k == quaternion(0, 0, 0, 3.5));
}
#endif
