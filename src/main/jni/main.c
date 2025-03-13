#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "com_frazieje_findinpi_service_NativePiFinder.h"

#define  READALL_OK          0  /* Success */
#define  READALL_INVALID    -1  /* Invalid parameters */
#define  READALL_ERROR      -2  /* Stream error */
#define  READALL_TOOMUCH    -3  /* Too much input */
#define  READALL_NOMEM      -4  /* Out of memory */

int64_t MAX(int64_t a, int64_t b) { return((a) > (b) ? a : b); }

int64_t MIN(int64_t a, int64_t b) { return((a) < (b) ? a : b); }

char *data;
size_t data_size;

/* This function returns one of the READALL_ constants above.
   If the return value is zero == READALL_OK, then:
     (*dataptr) points to a dynamically allocated buffer, with
     (*sizeptr) chars read from the file.
     The buffer is allocated for one extra char, which is NUL,
     and automatically appended after the data.
   Initial values of (*dataptr) and (*sizeptr) are ignored.
*/
int readall(FILE *in, char **dataptr, size_t *sizeptr, int chunk_size)
{
    char  *data = NULL, *temp;
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

        if (used + chunk_size + 1 > size) {
            size = used + chunk_size + 1;

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

        n = fread(data + used, 1, chunk_size, in);
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

unsigned long long searchtext(char *str) {

    unsigned long long find_result = -1;
    char *loc;

    loc = strstr(data, str);
    if(loc != NULL) {
        find_result = loc - data;
    }

    if(find_result == -1) {
        printf("Sorry, couldn't find a match.\n");
    }

    return find_result;
}

JNIEXPORT void JNICALL Java_com_frazieje_findinpi_service_NativePiFinder_init(
    JNIEnv *env,
    jobject thisObj,
    jstring filePath,
    jlong buffer_size
) {
    FILE *fp = NULL;
    int load_result = READALL_INVALID;
    char *search_string, *file_path;
    struct timeval tval_before, tval_after, tval_result;
    int64_t elapsed;

    file_path = ((char *)((*env)->GetStringUTFChars(env, filePath, 0)));

    if((fp = fopen(file_path, "r")) == NULL) {
        perror("fopen");
        return;
    }

    printf("Loading data file into main memory...\n");
    fflush(stdout);

    gettimeofday(&tval_before, NULL);
    load_result = readall(fp, &data, &data_size, (int)buffer_size);
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);

    elapsed = (tval_result.tv_sec*1000000 + tval_result.tv_usec) / 1000;
    printf("Finished loading data file in %lldms. size = %lu\n", elapsed, data_size);
    fflush(stdout);
}

JNIEXPORT jobject JNICALL Java_com_frazieje_findinpi_service_NativePiFinder_search(
    JNIEnv * env,
    jobject thisObj,
    jstring searchText
) {

    char *search_string;
    unsigned long long result = -1;

    unsigned char found;

    struct timeval tval_before, tval_after, tval_result;
    int64_t elapsed;

    search_string = ((char *)((*env)->GetStringUTFChars(env, searchText, 0)));

    gettimeofday(&tval_before, NULL);
    result = searchtext(search_string);
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);

    elapsed = (tval_result.tv_sec*1000000 + tval_result.tv_usec) / 1000;

    if (result == -1) {
        found = (unsigned char)0;
    } else {
        found = (unsigned char)1;
    }

    if (found) {
        printf("%llu location found in %lldms", result, elapsed);
    }

    jclass cls_search_result = (*env)->FindClass(env, "com/frazieje/findinpi/model/SearchResult");
    jmethodID cnstr_search_result = (*env)->GetMethodID(env, cls_search_result, "<init>", "(ZJJLjava/lang/String;)V");
    jobject obj_result = (*env)->NewObject(env, cls_search_result, cnstr_search_result, found, result, elapsed, NULL);

    fflush(stdout);

    return obj_result;
}
