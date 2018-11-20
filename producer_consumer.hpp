#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T, typename C> class producer;
template<typename T, typename C> class consumer;

template<typename T, typename C>
class transporter
{
	std::mutex mx;
	std::condition_variable do_pull;
	std::priority_queue<T, std::vector<T>, C> q;
	bool finished = false;

	friend producer<T, C>;
	friend consumer<T, C>;

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
			do_pull.wait(lk, [this]() -> bool {return !q.empty() || finished; });
		if (!q.empty())
		{
			product = q.top();
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

public:
	std::unique_ptr<producer<T, C>> producer_access();
	std::unique_ptr<consumer<T, C>> consumer_access();
};

template<typename T, typename C>
class producer
{
	transporter<T, C> *transp;
public:
	producer(transporter<T, C> *t) : transp{ t } {}
	inline ~producer() { transp->finish(); }
	inline void push(T product) { transp->push(product); }
};

template<typename T, typename C>
class consumer
{
	transporter<T, C> *transp;
public:
	consumer(transporter<T, C> *t) : transp{ t } {}
	inline bool pull_inside(T& product) { return transp->pull_inside(product); }
};

template<typename T, typename C>
std::unique_ptr<producer<T, C>> transporter<T, C>::producer_access()	
{
	return std::make_unique<producer<T, C>>(this);
}

template<typename T, typename C>
std::unique_ptr<consumer<T, C>> transporter<T, C>::consumer_access()
{
	return std::make_unique<consumer<T, C>>(this);
}
