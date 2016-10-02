/* 
 * Author: Thitipong Jampajeen <jampajeen@gmail.com>
 *
 */

#ifndef WORKER_H
#define	WORKER_H

#include "common.h"

using namespace std;

class WorkPool;
class Task;
class Worker {
public:
    
    Worker();
    ~Worker();
    
    void createThread(WorkPool *workPool);
    void terminateThread();
    void joinThread();
    
private:
    thread thread_;
    atomic_bool tryTerminate_;
    
    void threadFunction(WorkPool *workPool);
};

#endif	/* WORKER_H */