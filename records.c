#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SUBJECTS 5
#define MAX_NAME 50
#define MAX_COURSE 50

struct Student {
    int id;
    char name[MAX_NAME];
    char course[MAX_COURSE];
    int age;
    float grades[MAX_SUBJECTS];
    int num_subjects;
    float gpa;
};

struct Student *students = NULL;
int num_students = 0;
int capacity = 0;

// fgets leaves a newline, this removes it
void fix_newline(char *s) {
    int i = 0;
    while (s[i] != '\n' && s[i] != '\0') i++;
    s[i] = '\0';
}

float calc_gpa(float *grades, int n) {
    float sum = 0;
    int i;
    if (n == 0) return 0;
    for (i = 0; i < n; i++) sum += grades[i];
    return sum / n;
}

int find_by_id(int id) {
    int i;
    for (i = 0; i < num_students; i++) {
        if (students[i].id == id) return i;
    }
    return -1;
}

void print_student(int idx) {
    struct Student *s = &students[idx];
    int i;
    printf("  ID:%d  Name:%s  Course:%s  Age:%d  GPA:%.2f\n",
           s->id, s->name, s->course, s->age, s->gpa);
    printf("  grades: ");
    for (i = 0; i < s->num_subjects; i++) printf("%.1f ", s->grades[i]);
    printf("\n");
}

void show_menu() {
    printf("\n1.  Add student\n");
    printf("2.  Show all\n");
    printf("3.  Update student\n");
    printf("4.  Delete student\n");
    printf("5.  Search by ID\n");
    printf("6.  Search by name\n");
    printf("7.  Sort by GPA\n");
    printf("8.  Sort by name\n");
    printf("9.  Sort by ID\n");
    printf("10. Report\n");
    printf("11. Save\n");
    printf("12. Load\n");
    printf("0.  Exit\n");
    printf("Choice: ");
}

void add_student() {
    int i;
    struct Student *s;

    if (num_students >= capacity) {
        int new_cap = (capacity == 0) ? 4 : capacity * 2;
        struct Student *tmp = realloc(students, new_cap * sizeof(struct Student));
        if (tmp == NULL) {
            printf("not enough memory\n");
            return;
        }
        students = tmp;
        capacity = new_cap;
    }

    s = &students[num_students];

    printf("Student ID: ");
    if (scanf("%d", &s->id) != 1) {
        printf("invalid\n");
        getchar();
        return;
    }
    getchar();

    if (s->id <= 0) {
        printf("ID has to be positive\n");
        return;
    }

    if (find_by_id(s->id) != -1) {
        printf("ID %d already exists\n", s->id);
        return;
    }

    printf("Name: ");
    fgets(s->name, MAX_NAME, stdin);
    fix_newline(s->name);
    if (strlen(s->name) == 0) {
        printf("name cant be empty\n");
        return;
    }

    printf("Course: ");
    fgets(s->course, MAX_COURSE, stdin);
    fix_newline(s->course);

    printf("Age: ");
    if (scanf("%d", &s->age) != 1 || s->age < 1 || s->age > 120) {
        printf("bad age\n");
        getchar();
        return;
    }
    getchar();

    printf("Number of subjects (1-%d): ", MAX_SUBJECTS);
    if (scanf("%d", &s->num_subjects) != 1 || s->num_subjects < 1 || s->num_subjects > MAX_SUBJECTS) {
        printf("invalid\n");
        getchar();
        return;
    }
    getchar();

    for (i = 0; i < s->num_subjects; i++) {
        printf("Grade %d (0-100): ", i + 1);
        if (scanf("%f", &s->grades[i]) != 1 || s->grades[i] < 0 || s->grades[i] > 100) {
            printf("grade must be 0 to 100\n");
            getchar();
            return;
        }
    }
    getchar();

    s->gpa = calc_gpa(s->grades, s->num_subjects);
    num_students++;
    printf("added! GPA = %.2f\n", s->gpa);
}

void display_all() {
    int i;
    if (num_students == 0) {
        printf("no students yet\n");
        return;
    }
    printf("\n%d student(s):\n", num_students);
    for (i = 0; i < num_students; i++) print_student(i);
}

void update_student() {
    int id, idx, opt, i;
    struct Student *s;

    printf("ID to update: ");
    if (scanf("%d", &id) != 1) { getchar(); return; }
    getchar();

    idx = find_by_id(id);
    if (idx == -1) {
        printf("not found\n");
        return;
    }

    s = &students[idx];
    printf("editing %s\n", s->name);
    printf("1=name  2=course  3=age  4=grades\nchoice: ");
    if (scanf("%d", &opt) != 1) { getchar(); return; }
    getchar();

    if (opt == 1) {
        printf("new name: ");
        fgets(s->name, MAX_NAME, stdin);
        fix_newline(s->name);

    } else if (opt == 2) {
        printf("new course: ");
        fgets(s->course, MAX_COURSE, stdin);
        fix_newline(s->course);

    } else if (opt == 3) {
        printf("new age: ");
        if (scanf("%d", &s->age) != 1 || s->age < 1 || s->age > 120) {
            printf("bad age\n");
            getchar();
            return;
        }
        getchar();

    } else if (opt == 4) {
        for (i = 0; i < s->num_subjects; i++) {
            printf("grade %d: ", i + 1);
            if (scanf("%f", &s->grades[i]) != 1 || s->grades[i] < 0 || s->grades[i] > 100) {
                printf("invalid\n");
                getchar();
                return;
            }
        }
        getchar();
        s->gpa = calc_gpa(s->grades, s->num_subjects);
        printf("new GPA: %.2f\n", s->gpa);

    } else {
        printf("invalid choice\n");
        return;
    }

    printf("updated\n");
}

void delete_student() {
    int id, idx, i;
    char ans;

    printf("ID to delete: ");
    if (scanf("%d", &id) != 1) { getchar(); return; }
    getchar();

    idx = find_by_id(id);
    if (idx == -1) {
        printf("not found\n");
        return;
    }

    printf("delete %s? (y/n): ", students[idx].name);
    scanf(" %c", &ans);
    getchar();
    if (ans != 'y' && ans != 'Y') {
        printf("cancelled\n");
        return;
    }

    for (i = idx; i < num_students - 1; i++)
        students[i] = students[i + 1];

    num_students--;
    printf("deleted\n");
}

void search_by_id() {
    int id, idx;
    printf("enter ID: ");
    if (scanf("%d", &id) != 1) { getchar(); return; }
    getchar();

    idx = find_by_id(id);
    if (idx == -1) printf("not found\n");
    else print_student(idx);
}

void search_by_name() {
    char query[MAX_NAME];
    int i, found = 0;

    printf("enter name: ");
    fgets(query, MAX_NAME, stdin);
    fix_newline(query);

    for (i = 0; i < num_students; i++) {
        if (strstr(students[i].name, query) != NULL) {
            print_student(i);
            found++;
        }
    }
    if (found == 0) printf("no matches\n");
    else printf("%d found\n", found);
}

void sort_by_gpa() {
    int i, j;
    struct Student tmp;
    for (i = 0; i < num_students - 1; i++) {
        for (j = 0; j < num_students - 1 - i; j++) {
            if (students[j].gpa < students[j+1].gpa) {
                tmp = students[j];
                students[j] = students[j+1];
                students[j+1] = tmp;
            }
        }
    }
    printf("sorted\n");
}

void sort_by_name() {
    int i, j;
    struct Student tmp;
    for (i = 0; i < num_students - 1; i++) {
        for (j = 0; j < num_students - 1 - i; j++) {
            if (strcmp(students[j].name, students[j+1].name) > 0) {
                tmp = students[j];
                students[j] = students[j+1];
                students[j+1] = tmp;
            }
        }
    }
    printf("sorted\n");
}

void sort_by_id() {
    int i, j;
    struct Student tmp;
    for (i = 0; i < num_students - 1; i++) {
        for (j = 0; j < num_students - 1 - i; j++) {
            if (students[j].id > students[j+1].id) {
                tmp = students[j];
                students[j] = students[j+1];
                students[j+1] = tmp;
            }
        }
    }
    printf("sorted\n");
}

void generate_report() {
    int i, j, n;
    float tot, hi, lo, median;
    int hi_idx, lo_idx;
    float *temp;
    int *order;

    if (num_students == 0) {
        printf("no students\n");
        return;
    }

    tot = 0;
    hi = students[0].gpa;
    lo = students[0].gpa;
    hi_idx = 0;
    lo_idx = 0;

    for (i = 0; i < num_students; i++) {
        tot += students[i].gpa;
        if (students[i].gpa > hi) { hi = students[i].gpa; hi_idx = i; }
        if (students[i].gpa < lo) { lo = students[i].gpa; lo_idx = i; }
    }

    // copy gpas to temp array to find median without touching real data
    temp = malloc(num_students * sizeof(float));
    if (temp == NULL) { printf("memory error\n"); return; }
    for (i = 0; i < num_students; i++) temp[i] = students[i].gpa;

    for (i = 0; i < num_students - 1; i++) {
        for (j = 0; j < num_students - 1 - i; j++) {
            if (temp[j] > temp[j+1]) {
                float t = temp[j];
                temp[j] = temp[j+1];
                temp[j+1] = t;
            }
        }
    }

    if (num_students % 2 == 0)
        median = (temp[num_students/2 - 1] + temp[num_students/2]) / 2.0;
    else
        median = temp[num_students/2];
    free(temp);

    printf("\n--- report ---\n");
    printf("students: %d\n", num_students);
    printf("avg GPA: %.2f\n", tot / num_students);
    printf("highest: %.2f (%s)\n", hi, students[hi_idx].name);
    printf("lowest:  %.2f (%s)\n", lo, students[lo_idx].name);
    printf("median:  %.2f\n", median);

    printf("\nhow many top students to show? ");
    if (scanf("%d", &n) != 1 || n < 1) n = 3;
    getchar();
    if (n > num_students) n = num_students;

    // index array so i dont change the real order
    order = malloc(num_students * sizeof(int));
    if (order == NULL) { printf("memory error\n"); return; }
    for (i = 0; i < num_students; i++) order[i] = i;

    for (i = 0; i < num_students - 1; i++) {
        int best = i;
        for (j = i + 1; j < num_students; j++) {
            if (students[order[j]].gpa > students[order[best]].gpa)
                best = j;
        }
        int x = order[i];
        order[i] = order[best];
        order[best] = x;
    }

    printf("\ntop %d:\n", n);
    for (i = 0; i < n; i++) {
        printf("  %d. ", i + 1);
        print_student(order[i]);
    }
    free(order);

    char *courses[100];
    int ncourses = 0;

    for (i = 0; i < num_students; i++) {
        int already = 0;
        for (j = 0; j < ncourses; j++) {
            if (strcmp(courses[j], students[i].course) == 0) { already = 1; break; }
        }
        if (!already && ncourses < 100)
            courses[ncourses++] = students[i].course;
    }

    printf("\ncourse averages:\n");
    for (i = 0; i < ncourses; i++) {
        float ctot = 0;
        int cnt = 0, best_idx = -1;
        float best = -1;
        for (j = 0; j < num_students; j++) {
            if (strcmp(students[j].course, courses[i]) == 0) {
                ctot += students[j].gpa;
                cnt++;
                if (students[j].gpa > best) { best = students[j].gpa; best_idx = j; }
            }
        }
        printf("  %s: avg %.2f, best: %s (%.2f)\n",
               courses[i], ctot / cnt, students[best_idx].name, best);
    }
}

void save_to_file() {
    int i, j;
    FILE *fp = fopen("students.txt", "w");
    if (fp == NULL) {
        printf("couldnt open file\n");
        return;
    }
    fprintf(fp, "%d\n", num_students);
    for (i = 0; i < num_students; i++) {
        struct Student *s = &students[i];
        fprintf(fp, "%d\n%s\n%s\n%d\n%d\n", s->id, s->name, s->course, s->age, s->num_subjects);
        for (j = 0; j < s->num_subjects; j++)
            fprintf(fp, "%.2f\n", s->grades[j]);
    }
    fclose(fp);
    printf("saved %d students\n", num_students);
}

void load_from_file() {
    int i, j, n;
    FILE *fp = fopen("students.txt", "r");
    if (fp == NULL) {
        printf("no save file\n");
        return;
    }

    if (fscanf(fp, "%d\n", &n) != 1 || n < 0) {
        printf("file looks broken\n");
        fclose(fp);
        return;
    }

    free(students);
    students = malloc(n * sizeof(struct Student));
    if (students == NULL && n > 0) {
        printf("memory error\n");
        fclose(fp);
        return;
    }
    capacity = n;
    num_students = 0;

    for (i = 0; i < n; i++) {
        struct Student *s = &students[i];
        if (fscanf(fp, "%d\n", &s->id) != 1) break;
        if (!fgets(s->name, MAX_NAME, fp)) break;
        fix_newline(s->name);
        if (!fgets(s->course, MAX_COURSE, fp)) break;
        fix_newline(s->course);
        if (fscanf(fp, "%d\n%d\n", &s->age, &s->num_subjects) != 2) break;
        for (j = 0; j < s->num_subjects; j++)
            fscanf(fp, "%f\n", &s->grades[j]);
        s->gpa = calc_gpa(s->grades, s->num_subjects);
        num_students++;
    }

    fclose(fp);
    printf("loaded %d students\n", num_students);
}

int main() {
    int choice, running = 1;

    printf("Academic Records Analyzer\n");
    load_from_file();

    while (running) {
        show_menu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("enter a number\n");
            continue;
        }
        getchar();

        if (choice == 1) add_student();
        else if (choice == 2) display_all();
        else if (choice == 3) update_student();
        else if (choice == 4) delete_student();
        else if (choice == 5) search_by_id();
        else if (choice == 6) search_by_name();
        else if (choice == 7) { sort_by_gpa(); display_all(); }
        else if (choice == 8) { sort_by_name(); display_all(); }
        else if (choice == 9) { sort_by_id(); display_all(); }
        else if (choice == 10) generate_report();
        else if (choice == 11) save_to_file();
        else if (choice == 12) load_from_file();
        else if (choice == 0) { save_to_file(); running = 0; }
        else printf("invalid\n");
    }

    free(students);
    printf("bye!\n");
    return 0;
}
