
#include "workpool.h"
#include "task.h"
#include "worker.h"

WorkPool::WorkPool() : shutdown_(false), tasksQueueSize_(0) {

}

ret_t WorkPool::start(int numWorkers = 1) {
    INFO("Starting %d workers...", numWorkers);

    for (int i = 0; i < numWorkers; i++) {
        Worker *worker = new Worker;
        worker->createThread(this);

        workers_.push_back(worker);
    }
    shutdown_ = false;

    return ret_t::SUCCESS;
}

ret_t WorkPool::addTask(task_callback_func callback, void *userData) {

    if (tasksQueueSize_ >= MAX_TASK_IN_QUEUE) {
        ERROR("Work Queue is full");
        return ret_t::ERROR_QUEUE_FULL;
    }

    Task* task = new Task(callback, userData);

    unique_lock<mutex> locker(mutex_);
    tasksQueue_.push(task);
    ++tasksQueueSize_;
    locker.unlock();

    cond_.notify_one();
    return ret_t::SUCCESS;
}

ret_t WorkPool::shutdown() {
    INFO("Shutting down all workers...");

    for (Worker *worker : workers_) {
        worker->terminateThread();
    }

    cond_.notify_all();

    for (Worker *worker : workers_) {
        worker->joinThread();
    }

    workers_.empty();
    shutdown_ = true;

    INFO("All workers is already shutdown");
    return ret_t::SUCCESS;
}

WorkPool::~WorkPool() {
    if (!shutdown_) {
        shutdown();
    }
}
