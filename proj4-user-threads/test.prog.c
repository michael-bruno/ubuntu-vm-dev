#include <stdio.h>
#include "ut_threads.h"

void busywait(unsigned int n)
{
  for (; n > 0; n--)
        if (n % 100 == 0)
            ut_yield(); 
}

void func_a(int id){
  int i = 0;
  for( ; i <50; i++){
    printf(" -=A%d=- ", ut_getid());
    busywait(0x1FFFFF);
  }
  ut_finish(id);
}

void func_b(int id){
  int i = 0;
  for( ; i <50; i++){
    printf(" -=B%d=- ", ut_getid());
    busywait(0x1FFFFF);
  }
  ut_finish(id);
}

void func_c(int id){
  int i = 0;
  for( ; i <10; i++){
    printf(" -=C%d=- ", ut_getid());
    ut_yield();
  }
  ut_finish(id);
    printf(" Should not get here! \n");
}

int main()
{
    char threadstackbuf[MAX_THREADS * STACK_SIZE];
    
  printf("User-level Threads Scheduler\n");
  setvbuf(stdout, 0, _IONBF, BUFSIZ);

  ut_init(threadstackbuf);
  
  printf("Hello! I'm the main thread, my ID is %d.\n", ut_getid());
  printf("Now I'll create another 3 threads and wait until their jobs finish!\n");
  
  int thrA = ut_create(&func_a, 1979, 0);
  int thrB = ut_create(&func_b, 2008, 0);
  int thrC = ut_create(&func_c, 29, 0);
  
  int retA, retB, retC;
  
  ut_join(thrB, &retB);
  ut_join(thrA, &retA);
  ut_join(thrC, &retC);
  
  printf("\n\n");
  printf("Thread A returned %d\n", retA);
  printf("Thread B returned %d\n", retB);
  printf("Thread C returned %d\n", retC);
  
  printf("I`m also done ! Bye ...\n");

  return 0;
}
