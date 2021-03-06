﻿#ifndef _H_JSON
#define _H_JSON

#include <math.h>
#include <float.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//-------------------------------------json prevs begin-----------------------------------

#ifndef _H_STREXT
#define _H_STREXT

//
// str_cmpi - 字符串不区分大小写比较函数
// ls       : 左串
// rs       : 右串
// return   : ls > rs 返回 > 0; ... < 0; ... =0
//
extern int str_cmpi(const char * ls, const char * rs);

//
// str_cmpin - 字符串不区分小写的限定字符比较函数
// ls       : 左串
// rs       : 右串
// n        : 长度
// return   : ls > rs 返回 > 0; ... < 0; ... =0
//
extern int str_cmpin(const char * ls, const char * rs, size_t n);

//
// str_dup - 字符串拷贝malloc函数, 需要自己free
// str      : 待拷贝的串
// return   : 返回拷贝后的串
//
extern char * str_dup(const char * str);

//
// str_freads - 简单的文件读取类,会读取完毕这个文件内容返回, 需要自己free
// path     : 文件路径
// return   : 创建好的字符串内容, 返回NULL表示读取失败
//
extern char * str_freads(const char * path);

#endif//_H_STREXT

#ifndef _H_TSTR
#define _H_TSTR

#ifndef _STRUCT_TSTR

struct tstr {
    size_t len;   // 长度
    size_t cap;   // 容量
    char * str;   // 字符池
};

typedef struct tstr * tstr_t;

//
// TSTR_CREATE - 栈上创建tstr_t结构
// TSTR_DELETE - 释放栈上tstr_t结构
// var  : 变量名
//
#define TSTR_CREATE(var) \
struct tstr var[1] = { { 0, 0, NULL } }
#define TSTR_DELETE(var) \
free((var)->str)

#define _STRUCT_TSTR
#endif//_STRUCT_TSTR

//
// 向 tstr_t 串结构中添加字符等, 内存分配失败内部会自己处理
// str      : 添加的c串
// sz       : 添加串的长度
//
extern void tstr_appends(tstr_t tsr, const char * str);
extern void tstr_appendn(tstr_t tsr, const char * str, size_t sz);

//
// tstr_cstr - 通过cstr_t串得到一个c的串以'\0'结尾
// tsr      : tstr_t 串
// return   : 返回构建好的c的串, 内存地址 tsr->str
//
extern char * tstr_cstr(tstr_t tsr);

//
// tstr_expand - 为当前字符串扩容, 属于低级api
// tsr      : 可变字符串
// len      : 扩容的长度
// return   : tsr->str + tsr->len 位置的串
//
char * tstr_expand(tstr_t tsr, size_t len);

#endif//_H_TSTR

//-------------------------------------json prevs end-------------------------------------

//
// c json fast parse, type is all design
//

#ifndef _STRUCT_JSON

#define JSON_NULL      (0u << 0)
#define JSON_TRUE      (1u << 0)
#define JSON_FALSE     (1u << 1)
#define JSON_NUMBER    (1u << 2)
#define JSON_STRING    (1u << 3)
#define JSON_OBJECT    (1u << 4)
#define JSON_ARRAY     (1u << 5)

struct json {
    unsigned char type;         // CJSON_NULL - JSON_ARRAY
    struct json * next;         // type = OBJECT or ARRAY 结点采用链表链接
    struct json * chid;         // type = OBJECT or ARRAY 时候 chid 结点就有数据

    char * keys;                // json 结点的 key 名称串
    union {
        char * vals;            // type == STRING 就是个 字符串
        double vald;            // type == NUMBER 就是个 number
    };
};

//
// 定义 json 对象类型
//
typedef struct json * json_t;

//
// json_vali - 得到结点的 int 值
// item     : 处理的结点
//          : 返回 number int 值
//
#define json_vali(item) ((int)(item)->vald)

#define _STRUCT_JSON
#endif//_STRUCT_JSON

//
// json_delete - json 对象销毁
// c        : json 对象
// return   : void
//
extern void json_delete(json_t c);

//
// json_len - 获取当前 json 数组长度
// arr      : 待处理的 json_t 数组
// return   : 返回对象中长度
//
extern int json_len(json_t arr);

//
// json_array - 获取 array 数组中子结点
// arr      : 待处理的 json_t 数组
// i        : [0, len(array)) 索引
// return   : 返回查询到数组结点
//
extern json_t json_array(json_t arr, int i);

//
// json_object - 获取 object 对象中子对象 
// obj      : 待处理的 json_t 对象
// k        : 待查询 key
// return   : 返回查询到对象结点
//
extern json_t json_object(json_t obj, const char * k);

//-------------------------------------json parse begin-----------------------------------

//
// low level api
// json_mini - json 清洗函数
// json_parse - json 解析函数
//
size_t json_mini(char * str);
json_t json_parse(const char * str);

//
// json_file - 通过 path   路径构造 json 对象
// json_create  - 通过 char * 对象构造 json 对象
// str      : const char * 字符串
// path     : 文件路径
// return   : json_t 对象
//
extern json_t json_file(const char * path);
extern json_t json_create(const char * str);

//-------------------------------------json parse end-------------------------------------

//-------------------------------------json print begin-----------------------------------

//
// json_print - 获取 json 对象的打印字符串
// c        : json_t 对象
// return   : 返回生成的 json 字符串, 需要自行 free
//
extern char * json_print(json_t c);

//-------------------------------------json print end-------------------------------------

//-------------------------------------json utils begin-----------------------------------

// json_new - 构造 json 对象
inline json_t json_new(void) {
    return calloc(1, sizeof(struct json));
}

//
// json_new_xxxx - 创建对应的对象
// t        : 创建对象类型
// return   : json_t 创建好的对象
//
inline json_t json_new_type(unsigned char t) {
    json_t item = json_new();
    item->type = t;
    return item;
}

inline json_t json_new_null(void) {
    return json_new();
}

inline json_t json_new_bool(bool v) {
    json_t item = json_new();
    item->type = v ? JSON_TRUE : JSON_FALSE;
    item->vald = v ;
    return item;
}

inline json_t json_new_number(double v) {
    json_t item = json_new_type(JSON_NUMBER);
    item->vald = v;
    return item;
}

inline json_t json_new_string(const char * v) {
    json_t item = json_new_type(JSON_STRING);
    item->vals = str_dup(v);
    return item;
}

inline json_t json_new_object(void) {
    return json_new_type(JSON_OBJECT);
}

inline json_t json_new_array(void) {
    return json_new_type(JSON_ARRAY);
}

//
// json_new_arrays - 通过类型创建类型数组 json 对象
// t        : 类型宏
// arr      : 源数组对象
// n        : 源数组长度
// return   : 返回创建好的数组 json 对象
//
extern json_t json_new_arrays(unsigned char t, const void * arr, int n);

//
// json_detach_xxxxx - 通过索引分离出 json 子 json 对象
// arr      : 待处理的 json_t 数组
// i        : [0, len(array)) 索引
// obj      : 待处理的 json_t 对象
// k        : 待查询 key
// return   : 分离后的对象
//
extern json_t json_detach_array(json_t arr, int i);
extern json_t json_detach_object(json_t obj, const char * k);

//-------------------------------------json utils end-------------------------------------

#endif//_H_JSON
