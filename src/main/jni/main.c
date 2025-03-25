#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "femto.h"
#include "com_frazieje_findinpi_service_NativePiFinder.h"

femto_server_t femto_server;
char *femto_index_path;

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
    jstring dataFilePath
) {
    femto_index_path = ((char *)((*env)->GetStringUTFChars(env, dataFilePath, 0)));

    const int rc = femto_start_server(&femto_server);
    if (rc != 0) {
        perror("femto_start_server");
        return;
    }

    printf("Started femto server. data file: %s\n", femto_index_path);
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

JNIEXPORT jobject JNICALL Java_com_frazieje_findinpi_service_NativePiFinder_searchInternal(
    JNIEnv *env,
    jobject thisObj,
    jstring searchText,
    jint maxResultCount
) {
    char *search_string;
    unsigned long long result = -1;

    struct timeval tval_before, tval_after, tval_result;
    int64_t elapsed;

    search_string = ((char *)((*env)->GetStringUTFChars(env, searchText, 0)));

    char *search_result;

    gettimeofday(&tval_before, NULL);
    result = femto_do_request(search_string, (int)maxResultCount, &search_result);
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);

    elapsed = (tval_result.tv_sec*1000000 + tval_result.tv_usec) / 1000;

    printf("search result returned in %lldms\n", elapsed);

    jclass cls_native_result = (*env)->FindClass(env, "com/frazieje/findinpi/service/NativeResult");
    jmethodID cnstr_native_result = (*env)->GetMethodID(env, cls_native_result, "<init>", "(Ljava/lang/String;J)V");
    jstring native_result_str = (*env)->NewStringUTF(env, search_result);
    jobject obj_result = (*env)->NewObject(env, cls_native_result, cnstr_native_result, native_result_str, elapsed);

    fflush(stdout);
    free(search_result);
    return obj_result;
}
