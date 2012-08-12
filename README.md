# struct

Python struct module implemented in C++.

You can use struct just like Python struct module. Define how your data structure looks like and use generated class with methods .pack(...) and .unpack().

## Requirements

CMake (2.6+)
Boost.Preprocessor (1.39.0+)
Boost.Tuple

## Building

* cd struct
* mkdir build
* cd build
* cmake ..
* make && make test

## License

GPLv3.

## TODO

* char types of size 1 are not char*'s.
* test packing funtion assembly to see if it is inlined properly.

## Help

How to use C++ struct library.

### Definition

	DEFINE_STRUCT(test,
		((2, TYPE_UNSIGNED_INT))
		((20, TYPE_CHAR))
		((20, TYPE_CHAR))
	)

This structure will match Python code:

	import struct
	test = struct.Struct('2I20s20s')

### Data size

Size of defined structure is implemented as enum inside the generated class.

	test::size

In our example this will return 48.

Same as Python:

	>> test.size
	48

### Packing data

C++ struct library generates pack() function by enumerating data types sequence to N parameters. In our example the test::pack() function is implemented as:

	inline char * pack(unsigned int p1, unsigned int p2, const char * p3, const char * p4) const
	{
		char * buffer = new char[size];
		char * result = buffer;
		
		*reinterpret_cast<unsigned int*>(result) = p1;
		result += 1 * sizeof(unsigned int);
		
		*reinterpret_cast<unsigned int*>(result) = p2;
		result += 1 * sizeof(unsigned int);
		
		memcpy(result, p3, 20);
		result += 20;
		
		memcpy(result, p4, 20);
		result += 20;
		
		return buffer;
	}
	
You have to pass values for each of the type defined.

	test t;
	char * data = t.pack(0x12345678, 0xdeadbeef, "Hello", "World");
	some_socket.write(data, test::size);
	delete[] data; // !

You have to take care of the result of pack function, otherwise you will end up with memory leaks.

Above code matches Python code:

	>> data = test.pack(0x12345678, 0xdeadbeef, "Hello", "World")
	>> some_socket.write(data)

### Unpacking data

C++ struct library generates unpack() function by enumerating data types sequence to tuple<> type. In our example the test::unpack() function is implemented as:

	boost::tuple<unsigned int, unsigned int, char *, char *>
	inline unpack(char * input) const
	{
		boost::tuple<unsigned int, unsigned int, char *, char *> result;
		
		get<0>(result) = *reinterpret_cast<unsigned int*>(input);
		input += 1 * sizeof(unsigned int);
		
		get<1>(result) = *reinterpret_cast<unsigned int*>(input);
		input += 1 * sizeof(unsigned int);
		
		get<2>(result) = reinterpret_cast<char*>(input);
		input += 20 * sizeof(char);
		
		get<3>(result) = reinterpret_cast<char*>(input);
		
		return result;
	}
	
Usage:

	using boost::tie;
	
	unsigned int a, unsigned int b;
	char * c, * d;
	
	test t;
	tie(a, b, c, d) = t.unpack(data_buffer);
	

Remember that decoded char * pointers are valid as long as data_buffer is valid.

Everything is as simple as you can do it in Python:

	>> import struct
	>> test = struct.Struct('2I20s20s')
	>> a, b, c, d = test.unpack(data_buffer)
	...
