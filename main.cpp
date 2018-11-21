#include <thread>
#include <random>
#include "producer_consumer.hpp"
#include "register_thread.hpp"
#include "frame.hpp"	


static const int nproducers = 1;
static const int nconsumers = 10;

static transporter<Frame> transports[nproducers];


std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<int> dist_time_produce(25, 28);
std::uniform_int_distribution<int> dist_time_consume(5000, 6000);
std::uniform_int_distribution<int> dist_number_products(100, 200);
std::uniform_int_distribution<int> dist_number_objects(0, 4);

static std::mutex mx_elem;
static unsigned int id = 0;

static Frame get_frame()
{
	std::lock_guard<std::mutex> lk(mx_elem);
	return Frame(++id, dist_number_objects(mt));
}

void producer1(std::unique_ptr<producer<Frame>> access)
{
	thread_log("producer: Beginning");
	auto nproducts = dist_number_products(mt);
	for (int i = 0; i < nproducts; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(dist_time_produce(mt)));
		auto frame = get_frame();
		if(frame.n_objects > 0)
		{
			access->push(frame);
		    thread_log("push" /*<< transp->get_id()*/ << " frame: " << frame.id << " - nobjs: " << frame.n_objects);
                }
	}
	thread_log("producer: Finalice");
}

void consumer1(std::unique_ptr<consumer<Frame>> access)
{
	thread_log("consumer : Beginning ");
	Frame frame;
	while (access->pull_inside(frame))
	{
		thread_log("consumed" /*<< transp->get_id()*/ << ": " << frame.id << " - nobjs: " << frame.n_objects);
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
		threads.push_back(std::make_unique<std::thread>(producer1, transports[i].producer_access()));
	}
	//create consumers
	for (int i = 0; i < nproducers; i++)
	{
		for (int j = 0; j < nconsumers; j++)
		{
			threads.push_back(std::make_unique<std::thread>(consumer1, transports[i].consumer_access()));
		}
	}
	//joins threads
	for (auto &thread : threads)
	{
		thread->join();
	}	
	std::cout << "finished!" << std::endl;
}
