#include "Thread_pool.h"

Thread_pool::Thread_pool() : m_pool_size(DEFAULT_POOL_SIZE)
{
  cout << "Constructed Thread_pool of size " << m_pool_size << endl;
}

Thread_pool::Thread_pool(int pool_size) : m_pool_size(pool_size)
{
  cout << "Constructed Thread_pool of size " << m_pool_size << endl;
}

Thread_pool::~Thread_pool()
{
  // Release resources
  if (m_pool_state != STOPPED) {
    destroy_threadpool();
  }
}

// We can't pass a member function to pthread_create.
// So created the wrapper function that calls the member function
// we want to run in the thread.
extern "C"
void* start_thread(void* arg)
{
  Thread_pool* tp = (Thread_pool*) arg;
  tp->execute_thread();
  return NULL;
}

int Thread_pool::initialize_threadpool()
{
  // TODO: COnsider lazy loading threads instead of creating all at once
  m_pool_state = STARTED;
  int ret = -1;
  for (int i = 0; i < m_pool_size; i++) {
    pthread_t tid;
    ret = pthread_create(&tid, NULL, start_thread, (void*) this);
    if (ret != 0) {
      cerr << "pthread_create() failed: " << ret << endl;
      return -1;
    }
    m_threads.push_back(tid);
  }

  //should be removed
  /*m_task_mutex.lock();
  cout << m_pool_size << " threads created by the thread pool" << endl;
  m_task_mutex.unlock();
*/
  return 0;
}

int Thread_pool::destroy_threadpool()
{
  // Note: this is not for synchronization, its for thread communication!
  // destroy_threadpool() will only be called from the main thread, yet
  // the modified m_pool_state may not show up to other threads until its
  // modified in a lock!
  m_task_mutex.lock();
  m_pool_state = STOPPED;
  m_task_mutex.unlock();
  cout << "Broadcasting STOP signal to all threads..." << endl;
  m_task_cond_var.broadcast(); // notify all threads we are shttung down

  int ret = -1;
  for (int i = 0; i < m_pool_size; i++) {
    void* result;
    ret = pthread_join(m_threads[i], &result);
    //cout << "pthread_join() returned " << ret << ": " << strerror(errno) << endl;
    m_task_cond_var.broadcast(); // try waking up a bunch of threads that are still waiting
  }
  cout << m_pool_size << " threads exited from the thread pool" << endl;
  return 0;
}

void* Thread_pool::execute_thread()
{
  Task* task = NULL;

  //should be removed
 /* m_task_mutex.lock();
  cout << "Starting thread " << pthread_self() << endl;
  m_task_mutex.unlock();
*/

  while(true) {
    // Try to pick a task
    m_task_mutex.lock();
    //cout << "Took lock: " << pthread_self() << endl;

    // We need to put pthread_cond_wait in a loop for two reasons:
    // 1. There can be spurious wakeups (due to signal/ENITR)
    // 2. When mutex is released for waiting, another thread can be waken up
    //    from a signal/broadcast and that thread can mess up the condition.
    //    So when the current thread wakes up the condition may no longer be
    //    actually true!
    while ((m_pool_state != STOPPED) && (m_tasks.empty())) {
      // Wait until there is a task in the queue
      // Unlock mutex while wait, then lock it back when signaled
      //cout << "Unlocking and waiting: " << pthread_self() << endl;
      m_task_cond_var.wait(m_task_mutex.get_mutex_ptr());
      //cout << "Signaled and locking: " << pthread_self() << endl;
    }

    // If the thread was woken up to notify process shutdown, return from here
    if (m_pool_state == STOPPED) {
      cout << "Unlocking and exiting: " << pthread_self() << endl;
      m_task_mutex.unlock();
      pthread_exit(NULL);
    }

    task = m_tasks.front();
    m_tasks.pop_front();
    //cout << "Unlocking: " << pthread_self() << endl;
    m_task_mutex.unlock();

    //cout << "Executing thread " << pthread_self() << endl;
    // execute the task
    (*task)(); // could also do task->run(arg);
    //cout << "Done executing thread " << pthread_self() << endl;
    delete task;
  }
  return NULL;
}

int Thread_pool::add_task(Task* task)
{
  m_task_mutex.lock();

  // TODO: put a limit on how many tasks can be added at most
  m_tasks.push_back(task);

  m_task_cond_var.signal(); // wake up one thread that is waiting for a task to be available

  m_task_mutex.unlock();

  return 0;
}



/*
#include "Thread_pool.h"

#include <thread>
#include <functional>
#include <boost/asio.hpp>

using namespace boost;

void Thread_pool::create_thread_pool()
{
	asio::io_service io_service;
	asio::io_service::work work(io_service);
	boost::thread_group threads;
	 
	for (std::size_t i = 0; i < 4; ++i)
	   threads.create_thread(boost::bind(&asio::io_service::run, &io_service));
	 
	io_service.post(boost::bind(an_expensive_calculation, 42));
	io_service.post(boost::bind(a_long_running_task, 123));
	 
	io_service.stop();
	threads.join_all();
}

/*
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>


void Thread_pool::create_thread_pool()
{

	boost::asio::io_service::work work(ioService);

	for (int i=0 ; i<4; i++)
	{
		threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &ioService)) ;
	}

}


void Thread_pool::destroy_thread_pool()
{

	ioService.stop();

	threadpool.join_all();
}
*/
/*
boost::asio::io_service ioService;
boost::thread_group threadpool;


boost::asio::io_service::work work(ioService);

threadpool.create_thread(
    boost::bind(&boost::asio::io_service::run, &ioService)
);
threadpool.create_thread(
    boost::bind(&boost::asio::io_service::run, &ioService)
);


ioService.post(boost::bind(myTask, "Hello World!"));
ioService.post(boost::bind(clearCache, "./cache"));
ioService.post(boost::bind(getSocialUpdates, "twitter,gmail,facebook,tumblr,reddit"));


ioService.stop();

threadpool.join_all();
*/

