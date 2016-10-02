/* 
 * Author: Thitipong Jampajeen <jampajeen@gmail.com>
 *
 */
#ifndef WORKPOOL_H
#define	WORKPOOL_H

#include "common.h"
#include "task.h"

using namespace std;

class Worker;
class WorkPool
{
    const int MAX_TASK_IN_QUEUE = 65535;
    const int MAX_WORKER = 64;
    friend class Worker;
    
public:
    
    WorkPool();
    ~WorkPool();

    ret_t addTask(task_callback_func callback, void *userData);
    ret_t start(int numWorkers);
    ret_t shutdown();

private:
    void freeTask(Task *task);
    
    atomic_uint tasksQueueSize_;
    queue<Task*> tasksQueue_;
    vector<Worker*> workers_;
    bool shutdown_;
    mutex mutex_;
    condition_variable cond_;
};

inline void WorkPool::freeTask(Task* task) {
    delete task;
}

#endif	/* WORKPOOL_H */
