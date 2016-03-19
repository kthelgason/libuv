#ifndef _UV_PTHREAD_BARRIER_
#define _UV_PTHREAD_BARRIER_
#include <errno.h>
#include <pthread.h>

/* Anything except 0 will do here.*/
#define PTHREAD_BARRIER_SERIAL_THREAD  0x12345

typedef struct {
  pthread_mutex_t  mutex;
  pthread_cond_t   cond;
  unsigned         threshold;
  unsigned         in;
  unsigned         out;
} pthread_barrier_t;

int pthread_barrier_init(pthread_barrier_t* barrier,
                         const void* barrier_attr,
                         unsigned count);

int pthread_barrier_wait(pthread_barrier_t* barrier);
int pthread_barrier_destroy(pthread_barrier_t *barrier);

#endif /* _UV_PTHREAD_BARRIER_ */
