/*
DScheduler.c

Student Name : Michael Pintur
Student ID # : 150785070

*/

#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "dscheduler.h"


/*
Any required standard libraries and your header files here
*/

struct schedulerInput loadRequest(){
    struct schedulerInput results;
    int numRequests;
    char line_buffer[MAX_LINE_LENGTH];
    char direction;
    char *token;

    //Process simulation input line by line
    fgets(line_buffer, MAX_LINE_LENGTH, stdin);

      token = strtok(line_buffer, " ");
    sscanf(token, "%d",&numRequests);

    token = strtok(NULL, " ");
    sscanf(token, "%d",&results.startTrack);

    token = strtok(NULL, " ");
    sscanf(token, "%c",&direction);
    results.direction = direction == 'u' ? 1 : -1;

    results.requests.elements = numRequests;
    results.requests.data = (int *)malloc(sizeof(int)*numRequests);
    if ( results.requests.data == NULL ){
        fprintf( stderr, "Was unable to allocate space for requests data.\n");
        exit( BAD_MALLOC );
    }

    for (int i = 0; i < numRequests; i++){
        token = strtok(NULL, " ");
        sscanf(token, "%d", &results.requests.data[i]);
    }

    return results;
}

void printResults(struct schedulerResult results){
    for (int i = 0; i < results.requests.elements; i++){
        printf("%4d ", results.requests.data[i]);
    }
    printf("Total Head Movement %5d\n", results.totalHeadMovement);
}

struct schedulerResult processRequest(enum POLICIES policy, struct schedulerInput request){
    struct schedulerResult results;

    switch(policy){
        case FCFS:
            return process_FCFS_request(request);
        case SSTF:
            return process_SSTF_request(request);
        case SCAN:
            return process_SCAN_request(request);
        case C_SCAN:
            return process_C_SCAN_request(request);
    }
    return results;
}



/* Fill in the following functions */
struct schedulerResult process_FCFS_request(struct schedulerInput request){
    struct schedulerResult results;

    printf("FCFS ->");
    int start = request.startTrack;
    int num = request.requests.elements;
    int location = start;
    int movement = 0;
    results.requests.elements = num;
    results.requests.data = (int *)malloc(sizeof(int)*num);
    int i;
    for (i = 0; i < num; i++) {
      movement += abs(location - request.requests.data[i]);
      location = request.requests.data[i];
      results.requests.data[i] = request.requests.data[i];
    }
    results.totalHeadMovement = movement;

    return results;
}

struct schedulerResult process_SSTF_request(struct schedulerInput request){
    struct schedulerResult results;
    printf("SSTF ->");

    int start = request.startTrack;
    int num = request.requests.elements;
    int location = start;
    int movement = 0;
    int sstf = 0;


    results.requests.elements = num;
    results.requests.data = (int *)malloc(sizeof(int)*num);
    int i, j, k;
    for (i = 0; i < num; i++) {

      for (j = 0; j < request.requests.elements; j++) {
        if (contains(results, request.requests.data[j]) == 1) {
          continue;
        }
        if (abs(location - request.requests.data[j]) < abs(location - request.requests.data[sstf])) {
          sstf = j;
        }
      }
      movement += abs(location - request.requests.data[sstf]);
      location = request.requests.data[sstf];
      results.requests.data[i] = request.requests.data[sstf];

      for (k = 0; k < request.requests.elements; k++) {
        if (contains(results, request.requests.data[k]) == 0) {
          sstf = k;
          break;
        }
      }

    }

    results.totalHeadMovement = movement;
    return results;
}

struct schedulerResult process_SCAN_request(struct schedulerInput request){
    struct schedulerResult results;
    printf("SCAN ->");
    int start = request.startTrack;
    int num = request.requests.elements;
    int location = start;
    int movement = 0;
    int requests[num];
    int dirChanges = 0;
    int processed = 0;
    results.requests.elements = num;
    results.requests.data = (int *)malloc(sizeof(int)*num);

    memcpy(requests, request.requests.data, sizeof requests);
    int i, j;

//arrange arraycopy to ascending order
    for (int i = 0; i < num; i++)
      {
        for (int j = 0; j < num; j++)
        {
          if (requests[j] > requests[i])
          {
            int tmp = requests[i];
            requests[i] = requests[j];
            requests[j] = tmp;
          }
        }
      }


    int count = 0;
    for (i = 0; i < 2; i++) {
      if (request.direction == 1) {
        for (j = 0; j < num; j ++) {
          if (requests[j] > location) {
            if (contains(results, requests[j]) == 1) {
              continue;
            }
            movement += abs(location - requests[j]);
            location = requests[j];
            results.requests.data[count] = requests[j];
            processed += 1;
            count += 1;
          }
        }

        request.direction = -1;
        dirChanges += 1;
        if (dirChanges >= 2) {
          break;
        } else if (processed >= num) {
          break;
        }
        movement += abs(location - 1023);
        location = 1023;

      } else {
        for (j = num - 1; j >= 0; j--) {
          if (requests[j] < location) {
            if (contains(results, requests[j]) == 1) {
              continue;
            }
            movement += abs(location - requests[j]);
            location = requests[j];
            results.requests.data[count] = requests[j];
            processed += 1;
            count += 1;
          }
        }
        request.direction = 1;
        dirChanges += 1;
        if (dirChanges >= 2) {
          break;
        } else if (processed >= num ) {
          break;
        }
        movement += abs(location - 0);
        location = 0;

      }
    }
    results.totalHeadMovement = movement;

    return results;
}

struct schedulerResult process_C_SCAN_request(struct schedulerInput request){
    struct schedulerResult results;
    printf("SCAN ->");
    int start = request.startTrack;
    int num = request.requests.elements;
    int location = start;
    int movement = 0;
    int requests[num];
    int count = 0;
    results.requests.elements = num;
    results.requests.data = (int *)malloc(sizeof(int)*num);

    memcpy(requests, request.requests.data, sizeof requests);
    int i, j;

    for (int i = 0; i < num; i++)
      {
        for (int j = 0; j < num; j++)
        {
          if (requests[j] > requests[i])
          {
            int tmp = requests[i];
            requests[i] = requests[j];
            requests[j] = tmp;
          }
        }
      }


      for (i = 0; i < num; i++) {
        if (requests[i] > location) {
          movement += abs(location - requests[i]);
          location = requests[i];
          results.requests.data[count] = requests[i];
          count += 1;
        }
      }
      movement += abs(location - 1023);
      location = 1023;
      movement += abs(location - 0);
      location = 0;
      for (j = 0; j < num; j++) {
        if (requests[j] > location) {
          if (contains(results, requests[j]) == 1) {
            continue;
          }
          movement += abs(location - requests[j]);
          location = requests[j];
          results.requests.data[count] = requests[j];
          count += 1;
        }
      }
      results.totalHeadMovement = movement;

    return results;
}

int contains(struct schedulerResult results, int tN) {
  int i = 0;
  for (i=0; i < results.requests.elements; i++) {
    if (tN == results.requests.data[i]) {
      return 1;
    }
  }
  return 0;
}
