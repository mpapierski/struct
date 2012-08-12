CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (simple)

ADD_EXECUTABLE (simple
	simple.cxx
)

ADD_TEST (simple simple)
