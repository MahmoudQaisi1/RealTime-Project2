
#include "local.h"




main(int argc, char *argv[])
{
  pid_t          sender_id, receiver_id,master_spy_id, pid = getpid();
  int            i,numOfHelpers,numOfSpies;


  if ( argc != 3 ) {
    fprintf(stderr, "%s Number_of_helpers number_of_spies \n", argv[0]);
    exit(-1);
  }

  numOfHelpers = atoi(argv[1]);
  numOfSpies = atoi(argv[2]);

  pid_t           helpers_id[numOfHelpers];
  pid_t           spies_id[numOfSpies];

  // forking the needed processes//////////////////////////////////////////////////////////////
  //sender////////
  if ( (sender_id = fork()) == -1 ) {
    perror("fork -- sender");
    exit(5); 
  }
  else if ( sender_id == 0 ) {
    execl("./sender", "./sender", argv[1], (char *) 0);
    perror("execl -- sender");
    exit(6);
  }
  sleep(0.5);

  //receiver//////
  if ( (receiver_id = fork()) == -1 ) {
    perror("fork -- receiver");
    exit(5); 
  }
  else if ( receiver_id == 0 ) {
    execl("./receiver", "./receiver", argv[1], (char *) 0);
    perror("execl -- receiver");
    exit(6);
  }
  sleep(0.5);

  //master Spy/////
  if ( (master_spy_id = fork()) == -1 ) {
    perror("fork -- master spy");
    exit(5); 
  }
  else if ( master_spy_id == 0 ) {
    execl("./master_spy", "./master_spy", argv[1], (char *) 0);
    perror("execl -- master spy");
    exit(6);
  }
  sleep(0.5);

  //helpers////////////////////
  for ( i = 0; i < numOfHelpers; i++)
  {
    if ( (helpers_id[i] = fork()) == -1 ) {
      perror("fork -- helper");
      exit(5); 
    }
    else if ( helpers_id[i] == 0 ) {
      execl("./helper", "./helper", argv[1], (char *) 0);
      perror("execl -- helper");
      exit(6);
    }
    sleep(0.5);
  }

  //spies///////////////////
  for ( i = 0; i < numOfSpies; i++)
  {
    if ( (spies_id[i] = fork()) == -1 ) {
      perror("fork -- spy");
      exit(5); 
    }
    else if ( helpers_id[i] == 0 ) {
      execl("./spy", "./spy", argv[1], (char *) 0);
      perror("execl -- spy");
      exit(6);
    }
    sleep(0.5);
  }

  //Clean House////////////////////////////////////////////////////////////////////////////////
  

  exit(0);
}