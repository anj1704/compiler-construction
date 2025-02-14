#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void removeComments(char *testcaseFile, char *cleanFile) {
  FILE *testcaseFp = fopen(testcaseFile, "r");
  FILE *cleanFp = fopen(cleanFile, "w");

  if (!testcaseFp) {
    printf("Opening testcase file failed\n");
    return;
  }

  if (!cleanFp) {
    printf("Opening clean file failed\n");
    return;
  }

  char *line = malloc(200 * sizeof(char));
  bool isComment = false;

  while (fgets(line, 200, testcaseFp)) {
    if (!line) {
      break;
    }

    isComment = false;
    for (int i = 0; line[i] != '\n'; ++i) {
      if (line[i] == ' ') {
        continue;
      } else if (line[i] == '%') {
        isComment = true;
        break;
      } else {
        isComment = false;
        break;
      }
    }

    if (!isComment) {
      fputs(line, cleanFp);
    }
  }

  fclose(testcaseFp);
  fclose(cleanFp);
}
