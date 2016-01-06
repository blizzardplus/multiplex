#include <pthread.h>
#include <unistd.h>
#include <deque>
#include <iostream>
#include <vector>
#include <errno.h>
#include <string.h>

#include "Mutex.h"
#include "Task.h"
#include "CondVar.h"
#include "Global.h"

using namespace std;


class Thread_pool
{
public:
  Thread_pool();
  Thread_pool(int pool_size);
  ~Thread_pool();
  int initialize_threadpool();
  int destroy_threadpool();
  void* execute_thread();
  int add_task(Task* task);
private:
  int m_pool_size;
  Mutex m_task_mutex;
  CondVar m_task_cond_var;
  std::vector<pthread_t> m_threads; // storage for threads
  std::deque<Task*> m_tasks;
  volatile int m_pool_state;
};




/*
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

class Thread_pool {

public:
	void create_thread_pool();
	void destroy_thread_pool();

private:
	boost::asio::io_service ioService;
	boost::thread_group threadpool;


};
*/
