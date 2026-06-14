//CourseWork.cpp (实现"课程"实体的业务操作函数)
#define _CRT_SECURE_NO_WARNINGS
#pragma execution_character_set("utf-8")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CourseWork.h"

//业务1：创建课程链表（预置10门课程）
void createCourse(CourseNode*& LC) {
    LC = NULL;  //初始化链表为空

    //课程数据：编号和名称（按用户要求）
    int ids[]   = {101, 102, 103, 104, 105, 106, 107, 108, 109, 110};
    const char* names[] = {"高等数学", "大学英语", "线性代数", "计算机导论",
                          "C语言", "数据结构", "数据库系统", "操作系统",
                          "Java语言", "单片机"};
    int n = 10;

    //尾插法建立链表
    CourseNode* tail = NULL;
    for (int i = 0; i < n; i++) {
        CourseNode* p = (CourseNode*)malloc(sizeof(CourseNode));
        p->cId = ids[i];
        strcpy(p->cName, names[i]);
        p->cNext = NULL;

        if (LC == NULL) {
            LC = p;
            tail = p;
        } else {
            tail->cNext = p;
            tail = p;
        }
    }
}

//业务2：显示全部课程
void displayCourse(CourseNode* LC) {
    if (LC == NULL) {
        printf("课程链表为空！\n");
        return;
    }
    printf("%-10s %-20s\n", "课程编号", "课程名称");
    printf("----------------------------------\n");
    CourseNode* p = LC;
    while (p != NULL) {
        printf("%-10d %-20s\n", p->cId, p->cName);
        p = p->cNext;
    }
}

//业务3：根据课程编号取得课程名称
bool getCourseName(CourseNode* LC, int cId, char cName[]) {
    CourseNode* p = locateCourse(LC, cId);
    if (p == NULL) return false;
    strcpy(cName, p->cName);
    return true;
}

//辅助业务1：根据课程编号定位课程
CourseNode* locateCourse(CourseNode* LC, int cId) {
    CourseNode* p = LC;
    while (p != NULL) {
        if (p->cId == cId) return p;
        p = p->cNext;
    }
    return NULL;
}

//辅助业务2：统计课程数
int getCourseNumber(CourseNode* LC) {
    int count = 0;
    CourseNode* p = LC;
    while (p != NULL) {
        count++;
        p = p->cNext;
    }
    return count;
}

//辅助业务3：取得所有课程的课程号（返回课程数）
bool getCourseIdsAndNames(CourseNode* LC, int cIds[]) {
    if (LC == NULL) return false;
    int i = 0;
    CourseNode* p = LC;
    while (p != NULL) {
        cIds[i++] = p->cId;
        p = p->cNext;
    }
    return true;
}

//业务4：添加一门课程（尾插法，课程编号不可重复）
bool addCourse(CourseNode*& LC, int cId, const char cName[]) {
    //检查课程编号是否已存在
    if (locateCourse(LC, cId) != NULL) return false;

    CourseNode* p = (CourseNode*)malloc(sizeof(CourseNode));
    if (!p) return false;
    p->cId = cId;
    strncpy(p->cName, cName, 19); p->cName[19] = '\0';
    p->cNext = NULL;

    //空链表，直接作为头节点
    if (LC == NULL) {
        LC = p;
    } else {
        //找到尾节点，尾插
        CourseNode* tail = LC;
        while (tail->cNext != NULL) tail = tail->cNext;
        tail->cNext = p;
    }
    return true;
}

//业务5：删除一门课程（按课程编号）
bool deleteCourse(CourseNode*& LC, int cId) {
    if (LC == NULL) return false;

    //待删节点是头节点
    if (LC->cId == cId) {
        CourseNode* tmp = LC;
        LC = LC->cNext;
        free(tmp);
        return true;
    }

    //待删节点在链表中间或尾部
    CourseNode* prev = LC;
    while (prev->cNext != NULL && prev->cNext->cId != cId) {
        prev = prev->cNext;
    }
    if (prev->cNext == NULL) return false; //未找到

    CourseNode* tmp = prev->cNext;
    prev->cNext = tmp->cNext;
    free(tmp);
    return true;
}