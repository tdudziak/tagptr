#include <cstdio>
#include <cstdint>
#include <cmath>
#include <boost/foreach.hpp>

#include "Value.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>

using namespace boost::unit_test;

typedef tagptr::ValueTemplate<int64_t, double> Value;
typedef tagptr::ValueTemplate<int, double> SValue;

void test_real(double x)
{
    Value val(x);
    BOOST_CHECK(val.isReal());
    BOOST_CHECK(val.asReal() == x);

    val = Value(x);
    BOOST_CHECK(val.isReal());
    BOOST_CHECK(val.asReal() == x);

    val = Value(val);
    BOOST_CHECK(val.isReal());
    BOOST_CHECK(val.asReal() == x);
}

void test_int(int64_t x)
{
    Value val(x);
    SValue sval((int) x);
    BOOST_CHECK(val.asInt() == x);
    BOOST_CHECK(val.isInt());
    BOOST_CHECK(sval.isInt());
    BOOST_CHECK(int(val.asInt()) == sval.asInt());

    val = Value(x);
    sval = SValue((int) x);
    BOOST_CHECK(val.asInt() == x);
    BOOST_CHECK(val.isInt());
    BOOST_CHECK(sval.isInt());
    BOOST_CHECK(int(val.asInt()) == sval.asInt());

    val = Value(val);
    sval = SValue(sval);
    BOOST_CHECK(val.asInt() == x);
    BOOST_CHECK(val.isInt());
    BOOST_CHECK(sval.isInt());
    BOOST_CHECK(int(val.asInt()) == sval.asInt());
}

BOOST_AUTO_TEST_CASE(test_self_assignment)
{
    Value x(5LL);
    x = x;
    BOOST_CHECK(x.isInt());
    BOOST_CHECK(x.asInt() == 5);

    Value y(3.14);
    y = y;
    BOOST_CHECK(y.isReal());
    BOOST_CHECK(y.asReal() == 3.14);

    Value z((void*) &x);
    z = z;
    BOOST_CHECK(z.isPtr());
    BOOST_CHECK(z.asPtr() == (void*) &x);
}

BOOST_AUTO_TEST_CASE(test_different_assignments)
{
    Value x(5LL), y(3.14), z((void*) &x);
    x = y;
    BOOST_CHECK(x.asReal() == y.asReal());
    y = z;
    BOOST_CHECK(y.asPtr() == z.asPtr());
    x = y = z = Value(5LL);
    BOOST_CHECK(x.asInt() == 5);
    BOOST_CHECK(y.asInt() == 5);
    BOOST_CHECK(z.asInt() == 5);
}

#define PARAM_TEST(func, params) framework::master_test_suite().add(BOOST_PARAM_TEST_CASE(func, params, params+sizeof(params)/sizeof(params[0])))

bool init_unit_test()
{
    double double_params[] = { 3.14, INFINITY, -INFINITY, 1.0/INFINITY, -1.0/INFINITY };
    PARAM_TEST(test_real, double_params);
    int64_t int_params[] = { 0, 1, -1, INT32_MIN, INT32_MAX, UINT32_MAX, int64_t(INT32_MIN)-1, int64_t(INT32_MAX)+1, int64_t(UINT32_MAX)+1, INT64_MIN, INT64_MAX };
    PARAM_TEST(test_int, int_params);
    return true;
}

int main(int argc, char* argv[])
{
    return ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
