/* 
 * Author: Thitipong Jampajeen <jampajeen@gmail.com>
 *
 */

#include "task.h"
#include "worker.h"
#include "workpool.h"

Worker::Worker() : tryTerminate_(false) {

}

Worker::~Worker() {
    
}

void Worker::terminateThread() {
    tryTerminate_ = true;
}

void Worker::joinThread() {
    thread_.join();
}

void Worker::createThread(WorkPool *workPool) {
    thread_ = thread(&Worker::threadFunction, this, workPool);
}

void Worker::threadFunction(WorkPool *workPool) {
    Task *task;
    
    INFO("Worker is running thread id(%d)", thread_.get_id() );
    while (true) {
        unique_lock<mutex> locker(workPool->mutex_);

        while (workPool->tasksQueue_.empty()) {
            workPool->cond_.wait(locker);
        }
        
        task = workPool->tasksQueue_.front();
        workPool->tasksQueue_.pop();
        --workPool->tasksQueueSize_;
        
        locker.unlock();
        
        if (tryTerminate_) {
            break;
        }

        if (task == NULL) {
            continue;
        }

        task->run();
        
        workPool->freeTask(task);
    }
    
    INFO("Shutdown worker thread id(%d)", thread_.get_id());
}