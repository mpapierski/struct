#if !defined(STRUCT_HPP)
#define STRUCT_HPP

#include <boost/tuple/tuple.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/logical/not.hpp>
#include <boost/preprocessor/comparison/less.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/comparison/not_equal.hpp>
#include <boost/preprocessor/control/expr_if.hpp>

// type definitions (python-like)
#define MATCH_TYPE_s char
#define MATCH_TYPE_I unsigned int
#define MATCH_TYPE_i int

// map types to ids and aliases
#define TYPE_s 0
#define TYPE_CHAR TYPE_s
#define MATCH_TYPE_0 MATCH_TYPE_s

#define TYPE_I 1
#define TYPE_UNSIGNED_INT TYPE_I
#define MATCH_TYPE_1 MATCH_TYPE_I

#define TYPE_i 2
#define TYPE_INT TYPE_i
#define MATCH_TYPE_2 MATCH_TYPE_i

// todo:
// separate macros to:
// 1. get type for tuple type (no changes - char is char)
// 2. get type for pack function (char of size > 1 is const char *)
// 3. get type for unpack function (char of size > 1 is char*)

// double expand to get type based on a value
#define GET_TYPE_(type_value)                                                  \
	MATCH_TYPE_ ## type_value
#define GET_TYPE(type_value)                                                   \
	GET_TYPE_(type_value)

// get type used in code based on type id (add * to char type)
#define GET_TYPE_2(type_value) GET_TYPE_(type_value)                           \
	BOOST_PP_EXPR_IF(BOOST_PP_EQUAL(type_value, TYPE_s), *)

// define tuple type from (expanded) data types 
#define DEFINE_STRUCT_GET_TUPLE_TYPE_DISPATCH(r, data, i, elem)                \
	GET_TYPE_2(BOOST_PP_TUPLE_ELEM(2, 1, elem))                                \
	BOOST_PP_COMMA_IF(BOOST_PP_LESS(i, data))

// enumerate types to get type, type, type
// from sequence ((x, type))((x, type))((x, type))
#define DEFINE_STRUCT_GET_TYPES_ENUM(data_types)                               \
	BOOST_PP_SEQ_FOR_EACH_I_R(                                                 \
		1,                                                                     \
		DEFINE_STRUCT_GET_TUPLE_TYPE_DISPATCH,                                 \
		BOOST_PP_DEC(                                                          \
			BOOST_PP_SEQ_SIZE(data_types)                                      \
		),                                                                     \
		data_types                                                             \
	)
	
#define DEFINE_STRUCT_GET_TUPLE_TYPE(expanded_data_types)                      \
	boost::tuple<                                                              \
		DEFINE_STRUCT_GET_TYPES_ENUM(expanded_data_types)                      \
	>

// expand data types of type (n, type_id) to (1, type_id)(1, type_id)...
// dont expand char type
// sequence ((2, type_int))((20, char)) becomes
//  ((1, type_int))
//  ((1, type_int))
//  ((20, type_char))
#define EXPAND_DATA_TYPES_REPEATER(z, n, data)                                 \
	((1, data))

// dispatch for data types expander
// if char type then do nothing
// otherwise repeat seq n-times
#define EXPAND_DATA_TYPES_DISPATCH(r,data,i,elem)                              \
		BOOST_PP_IF(                                                           \
			BOOST_PP_EQUAL(                                                    \
				BOOST_PP_TUPLE_ELEM(2, 1, elem),                               \
				TYPE_s                                                         \
			),                                                                 \
			(elem),                                                            \
			BOOST_PP_REPEAT(                                                   \
				BOOST_PP_TUPLE_ELEM(2, 0, elem),                               \
				EXPAND_DATA_TYPES_REPEATER,                                    \
				BOOST_PP_TUPLE_ELEM(2, 1, elem)                                \
			)                                                                  \
		)
// expand data types in form of seq ((n1, type1))((n2, type2))((...))
#define EXPAND_DATA_TYPES(data_types)                                          \
	BOOST_PP_SEQ_FOR_EACH_I_R(                                                 \
		1,                                                                     \
		EXPAND_DATA_TYPES_DISPATCH,                                            \
		BOOST_PP_SEQ_SIZE(data_types),                                         \
		data_types                                                             \
	)

// reader code
// do not dereference char* type
#define READER_AUX_IMPL(i, n, type, type_count, type_id)                       \
	boost::get<i>(result) = BOOST_PP_EXPR_IF(                                         \
		BOOST_PP_NOT_EQUAL(type_id, TYPE_s),                                   \
		*) reinterpret_cast<type*>(input);                                     \
	BOOST_PP_EXPR_IF(BOOST_PP_LESS(i, n),                                      \
		input += type_count * sizeof(type);                                    \
	)

// dispatcher
#define DEFINE_READER_DISPATCH(r, data, i, elem)                               \
	READER_AUX_IMPL(                                                           \
		i,                                                                     \
		data,                                                                  \
		/* type */ GET_TYPE(BOOST_PP_TUPLE_ELEM(2, 1, elem)),                  \
		/* length */ BOOST_PP_TUPLE_ELEM(2, 0, elem),                          \
		/* type id */ BOOST_PP_TUPLE_ELEM(2, 1, elem)                          \
	)

// generate readers	
#define DEFINE_READER(data_types)                                              \
	BOOST_PP_SEQ_FOR_EACH_I_R(                                                 \
		1,                                                                     \
		DEFINE_READER_DISPATCH,                                                \
		BOOST_PP_DEC(                                                          \
			BOOST_PP_SEQ_SIZE(data_types)                                      \
		),                                                                     \
		data_types                                                             \
	)

// complicated formula to calculate data types tuple size (not expanded!)
// type is not converted to reader/writer form (char is char)
#define DEFINE_CALCSIZE_FORMULA(count, type)                                   \
	(count * sizeof(type))
	
// calcsize formula
// returns (n1 * sizeof(type1)) + .... (n * sizeof(typen))
#define DEFINE_CALCSIZE_DISPATCH(r, data, i, elem)                             \
	DEFINE_CALCSIZE_FORMULA(                                                   \
		BOOST_PP_TUPLE_ELEM(2, 0, elem),                                       \
		GET_TYPE(                                                              \
			BOOST_PP_TUPLE_ELEM(2, 1, elem)                                    \
		)                                                                      \
	) BOOST_PP_EXPR_IF(BOOST_PP_LESS(i, data), +)

// generate formula for structure size
// data_types is a seq of form ((n1, type1))((n2, type2))((n3, type3))
#define DEFINE_CALCSIZE(data_types)                                            \
	BOOST_PP_SEQ_FOR_EACH_I_R(                                                 \
		1,                                                                     \
		DEFINE_CALCSIZE_DISPATCH,                                              \
		BOOST_PP_DEC(                                                          \
			BOOST_PP_SEQ_SIZE(data_types)                                      \
		),                                                                     \
		data_types                                                             \
	)

// define parameters for pack (writer) method
// returns enum:
// type1 p0, type2 p1, type3 p2
#define DEFINE_WRITE_PARAMS_DISPATCH(r, data, i, elem)                         \
	BOOST_PP_EXPR_IF(                                                          \
		BOOST_PP_EQUAL(                                                        \
			BOOST_PP_TUPLE_ELEM(2, 1, elem),                                   \
			TYPE_s                                                             \
		),                                                                     \
		const                                                                  \
	)                                                                          \
	GET_TYPE_2(                                                                \
		BOOST_PP_TUPLE_ELEM(2, 1, elem)                                        \
	)                                                                          \
	BOOST_PP_CAT(                                                              \
		p,                                                                     \
		i                                                                      \
	)                                                                          \
	BOOST_PP_COMMA_IF(                                                         \
		BOOST_PP_LESS(i, data)                                                 \
	)
	
#define DEFINE_WRITER_PARAMS(data_types)                                       \
	BOOST_PP_SEQ_FOR_EACH_I_R(                                                 \
		1,                                                                     \
		DEFINE_WRITE_PARAMS_DISPATCH,                                          \
		BOOST_PP_DEC(                                                          \
		BOOST_PP_SEQ_SIZE(                                                     \
			data_types                                                         \
			)                                                                  \
		),                                                                     \
		data_types                                                             \
	)

// writer
#define WRITER(i, type_length, type_id, raw_data_type, data_type)              \
	BOOST_PP_IF(BOOST_PP_EQUAL(type_id, TYPE_s),                               \
		memcpy(result, BOOST_PP_CAT(p, i), type_length);,                      \
		*reinterpret_cast<data_type*>(result) = BOOST_PP_CAT(p, i);            \
	)                                                                          \
	result += type_length * sizeof(raw_data_type);                             \

#define DEFINE_WRITER_DISPATCH(r, data, i, elem)                               \
	WRITER(                                                                    \
		i,                                                                     \
		BOOST_PP_TUPLE_ELEM(2, 0, elem),                                       \
		BOOST_PP_TUPLE_ELEM(2, 1, elem),                                       \
		GET_TYPE(                                                              \
			BOOST_PP_TUPLE_ELEM(2, 1, elem)                                    \
		),                                                                     \
		GET_TYPE_2(                                                            \
			BOOST_PP_TUPLE_ELEM(2, 1, elem)                                    \
		)                                                                      \
	)
	
// packing function
#define DEFINE_WRITER(data_types)                                              \
	BOOST_PP_SEQ_FOR_EACH_I_R(1, DEFINE_WRITER_DISPATCH, _, data_types)

// struct implementation
// data_types is raw data types sequence
// expanded_data_types is an optimization so data types are expanded only once.
#define DEFINE_STRUCT_AUX_IMPL(name, data_types, expanded_data_types)          \
	struct name                                                                \
	{                                                                          \
		typedef DEFINE_STRUCT_GET_TUPLE_TYPE(expanded_data_types) tuple_type;  \
		inline tuple_type unpack(char* input) const                            \
		{                                                                      \
			tuple_type result;                                                 \
			DEFINE_READER(expanded_data_types)                                 \
			return result;                                                     \
		}                                                                      \
		inline char* pack( DEFINE_WRITER_PARAMS(expanded_data_types) ) const   \
		{                                                                      \
			char* buffer = new char[name::size];                               \
			char* result = buffer;                                             \
			DEFINE_WRITER(expanded_data_types);                                \
			return buffer;                                                     \
		}                                                                      \
		enum { size = DEFINE_CALCSIZE(data_types) };                           \
	};
	
// define named struct
#define DEFINE_STRUCT(name, data_types)                                        \
	DEFINE_STRUCT_AUX_IMPL(                                                    \
		name,                                                                  \
		data_types,                                                            \
		EXPAND_DATA_TYPES(data_types)                                          \
	)

#endif /* STRUCT_HPP */
