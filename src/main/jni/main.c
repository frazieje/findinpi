#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <ctype.h>
#include <sys/time.h>
#include <time.h>
#include "divsufsort.h"
#include "femto.h"
#include "com_frazieje_findinpi_service_NativePiFinder.h"

//femto globals
femto_server_t femto_server;
char *femto_index_path;

//divsufsort globals
sauchar_t *data;
size_t size;
saidx_t *SA;

char *full_data_file_path;

/* Size of each input chunk to be
   read and allocate for. */
#ifndef  CHUNK_SIZE
#define  CHUNK_SIZE  2097152
#endif

#define  READALL_OK          0  /* Success */
#define  READALL_INVALID    -1  /* Invalid parameters */
#define  READALL_ERROR      -2  /* Stream error */
#define  READALL_TOOMUCH    -3  /* Too much input */
#define  READALL_NOMEM      -4  /* Out of memory */

/* This function returns one of the READALL_ constants above.
   If the return value is zero == READALL_OK, then:
     (*dataptr) points to a dynamically allocated buffer, with
     (*sizeptr) chars read from the file.
     The buffer is allocated for one extra char, which is NUL,
     and automatically appended after the data.
   Initial values of (*dataptr) and (*sizeptr) are ignored.
*/
int readall(FILE *in, unsigned char **dataptr, size_t *sizeptr)
{
    unsigned char  *data = NULL, *temp;
    size_t size = 0;
    size_t used = 0;
    size_t n;

    /* None of the parameters can be NULL. */
    if (in == NULL || dataptr == NULL || sizeptr == NULL)
        return READALL_INVALID;

    /* A read error already occurred? */
    if (ferror(in))
        return READALL_ERROR;

    while (1) {
        if (used + CHUNK_SIZE + 1 > size) {
            size = used + CHUNK_SIZE + 1;
            /* Overflow check. Some ANSI C compilers
               may optimize this away, though. */
            if (size <= used) {
                free(data);
                return READALL_TOOMUCH;
            }
            temp = realloc(data, size);
            if (temp == NULL) {
                free(data);
                return READALL_NOMEM;
            }
            data = temp;
        }

        n = fread(data + used, 1, CHUNK_SIZE, in);
        if (n == 0)
            break;

        used += n;
    }

    if (ferror(in)) {
        free(data);
        return READALL_ERROR;
    }

    temp = realloc(data, used + 1);
    if (temp == NULL) {
        free(data);
        return READALL_NOMEM;
    }
    data = temp;
    data[used] = '\0';

    *dataptr = data;
    *sizeptr = used;

    return READALL_OK;
}

ssize_t read_n_at(int fd, unsigned long long offset, void *buf, size_t n) {
    size_t total = 0;
    char *p = (char *)buf;

    while (total < n) {
        ssize_t r = pread(fd, p + total, n - total, offset + total);
        if (r == 0) {
            // EOF before reading n bytes
            break;
        }
        if (r < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        total += (size_t)r;
    }

    return (ssize_t)total;
}

static int femto_do_count_request(char *search_pattern, char **result) {
    int rc;
    femto_request_t *femto_request = NULL;
    struct timespec start;
    struct timespec now;

    printf("Starting FEMTO request for index %s. Counting occurrences of pattern '%s'\n", femto_index_path, search_pattern);

    size_t pattern_len = strlen(search_pattern);

    const char req_type[] = "find_strings ";

    size_t req_type_len = strlen(req_type);

    char req_pattern[req_type_len + pattern_len + 1];

    strcpy(req_pattern, req_type);
    strcat(req_pattern, search_pattern);

    printf("Sending FEMTO request: %s\n", req_pattern);

    rc = femto_create_generic_request(
        &femto_request,
        &femto_server,
        femto_index_path,
        req_pattern
    );

    if(rc != 0) {
        perror("femto_create_generic_request");
        return rc;
    }

    rc = femto_begin_request(&femto_server, femto_request);
    if(rc != 0) {
        perror("femto_begin_request");
        return rc;
    }

    femto_wait_request(&femto_server, femto_request);

    char* response;

    rc = femto_response_for_generic_request(femto_request, &femto_server, &response);
    if(rc != 0) {
        perror("femto_response_for_generic_request");
        return rc;
    }

    *result = response;

    printf("FEMTO sending result %s\n", *result);

    femto_destroy_request(femto_request);

    return rc;
}

static int femto_do_request(char *search_pattern, int maxResultCount, char **result) {
    int rc;
    femto_request_t *femto_request = NULL;
    struct timespec start;
    struct timespec now;

    printf("Starting FEMTO request for index %s. Searching for pattern '%s'\n", femto_index_path, search_pattern);

    size_t pattern_len = strlen(search_pattern);

    const char req_type[] = "find_docs";

    char req_params[100];

    sprintf(req_params, "%s %d 1 ", req_type, maxResultCount);

    size_t req_params_len = strlen(req_params);

    char req_pattern[req_params_len + pattern_len + 1];

    strcpy(req_pattern, req_params);
    strcat(req_pattern, search_pattern);

    printf("Sending FEMTO request: %s\n", req_pattern);

    rc = femto_create_generic_request(
        &femto_request,
        &femto_server,
        femto_index_path,
        req_pattern
    );

    if(rc != 0) {
        perror("femto_create_generic_request");
        return rc;
    }

    rc = femto_begin_request(&femto_server, femto_request);
    if(rc != 0) {
        perror("femto_begin_request");
        return rc;
    }

    femto_wait_request(&femto_server, femto_request);

    char* response;

    rc = femto_response_for_generic_request(femto_request, &femto_server, &response);
    if(rc != 0) {
        perror("femto_response_for_generic_request");
        return rc;
    }

    *result = response;

    printf("FEMTO sending result %s\n", *result);

    femto_destroy_request(femto_request);

    return rc;
}

JNIEXPORT void JNICALL Java_com_frazieje_findinpi_service_NativePiFinder_init(
    JNIEnv *env,
    jobject thisObj,
    jstring dataFilePath,
    jstring suffixArrayFilePath,
    jstring fmIndexFilePath,
    jstring fullDataFilePath
) {
    FILE *fp = NULL;
    int load_result = READALL_INVALID;
    struct timeval tval_before, tval_after, tval_result;
    int64_t elapsed;

    char *full_path = ((char *)((*env)->GetStringUTFChars(env, fullDataFilePath, 0)));
    int full_path_len = strlen(full_path);
    strncpy(full_data_file_path, full_path, full_path_len + 1);
    (*env)->ReleaseStringUTFChars(env, fullDataFilePath, full_path);

    char *data_file_path = ((char *)((*env)->GetStringUTFChars(env, dataFilePath, 0)));

    if((fp = fopen(data_file_path, "r")) == NULL) {
        perror("fopen");
        return;
    }

    (*env)->ReleaseStringUTFChars(env, dataFilePath, data_file_path);

    printf("Loading data file into main memory...\n");
    fflush(stdout);

    //load data file into memory
    gettimeofday(&tval_before, NULL);
    load_result = readall(fp, &data, &size);
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);

    fclose(fp);

    elapsed = (tval_result.tv_sec*1000000 + tval_result.tv_usec) / 1000;

    if (load_result != READALL_OK) {
        perror("Error");
        return;
    }

    printf("Finished loading data file in %ldms. size = %lu\n", elapsed, size);

    printf("Allocating %lu bytes for suffix array...\n", sizeof(saidx_t) * size);
    fflush(stdout);

    SA = malloc(sizeof(saidx_t) * size);
    if (SA == NULL) {
        perror("malloc");
        return;
    }

    char *suffix_array_file_path = ((char *)((*env)->GetStringUTFChars(env, suffixArrayFilePath, 0)));

    printf("Allocated %lu bytes for suffix array.\n", sizeof(saidx_t) * size);

    //load or calculate the suffix array
    if((fp = fopen(suffix_array_file_path, "r")) == NULL) {
        perror("fopen");
        return;
    }

    printf("Reading suffix array from %s...\n", suffix_array_file_path);
    fflush(stdout);

    (*env)->ReleaseStringUTFChars(env, suffixArrayFilePath, suffix_array_file_path);

    gettimeofday(&tval_before, NULL);
    size_t n_read = fread(SA, sizeof(saidx_t), size, fp);
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);

    fclose(fp);

    elapsed = (tval_result.tv_sec*1000000 + tval_result.tv_usec) / 1000;

    printf("Finished reading %ld byte suffix array in %ldms\n", n_read * sizeof(saidx_t), elapsed);

    femto_index_path = ((char *)((*env)->GetStringUTFChars(env, fmIndexFilePath, 0)));

    const int rc = femto_start_server(&femto_server);
    if (rc != 0) {
        perror("femto_start_server");
        return;
    }

    printf("Started femto server... data file: %s\n", femto_index_path);

    fflush(stdout);
}

JNIEXPORT jobject JNICALL Java_com_frazieje_findinpi_service_NativePiFinder_countInternal(
    JNIEnv *env,
    jobject thisObj,
    jstring searchText
) {
    char *search_string;
    unsigned long long result = -1;

    struct timeval tval_before, tval_after, tval_result;
    int64_t elapsed;

    search_string = ((char *)((*env)->GetStringUTFChars(env, searchText, 0)));

    char *search_result;

    gettimeofday(&tval_before, NULL);
    result = femto_do_count_request(search_string, &search_result);
    (*env)->ReleaseStringUTFChars(env, searchText, search_string);
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);

    elapsed = (tval_result.tv_sec*1000000 + tval_result.tv_usec) / 1000;

    printf("count result returned in %lldms\n", elapsed);

    jclass cls_native_result = (*env)->FindClass(env, "com/frazieje/findinpi/service/NativeResult");
    jmethodID cnstr_native_result = (*env)->GetMethodID(env, cls_native_result, "<init>", "(Ljava/lang/String;J)V");
    jstring native_result_str = (*env)->NewStringUTF(env, search_result);
    jobject obj_result = (*env)->NewObject(env, cls_native_result, cnstr_native_result, native_result_str, elapsed);

    fflush(stdout);
    free(search_result);
    return obj_result;
}

int searchtext(char *data, char *str, unsigned long long *result) {

    int find_result = -1;
    char *loc;

    loc = strstr(data, str);
    if(loc != NULL) {
        *result = loc - data;
        find_result = 0;
    }

    return find_result;
}

int extract_min_uint64(const char *s, unsigned long long *out_min) {
    if (s == NULL || out_min == NULL) {
        return 0;
    }

    char pattern[] = "\"offsets\":[";
    const char *p = strstr(s, pattern);

    s = p;

    // Find opening bracket
    while (*s && *s != '[') {
        s++;
    }
    if (*s != '[') {
        return 0;
    }
    s++; // move past '['

    int found_any = 0;
    unsigned long long min_value = 0;

    for (;;) {
        // Skip whitespace
        while (isspace((unsigned char)*s)) {
            s++;
        }

        // End of list?
        if (*s == ']') {
            break;
        }

        // Parse one number
        errno = 0;
        char *endptr = NULL;
        unsigned long long value = strtoull(s, &endptr, 10);

        // No digits parsed
        if (endptr == s) {
            return 0;
        }

        // Overflow
        if (errno == ERANGE) {
            return 0;
        }

        unsigned long long v = value;

        if (!found_any || v < min_value) {
            min_value = v;
            found_any = 1;
        }

        s = endptr;

        // Skip whitespace after number
        while (isspace((unsigned char)*s)) {
            s++;
        }

        if (*s == ',') {
            s++;   // move to next number
            continue;
        } else if (*s == ']') {
            break;
        } else {
            // Unexpected character
            return 0;
        }
    }

    if (!found_any) {
        return 0;
    }

    *out_min = min_value;
    return 1;
}

JNIEXPORT jobject JNICALL Java_com_frazieje_findinpi_service_NativePiFinder_searchInternal(
    JNIEnv *env,
    jobject thisObj,
    jstring searchText
) {
    char *search_string;
    unsigned long long result = -1;

    struct timeval tval_before, tval_after, tval_result;
    int64_t elapsed;

    search_string = ((char *)((*env)->GetStringUTFChars(env, searchText, 0)));

    int search_string_len = strlen(search_string);

    char *search_result;
    char json_prefix[] = "{\"results\":[{\"offsets\":[";
    int json_prefix_len = strlen(json_prefix);
    char offset_result[21]; // enough space for unsigned long long max value (20) + null char
    char json_suffix[] = "]}]}";
    int json_suffix_len = strlen(json_suffix);

    gettimeofday(&tval_before, NULL);

    if (result < 0 && search_string_len <= 5) {
        int sr = searchtext(data, search_string, &result);
        sprintf(offset_result, "%llu", result);
        int offset_result_len = strlen(offset_result);
        int search_result_len = json_prefix_len + offset_result_len + json_suffix_len + 1;
        search_result = malloc(search_result_len);
        strncpy(search_result, json_prefix, search_result_len);
        strncat(search_result, offset_result, offset_result_len + 1);
        strncat(search_result, json_suffix, json_suffix_len + 1);
    }

    if (result < 0 && search_string_len <= 7) {
        saidx_t num_matches, offset;
        int first_match = 2147483647;
        num_matches = sa_search(data, (saidx_t)size, (sauchar_t *)search_string, (saidx_t)search_string_len, SA, (saidx_t)size, &offset);
        printf("found %d matches using suffix array\n", num_matches);
        for (saidx_t i = 0; i < num_matches; i++) {
            int match = SA[offset + i];
            if (match < first_match) {
                first_match = match;
                result = first_match;
            }
        }
        sprintf(offset_result, "%d", first_match);
        int offset_result_len = strlen(offset_result);
        int search_result_len = json_prefix_len + offset_result_len + json_suffix_len + 1;
        search_result = malloc(search_result_len);
        strncpy(search_result, json_prefix, search_result_len);
        strncat(search_result, offset_result, offset_result_len + 1);
        strncat(search_result, json_suffix, json_suffix_len + 1);
    }

    if (result < 0) { // length >= 8
        result = femto_do_request(search_string, 1500 /* refactor to parameter/argument */, &search_result);
        unsigned long long min_value;
        extract_min_uint64(search_result, &min_value);
        free(search_result);
        sprintf(offset_result, "%d", min_value);
        int offset_result_len = strlen(offset_result);
        int search_result_len = json_prefix_len + offset_result_len + json_suffix_len + 1;
        search_result = malloc(search_result_len);
        strncpy(search_result, json_prefix, search_result_len);
        strncat(search_result, offset_result, offset_result_len + 1);
        strncat(search_result, json_suffix, json_suffix_len + 1);
    }

    (*env)->ReleaseStringUTFChars(env, searchText, search_string);

    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);

    elapsed = (tval_result.tv_sec*1000000 + tval_result.tv_usec) / 1000;

    printf("search result returned in %lldms\n", elapsed);

    FILE *fp = NULL;
    if((fp = fopen(full_data_file_path, "r")) == NULL) {
        perror("fopen");
        return;
    }
    char pibuf[64];
    read_n_at(full_data_file_path, result - 16, pibuf, 64);

    fclose(fp);

    printf("pi string = %s", pibuf);

    jclass cls_native_result = (*env)->FindClass(env, "com/frazieje/findinpi/service/NativeResult");
    jmethodID cnstr_native_result = (*env)->GetMethodID(env, cls_native_result, "<init>", "(Ljava/lang/String;J)V");
    jstring native_result_str = (*env)->NewStringUTF(env, search_result);
    jobject obj_result = (*env)->NewObject(env, cls_native_result, cnstr_native_result, native_result_str, elapsed);

    fflush(stdout);
    free(search_result);
    return obj_result;
}
