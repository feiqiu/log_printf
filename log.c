#define  log_inf(fmt, ...) printf("\033[1;37m%s %04u "fmt"\033[m", __FILE__, __LINE__, ##__VA_ARGS__)
#define  log_imp(fmt, ...) printf("\033[0;32;32m%s %04u "fmt"\033[m", __FILE__, __LINE__, ##__VA_ARGS__)
#define  log_wrn(fmt, ...) printf("\033[0;35m%s %04u "fmt"\033[m", __FILE__, __LINE__, ##__VA_ARGS__)
#define  log_err(fmt, ...) printf("\033[0;32;31m%s %04u "fmt"\033[m", __FILE__, __LINE__, ##__VA_ARGS__)














log.h

/*
 * log.h
 *
 *  Created on: 2015-5-19
 *      Author: wang.guilin
 */
 
#ifndef SOURCE_LIB_BASE_LOG_H_
#define SOURCE_LIB_BASE_LOG_H_
#include <stdio.h>
#include <stdint.h>
 
#define COLOR_N         "\033[m"
#define COLOR_R         "\033[0;32;31m"
#define COLOR_G         "\033[0;32;32m"
#define COLOR_P         "\033[0;35m"
#define COLOR_W         "\033[1;37m"
 
typedef enum {
    log_lv_ini,
    log_lv_non,
    log_lv_inf,
    log_lv_imp,
    log_lv_wrn,
    log_lv_err
}log_level_t;
 
 
#ifdef __cplusplus
extern "C" {
#endif
int print_log(log_level_t lv,FILE* out, const char* color, const char* flag, const char* file, uint32_t line, const char* fmt, ...);
#ifdef __cplusplus
}
#endif
 
 
 
#define LOG_PRINT_INFO(...) print_log(log_lv_inf,stderr, COLOR_W, "INFO", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_PRINT_IMPT(...) print_log(log_lv_imp,stderr, COLOR_G, "IMPT", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_PRINT_WARN(...) print_log(log_lv_wrn,stderr, COLOR_P, "WARN", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_PRINT_EROR(...) print_log(log_lv_err,stderr, COLOR_R, "EROR", __FILE__, __LINE__, __VA_ARGS__)
 
#define log_inf(...) print_log(log_lv_inf,stderr, COLOR_W, "INFO", __FILE__, __LINE__, __VA_ARGS__)
#define log_imp(...) print_log(log_lv_imp,stderr, COLOR_G, "IMPT", __FILE__, __LINE__, __VA_ARGS__)
#define log_wrn(...) print_log(log_lv_wrn,stderr, COLOR_P, "WARN", __FILE__, __LINE__, __VA_ARGS__)
#define log_err(...) print_log(log_lv_err,stderr, COLOR_R, "EROR", __FILE__, __LINE__, __VA_ARGS__)
 
 
 
 
#endif /* SOURCE_LIB_BASE_LOG_H_ */






log.c
/*
 * log.c
 *
 *  Created on: 2015-5-19
 *      Author: wang.guilin
 */
 
#include <libgen.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"
#include <sys/time.h>
#include <time.h>
 
static char* log_time(char* dst, size_t len)
{
    struct tm p = { 0 };
    struct timeval tv = { 0 };
 
    gettimeofday(&tv, NULL);
 
    localtime_r((const time_t *) &tv.tv_sec, &p);
 
    snprintf(dst, len, "%04d-%02d-%02d %02d:%02d:%02d %08ld", p.tm_year + 1900, p.tm_mon + 1, p.tm_mday, p.tm_hour, p.tm_min,
             p.tm_sec, tv.tv_usec);
    return dst;
}
 
log_level_t get_log_level(){
    char *env_str=NULL;
    env_str = getenv("GLOG_LEVEL");
    if(env_str == NULL){
        return log_lv_imp;
    }
     
    if(strcmp(env_str,"inf")==0){
        return log_lv_inf;
    }
     
    if(strcmp(env_str,"imp")==0){
        return log_lv_imp;
    }
     
    if(strcmp(env_str,"wrn")==0){
        return log_lv_wrn;
    }
     
    if(strcmp(env_str,"err")==0){
        return log_lv_err;
    }
     
    return log_lv_imp;
}
 
int print_log(log_level_t lv,FILE* out, const char* color, const char* flag, const char* file, uint32_t line, const char* fmt, ...)
{
    int n = 0;
    va_list va;
    char buffer[128] = "";
    char path[260] = "";
    static char *file_filter = NULL;
    static log_level_t level = log_lv_ini;
    if(level == log_lv_ini){
        level = get_log_level();
    }
 
    if(file_filter == NULL){
        file_filter = getenv("GLOG_FILE");
        if(file_filter == NULL){
            file_filter = "";
        }
    }
 
    if(file_filter[0] != '\0'){
        if(strstr(file,file_filter) == NULL){
            return 0;
        }
    }
 
    if(lv<level){
        return 0;
    }
 
    strncpy(path, file, sizeof(path));
    n += fprintf(out, "%s[%s] %-16.16s %04u %s%s ", color, flag, basename(path), line, log_time(buffer, sizeof(buffer)),
                 COLOR_N);
    va_start(va, fmt);
    n += vfprintf(out, fmt, va);
    va_end(va);
    return n;
}
