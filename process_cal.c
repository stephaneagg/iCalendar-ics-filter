#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LINE_LEN 132
#define MAX_EVENTS 500


// Event structure
typedef struct {
    char time_start[MAX_LINE_LEN];
    char time_end[MAX_LINE_LEN];
    char location[MAX_LINE_LEN];
    char summary[MAX_LINE_LEN];
    char rule[MAX_LINE_LEN];
    char rule_end[MAX_LINE_LEN];
} Event;

/*
 * Function dt_format.
 *
 * Given a date-time, creates a more readable version of the
 * calendar date by using some C-library routines. For example,
 * if the string in "dt_time" corresponds to:
 *
 *   20190520T111500
 *
 * then the string stored at "formatted_time" is:
 *
 *   May 20, 2019 (Mon).
 *
 */

void dt_format(char *formatted_time, const char *dt_time, const int len)
{
    struct tm temp_time;
    time_t    full_time;
    char      temp[5];

    /*  
     * Ignore for now everything other than the year, month and date.
     * For conversion to work, months must be numbered from 0, and the 
     * year from 1900.
     */  
    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(dt_time, "%4d%2d%2d",
        &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    full_time = mktime(&temp_time);
    strftime(formatted_time, len, "%B %d, %Y (%a)", 
        localtime(&full_time));
}


/*
 * Function dt_increment:
 *
 * Given a date-time, it adds the number of days in a way that
 * results in the correct year, month, and day. For example,
 * if the string in "before" corresponds to:
 *
 *   20190520T111500
 *
 * then the datetime string stored in "after", assuming that
 * "num_days" is 100, will be:
 *
 *   20190828T111500
 *
 * which is 100 days after May 20, 2019 (i.e., August 28, 2019).
 *
 */

void dt_increment(char *after, const char *before, int const num_days)
{
    struct tm temp_time, *p_temp_time;
    time_t    full_time;
    char      temp[5];

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(before, "%4d%2d%2dT%2d%2d%2d", &temp_time.tm_year,
        &temp_time.tm_mon, &temp_time.tm_mday, &temp_time.tm_hour, 
        &temp_time.tm_min, &temp_time.tm_sec);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    temp_time.tm_mday += num_days;

    full_time = mktime(&temp_time);
    after[0] = '\0';
    strftime(after, 16, "%Y%m%dT%H%M%S", localtime(&full_time));
    strncpy(after + 16, before + 16, MAX_LINE_LEN - 16); 
    after[MAX_LINE_LEN - 1] = '\0';
}

// function formats 24 hour HHMM time to 12 hour HH:MM
void time_format(char *formatted_time, const char *dt_time, const int len) {
    struct tm temp_time;
    time_t    full_time;
    char      temp[MAX_LINE_LEN];

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(dt_time, "%2d%2d", &temp_time.tm_hour, &temp_time.tm_min);
    full_time = mktime(&temp_time);
    strftime(formatted_time, len, "%I:%M %p", localtime(&full_time));
}

// cuts string to make it start with 'UNTIL='
void cut_string(char *s) {
    char temp[MAX_LINE_LEN];
    for (int i = 0; i < MAX_LINE_LEN; i++) {
        if (s[i] == 'U') {
            strcpy(s, s+i);
            break;
        }
    }
}

// compares two date strings of format YYYMMDDTHHMMSS
// returns 1 if d1 is before d2, 0 otherwise 
int isBefore(char *d1, char *d2) {
    int d1_y, d1_m, d1_d, d2_y, d2_m, d2_d;
    sscanf(d1, "%4d%2d%2d", &d1_y, &d1_m, &d1_d);
    sscanf(d2, "%4d%2d%2d", &d2_y, &d2_m, &d2_d);

    if (d1_y < d2_y){
        return 1;
    }
    if (d1_y == d2_y) {
        if (d1_m < d2_m) {
            return 1;
        }
        if (d1_m == d2_m) {
            if (d1_d < d2_d) {
                return 1;
            }
        }
    }
    return 0;
}
// checks if event e is in between dates from and to
// if e is after from and before to, 1 is returned, 0 otherwise
int isBetween(Event *e, int from_y, int from_m, int from_d, int to_y, int to_m, int to_d) {
    int e_y, e_m, e_d;
    int after = 0;
    int before = 0;
    sscanf(e->time_start, "%4d%2d%2d", &e_y, &e_m, &e_d);

    // check if e is after from
    if (from_y < e_y) {
        after = 1;
    }
    if (from_y == e_y) {
        if (from_m < e_m) {
            after = 1;
        }
        if (from_m == e_m) {
            if (from_d < e_d) {
                after = 1;
            }
            if (from_d == e_d) {
                after = 1;
            }
        }
    }
    // check if e is before to
    if (e_y < to_y) {
        before = 1;
    }
    if (e_y == to_y) {
        if (e_m < to_m) {
            before = 1;
        }
        if (e_m == to_m) {
            if (e_d < to_d) {
                before = 1;
            }
            if (e_d == to_d) {
                before = 1;
            }
        }
    }
    //checks if e was found to come after from and before to
    if (before == 1 && after == 1) {
        return 1;
    } else {
        return 0;
    }
}

// swaps two events in an array;
void swap(Event events[MAX_EVENTS], int i1, int i2) {
    Event temp = events[i2];
    events[i2] = events[i1];
    events[i1] = temp;
}

// checks if e1 is before e2 accounting for date and time
// returns 1 if e1 is before e2, 0 otherwise
int isBeforeE(Event *e1, Event *e2) {
    char e1_t[MAX_LINE_LEN];
    char e2_t[MAX_LINE_LEN];
    strncpy(e1_t, e1->time_start, 15);
    strncpy(e2_t, e2->time_start, 15);
    int e1_y, e1_m, e1_d, e1_h, e1_min, e1_s, e2_y, e2_m, e2_d, e2_h, e2_min, e2_s;
    sscanf(e1_t, "%4d%2d%2dT%2d%2d%2d", &e1_y, &e1_m, &e1_d, &e1_h, &e1_min, &e1_s);
    sscanf(e2_t, "%4d%2d%2dT%2d%2d%2d", &e2_y, &e2_m, &e2_d, &e2_h, &e2_min, &e2_s);

    //check if e1 date is before e2 date
    if (e1_y < e2_y) {
        return 1;
    } if (e1_y == e2_y) {
        if (e1_m < e2_m) {
            return 1;
        } if (e1_m == e2_m) {
            if (e1_d < e2_d) {
                return 1;
            } if (e1_d == e2_d) {
                if (e1_h < e2_h) {
                    return 1;
                } if (e1_h == e2_h) {
                    if (e1_min < e2_min) {
                        return 1;
                    } if (e1_min == e2_min) {
                        if (e1_s < e2_s) {
                            return 1;
                        } if (e1_s == e2_s) {
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

// bubble sort implementation on events
void bubble_sort(Event arr[MAX_EVENTS]) {
    for (int i = 0; i < MAX_EVENTS-1; i++) {
        for (int j = 0; j < MAX_EVENTS-i-1; j++) {
            if (isBeforeE(&arr[j+1], &arr[j]) == 1) { // if arr[j+1] < arr[j] or isBefore(arr[j+1], arr[j])
                swap(arr, j+1, j);
            }
        }
    }
}

void print_events(int from_yy, int from_mm, int from_dd, 
    int to_yy, int to_mm, int to_dd)
{

}


int main(int argc, char *argv[])
{
    int from_y = 0, from_m = 0, from_d = 0;
    int to_y = 0, to_m = 0, to_d = 0;
    char *filename = NULL;
    int i; 

    // populates above variables
    for (i = 0; i < argc; i++) {
        if (strncmp(argv[i], "--start=", 8) == 0) {
            sscanf(argv[i], "--start=%d/%d/%d", &from_y, &from_m, &from_d);
        } else if (strncmp(argv[i], "--end=", 6) == 0) {
            sscanf(argv[i], "--end=%d/%d/%d", &to_y, &to_m, &to_d);
        } else if (strncmp(argv[i], "--file=", 7) == 0) {
            filename = argv[i]+7;
        }
    }

    // invalid command line input check
    if (from_y == 0 || to_y == 0 || filename == NULL) {
        fprintf(stderr, 
            "usage: %s --start=yyyy/mm/dd --end=yyyy/mm/dd --file=icsfile\n",
            argv[0]);
        exit(1);
    }

    // open file
    FILE* input_file = fopen(filename, "r");
    
    // reads file
    Event events[MAX_EVENTS];
    Event event;
    events[0] = event;
    char line[MAX_LINE_LEN];
    int event_counter = 0;
    char temp_rule[MAX_LINE_LEN];
    char temp_temp[MAX_LINE_LEN];
    while( fgets(line, MAX_LINE_LEN, input_file) != NULL) {
        if (strncmp(line, "BEGIN:VEVENT", 12) == 0) {
            continue;
        }
        if (strncmp(line, "DTSTART:", 8) == 0) {
            strcpy(events[event_counter].time_start, line+8);
            continue;
        }
        if (strncmp(line, "DTEND:", 6) == 0) {
            strcpy(events[event_counter].time_end, line+6);
            continue;
        }
        if (strncmp(line, "RRULE:", 6) == 0) {
            strcpy(temp_rule, line+6);
            sscanf(temp_rule, "FREQ=%6c", events[event_counter].rule);
            cut_string(temp_rule);
            sscanf(temp_rule, "UNTIL=%15s", events[event_counter].rule_end);
        }
        if (strncmp(line, "LOCATION:", 9) == 0) {
            strcpy(events[event_counter].location, line+9);
            continue;
        }
        if (strncmp(line, "SUMMARY:", 8) == 0) {
            strcpy(events[event_counter].summary, line+8);
            continue;
        }
        if (strncmp(line, "END:VEVENT", 10) == 0) {
            // if this event has a rule
            // dupes the event with incremented time in events[i+1]
            if (strncmp(events[event_counter].rule, "WEEKLY", 6) == 0) {
                int done = 0;
                while (done != 1) {
                    char holder[15];
                    dt_increment(holder, events[event_counter].time_start, 7);
                    if (isBefore(holder, events[event_counter].rule_end) == 1) {
                        strcpy(events[event_counter+1].time_start, holder); // copy time_start
                        dt_increment(holder, events[event_counter].time_end, 7);
                        strcpy(events[event_counter+1].time_end, holder); // copy time_end
                        strcpy(events[event_counter+1].location, events[event_counter].location); // copy location
                        strcpy(events[event_counter+1].summary, events[event_counter].summary); // copy summary
                        event_counter++;                      
                    }
                    else {
                        done = 1;
                    }
                }
            }
            
            event_counter++;
        } 
    }
    fclose(input_file);
    
   // sorts events
   if (event_counter > 1) {
       bubble_sort(events);
   }

    // prints out events
    char output[MAX_LINE_LEN];
    int first = 1; // tracks whether the first event was printed
    for (int i = 0; i < event_counter; i++) {
        // check if event is within date range
       if (isBetween(&events[i], from_y, from_m, from_d, to_y, to_m, to_d) == 0) {
           continue;
       }
        // only executed on the first event that fall between cl input dates
        // to ensure a foramted date is printed
        if (first == 1) {
            //prints the formated date
            dt_format(output, events[i].time_start, MAX_LINE_LEN);
            printf("%s\n", output);

            //prints the number of - needed
            for (int j = 0; j < strlen(output); j++) {
                printf("-");
            }
            printf("\n");
            first = 0;
        }else {
            char prev_date[8];
            char cur_date[MAX_LINE_LEN];
            sscanf(events[i].time_start, "%8s", cur_date);
            sscanf(events[i-1].time_start, "%8s", prev_date);
            if (strcmp(cur_date, prev_date) != 0) {
                //prints the formated date
                printf("\n");
                dt_format(output, events[i].time_start, MAX_LINE_LEN);
                printf("%s\n", output);

                //prints the number of - needed
                for (int j = 0; j < strlen(output); j++) {
                    printf("-");
                }
                printf("\n");

            }
        }
        
        // prints the formated time
        char temp[6];
        strncpy(temp, events[i].time_start+9, MAX_LINE_LEN);
        time_format(output, temp, MAX_LINE_LEN);
        if(output[0] == '0') {
            strncpy(output, " ", 1);
        }
        printf("%s to ", output);
        strncpy(temp, events[i].time_end+9, MAX_LINE_LEN);
        time_format(output, temp, MAX_LINE_LEN);
        if(output[0] == '0') {
            strncpy(output, " ", 1);
        }
        printf("%s: ", output);

        // prints the summary
        events[i].summary[strlen(events[i].summary)-1] = '\0';
        printf("%s ", events[i].summary);

        //prints the location
        events[i].location[strlen(events[i].location)-1] = '\0';
        printf("{{%s}}", events[i].location);
        printf("\n");
    }
    exit(0);
}
