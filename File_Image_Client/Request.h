#ifndef REQUEST_H
#define REQUEST_H
struct Request
{
	char header[64];//header consists of command
};

const char HEADER_TO_EXIT[] = "closeServer";
const char HEADER_TO_MSG[] = "messageServer";
const char HEADER_TO_GET_CPU_LOADING[] = "cpuServer";
const char HEADER_TO_GET_CORE_LOADING[] = "coreServer";
#endif