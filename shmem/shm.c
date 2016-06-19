#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <string.h>
#include <stdlib.h>

#define SEGSIZE 2

main(int argc, char *argv[])
{
   key_t key;
   int   shmid, cntr;
   char  *segptr;

   if(argc == 1)
      usage();

   /* Create unique key via call to ftok() */
   key = ftok("/tmp", 'A');
   printf("The key is %d\n", key);

   /* Open the shared memory segment - create if necessary */
   if((shmid = shmget(key, SEGSIZE, IPC_CREAT|IPC_EXCL|0666)) == -1)
   {
      printf("Shared memory segment exists - opening as client\n");

      /* Segment probably already exists - try as a client */
      if((shmid = shmget(key, SEGSIZE, 0)) == -1)
      {
          perror("shmget");
          exit(1);
      }
   }
   else
   {
      printf("Creating new shared memory segment\n");
   }

   /* Attach (map) the shared memory segment into the current process */
   if((segptr = shmat(shmid, 0, 0)) == -1)
   {
      perror("shmat");
      exit(1);
   }
   switch(tolower(argv[1][0]))
   {
   case 'w': writeshm(shmid, segptr, argv[2]/*, argv[3]*/);
      break;
   case 'r': readshm(shmid, segptr);
      break;
   case 'd': removeshm(shmid);
      break;
   case 'm': changemode(shmid, argv[2]);
      break;
   default: usage();
   }

}

writeshm(int shmid, char *segptr, char *text/*, char offset*/)
{
/*   if(offset == '0')
     strcpy(segptr, text);
   else */
//     strncpy((segptr +5), text, 3);
     strcpy(segptr, text);
     strcat(segptr, "______");
     strcat(segptr, text);
   printf("Done...\n");
}

readshm(int shmid, char *segptr)
{
   printf("segptr: %s\n", segptr);
   printf("Length: %d\n", strlen(segptr));
}

removeshm(int shmid)
{
   shmctl(shmid, IPC_RMID, 0);
   printf("Shared memory segment marked for deletion\n");
}

changemode(int shmid, char *mode)
{
   struct shmid_ds myshmds;
   /* Get current values for internal data structure */
   shmctl(shmid, IPC_STAT, &myshmds);
   /* Display old permissions */
   printf("Old permissions were: %o\n", myshmds.shm_perm.mode);
   /* Convert and load the mode */
   sscanf(mode, "%o", &myshmds.shm_perm.mode);
   /* Update the mode */
   shmctl(shmid, IPC_SET, &myshmds);
   printf("New permissions are : %o\n", myshmds.shm_perm.mode);
}


usage()
{
  fprintf(stderr, "shmtool - A utility for tinkering with shared memory\n");
  fprintf(stderr, "\nUSAGE:  shmtool (w)rite <text>\n");
  fprintf(stderr, "                (r)ead\n");
  fprintf(stderr, "                (d)elete\n");
  fprintf(stderr, "                (m)ode change <octal mode>\n");
  exit(1);
}
