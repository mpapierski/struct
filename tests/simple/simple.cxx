#define BOOST_TEST_MODULE simple

#include <cstring>
#include <boost/test/included/unit_test.hpp>
#include <struct/struct.hpp>

using namespace boost;

DEFINE_STRUCT(simple,
	((2, TYPE_UNSIGNED_INT))
	((20, TYPE_CHAR))
	((20, TYPE_CHAR))
	((1, TYPE_UNSIGNED_INT))
)

static void write_test_data(char* dest)
{
	*reinterpret_cast<unsigned int*>(dest) = 0xdeadbeef;
	dest += sizeof(unsigned int);
	
	*reinterpret_cast<unsigned int*>(dest) = 0x99887766;
	dest += sizeof(unsigned int);
	
	strncpy(dest, "Hello world!", 20);
	dest += 20;
	
	strncpy(dest, "12345678901234567890", 20);
	dest += 20;
	
	*reinterpret_cast<unsigned int*>(dest) = 0xfbadc0d3;
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_CASE (test_ctor)
{
	// make sure there is no errors in generated code
	simple s;
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_CASE( test_size )
{
	// test size of the struct data
	BOOST_CHECK(
		simple::size ==
		(2 * sizeof(unsigned int)) + 20 + 20 + sizeof(unsigned int)
	);
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_CASE( test_read )
{
	// read test data
	
	unsigned int a, b;
	char* c, *d;
	unsigned int e;
	
	// test data
	char testdata[simple::size] = {0};
	write_test_data(testdata);
	
	simple s;
	tie(a, b, c, d, e) = s.unpack(testdata);
	
	BOOST_CHECK(a == 0xdeadbeef);
	BOOST_CHECK(b == 0x99887766);
	BOOST_CHECK(strncmp(c, "Hello world!", 20) == 0);
	BOOST_CHECK(strncmp(d, "12345678901234567890", 20) == 0);
	BOOST_CHECK(e == 0xfbadc0d3);
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_CASE ( test_write_and_read )
{
	simple s;
	char * result = s.pack(
		0xdeadbeef,
		0x99887766,
		"Hello world!",
		"12345678901234567890",
		0xfbadc0d3
	);
	
	BOOST_CHECK(!!result);
	
	char * p = result;
	
	BOOST_CHECK(*reinterpret_cast<unsigned int*>(p) == 0xdeadbeef);
	p += sizeof(unsigned int);
	
	BOOST_CHECK(*reinterpret_cast<unsigned int*>(p) == 0x99887766);
	p += sizeof(unsigned int);
	
	BOOST_CHECK(strncmp(p, "Hello world!", 20) == 0);
	p += 20;
	
	BOOST_CHECK(strncmp(p, "12345678901234567890", 20) == 0);
	p += 20;
	
	unsigned int a, b;
	char* c, *d;
	unsigned int e;
	
	simple s2;
	tie(a, b, c, d, e) = s2.unpack(result);
	
	BOOST_CHECK(a == 0xdeadbeef);
	BOOST_CHECK(b == 0x99887766);
	BOOST_CHECK(strncmp(c, "Hello world!", 20) == 0);
	BOOST_CHECK(strncmp(d, "12345678901234567890", 20) == 0);
	BOOST_CHECK(e == 0xfbadc0d3);
	
	delete[] result;
}
