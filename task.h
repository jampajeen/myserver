/* 
 * Author: Thitipong Jampajeen <jampajeen@gmail.com>
 *
 */
#ifndef TASK_H
#define	TASK_H

#include "common.h"

class Task {
public:
    Task(task_callback_func callback, void *userData);
    ~Task();

    void run();

private:
    task_callback_func callback_;
    void *userData_;
};

#endif	/* TASK_H */
