/* solution.c -- aoc 2015 07 -- troy brumley */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"
#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"
#define TXBPMUTE_IMPLEMENTATION
#include "txbpmute.h"

#include "solution.h"


int minDistance;
int maxDistance;


/*
 * part one:
 *
 */

int
partOne(
   const char *fname
) {

   FILE *ifile = fopen(fname, "r");
   if (!ifile) {
      printf("could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   /* clear any prior work */
   resetData();

   char iline[INPUT_LEN_MAX];
   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      addDistance(iline);
   }

   printf("part one: %d\n", shortestPath());

   fclose(ifile);
   return EXIT_SUCCESS;
}


/*
 * part two:
 *
 */

int
partTwo(
   const char *fname
) {
   FILE *ifile;

   ifile = fopen(fname, "r");
   if (!ifile) {
      printf("could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   /* clear any prior work */
   resetData();

   char iline[INPUT_LEN_MAX];
   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      addDistance(iline);
   }

   printf("part two: %d\n", longestPath());

   fclose(ifile);
   return EXIT_SUCCESS;
}


/*
 * map the location name to an index. as the dataset is very small,
 * there's no need to sort or optimize.
 *
 * this is both a lookup and a load. if a location is not in the
 * table, add it and increment the number of locations.
 */

int
indexOfLocation(
   const char *s
) {
   int i;
   /* if it's already here, return it */
   for (i = 0; i < numLocations; i++) {
      if (strcmp(locations[i], s) == 0) {
         return i;
      }
   }
   assert(numLocations <= LOCATIONS_MAX);
   locations[numLocations] = strdup(s);
   numLocations += 1;
   return numLocations - 1;
}


/*
 * calculate the distance along the path.
 */

int
distanceForPath(int n, int path[n]) {
   int d = 0;
   for (int i = 0; i < n - 1; i++) {
      d += distances[path[i]][path[i + 1]];
   }
   return d;
}


/*
 * travelling salesman problem api. these are the functions to
 * setup and then iterate over all possible paths to find the
 * longest and shortest of the paths.
 */


/* initilize for a new run */

void
resetData(void) {
   memset(distances, 0, sizeof(distances));
   for (int i = 0; i < numLocations; i++) {
      free(locations[i]);
   }
   memset(locations, 0, sizeof(locations));
   numLocations = 0;
   minDistance = -1;
   maxDistance = -1;
}


/* parse the distance between two locations from an input record.
   format is 'loc1 to loc2 = dst'. the locations are stored in an
   unordered list of strings. the index of a location in the list
   is its row/column identifier in the distances matrix.

   the locations and distances structures should have been reset
   via a call to resetData before loading a new set of data. */

void
addDistance(
   const char *s
) {

   /* first city in 1, second in 3, distance in 5 */
   const char **tokens = split_string(s, " \n");

   /* find in locations table -or- add new entry */
   int lx1 = indexOfLocation(tokens[1]);
   int lx2 = indexOfLocation(tokens[3]);

   /* if no distance logged yet, add it. we could just overwrite but
      this could be useful in debugging or to detect bad data.  */
   if (distances[lx1][lx2] == 0) {
      distances[lx1][lx2] = strtol(tokens[5], NULL, 10);
      distances[lx2][lx1] = distances[lx1][lx2];
   } else {
      assert(distances[lx1][lx2] == strtol(tokens[5], NULL, 10));
      assert(distances[lx1][lx2] == distances[lx2][lx1]);
   }

   /* free the copy of the input record and then the token pointers */
   free((void *)tokens[0]);
   free(tokens);
}


/* called by queries for shortest and longest paths, permute and
   find the path distances. */

void
permutePaths(void) {
   int path[numLocations];
   for (int i = 0; i < numLocations; i++) {
      path[i] = i;
   }
   int currDistance = distanceForPath(numLocations, path);
   minDistance = currDistance;
   maxDistance = currDistance;
   while (permute_next(numLocations, path)) {
      currDistance = distanceForPath(numLocations, path);
      minDistance = min(minDistance, currDistance);
      maxDistance = max(maxDistance, currDistance);
   }
}


/* return the lengths of the longest and shortest distances found
   among the permuted paths. */

int
shortestPath(void) {
   if (minDistance == -1) {
      permutePaths();
   }
   return minDistance;
}


int
longestPath(void) {
   if (maxDistance == -1) {
      permutePaths();
   }
   return maxDistance;
}
