#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable> 

template<typename T> class producer;
template<typename T> class consumer;

template<typename T>
class transporter
{
	std::mutex mx;
	std::condition_variable do_pull;
	std::queue<T> q;
	bool finished = false;

	friend producer<T>;
	friend consumer<T>;

	inline void push(T product)
	{
		{
			std::lock_guard<std::mutex> lk(mx);
			q.push(product);
		}
		do_pull.notify_one();
	}
	inline bool pull_inside(T& product)
	{
		bool pulled = false;
		std::unique_lock<std::mutex> lk(mx);
		if (!finished)
			do_pull.wait(lk, [this]() {return !q.empty() || finished; });
		if (!q.empty())
		{
			product = q.front();
			q.pop();
			pulled = true;
		}
		lk.unlock();
		do_pull.notify_one();
		return pulled;
	}
	inline void finish()
	{
		{
			std::lock_guard<std::mutex> lk(mx);
			finished = true;
		}
		do_pull.notify_all();
	}
};

template<typename T>
class producer
{
	transporter<T> *transp;
public:
	producer(transporter<T> *t) : transp{ t } {}
	inline ~producer() { transp->finish(); }
	inline void push(T product) { transp->push(product); }
};

template<typename T>
class consumer
{
	transporter<T> *transp;
public:
	consumer(transporter<T> *t) : transp{ t } {}
	inline bool pull_inside(T& product) { return transp->pull_inside(product); }
};
