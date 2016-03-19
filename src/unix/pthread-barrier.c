#include "pthread-barrier.h"

/* TODO: support barrier_attr */
int pthread_barrier_init(pthread_barrier_t* barrier,
                         const void* barrier_attr,
                         unsigned count) {
  int rc = 0;

  if (barrier == NULL)
    return EINVAL;

  barrier->in = 0;
  barrier->out = 0;
  barrier->threshold = count;

  if((rc = pthread_mutex_init(&barrier->mutex, NULL)) != 0)
    return rc;
  if((rc = pthread_cond_init(&barrier->cond, NULL)) != 0) {
    pthread_mutex_destroy(&barrier->mutex);
    return rc;
  }

  return 0;

}

int pthread_barrier_wait(pthread_barrier_t* barrier) {
  int rc;
  int had_err = 0;

  if (barrier == NULL)
    return EINVAL;

  /* Lock the mutex*/
  if ((rc = pthread_mutex_lock(&barrier->mutex)) != 0)
    return rc;

  /* Increment the count. If this is the first thread to reach the threshold,
     wake up waiters, unlock the mutex, then return
     PTHREAD_BARRIER_SERIAL_THREAD. */
  if (++barrier->in == barrier->threshold) {
    /* First thread to reach the barrier */
    barrier->in = 0;
    barrier->out = barrier->threshold - 1;
    if ((rc = pthread_cond_broadcast(&barrier->cond)) != 0)
      had_err = 1;

    pthread_mutex_unlock(&barrier->mutex);
    return had_err ? rc : PTHREAD_BARRIER_SERIAL_THREAD;
  }
  /* Otherwise, wait for other threads until in is set to 0,
     then return 0 to indicate this is not the first thread. */
  rc = 0;
  do {
    if ((rc = pthread_cond_wait(&barrier->cond, &barrier->mutex)) != 0)
      break;
  } while (barrier->in != 0);

  /* mark thread exit */
  barrier->out--;
  pthread_mutex_unlock(&barrier->mutex);
  return rc;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier) {
  int rc;

  if (barrier == NULL)
    return EINVAL;

  if ((rc = pthread_mutex_lock(&barrier->mutex)) != 0)
    return rc;

  if (barrier->in > 0 || barrier->out > 0)
    return EBUSY;

  pthread_mutex_unlock(&barrier->mutex);
  pthread_cond_destroy(&barrier->cond);
  pthread_mutex_destroy(&barrier->mutex);
  return 0;
}
