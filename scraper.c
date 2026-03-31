#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>

#define MAX_URLS 20
#define MAX_URL_LEN 256

struct Job {
    int num;
    char url[MAX_URL_LEN];
};

size_t save_data(void *data, size_t size, size_t count, void *file) {
    return fwrite(data, size, count, (FILE *)file);
}

void *fetch_url(void *arg) {
    struct Job *job = (struct Job *)arg;
    char filename[50];
    CURL *curl;
    FILE *fp;
    CURLcode res;

    sprintf(filename, "output_%d.txt", job->num);

    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("thread %d: cant open file\n", job->num);
        return NULL;
    }

    curl = curl_easy_init();
    if (curl == NULL) {
        printf("thread %d: curl init failed\n", job->num);
        fclose(fp);
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, job->url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, save_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        printf("thread %d: failed - %s\n", job->num, curl_easy_strerror(res));
        fprintf(fp, "fetch failed: %s\n", curl_easy_strerror(res));
    } else {
        printf("thread %d: done, saved to %s\n", job->num, filename);
    }

    curl_easy_cleanup(curl);
    fclose(fp);
    return NULL;
}

int main() {
    char urls[MAX_URLS][MAX_URL_LEN];
    pthread_t threads[MAX_URLS];
    struct Job jobs[MAX_URLS];
    int count = 0;
    int i, len;

    printf("Multi-threaded Web Scraper\n");
    printf("enter URLs one by one, blank line to start\n\n");

    while (count < MAX_URLS) {
        printf("URL %d: ", count + 1);
        fgets(urls[count], MAX_URL_LEN, stdin);

        // remove newline fgets adds
        len = strlen(urls[count]);
        if (len > 0 && urls[count][len - 1] == '\n')
            urls[count][len - 1] = '\0';

        if (strlen(urls[count]) == 0)
            break;

        count++;
    }

    if (count == 0) {
        printf("no URLs entered\n");
        return 0;
    }

    printf("\nstarting %d thread(s)...\n\n", count);

    for (i = 0; i < count; i++) {
        jobs[i].num = i + 1;
        strcpy(jobs[i].url, urls[i]);

        if (pthread_create(&threads[i], NULL, fetch_url, &jobs[i]) != 0) {
            printf("could not start thread %d\n", i + 1);
            threads[i] = 0;
        }
    }

    for (i = 0; i < count; i++) {
        if (threads[i] != 0)
            pthread_join(threads[i], NULL);
    }

    printf("\nall done, check output_1.txt etc for results\n");
    return 0;
}
