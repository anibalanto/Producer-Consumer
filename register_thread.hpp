#pragma once

#include <iostream>
#include <thread>

#ifndef thread_log_mutex
static std::mutex thr_lm;
#define thread_log_mutex thr_lm
#endif // !thread_log_mutex

#define thread_id "[" << std::this_thread::get_id() << "]"

#define thread_log(msg)										\
do{															\
	std::lock_guard<std::mutex> lk(thread_log_mutex);		\
	std::cout << /*thread_id << " " << */msg << std::endl;		\
}while(0)