//main.cpp - 学生成绩管理系统主函数（HTTP服务器模式）
#define _CRT_SECURE_NO_WARNINGS
#pragma execution_character_set("utf-8")
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
extern "C" __declspec(dllimport) int __stdcall SetConsoleOutputCP(unsigned int);
#include "DataType.h"
#include "CourseController.h"
#include "StudentController.h"
#include "ScoreController.h"
#include "HttpServer.h"
#include "StudentWork.h"
#include "ScoreWork.h"

// 全局数据
CourseNode*  LC = NULL;   // 课程链表头指针
StudentNode* L  = NULL;   // 学生链表头指针

int main() {
    SetConsoleOutputCP(65001);

    // 创建课程和初始化学生数据
    printf("Young__Yang的学生成绩管理系统\n");
    toCreateCourse(LC);
    toInitStudent(L);

    // 初始化样本成绩所有学生所有课程
    printf("正在初始化样本成绩（所有学生 x 所有课程）...\n");
    initScores(L, LC);
    printf("样本成绩初始化完成。\n");

    // 启动HTTP服务器（默认端口8888）
    HttpServer server;
    server.setData(LC, L);
    server.start(8888);

    // 释放内存（服务器退出后执行）
    if (L != NULL) destroyStudent(L);
    CourseNode* cp = LC;
    while (cp != NULL) {
        CourseNode* tmp = cp->cNext;
        free(cp);
        cp = tmp;
    }
    return 0;
}