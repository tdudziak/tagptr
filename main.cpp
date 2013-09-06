#include <cstdio>
#include <cstdint>
#include <cmath>
#include <boost/foreach.hpp>

#include "Value.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>

using tagptr::Value;
using namespace boost::unit_test;

void test_double(double x)
{
    Value val(x);
    BOOST_CHECK(val.isDouble());
    BOOST_CHECK(val.asDouble() == x);

    val = Value(x);
    BOOST_CHECK(val.isDouble());
    BOOST_CHECK(val.asDouble() == x);

    val = Value(val);
    BOOST_CHECK(val.isDouble());
    BOOST_CHECK(val.asDouble() == x);
}

void test_int(int64_t x)
{
    Value val(x);
    BOOST_CHECK(val.asInt32() == (int32_t) x);
    BOOST_CHECK(val.asInt64() == x);
    BOOST_CHECK(val.isInt());

    val = Value(x);
    BOOST_CHECK(val.asInt32() == (int32_t) x);
    BOOST_CHECK(val.asInt64() == x);
    BOOST_CHECK(val.isInt());

    val = Value(val);
    BOOST_CHECK(val.asInt32() == (int32_t) x);
    BOOST_CHECK(val.asInt64() == x);
    BOOST_CHECK(val.isInt());
}

BOOST_AUTO_TEST_CASE(test_self_assignment)
{
    Value x(5);
    x = x;
    BOOST_CHECK(x.isInt());
    BOOST_CHECK(x.asInt32() == 5);

    Value y(3.14);
    y = y;
    BOOST_CHECK(y.isDouble());
    BOOST_CHECK(y.asDouble() == 3.14);

    Value z((void*) &x);
    z = z;
    BOOST_CHECK(z.isPtr());
    BOOST_CHECK(z.asPtr() == (void*) &x);
}

BOOST_AUTO_TEST_CASE(test_different_assignments)
{
    Value x(5), y(3.14), z((void*) &x);
    x = y;
    BOOST_CHECK(x.asDouble() == y.asDouble());
    y = z;
    BOOST_CHECK(y.asPtr() == z.asPtr());
    x = y = z = Value(5);
    BOOST_CHECK(x.asInt32() == 5);
    BOOST_CHECK(y.asInt32() == 5);
    BOOST_CHECK(z.asInt32() == 5);
}

#define PARAM_TEST(func, params) framework::master_test_suite().add(BOOST_PARAM_TEST_CASE(func, params, params+sizeof(params)/sizeof(params[0])))

bool init_unit_test()
{
    double double_params[] = { 3.14, INFINITY, -INFINITY, 1.0/INFINITY, -1.0/INFINITY };
    PARAM_TEST(test_double, double_params);
    int64_t int_params[] = { 0, 1, -1, INT32_MIN, INT32_MAX, int64_t(INT32_MIN)-1, int64_t(INT32_MAX)+1, INT64_MIN, INT64_MAX };
    PARAM_TEST(test_int, int_params);
    return true;
}

int main(int argc, char* argv[])
{
    return ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
