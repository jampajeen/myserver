/* 
 * Author: Thitipong Jampajeen <jampajeen@gmail.com>
 *
 */

#include "task.h"

Task::Task(task_callback_func callback, void* userData) {
    callback_ = callback;
    userData_ = userData;
}

Task::~Task() {
    // don't forget destruct userData_ object on callback side
}

void Task::run() {
    callback_(userData_);
}
