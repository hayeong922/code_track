#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#define MYSTACKSIZE 40960

//using namespace std;

pthread_mutex_t m;

void *odd(void *max)
{
  int i;
  FILE *fd;
  struct timeval tp;

  fd = fopen("odd_num", "w");
  for (i = 1; i < *(int*)max; i+= 2)
    fprintf(fd, "%d\n", i);
  pthread_mutex_lock(&m);
  for (i = 0; i < 10000; i++)
    printf("odd\n");
  pthread_mutex_unlock(&m);
}

void *even(void *max)
{
  int i;
  FILE *fd;
  struct timeval tp;
 
  fd = fopen("even_num", "w");
  for (i = 2; i < *(int*)max; i+= 2)
    fprintf(fd, "%d\n", i);
  pthread_mutex_lock(&m);
  for (i = 0; i < 10000; i++)
    printf("even\n");
  pthread_mutex_unlock(&m);
}

main()
{
  int max = 50, max1 = 100, max2 = 200, i;
  FILE *fd;
  pthread_attr_t attr;
  pthread_t *th1, *th2;
  void *st1, *st2;
  size_t sz;
  int policy;
  struct timeval tp;

  pthread_mutex_init(&m, NULL);

  pthread_attr_init(&attr);
  st1 = (void *) malloc(MYSTACKSIZE);
  pthread_attr_setstacksize(&attr, MYSTACKSIZE);
  pthread_attr_setstack(&attr, st1, MYSTACKSIZE);

  pthread_attr_getstacksize(&attr, &sz);

  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
  pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

  th1 = (pthread_t *) malloc(sizeof(pthread_t));
//  if (pthread_create(th1, &attr, odd, (void *)&max1)) {
  if (pthread_create(th1, &attr, odd, &max1)) {
    perror("error creating the first thread");
    exit(1);
  }
  printf("created the first thread\n");
 
  st2 = (void *)malloc(MYSTACKSIZE);
  pthread_attr_setstacksize(&attr, MYSTACKSIZE);
  pthread_attr_setstack(&attr, st2, MYSTACKSIZE);
  th2 = (pthread_t *) malloc(sizeof(pthread_t));
//  if (pthread_create(th2, &attr, even, (void *)&max2)) {
  if (pthread_create(th2, &attr, even, &max2)) {
    perror("error creating the second thread");
    exit(1);
  }
  printf("created the second thread\n");

  fd = fopen("whole_num", "w");
  pthread_mutex_lock(&m);
  for (i = 0; i < 10000; i++)
    printf("main\n");
  pthread_mutex_unlock(&m);
  for (i = 1; i < max; i++)
    fprintf(fd, "%d\n", i);
  pthread_join(*th1, 0);
  pthread_join(*th2, 0);
}
