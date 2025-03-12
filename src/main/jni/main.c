#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "com_frazieje_findinpi_service_NativePiFinder.h"


int64_t MAX(int64_t a, int64_t b) { return((a) > (b) ? a : b); }

int64_t MIN(int64_t a, int64_t b) { return((a) < (b) ? a : b); }

unsigned long long searchtext(char *fname, char *str, int chunk_size, long int offset, long long length, JNIEnv * env, jobject isActiveKFunc) {
    FILE *fp;
    long long offset_count = 0;
    long chunk_count = 0;
    long long find_result = -1;
    char *temp = malloc(chunk_size);
    int64_t totalElapsed = 0;
    int64_t chunk_time;
    int64_t avg_chunk_time;

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

    int seek_result = fseek(fp, offset, SEEK_CUR);

    if (seek_result != 0) {
        free(temp);
        perror("Error setting file offset");
        return(-1);
    }

    int search_len = strlen(str);
    int len;

    jclass cls_kfunc0 = (*env)->GetObjectClass(env, isActiveKFunc);
    jmethodID cls_kfunc0_invoke = (*env)->GetMethodID(env, cls_kfunc0, "invoke", "()Ljava/lang/Object;");
    jclass boolClass = (*env)->FindClass(env, "java/lang/Boolean");
    jmethodID booleanValueMID = (*env)->GetMethodID(env, boolClass, "booleanValue", "()Z");

    jobject booleanObject;
    jboolean result;
    unsigned char is_active = 1;

    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);

    while(is_active && fgets(temp, MIN(length - offset_count, (long long)chunk_size), fp) != NULL && length - offset_count >= search_len) {
        loc = strstr(temp, str);
        len = strlen(temp);
        if(loc != NULL) {
            find_result = offset + offset_count + (loc - temp);
            break;
        }
        offset_count += len;
        booleanObject = (jobject)((*env)->CallObjectMethod(env, isActiveKFunc, cls_kfunc0_invoke));
        result = (jboolean)(*env)->CallBooleanMethod(env, booleanObject, booleanValueMID);
        is_active = (unsigned char)result;
        gettimeofday(&tval_after, NULL);
        timersub(&tval_after, &tval_before, &tval_result);
        chunk_time = (tval_result.tv_sec*1000000 + tval_result.tv_usec);
        totalElapsed += chunk_time;
        chunk_count++;
        fflush(stdout);
        gettimeofday(&tval_before, NULL);
    }

    avg_chunk_time = totalElapsed / chunk_count;

    printf("avg chunk time %lld us\n", avg_chunk_time);
    fflush(stdout);

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

JNIEXPORT jobject JNICALL Java_com_frazieje_findinpi_service_NativePiFinder_search(
    JNIEnv * env,
    jobject thisObj,
    jstring filePath,
    jstring searchText,
    jlong bufferSize,
    jlong offset,
    jlong length,
    jobject isActiveKFunc
) {

    char *search_string, *file_path;
    unsigned long long result = -1;

    unsigned char found;

    struct timeval tval_before, tval_after, tval_result;
    int64_t elapsed;

    file_path = ((char *)((*env)->GetStringUTFChars(env, filePath, 0)));
    search_string = ((char *)((*env)->GetStringUTFChars(env, searchText, 0)));

    printf("file_path = %s, search = %s\n", file_path, search_string);

    gettimeofday(&tval_before, NULL);
    result = searchtext(file_path, search_string, (int)bufferSize, offset, length, env, isActiveKFunc);
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);

    elapsed = (tval_result.tv_sec*1000000 + tval_result.tv_usec) / 1000;

    if (result == -1) {
        found = (unsigned char)0;
    } else {
        found = (unsigned char)1;
    }

    if (found) {
        printf("%llu location found", result);
    }

    jclass cls_search_result = (*env)->FindClass(env, "com/frazieje/findinpi/model/SearchResult");
    jmethodID cnstr_search_result = (*env)->GetMethodID(env, cls_search_result, "<init>", "(ZJJLjava/lang/String;)V");
    jobject obj_result = (*env)->NewObject(env, cls_search_result, cnstr_search_result, found, result, elapsed, NULL);

    fflush(stdout);

    return obj_result;
}
