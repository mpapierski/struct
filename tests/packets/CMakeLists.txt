CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (packets)

ADD_EXECUTABLE (packets
	packets.cxx
)

ADD_TEST (packets packets)
