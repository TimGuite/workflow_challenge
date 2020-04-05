#include "catch.hpp"

#include <limits>
#include <type_traits>

#include "Exception.hpp"
#include "MyLibrary.hpp"

using namespace cpp_template;

// This tests the output of the `get_nth_prime` function
TEST_CASE("correct primes are returned", "[primes]") {
  REQUIRE(get_nth_prime(0) == 2);
}