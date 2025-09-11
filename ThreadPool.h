#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <mutex>

class ITaskInterface
{
public:
	virtual void execute() = 0; // Pure virtual function to be overridden by derived classes
	virtual ~ITaskInterface() = default; // Virtual destructor for proper cleanup
	virtual void setThreadPool(void* threadPool) = 0;
};

enum ResultType {
	NOTEXISTS,
	WAITING,
	RUNNING,
	SUCCESS,
	FAILURE,
};

class TaskData
{
public:
	TaskData(ITaskInterface* task) : m_task(task) {}
	ITaskInterface* m_task;
	ResultType m_result = WAITING;
};


class ThreadPool
{
	std::vector<std::thread> workers;
	std::queue<TaskData*> waiting_tasks;
	std::vector <TaskData*> tasks;
	std::mutex queue_mutex;  //controlling work with TaskData items: getting from queue, adding to vector, etc.
	std::mutex event_mutex_add_task; // mutex for event causing the tread to wakeup when a new task is added
	std::condition_variable event_cv_add_task; // Cond.var for event causing the tread to wakeup when a new task is added
	bool stop = false;  // Flag to stop the thread pool
	bool task_added = false;

	void workerFunction(int threadIndex)
	{
		while (true)
		{
			{
				std::unique_lock<std::mutex> lock(event_mutex_add_task);
				event_cv_add_task.wait(lock, [this] { return task_added; }); // 
			}
			task_added = false;
			if (stop)
				return;
			queue_mutex.lock();
			if (!waiting_tasks.empty())
			{
				std::lock_guard<std::mutex> lock(event_mutex_add_task);
				task_added = true;
				event_cv_add_task.notify_one();
			}
			auto& item = waiting_tasks.front();
			waiting_tasks.pop();
			item->m_result = RUNNING;
			item->m_task->execute();
			tasks.push_back(item);
			queue_mutex.unlock();

		}
	}
public:
	ThreadPool(size_t numThreads)
	{
		for (size_t i = 0; i < numThreads; ++i)
		{
			workers.emplace_back(i);
		}
	}
	virtual ~ThreadPool()
	{
		for (std::thread &worker : workers)
		{
			if (worker.joinable())
				worker.join();
		}
	}
	void *addTask(ITaskInterface* task)
	{
		if (stop)
		{
			return nullptr; // Thread pool is stopping, do not accept new tasks
		}
		queue_mutex.lock();
		TaskData* taskData = new TaskData(task);
		task->setThreadPool(this);
		waiting_tasks.push(taskData);
		queue_mutex.unlock();
		{
			std::lock_guard<std::mutex> lock(event_mutex_add_task);
			task_added = true;
			event_cv_add_task.notify_one();
		}
	}
	void removeTask(void *task_id)
	{
		queue_mutex.lock();
		for (auto it = tasks.begin(); it != tasks.end(); ++it)
		{
			if (*it == task_id)
			{
				tasks.erase(it);
				break;
			}
		}
		queue_mutex.unlock();
	}
	ResultType getTaskResult(void *task_id)
	{
		for (auto& x : tasks)
			if (x == task_id)
				return x->m_result;
		return NOTEXISTS;
	}
	ITaskInterface* getTask(void *task_id)
	{
		for (auto& x : tasks)
			if (x == task_id)
				return x->m_task;
		return nullptr;
	}
};



