#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "com_frazieje_findinpi_service_NativePiFinder.h"

unsigned long long searchtext(char *fname, char *str, int chunk_size) {
    FILE *fp;
    unsigned long long chunk_num = 1;
    unsigned long long find_result = -1;
    char *temp = malloc(chunk_size);

    if (temp == NULL) {
        perror("Error allocating search buffer");
        return(-1);
    }

    char *loc;

    if((fp = fopen(fname, "r")) == NULL) {
        free(temp);
        char err[] = "Error opening pi data file (%s)";
        char errMsg[strlen(err)-2+strlen(fname)+1];
        sprintf(errMsg, err, fname);
        perror(errMsg);
    	return(-1);
    }

    while(fgets(temp, chunk_size, fp) != NULL) {
        loc = strstr(temp, str);
        if(loc != NULL) {
            find_result = ((chunk_num - 1) * (chunk_size - 1) + (loc - temp));
            break;
        }
        chunk_num++;
    }

    if(find_result == -1) {
        printf("Sorry, couldn't find a match.\n");
    }

    //Close the file if still open.
    if(fp) {
        fclose(fp);
    }

    free(temp);

    return find_result;
}

int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
    return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

JNIEXPORT jobject JNICALL Java_com_frazieje_findinpi_service_NativePiFinder_search
        (JNIEnv * env, jobject thisObj, jstring filePath, jstring searchText, jint bufferSize) {

    char *search_string, *file_path;
    unsigned long long result = -1;

    unsigned char found;

    struct timeval tval_before, tval_after, tval_result;
    int64_t elapsed;

    file_path = ((char *)((*env)->GetStringUTFChars(env, filePath, 0)));
    search_string = ((char *)((*env)->GetStringUTFChars(env, searchText, 0)));

    printf("file_path = %s, search = %s\n", file_path, search_string);

    gettimeofday(&tval_before, NULL);
    result = searchtext(file_path, search_string, (int)bufferSize);
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);

    elapsed = (tval_result.tv_sec*1000000 + tval_result.tv_usec) / 1000;

    if (result == -1) {
        found = (unsigned char)0;
    } else {
        found = (unsigned char)1;
    }

    printf("%llu location found", result);

    jclass cls_search_result = (*env)->FindClass(env, "com/frazieje/findinpi/model/SearchResult");
    jmethodID cnstr_search_result = (*env)->GetMethodID(env, cls_search_result, "<init>", "(ZJJ)V");
    jobject obj_result = (*env)->NewObject(env, cls_search_result, cnstr_search_result, found, result, elapsed);

    return obj_result;
}
