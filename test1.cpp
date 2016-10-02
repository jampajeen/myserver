
#include <stdlib.h>
#include <iostream>
#include "common.h"

#include "connection.h"
#include "worker.h"
#include "task.h"
#include "workpool.h"

struct val {
    int i;
};

static int c = 1, limit = 25000;

void process_task(void *arg) {
    //std::this_thread::sleep_for (std::chrono::seconds(3));
    val *v = (val*) arg;
    if (c++ % 10000 == 0) {
        printf(">> %d\n", c);
    }
}

void test1() {
    WorkPool wp;
    val v;

    wp.start(6);

    task_callback_func callback = &process_task;
loop:
    for (int i = 0; i < limit; i++) {
        wp.addTask(callback, &v);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    goto loop;

    while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(1000));
    }
}

//
//int main(int argc, char** argv) {
//    test1();
//    return (EXIT_SUCCESS);
//}
