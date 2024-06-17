#include "thread.hpp"

void* read_parallel(void *args) {
    ThreadArgs* t_argv = static_cast<ThreadArgs*>(args);
    t_argv->img.read(t_argv->tid);
    delete t_argv;
    pthread_exit(NULL);
}

void* write_parallel(void *args) {
    
    ThreadArgs* t_argv = static_cast<ThreadArgs*>(args);

    t_argv->img.write(t_argv->tid);
    delete t_argv;
    
    pthread_exit(NULL);
}

void create_threads(void *(*func)(void *),BMPImage& img, BMPImage &copy_img)
{
    
    pthread_t threads_l[NUMBER_OF_THREADS];
    int rc;
    long t;
    for (t = 0; t < NUMBER_OF_THREADS; t++)
    {
        ThreadArgs* args = new ThreadArgs(t , img, copy_img);
        rc = pthread_create(&threads_l[t], NULL, func, args);
        if (rc)
        {
            std::cout << "Error:unable to create thread" << rc << std::endl;
            exit(-1);
        }
    }
    for (t = 0; t < NUMBER_OF_THREADS; t++)
    {
        rc = pthread_join(threads_l[t], NULL);
        if (rc)
        {
            std::cout << "Error:unable to join threads" << rc << std::endl;
            exit(-1);
        }
    }
}