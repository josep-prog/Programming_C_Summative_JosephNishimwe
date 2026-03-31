#include <stdio.h>
#include <stdlib.h>

double *data = NULL;
int data_size = 0;
int data_cap = 0;

void create_dataset() {
    int n, i;
    printf("how many values? ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("invalid\n");
        getchar();
        return;
    }
    getchar();

    double *tmp = realloc(data, n * sizeof(double));
    if (tmp == NULL) {
        printf("memory error\n");
        return;
    }
    data = tmp;
    data_cap = n;
    data_size = 0;

    for (i = 0; i < n; i++) {
        printf("value %d: ", i + 1);
        if (scanf("%lf", &data[i]) != 1) {
            printf("invalid, stopping\n");
            getchar();
            break;
        }
        data_size++;
    }
    getchar();
    printf("created %d values\n", data_size);
}

void add_value() {
    double val;
    printf("value to add: ");
    if (scanf("%lf", &val) != 1) {
        getchar();
        return;
    }
    getchar();

    if (data_size >= data_cap) {
        int new_cap = (data_cap == 0) ? 4 : data_cap * 2;
        double *tmp = realloc(data, new_cap * sizeof(double));
        if (tmp == NULL) {
            printf("memory error\n");
            return;
        }
        data = tmp;
        data_cap = new_cap;
    }

    data[data_size] = val;
    data_size++;
    printf("added %.2f, total: %d\n", val, data_size);
}

void reset_dataset() {
    free(data);
    data = NULL;
    data_size = 0;
    data_cap = 0;
    printf("cleared\n");
}

void save_to_file() {
    int i;
    if (data_size == 0) {
        printf("nothing to save\n");
        return;
    }
    FILE *f = fopen("dataset.txt", "w");
    if (f == NULL) {
        printf("couldnt open file\n");
        return;
    }
    fprintf(f, "%d\n", data_size);
    for (i = 0; i < data_size; i++) {
        fprintf(f, "%.6f\n", data[i]);
    }
    fclose(f);
    printf("saved %d values\n", data_size);
}

void load_from_file() {
    int n, i;
    FILE *f = fopen("dataset.txt", "r");
    if (f == NULL) {
        printf("no file found\n");
        return;
    }
    if (fscanf(f, "%d\n", &n) != 1 || n <= 0) {
        printf("bad file\n");
        fclose(f);
        return;
    }
    double *tmp = realloc(data, n * sizeof(double));
    if (tmp == NULL) {
        printf("memory error\n");
        fclose(f);
        return;
    }
    data = tmp;
    data_cap = n;
    data_size = 0;
    for (i = 0; i < n; i++) {
        if (fscanf(f, "%lf\n", &data[i]) != 1) break;
        data_size++;
    }
    fclose(f);
    printf("loaded %d values\n", data_size);
}

void show_data() {
    int i;
    if (data_size == 0) {
        printf("no data\n");
        return;
    }
    printf("data (%d values): ", data_size);
    for (i = 0; i < data_size; i++) {
        printf("%.2f", data[i]);
        if (i < data_size - 1) printf(", ");
    }
    printf("\n");
}

void show_sum_avg() {
    int i;
    double sum = 0;
    if (data_size == 0) {
        printf("no data\n");
        return;
    }
    for (i = 0; i < data_size; i++) sum += data[i];
    printf("sum: %.4f\n", sum);
    printf("avg: %.4f\n", sum / data_size);
}

void show_min_max() {
    int i;
    double lo, hi;
    if (data_size == 0) {
        printf("no data\n");
        return;
    }
    lo = data[0];
    hi = data[0];
    for (i = 1; i < data_size; i++) {
        if (data[i] < lo) lo = data[i];
        if (data[i] > hi) hi = data[i];
    }
    printf("min: %.4f\n", lo);
    printf("max: %.4f\n", hi);
}

void search_value() {
    int i, found;
    double target;
    if (data_size == 0) {
        printf("no data\n");
        return;
    }
    printf("search for: ");
    if (scanf("%lf", &target) != 1) {
        getchar();
        return;
    }
    getchar();

    found = 0;
    for (i = 0; i < data_size; i++) {
        if (data[i] == target) {
            printf("found at index %d\n", i);
            found++;
        }
    }
    if (found == 0) printf("not found\n");
    else printf("found %d time(s)\n", found);
}

double threshold = 0.0;

int above_thresh(double v) {
    return v > threshold;
}

double double_val(double v) {
    return v * 2.0;
}

// qsort needs this format, returns negative/zero/positive
int sort_asc(const void *a, const void *b) {
    double x = *(double *)a;
    double y = *(double *)b;
    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}

void apply_filter(int (*cb)(double)) {
    int i, n;
    double *result;
    if (data_size == 0) {
        printf("no data\n");
        return;
    }
    result = malloc(data_size * sizeof(double));
    if (result == NULL) {
        printf("memory error\n");
        return;
    }
    n = 0;
    for (i = 0; i < data_size; i++) {
        if (cb(data[i])) {
            result[n] = data[i];
            n++;
        }
    }
    free(data);
    data = result;
    data_size = n;
    data_cap = n;
    printf("%d values left after filter\n", data_size);
}

void apply_transform(double (*cb)(double)) {
    int i;
    if (data_size == 0) {
        printf("no data\n");
        return;
    }
    for (i = 0; i < data_size; i++) {
        data[i] = cb(data[i]);
    }
    printf("done, %d values changed\n", data_size);
}

void do_filter() {
    printf("keep values above: ");
    if (scanf("%lf", &threshold) != 1) {
        getchar();
        return;
    }
    getchar();
    apply_filter(above_thresh);
}

void do_transform() {
    apply_transform(double_val);
}

void do_sort() {
    if (data_size == 0) {
        printf("no data\n");
        return;
    }
    qsort(data, data_size, sizeof(double), sort_asc);
    printf("sorted\n");
    show_data();
}

struct MenuItem {
    char name[50];
    void (*fn)(void);
};

struct MenuItem menu[] = {
    {"create dataset", create_dataset},
    {"add a value", add_value},
    {"clear dataset", reset_dataset},
    {"save to file", save_to_file},
    {"load from file", load_from_file},
    {"show data", show_data},
    {"sum and average", show_sum_avg},
    {"min and max", show_min_max},
    {"search value", search_value},
    {"filter above threshold", do_filter},
    {"double all values", do_transform},
    {"sort ascending", do_sort},
};

int main() {
    int choice, i;
    int total = sizeof(menu) / sizeof(menu[0]);

    printf("Data Analysis Toolkit\n");

    while (1) {
        printf("\n");
        for (i = 0; i < total; i++) {
            printf(" %2d. %s\n", i + 1, menu[i].name);
        }
        printf("  0. exit\n");
        printf("choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("enter a number\n");
            continue;
        }
        getchar();

        if (choice == 0) break;

        if (choice >= 1 && choice <= total) {
            if (menu[choice - 1].fn == NULL) {
                printf("error: null function pointer\n");
            } else {
                menu[choice - 1].fn();
            }
        } else {
            printf("invalid\n");
        }
    }

    free(data);
    printf("bye!\n");
    return 0;
}
