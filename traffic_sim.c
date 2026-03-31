#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define RED 0
#define YELLOW 1
#define GREEN 2

#define DEFAULT_GREEN 5
#define YELLOW_TIME 2

struct Intersection {
    char name[20];
    int state;
    int vehicles;
    int green_secs;
};

// globals so the sensor thread can see them
struct Intersection *inter_a = NULL;
struct Intersection *inter_b = NULL;
int keep_going = 1;

void log_entry(const char *msg) {
    FILE *fp = fopen("traffic_log.txt", "a");
    if (fp == NULL) {
        printf("warning: cant write to log\n");
        return;
    }
    time_t t = time(NULL);
    char *ts = ctime(&t);
    ts[strlen(ts) - 1] = '\0';
    fprintf(fp, "[%s] %s\n", ts, msg);
    fclose(fp);
}

void change_state(struct Intersection *x, int new_state) {
    char buf[80];
    if (new_state < RED || new_state > GREEN) {
        printf("invalid state %d\n", new_state);
        return;
    }
    x->state = new_state;

    if (new_state == RED)
        sprintf(buf, "%s -> RED", x->name);
    else if (new_state == YELLOW)
        sprintf(buf, "%s -> YELLOW", x->name);
    else
        sprintf(buf, "%s -> GREEN", x->name);

    log_entry(buf);
}

void print_light(struct Intersection *x) {
    printf("  %s: ", x->name);
    if (x->state == RED)
        printf("[RED] [ . ] [ . ]");
    else if (x->state == YELLOW)
        printf("[ . ] [YEL] [ . ]");
    else if (x->state == GREEN)
        printf("[ . ] [ . ] [GRN]");
    printf("  (%d cars)\n", x->vehicles);
}

void show_both(struct Intersection *a, struct Intersection *b) {
    printf("\n");
    print_light(a);
    print_light(b);
    printf("\n");
}

void set_green_time(struct Intersection *x) {
    if (x->vehicles > 10)
        x->green_secs = 10;
    else if (x->vehicles > 5)
        x->green_secs = 7;
    else
        x->green_secs = DEFAULT_GREEN;
}

// background thread - adds random cars every 3 seconds
void *sensor_thread(void *arg) {
    srand(time(NULL));
    while (keep_going) {
        sleep(3);
        if (!keep_going) break;
        if (inter_a != NULL) inter_a->vehicles += rand() % 4;
        if (inter_b != NULL) inter_b->vehicles += rand() % 4;
        log_entry("sensor updated counts");
    }
    return NULL;
}

void run_cycle(struct Intersection *a, struct Intersection *b) {
    char buf[120];
    sprintf(buf, "cycle start - %s: %d cars, %s: %d cars",
            a->name, a->vehicles, b->name, b->vehicles);
    log_entry(buf);

    change_state(a, GREEN);
    change_state(b, RED);
    printf("%s is GREEN for %d seconds\n", a->name, a->green_secs);
    show_both(a, b);
    sleep(a->green_secs);

    change_state(a, YELLOW);
    show_both(a, b);
    sleep(YELLOW_TIME);

    change_state(a, RED);
    a->vehicles = 0;

    change_state(b, GREEN);
    printf("%s is GREEN for %d seconds\n", b->name, b->green_secs);
    show_both(a, b);
    sleep(b->green_secs);

    change_state(b, YELLOW);
    show_both(a, b);
    sleep(YELLOW_TIME);

    change_state(b, RED);
    b->vehicles = 0;

    log_entry("cycle done");
    printf("cycle complete\n\n");
}

void manual_override(struct Intersection *a, struct Intersection *b) {
    int pick, st;
    printf("1. %s\n2. %s\nwhich one? ", a->name, b->name);
    scanf("%d", &pick);
    getchar();

    if (pick != 1 && pick != 2) {
        printf("bad choice\n");
        return;
    }

    printf("state? 0=RED 1=YELLOW 2=GREEN: ");
    scanf("%d", &st);
    getchar();

    if (pick == 1)
        change_state(a, st);
    else
        change_state(b, st);

    log_entry("manual override done");
    printf("done\n");
}

int main() {
    int choice, running, v;
    pthread_t sensor;

    inter_a = malloc(sizeof(struct Intersection));
    inter_b = malloc(sizeof(struct Intersection));

    if (inter_a == NULL || inter_b == NULL) {
        printf("malloc failed\n");
        return 1;
    }

    strcpy(inter_a->name, "Intersection A");
    inter_a->state = GREEN;
    inter_a->vehicles = 0;
    inter_a->green_secs = DEFAULT_GREEN;

    strcpy(inter_b->name, "Intersection B");
    inter_b->state = RED;
    inter_b->vehicles = 0;
    inter_b->green_secs = DEFAULT_GREEN;

    if (pthread_create(&sensor, NULL, sensor_thread, NULL) != 0) {
        printf("sensor thread failed to start\n");
        keep_going = 0;
    }

    log_entry("system started");
    printf("Smart Traffic Light Controller\n\n");

    running = 1;
    while (running) {
        printf("1. run a cycle\n");
        printf("2. show status\n");
        printf("3. manual override\n");
        printf("4. exit\n");
        printf(">> ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("enter a number\n");
            continue;
        }
        getchar();

        if (choice == 1) {
            printf("vehicles at %s: ", inter_a->name);
            scanf("%d", &v);
            getchar();
            if (v >= 0) inter_a->vehicles = v;

            printf("vehicles at %s: ", inter_b->name);
            scanf("%d", &v);
            getchar();
            if (v >= 0) inter_b->vehicles = v;

            set_green_time(inter_a);
            set_green_time(inter_b);
            run_cycle(inter_a, inter_b);

        } else if (choice == 2) {
            show_both(inter_a, inter_b);

        } else if (choice == 3) {
            manual_override(inter_a, inter_b);
            show_both(inter_a, inter_b);

        } else if (choice == 4) {
            running = 0;

        } else {
            printf("invalid\n");
        }
    }

    keep_going = 0;
    pthread_join(sensor, NULL);

    log_entry("shutting down");
    free(inter_a);
    free(inter_b);

    printf("bye!\n");
    return 0;
}
