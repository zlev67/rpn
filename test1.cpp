// test1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <list>
#include <vector>
#include <mutex>
#include <memory>
//import std.core;

class ThreadRunner
{
    public:
		virtual void run() = 0; // Pure virtual function to be overridden by derived classes
};

class ThreadRunnerImpl: public ThreadRunner
{
    public:
        virtual void run() override
        {
            std::cout << "ThreadRunnerImpl running\n";
            // Implementation of the run method
        }
};

typedef std::unique_ptr<ThreadRunner> ThreadRunnerPtr;

class ThreadPull: public ThreadRunner
{
    private:
		std::list <ThreadRunnerPtr> m_threadRunnerList;
		std::list <std::thread> m_threadList;
		bool m_endFlag = false;
        std::mutex m_wait_mutex;
        std::condition_variable m_cv;
        bool m_data_ready = false;

        void m_threadRunnerCallback()
        {
            std::cout << "ThreadRunner callback called\n";
            // Here you would typically call the run method of each ThreadRunnerPtr in m_threadRunnerList
            // wait for wait_mutex to be released
			std::unique_lock<std::mutex> lock(m_wait_mutex);
            m_cv.wait(lock, [this] { return m_data_ready; }); // 

            while (!m_endFlag)
            {
                for (auto& runner : m_threadRunnerList)
                {
                    if (runner)
                    {
                        runner->run();
                    }
                }
            }
		}
    public:
        ThreadPull()
        {
            std::cout << "ThreadPull constructor called\n";
		}
        ~ThreadPull()
        {

        }
        void Add(ThreadRunnerPtr &runner) 
        {
			m_threadRunnerList.push_back(std::move(runner));
            std::lock_guard<std::mutex> lock(m_wait_mutex);
            m_data_ready = true;
            m_cv.notify_one(); // wake threadA
        }
        void Stop()
        {
			m_endFlag = true;
            std::lock_guard<std::mutex> lock(m_wait_mutex);
            m_data_ready = true;
            m_cv.notify_one(); // wake threadA
            for (auto& thread : m_threadList)
            {
                if (thread.joinable())
                {
                    thread.join();
                }
			}
		}
};


int main()
{
    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
