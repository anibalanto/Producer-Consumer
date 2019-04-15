#pragma once

#include <thread>
#include <set>
#include <mutex>
#include <condition_variable>

template<typename T> class producer;
template<typename T> class consumer;


/*template<typename Container>
struct transporter_priority
{
		using type = less<typename Container::value_type> ;
}*/

template<typename T>
class transporter
{
	std::mutex mx;
	std::condition_variable do_pull;
	std::multiset<T> s;
	bool finished = false;
	unsigned int limit;

	friend producer<T>;
	friend consumer<T>;

public:
	transporter(unsigned int lim) : limit(lim) {}

protected:
	inline void push(T product)
	{
		{
			std::lock_guard<std::mutex> lk(mx);
			s.insert(product);
			std::cout << "transporter::limit(" << limit << ")\n";
			std::cout << "transporter::added: " << product;
			if (limit > 0)
			{
				limit--;
			}
			else
			{
				auto last_product = std::prev(s.end());
				std::cout << "transporter::removed: " << *last_product;
				s.erase(last_product);
			}
		}
		do_pull.notify_one();
	}
	inline bool extract_in(T& product)
	{
		bool pulled = false;
		std::unique_lock<std::mutex> lk(mx);
		if (!finished)
			do_pull.wait(lk, [this]() -> bool {return !s.empty() || finished; });
		if (!s.empty())
		{
			product = *s.begin();
			s.erase(s.begin());
			pulled = true;
			limit++;
			std::cout << "transporter::limit(" << limit << ")\n";
			std::cout << "transporter::extracted: " << product;
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
	std::unique_ptr<producer<T>> producer_access();
	std::unique_ptr<consumer<T>> consumer_access();
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
	inline bool extract_in(T& product) { return transp->extract_in(product); }
};

template<typename T>
std::unique_ptr<producer<T>> transporter<T>::producer_access()
{
	return std::make_unique<producer<T>>(this);
}

template<typename T>
std::unique_ptr<consumer<T>> transporter<T>::consumer_access()
{
	return std::make_unique<consumer<T>>(this);
}
