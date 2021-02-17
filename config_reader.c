#include <stdio.h>
#include <string.h>

#define LENGTH 20
#define PARAMETERS 15

const char *config_types[PARAMETERS] = {"SEED", "INIT_TIME", "FIN_TIME", "ARRIVE_MIN", "ARRIVE_MAX", "QUIT_PROB", "NET_PROB", "CPU_MIN", "CPU_MAX", "DISK1_MIN", "DISK1_MAX", "DISK2_MIN", "DISK2_MAX", "NETWORK_MIN", "NETWORK_MAX"};

int config_values[PARAMETERS] = {};

void readFile(char* FILE_NAME) {

    // opens the config file
    FILE * fp;
    fp = fopen(FILE_NAME, "r");

    // buffers for config file line reading
    char search[LENGTH];
    char line[LENGTH];

    // read each line in config file
    int i;
    while (fgets(line,LENGTH,fp) != NULL) {
        for (i = 0; i < PARAMETERS; i++) {
            strcpy(search, config_types[i]);
            strcat(search, " %d\n");
            sscanf(line, search, &config_values[i]);
        }
    }

    fclose(fp);

}