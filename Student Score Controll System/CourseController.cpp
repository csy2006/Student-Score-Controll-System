//CourseController.cpp (实现"课程"实体的流程控制及视图函数)
#define _CRT_SECURE_NO_WARNINGS
#pragma execution_character_set("utf-8")
#include <stdio.h>
#include "CourseController.h"
#include "CourseWork.h"

//调用业务1：创建课程链表
void toCreateCourse(CourseNode*& LC) {
    printf("\n创建课程链\n");
    createCourse(LC);
    printf("课程链表创建成功，预置 %d 门课程：\n", getCourseNumber(LC));
    displayCourse(LC);
}

//调用业务2：显示全部课程
void toDisplayCourse(CourseNode* LC) {
    printf("\n课程列表\n");
    displayCourse(LC);
}

//调用业务3：根据课程编号取得课程名称
void toGetCourseName(CourseNode* LC) {
    printf("\n查询课程名称\n");
    int cId;
    printf("请输入课程编号：");
    scanf("%d", &cId);

    char cName[20];
    if (getCourseName(LC, cId, cName)) {
        printf("课程编号 %d 对应课程名称：%s\n", cId, cName);
    } else {
        printf("未找到课程编号 %d 对应的课程。\n", cId);
    }
}

//调用业务4：添加一门课程
void toAddCourse(CourseNode*& LC) {
    printf("\n添加课程\n");
    int cId;
    char cName[20];
    printf("请输入课程编号：");
    scanf("%d", &cId);
    printf("请输入课程名称：");
    scanf("%s", cName);

    if (addCourse(LC, cId, cName)) {
        printf("课程添加成功！\n");
    } else {
        printf("课程添加失败（编号可能已存在）。\n");
    }
    displayCourse(LC);
}

//调用业务5：删除一门课程
void toDeleteCourse(CourseNode*& LC) {
    printf("\n删除课程\n");
    int cId;
    printf("请输入要删除的课程编号：");
    scanf("%d", &cId);

    if (deleteCourse(LC, cId)) {
        printf("课程删除成功！\n");
    } else {
        printf("未找到该课程编号。\n");
    }
    displayCourse(LC);
}
