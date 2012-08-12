#define BOOST_TEST_MODULE unpack_from

#include <cstring>
#include <boost/test/included/unit_test.hpp>
#include <struct/struct.hpp>

using namespace boost;

// header
DEFINE_STRUCT(header,
	((3, TYPE_INT)) /* Key, Size, Msg ID */
)

// packet 1
DEFINE_STRUCT(login_request,
	((10, TYPE_CHAR)) /* Login    */
	((10, TYPE_CHAR)) /* Password */
)

// packet 2
DEFINE_STRUCT(login_response,
	((1, TYPE_BOOL)) /* Success ?  */
)

//____________________________________________________________________________//

BOOST_AUTO_TEST_CASE (test_size)
{
	BOOST_CHECK(header::size + login_request::size  == 
		3 * sizeof(int) + 10 + 10);
	BOOST_CHECK(login_response::size == sizeof(bool));
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_CASE (test_write_two_packets)
{
	// write to
	char data[header::size + login_request::size] = {0};
	char * p = data;
	header().pack_to(p,
		0x12345678,   /* Key    */
		sizeof(data), /* Size   */
		0xDEADBEEF    /* Msg ID */
	);
	p += header::size;
	login_request().pack_to(p,
		"Login",
		"Password"
	);
	
	p = data;
	BOOST_CHECK(*reinterpret_cast<unsigned int*>(p) == 0x12345678);
	p += sizeof(unsigned int);
	BOOST_CHECK(*reinterpret_cast<unsigned int*>(p) == sizeof(data));
	p += sizeof(unsigned int);
	BOOST_CHECK(*reinterpret_cast<unsigned int*>(p) == 0xDEADBEEF);
	p += sizeof(unsigned int);
	
	// read
	char * input = data;
	
	// receive header
	unsigned int key, size, msg_id;
	tie(key, size, msg_id) = header().unpack(input);
	BOOST_CHECK(msg_id == 0xDEADBEEF);
	// receive login request
	char * login, * password;
	tie(login, password) = login_request().unpack(input + header::size);
	
	BOOST_CHECK(strncmp(login, "Login", 10) == 0);
	BOOST_CHECK(strncmp(password, "Password", 10) == 0);
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_CASE (test_response)
{
	// test response only (im lazy)
	char data[login_response::size] = {0};
	login_response().pack_to(data,
		true
	);
	bool result;
	tie(result) = login_response().unpack(data);
	BOOST_CHECK(result);
}
