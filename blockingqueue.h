#ifndef _BLOCKING_QUEUE_H_
#define _BLOCKING_QUEUE_H_

#include <queue>
#include <mutex>
#include <condition_variable>

namespace gs {

using namespace std;

template <typename T>
class BlockingQueue {
public:
	T pop(void)
	{
		unique_lock<mutex> l(lock);
		T item;

		while (q.empty()) {
			cv.wait(l);
		}

		item = q.front();
		q.pop();
		return item;
	}

	void push(const T& item)
	{
		unique_lock<mutex> l(lock);

		q.push(item);

		l.unlock();
		cv.notify_one();
	}

private:
	queue<T> q;
	mutex lock;
	condition_variable cv;
};

}

#endif
