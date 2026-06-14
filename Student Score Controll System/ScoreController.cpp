//SorceController.cpp (实现"成绩"实体的流程控制及视图函数)
#define _CRT_SECURE_NO_WARNINGS
#pragma execution_character_set("utf-8")
#include <stdio.h>
#include <string.h>
#include "ScoreController.h"
#include "ScoreWork.h"
#include "StudentWork.h"
#include "CourseWork.h"
#include "CourseController.h"

//调用成绩业务1：显示所有学生的成绩
void toDisplayAllScore(StudentNode* L, CourseNode* LC) {
    printf("\n所有学生成绩\n");
    Student tmp = { 0 };
    displayScore(L, tmp, 1, LC);
}

//调用成绩业务1：显示班级学生的成绩
void toDisplayScoreByClassName(StudentNode* L, CourseNode* LC) {
    printf("\n按班级查询成绩\n");
    Student stu;
    printf("请输入班级名称：");
    scanf("%s", stu.className);
    displayScore(L, stu, 2, LC);
}

//调用成绩业务1：显示一个学生的成绩
void toDisplayScoreById(StudentNode* L, CourseNode* LC) {
    printf("\n按学号查询成绩\n");
    int id;
    printf("请输入学号：");
    scanf("%d", &id);

    StudentNode* sNode = locateStudent(L, id);
    if (sNode == NULL) {
        printf("未找到学号 %d 的学生！\n", id);
        return;
    }
    Student stu;
    stu.id = id;
    displayScore(L, stu, 3, LC);
}

//调用成绩业务2：增加一个学生的全部成绩
void toAddScoreByStudent(StudentNode*& L, CourseNode* LC) {
    printf("\n为一个学生录入全部成绩\n");
    int id;
    printf("请输入学号：");
    scanf("%d", &id);
    if (addScoreByStudent(L, id, LC)) {
        printf("录入成功！\n");
    }
}

//调用成绩业务3：增加一门课程的成绩
void toAddScoreByCourse(StudentNode*& L, CourseNode* LC) {
    printf("\n为一门课程录入全部学生成绩\n");
    toDisplayCourse(LC);
    int cId;
    printf("请输入课程编号：");
    scanf("%d", &cId);
    if (addScoreByCourse(L, cId, LC)) {
        printf("录入成功！\n");
    }
}

//调用成绩业务4：增加学生一个课程的成绩
void toAddAScoreByStudent(StudentNode*& L, CourseNode* LC) {
    printf("\n为某学生录入单科成绩\n");
    int id, cId;
    printf("请输入学号：");
    scanf("%d", &id);
    toDisplayCourse(LC);
    printf("请输入课程编号：");
    scanf("%d", &cId);

    int ret = addAScoreByStudent(L, id, cId, LC);
    switch (ret) {
        case 0: printf("录入成功！\n"); break;
        case 1: printf("学生不存在！\n"); break;
        case 2: printf("课程不存在！\n"); break;
        case 3: printf("该成绩已存在，请用修改功能！\n"); break;
    }
}

//调用成绩业务5：清空(销毁)一个学生的全部成绩
void toDestroyScoreByStudent(StudentNode*& L) {
    printf("\n清空某学生全部成绩\n");
    int id;
    printf("请输入学号：");
    scanf("%d", &id);
    destroyScoreByStudent(L, id);
}

//调用成绩业务6：清空(销毁)所有学生的全部成绩
void toDestroyALLScore(StudentNode*& L) {
    printf("\n清空所有学生成绩\n");
    printf("确认清空所有成绩？(y/n)：");
    char confirm;
    scanf(" %c", &confirm);
    if (confirm == 'y' || confirm == 'Y') {
        destroyALLScore(L);
    } else {
        printf("取消操作。\n");
    }
}

//调用成绩业务7：删除学生一个课程成绩
void toDeleteAScoreByStudent(StudentNode* L, CourseNode* LC) {
    printf("\n删除某学生单科成绩\n");
    int id, cId;
    printf("请输入学号：");
    scanf("%d", &id);
    toDisplayCourse(LC);
    printf("请输入课程编号：");
    scanf("%d", &cId);

    int ret = deleteAScoreByStudent(L, id, cId, LC);
    switch (ret) {
        case 0: printf("删除成功！\n"); break;
        case 1: printf("学生不存在！\n"); break;
        case 2: printf("课程不存在！\n"); break;
        case 3: printf("该成绩记录不存在！\n"); break;
    }
}