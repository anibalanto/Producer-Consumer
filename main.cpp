#include <thread>
#include <random>
#include "transporter.h"
#include "register_thread.hpp"

static const int nproducers = 1;
static const int nconsumers = 10;

static transporter<int> transports[nproducers];


std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<int> dist_time_produce(25, 28);
std::uniform_int_distribution<int> dist_time_consume(5000, 6000);
std::uniform_int_distribution<int> distproducts(500, 600);

static std::mutex mx_elem;
static int elem = 0;

static int get_elem()
{
	std::lock_guard<std::mutex> lk(mx_elem);
	return ++elem;
}

void producer1(transporter<int> *transp)
{
	producer<int> prod(transp);
	thread_log("producer: Beginning");
	auto nproducts = distproducts(mt);
	for (int i = 0; i < nproducts; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(dist_time_produce(mt)));
		auto elem = get_elem();
		prod.push(elem);
		thread_log("push in " /*<< transp->get_id()*/ << " element: " << elem);
	}
	thread_log("producer: Finalice");
}

void consumer1(transporter<int> *transp)
{
	consumer<int> cons(transp);
	thread_log("consumer : Beginning ");
	int i;
	while (cons.pull_inside(i))
	{
		thread_log("consumed from " /*<< transp->get_id()*/ << ": " << i);
		std::this_thread::sleep_for(std::chrono::milliseconds(dist_time_consume(mt)));
	}
	thread_log("consumer: Finelize ");
}


int main(int argc, char *argv[])
{
	std::vector<std::unique_ptr<std::thread>> threads;
	//create producers
	for (int i = 0; i < nproducers; i++)
	{
		auto nproducts = distproducts(mt);
		threads.push_back(std::make_unique<std::thread>(producer1, &transports[i]));
	}
	//create conusmers
	for (int i = 0; i < nproducers; i++)
	{
		for (int j = 0; j < nconsumers; j++)
		{
			threads.push_back(std::make_unique<std::thread>(consumer1, &transports[i]));
		}
	}
	//joins threads
	for (auto &thread : threads)
	{
		thread->join();
	}	
	std::cout << "finished!" << std::endl;
}