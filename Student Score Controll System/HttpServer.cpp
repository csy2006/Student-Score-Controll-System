//HttpServer.cpp - HTTP服务器实现（基于cpp-httplib）
#define _CRT_SECURE_NO_WARNINGS
#pragma execution_character_set("utf-8")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "kernel32.lib")  // SetConsoleOutputCP 所在库
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
extern "C" __declspec(dllimport) int __stdcall SetConsoleOutputCP(unsigned int);
#include "httplib.h"
#include "HttpServer.h"
#include "CourseWork.h"
#include "StudentWork.h"
#include "ScoreWork.h"
#include "StudentController.h"
#include "CourseController.h"

// 简易JSON解析
//从json字符串中提取字符串字段值并返回新分配字符串
char* HttpServer::getJsonString(const char* json, const char* key) {
    if (!json || !key) return NULL;
    char pattern[128];
    sprintf(pattern, "\"%s\"", key);
    const char* p = strstr(json, pattern);
    if (!p) return NULL;
    p += strlen(pattern);
    //跳过点号和空格
    while (*p && *p == ' ') 
        p++;
    if (*p != ':') 
        return NULL;
    p++;
    while (*p && *p == ' ') 
        p++;
    if (*p != '\"') 
        return NULL;
    p++;
    //找到结束引号
    const char* end = p;
    while (*end && *end != '\"') {
            if (*end == '\\') end++;  //跳过转义
            end++;
        }
    if (*end != '\"') return NULL;
    int len = (int)(end - p);
    char* result = (char*)malloc(len + 1);
    strncpy(result, p, len);
    result[len] = '\0';
    return result;
}

//从json字符串中提取整数字段值
int HttpServer::getJsonInt(const char* json, const char* key) {
    if (!json || !key) return -1;
    char pattern[128];
    sprintf(pattern, "\"%s\"", key);
    const char* p = strstr(json, pattern);
    if (!p) 
        return -1;
    p += strlen(pattern);
    while (*p && *p == ' ') 
        p++;
    if (*p != ':') 
        return -1;
    p++;
    while (*p && *p == ' ') 
        p++;
    //解析整数
    int sign = 1;
    if (*p == '-') { sign = -1; p++; }
    int val = 0;
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        p++;
    }
    return sign * val;
}

// 构建课程列表JSON响应
void HttpServer::buildCoursesJson(char* out, int outSize) {
    CourseNode* LC = getLC();
    char* p = out;
    int remain = outSize - 1;
    int n = snprintf(p, remain + 1, "{\"courses\":[");
    p += n; remain -= n;

    CourseNode* cp = LC;
    int first = 1;
    while (cp != NULL && remain > 0) {
		if (!first) { n = snprintf(p, remain + 1, ","); p += n; remain -= n; }  //课程之间添加逗号
        first = 0;
		n = snprintf(p, remain + 1, "{\"cId\":%d,\"cName\":\"%s\"}", cp->cId, cp->cName);   //构建课程JSON对象
        p += n; remain -= n;
        cp = cp->cNext;
    }
    if (remain > 0) { n = snprintf(p, remain + 1, "]}"); p += n; remain -= n; }
}

// 构建学生列表JSON响应
void HttpServer::buildStudentsJson(char* out, int outSize) {
    StudentNode* L = getL();
    char* p = out;
    int remain = outSize - 1;
    int n = snprintf(p, remain + 1, "{\"students\":[");
    p += n; remain -= n;

    StudentNode* sp = L;
    int first = 1;
    while (sp != NULL && remain > 0) {
        if (!first) { n = snprintf(p, remain + 1, ","); p += n; remain -= n; }
        first = 0;
        char sexStr[8];
        if (sp->stu.sex == 'M') strcpy(sexStr, "男");
        else if (sp->stu.sex == 'F') strcpy(sexStr, "女");
        else { sexStr[0] = sp->stu.sex; sexStr[1] = '\0'; }
        float avg = calcAvgScore(sp->stu.LS);
        if (avg >= 0) {
            n = snprintf(p, remain + 1, "{\"id\":%d,\"name\":\"%s\",\"sex\":\"%s\",\"className\":\"%s\",\"avg\":%.1f}",
                         sp->stu.id, sp->stu.name, sexStr, sp->stu.className, avg);
        } else {
            n = snprintf(p, remain + 1, "{\"id\":%d,\"name\":\"%s\",\"sex\":\"%s\",\"className\":\"%s\",\"avg\":null}",
                         sp->stu.id, sp->stu.name, sexStr, sp->stu.className);
        }
        p += n; remain -= n;
        sp = sp->next;
    }
    if (remain > 0) { n = snprintf(p, remain + 1, "]}"); p += n; remain -= n; }
}

void HttpServer::buildScoresJson(char* out, int outSize) {
    StudentNode* L = getL();
    CourseNode* LC = getLC();
    char* p = out;
    int remain = outSize - 1;
    int n = snprintf(p, remain + 1, "{\"scores\":[");
    p += n; remain -= n;

    StudentNode* sp = L;
    int first = 1;
    while (sp != NULL && remain > 0) {
        if (!first) { n = snprintf(p, remain + 1, ","); p += n; remain -= n; }
        first = 0;
        n = snprintf(p, remain + 1, "{\"id\":%d,\"name\":\"%s\",\"className\":\"%s\"",
                     sp->stu.id, sp->stu.name, sp->stu.className);
        p += n; remain -= n;

        //遍历课程添加成绩
        CourseNode* cp = LC;
        while (cp != NULL && remain > 0) {
            ScoreNode* sc = locateStudentScore(sp->stu, cp->cId);
            if (sc != NULL) {
                n = snprintf(p, remain + 1, ",\"%d\":%d", cp->cId, sc->score);
            } else {
                n = snprintf(p, remain + 1, ",\"%d\":null", cp->cId);
            }
            p += n; remain -= n;
            cp = cp->cNext;
        }
        //平均分
        float avg = calcAvgScore(sp->stu.LS);
        if (avg >= 0) {
            n = snprintf(p, remain + 1, ",\"avg\":%.1f", avg);
        } else {
            n = snprintf(p, remain + 1, ",\"avg\":null");
        }
        p += n; remain -= n;

        // 修复 snprintf 后必须更新 p 和 remain
        if (remain > 0) { n = snprintf(p, remain + 1, "}"); p += n; remain -= n; }
        sp = sp->next;
    }
    if (remain > 0) { n = snprintf(p, remain + 1, "]}"); p += n; remain -= n; }
}

void HttpServer::buildStudentScoresJson(char* out, int outSize, int studentId) {
    StudentNode* L = getL();
    CourseNode* LC = getLC();
    StudentNode* sn = locateStudent(L, studentId);
    if (!sn) {
        snprintf(out, outSize, "{\"error\":\"学生不存在\"}");
        return;
    }
    char* p = out;
    int remain = outSize - 1;
    char sexStr[8];
    if (sn->stu.sex == 'M') strcpy(sexStr, "男");
    else if (sn->stu.sex == 'F') strcpy(sexStr, "女");
    else { sexStr[0] = sn->stu.sex; sexStr[1] = '\0'; }
    int n = snprintf(p, remain + 1, "{\"id\":%d,\"name\":\"%s\",\"sex\":\"%s\",\"className\":\"%s\",\"scores\":{",
                     sn->stu.id, sn->stu.name, sexStr, sn->stu.className);
    p += n; remain -= n;

    CourseNode* cp = LC;
    int first = 1;
    while (cp != NULL && remain > 0) {
        if (!first) { n = snprintf(p, remain + 1, ","); p += n; remain -= n; }
        first = 0;
        ScoreNode* sc = locateStudentScore(sn->stu, cp->cId);
        if (sc != NULL) {
            n = snprintf(p, remain + 1, "\"%d\":{\"score\":%d,\"cName\":\"%s\"}", cp->cId, sc->score, cp->cName);
        } else {
            n = snprintf(p, remain + 1, "\"%d\":null", cp->cId);
        }
        p += n; remain -= n;
        cp = cp->cNext;
    }
    float avg = calcAvgScore(sn->stu.LS);
    if (avg >= 0) {
        n = snprintf(p, remain + 1, "},\"avg\":%.1f}", avg);
    } else {
        n = snprintf(p, remain + 1, "},\"avg\":null}");
    }
    p += n;
}

float HttpServer::calcAvgScore(ScoreNode* LS) {
    if (!LS) return -1.0f;
    int total = 0, cnt = 0;
    ScoreNode* p = LS;
    while (p) { total += p->score; cnt++; p = p->scNext; }
    return cnt > 0 ? (float)total / cnt : -1.0f;
}

//绩点换算：90-100→4.0, 80-89→3.0, 70-79→2.0, 60-69→1.0, <60→0
float HttpServer::calcGradePoint(int score) {
    if (score >= 90) return 4.0f;
    else if (score >= 80) return 3.0f;
    else if (score >= 70) return 2.0f;
    else if (score >= 60) return 1.0f;
    else return 0.0f;
}

void HttpServer::buildGpaJson(char* out, int outSize) {
    StudentNode* L = getL();
    CourseNode* LC = getLC();
    char* p = out;
    int remain = outSize - 1;
    int n = snprintf(p, remain + 1, "{\"gpa\":[");
    p += n; remain -= n;

    StudentNode* sp = L;
    int first = 1;
    while (sp != NULL && remain > 0) {
        if (!first) { n = snprintf(p, remain + 1, ","); p += n; remain -= n; }
        first = 0;
        float totalGp = 0.0f;
        int cnt = 0;
        ScoreNode* sc = sp->stu.LS;
        while (sc != NULL) {
            totalGp += calcGradePoint(sc->score);
            cnt++;
            sc = sc->scNext;
        }
        float gpa = cnt > 0 ? totalGp / cnt : 0.0f;
        char sexStr[8];
        if (sp->stu.sex == 'M') strcpy(sexStr, "男");
        else if (sp->stu.sex == 'F') strcpy(sexStr, "女");
        else { sexStr[0] = sp->stu.sex; sexStr[1] = '\0'; }
        n = snprintf(p, remain + 1, "{\"id\":%d,\"name\":\"%s\",\"sex\":\"%s\",\"className\":\"%s\",\"gpa\":%.2f,\"courseCount\":%d}",
                     sp->stu.id, sp->stu.name, sexStr, sp->stu.className, gpa, cnt);
        p += n; remain -= n;
        sp = sp->next;
    }
    if (remain > 0) { n = snprintf(p, remain + 1, "]}"); p += n; remain -= n; }
}

void HttpServer::buildFeedbacksJson(char* out, int outSize) {
    char* p = out;
    int remain = outSize - 1;
    int n = snprintf(p, remain + 1, "{\"feedbacks\":[");
    p += n; remain -= n;

    for (int i = 0; i < feedbackCount && remain > 0; i++) {
        if (i > 0) { n = snprintf(p, remain + 1, ","); p += n; remain -= n; }
        n = snprintf(p, remain + 1, "{\"index\":%d,\"cId\":%d,\"rating\":%d,\"comment\":\"%s\"}",
                     i, feedbacks[i].cId, feedbacks[i].rating, feedbacks[i].comment);
        p += n; remain -= n;
    }
    if (remain > 0) { n = snprintf(p, remain + 1, "]}"); p += n; remain -= n; }
}

// 构造函数/析构函数
HttpServer::HttpServer() : svr(NULL), LC_ptr(NULL), L_ptr(NULL), feedbackCount(0) {
    svr = new httplib::Server();
}

HttpServer::~HttpServer() {
    if (svr) delete (httplib::Server*)svr;
}

void HttpServer::setData(CourseNode*& lc, StudentNode*& l) {
    LC_ptr = &lc;
    L_ptr  = &l;
}

// 启动服务器
bool HttpServer::start(int port) {
    // 设置控制台输出为UTF-8，解决运行框中文乱码问题
    SetConsoleOutputCP(65001);

    httplib::Server& server = *(httplib::Server*)svr;

    //CORS 预检
    server.Options(".*", [&](const httplib::Request& req, httplib::Response& res) {
		res.set_header("Access-Control-Allow-Origin", "*"); //允许所有来源
		res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");  //允许的HTTP
        res.set_header("Access-Control-Allow-Headers", "Content-Type"); //
        res.status = 204;
    });

	//API根路径
    // 课程列表
    server.Get("/api/courses", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        char buf[4096];
        buildCoursesJson(buf, 4096);
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 添加课程
    server.Post("/api/courses", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        char* body = (char*)req.body.c_str();
        int cId   = getJsonInt(body, "cId");
        char* cName = getJsonString(body, "cName");
        char buf[512];

        if (cId <= 0 || !cName) {
            snprintf(buf, 512, "{\"success\":false,\"msg\":\"参数不完整\"}");
        } else {
            if (addCourse(*LC_ptr, cId, cName)) {
                snprintf(buf, 512, "{\"success\":true,\"msg\":\"课程添加成功\"}");
            } else {
                snprintf(buf, 512, "{\"success\":false,\"msg\":\"课程编号已存在\"}");
            }
        }
        if (cName) free(cName);
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 删除课程
    server.Delete("/api/courses/([0-9]+)", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        int cId = atoi(req.matches[1].str().c_str());
        char buf[512];
        if (deleteCourse(*LC_ptr, cId)) {
            snprintf(buf, 512, "{\"success\":true,\"msg\":\"课程删除成功\"}");
        } else {
            snprintf(buf, 512, "{\"success\":false,\"msg\":\"课程不存在\"}");
        }
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 学生列表
    server.Get("/api/students", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        char buf[32768];
        buildStudentsJson(buf, 32768);
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 单个学生
    server.Get("/api/students/([0-9]+)", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        int id = atoi(req.matches[1].str().c_str());
        StudentNode* sn = locateStudent(getL(), id);
        char buf[4096];
        if (sn) {
            buildStudentScoresJson(buf, 4096, id);
        } else {
            snprintf(buf, 4096, "{\"error\":\"学生不存在\"}");
        }
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 添加学生
    server.Post("/api/students", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        char* body = (char*)req.body.c_str();
        int id   = getJsonInt(body, "id");
        char* name = getJsonString(body, "name");
        char* sexStr = getJsonString(body, "sex");
        char* className = getJsonString(body, "className");
        char buf[512];

        if (id <= 0 || !name || !className) {
            snprintf(buf, 512, "{\"success\":false,\"msg\":\"参数不完整\"}");
        } else {
            Student stu;
            stu.id = id;
            strncpy(stu.name, name, 19); stu.name[19] = '\0';
            // UTF-8 中文不能用 [0] 比较，需用 strncmp 比较前3字节
            if (sexStr && strncmp(sexStr, "男", 3) == 0) stu.sex = 'M';
            else if (sexStr && strncmp(sexStr, "女", 3) == 0) stu.sex = 'F';
            else stu.sex = sexStr ? sexStr[0] : 'M';
            strncpy(stu.className, className, 19); stu.className[19] = '\0';
            stu.LS = NULL;
            if (addStudent(*L_ptr, stu)) {
                snprintf(buf, 512, "{\"success\":true,\"msg\":\"添加成功\"}");
            } else {
                snprintf(buf, 512, "{\"success\":false,\"msg\":\"学号已存在\"}");
            }
        }
        if (name) free(name);
        if (sexStr) free(sexStr);
        if (className) free(className);
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 修改学生
    server.Put("/api/students/([0-9]+)", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        int id = atoi(req.matches[1].str().c_str());
		//调用getJsonString解析请求体中的字段
        char* body = (char*)req.body.c_str();
        char* name = getJsonString(body, "name");
        char* sexStr = getJsonString(body, "sex");
        char* className = getJsonString(body, "className");
        char buf[512];

        StudentNode* sn = locateStudent(getL(), id);
        if (!sn) {
            snprintf(buf, 512, "{\"success\":false,\"msg\":\"学生不存在\"}");
        } else {
            Student newStu = sn->stu;
            if (name) { strncpy(newStu.name, name, 19); newStu.name[19] = '\0'; }
            if (sexStr) {
                // UTF-8 中文需用 strncmp 比较前3字节
                if (strncmp(sexStr, "男", 3) == 0) newStu.sex = 'M';
                else if (strncmp(sexStr, "女", 3) == 0) newStu.sex = 'F';
                else newStu.sex = sexStr[0];
            }
            if (className) { strncpy(newStu.className, className, 19); newStu.className[19] = '\0'; }
            updateStudent(*L_ptr, sn->stu, newStu);
            snprintf(buf, 512, "{\"success\":true,\"msg\":\"修改成功\"}");
        }
        if (name) free(name);
        if (sexStr) free(sexStr);
        if (className) free(className);
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 删除学生
    server.Delete("/api/students/([0-9]+)", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        int id = atoi(req.matches[1].str().c_str());
        Student stu; stu.id = id;
        char buf[512];
        if (deleteStudent(*L_ptr, stu)) {
            snprintf(buf, 512, "{\"success\":true,\"msg\":\"删除成功\"}");
        } else {
            snprintf(buf, 512, "{\"success\":false,\"msg\":\"学生不存在\"}");
        }
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 成绩列表
    server.Get("/api/scores", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        char buf[65536];
        buildScoresJson(buf, 65536);
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 单个学生成绩
    server.Get("/api/scores/([0-9]+)", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        int id = atoi(req.matches[1].str().c_str());
        char buf[8192];
        buildStudentScoresJson(buf, 8192, id);
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 录入/修改成绩
    server.Post("/api/scores", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        char* body = (char*)req.body.c_str();
        int id   = getJsonInt(body, "id");
        int cId  = getJsonInt(body, "cId");
        int score= getJsonInt(body, "score");
        char buf[512];

        if (id <= 0 || cId <= 0 || score < 0 || score > 150) {
            snprintf(buf, 512, "{\"success\":false,\"msg\":\"参数错误\"}");
        } else {
            //先尝试修改，若成绩不存在则新增
            int ret = updateAScoreDirect(*L_ptr, id, cId, score);
            if (ret == 0) {
                snprintf(buf, 512, "{\"success\":true,\"msg\":\"成绩修改成功\"}");
            } else if (ret == 3) {
                //成绩不存在，新增
                ret = addAScoreDirect(*L_ptr, id, cId, score, getLC());
                if (ret == 0) {
                    snprintf(buf, 512, "{\"success\":true,\"msg\":\"成绩录入成功\"}");
                } else if (ret == 1) {
                    snprintf(buf, 512, "{\"success\":false,\"msg\":\"学生不存在\"}");
                } else if (ret == 2) {
                    snprintf(buf, 512, "{\"success\":false,\"msg\":\"课程不存在\"}");
                }
            } else if (ret == 1) {
                snprintf(buf, 512, "{\"success\":false,\"msg\":\"学生不存在\"}");
            }
        }
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 删除单科成绩
    server.Delete("/api/scores/([0-9]+)/([0-9]+)", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        int id  = atoi(req.matches[1].str().c_str());
        int cId = atoi(req.matches[2].str().c_str());
        int ret = deleteAScoreByStudent(getL(), id, cId, getLC());
        char buf[512];
        if (ret == 0) {
            snprintf(buf, 512, "{\"success\":true,\"msg\":\"删除成功\"}");
        } else if (ret == 1) {
            snprintf(buf, 512, "{\"success\":false,\"msg\":\"学生不存在\"}");
        } else if (ret == 2) {
            snprintf(buf, 512, "{\"success\":false,\"msg\":\"课程不存在\"}");
        } else {
            snprintf(buf, 512, "{\"success\":false,\"msg\":\"成绩不存在\"}");
        }
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 清空某学生全部成绩
    server.Delete("/api/scores/([0-9]+)", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        int id = atoi(req.matches[1].str().c_str());
        char buf[512];
        if (destroyScoreByStudent(*L_ptr, id)) {
            snprintf(buf, 512, "{\"success\":true,\"msg\":\"成绩已清空\"}");
        } else {
            snprintf(buf, 512, "{\"success\":false,\"msg\":\"操作失败\"}");
        }
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 清空所有学生
    server.Delete("/api/students/all", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        destroyStudent(*L_ptr);
        char buf[512];
        snprintf(buf, 512, "{\"success\":true,\"msg\":\"所有学生已清空\"}");
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 重新初始化样本数据
    server.Post("/api/students/init", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        destroyStudent(*L_ptr);
        *L_ptr = NULL;
        toInitStudent(*L_ptr);
        char buf[512];
        snprintf(buf, 512, "{\"success\":true,\"msg\":\"样本数据已重新初始化\"}");
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 清空所有学生成绩
    server.Delete("/api/scores/all", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        printf("[DEBUG] 收到 DELETE /api/scores/all 请求，开始清空所有成绩...\n");
        // 直接操作全局链表头
        StudentNode* p = *L_ptr;
        int stuCount = 0, scoreCount = 0;
        while (p != NULL) {
            stuCount++;
            ScoreNode* sc = p->stu.LS;
            while (sc != NULL) {
                ScoreNode* tmp = sc->scNext;
                scoreCount++;
                free(sc);
                sc = tmp;
            }
            p->stu.LS = NULL;
            p = p->next;
        }
        printf("[DEBUG] 已清空 %d 名学生的 %d 条成绩\n", stuCount, scoreCount);
        char buf[512];
        snprintf(buf, 512, "{\"success\":true,\"msg\":\"所有学生成绩已清空\",\"count\":%d}", scoreCount);
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 重新初始化所有成绩（90% ≥ 60分，10% < 60分）
    server.Post("/api/scores/init", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        // 清空旧成绩
        destroyALLScore(*L_ptr);
        // 调用统一初始化函数
        initScores(*L_ptr, getLC());
        // 统计成绩条数
        int total = 0;
        StudentNode* sp = getL();
        while (sp != NULL) {
            ScoreNode* sc = sp->stu.LS;
            while (sc != NULL) { total++; sc = sc->scNext; }
            sp = sp->next;
        }
        char buf[512];
        snprintf(buf, 512, "{\"success\":true,\"msg\":\"已重新初始化%d条成绩\",\"count\":%d}", total, total);
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 统计
    server.Get("/api/statistics", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        StudentNode* L = getL();
        CourseNode* LC = getLC();
        char buf[32768];
        char* p = buf;
        int remain = 32767;
        int n = snprintf(p, remain + 1, "{\"statistics\":[");
        p += n; remain -= n;
        CourseNode* cp = LC;
        int first = 1;
        while (cp != NULL && remain > 0) {
            if (!first) { n = snprintf(p, remain + 1, ","); p += n; remain -= n; }
            first = 0;
            int total = 0, cnt = 0, max = -1, min = 101;
            StudentNode* sp2 = L;
            while (sp2 != NULL) {
                ScoreNode* sc = locateStudentScore(sp2->stu, cp->cId);
                if (sc != NULL) {
                    total += sc->score; cnt++;
                    if (sc->score > max) max = sc->score;
                    if (sc->score < min) min = sc->score;
                }
                sp2 = sp2->next;
            }
            float avg = cnt > 0 ? (float)total / cnt : 0;
            if (max == -1) max = 0;
            if (min == 101) min = 0;
            n = snprintf(p, remain + 1, "{\"cId\":%d,\"cName\":\"%s\",\"avg\":%.1f,\"max\":%d,\"min\":%d,\"count\":%d}",
                         cp->cId, cp->cName, avg, max, min, cnt);
            p += n; remain -= n;
            cp = cp->cNext;
        }
        if (remain > 0) { n = snprintf(p, remain + 1, "]}"); p += n; remain -= n; }
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 排名
    server.Get("/api/ranking", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        StudentNode* L = getL();
        #define MAX_STU 1000
        typedef struct { int id; char name[20]; char className[20]; float avg; } RankItem;
        RankItem items[MAX_STU];
        int count = 0;
        StudentNode* sp = L;
        while (sp != NULL && count < MAX_STU) {
            items[count].id = sp->stu.id;
            strncpy(items[count].name, sp->stu.name, 19);
            items[count].name[19] = '\0';
            strncpy(items[count].className, sp->stu.className, 19);
            items[count].className[19] = '\0';
            items[count].avg = calcAvgScore(sp->stu.LS);
            count++;
            sp = sp->next;
        }
        for (int i = 0; i < count - 1; i++) {
            for (int j = 0; j < count - i - 1; j++) {
                if (items[j].avg < items[j+1].avg) {
                    RankItem tmp = items[j]; items[j] = items[j+1]; items[j+1] = tmp;
                }
            }
        }
        char buf[32768];
        char* p = buf;
        int remain = 32767;
        int n = snprintf(p, remain + 1, "{\"ranking\":[");
        p += n; remain -= n;
        for (int i = 0; i < count && remain > 0; i++) {
            if (i > 0) { n = snprintf(p, remain + 1, ","); p += n; remain -= n; }
            n = snprintf(p, remain + 1, "{\"rank\":%d,\"id\":%d,\"name\":\"%s\",\"className\":\"%s\",\"avg\":%.1f}",
                         i+1, items[i].id, items[i].name, items[i].className, items[i].avg >= 0 ? items[i].avg : 0);
            p += n; remain -= n;
        }
        if (remain > 0) { n = snprintf(p, remain + 1, "]}"); p += n; remain -= n; }
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // GPA排名
    server.Get("/api/gpa", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        char buf[32768];
        buildGpaJson(buf, 32768);
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 获取所有课程评价
    server.Get("/api/feedback", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        char buf[16384];
        buildFeedbacksJson(buf, 16384);
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 提交课程评价
    server.Post("/api/feedback", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        char* body = (char*)req.body.c_str();
        int cId   = getJsonInt(body, "cId");
        int rating = getJsonInt(body, "rating");
        char* comment = getJsonString(body, "comment");
        char buf[512];

        if (cId <= 0 || rating < 1 || rating > 5 || !comment) {
            snprintf(buf, 512, "{\"success\":false,\"msg\":\"参数不完整或无效\"}");
        } else if (feedbackCount >= MAX_FEEDBACKS) {
            snprintf(buf, 512, "{\"success\":false,\"msg\":\"评价数量已达上限\"}");
        } else {
            feedbacks[feedbackCount].cId = cId;
            feedbacks[feedbackCount].rating = rating;
            strncpy(feedbacks[feedbackCount].comment, comment, 127);
            feedbacks[feedbackCount].comment[127] = '\0';
            feedbackCount++;
            snprintf(buf, 512, "{\"success\":true,\"msg\":\"评价提交成功\"}");
        }
        if (comment) free(comment);
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 删除单条课程评价
    server.Delete(R"(/api/feedback/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        int index = atoi(req.matches[1].str().c_str());
        char buf[512];
        if (index < 0 || index >= feedbackCount) {
            snprintf(buf, 512, "{\"success\":false,\"msg\":\"评价不存在\"}");
        } else {
            // 将后面的元素前移
            for (int i = index; i < feedbackCount - 1; i++) {
                feedbacks[i] = feedbacks[i + 1];
            }
            feedbackCount--;
            snprintf(buf, 512, "{\"success\":true,\"msg\":\"评价删除成功\"}");
        }
        res.set_content(buf, "application/json; charset=utf-8");
    });

    // 静态文件服务（前端）
    server.Get("/$", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_redirect("/index.html");
    });
    server.set_mount_point("/", "C:/Users/20521/Desktop/文件夹/大学/数据结构/项目/Student Corce Controll System/前端");
    server.Get("/(.*)", [&](const httplib::Request& req, httplib::Response& res) {
        // 静态文件由mount_point处理，这里是后备
        res.set_header("Access-Control-Allow-Origin", "*");
        res.status = 404;
        res.set_content("Not Found", "text/plain");
    });

    // 启动服务器
    printf("学生成绩管理系统 HTTP 服务器启动，监听端口 %d...\n", port);
    printf("浏览器访问: http://localhost:%d\n", port);
    return server.listen("0.0.0.0", port);
}
