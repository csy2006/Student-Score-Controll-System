//StudentController.cpp (实现"学生"实体的流程控制及视图函数)
#define _CRT_SECURE_NO_WARNINGS
#pragma execution_character_set("utf-8")
#include <stdio.h>
#include <string.h>
#include "StudentController.h"
#include "StudentWork.h"

//调用业务1：初始化学生
void toInitStudent(StudentNode*& L) {
    printf("\n初始化学生数据\n");
    initStudent(L);
    printf("初始化完成，已预置5名学生：\n");
    Student tmp = { 0 };
    displayStudent(L, tmp, 1);
}

//调用业务2：输出全部学生
void toDisplayALLStudent(StudentNode* L) {
    printf("\n全部学生信息\n");
    Student tmp = { 0 };
    displayStudent(L, tmp, 1);
}

//调用业务2：输出班级的学生
void toDisplayStudentByClassName(StudentNode* L) {
    printf("\n按班级查询学生\n");
    Student stu;
    printf("请输入班级名称：");
    scanf("%s", stu.className);
    displayStudent(L, stu, 2);
}

//调用业务2：输出一个学生
void toDisplayStudentById(StudentNode* L) {
    printf("\n按学号查询学生\n");
    int id;
    printf("请输入学号：");
    scanf("%d", &id);
    Student stu;
    stu.id = id;
    displayStudent(L, stu, 3);
}

//调用业务3：增加学生
void toAddStudent(StudentNode*& L) {
    printf("\n增加学生\n");
    Student stu;
    printf("请输入学号：");
    scanf("%d", &stu.id);
    printf("请输入姓名：");
    scanf("%s", stu.name);
    printf("请输入性别(M/F)：");
    scanf(" %c", &stu.sex);
    printf("请输入班级：");
    scanf("%s", stu.className);
    stu.LS = NULL;

    if (addStudent(L, stu)) {
        printf("学生 [%s] 添加成功！\n", stu.name);
    }
}

//调用业务4：修改学生
void toUpdateStudent(StudentNode*& L) {
    printf("\n修改学生信息\n");
    int id;
    printf("请输入要修改的学号：");
    scanf("%d", &id);

    StudentNode* sNode = locateStudent(L, id);
    if (sNode == NULL) {
        printf("未找到学号 %d 的学生！\n", id);
        return;
    }
    printf("当前信息：\n");
    showStudentDetails(sNode->stu);

    Student oldStu = sNode->stu;
    Student newStu;
    newStu.id = id;  //学号不变
    printf("请输入新姓名（回车保持 %s）：", oldStu.name);
    char buf[20];
    // 消耗换行符
    getchar();
    fgets(buf, sizeof(buf), stdin);
    if (buf[0] == '\n') {
        strcpy(newStu.name, oldStu.name);
    } else {
        buf[strcspn(buf, "\n")] = '\0';
        strcpy(newStu.name, buf);
    }
    printf("请输入新性别(M/F，回车保持 %c)：", oldStu.sex);
    char sc = getchar();
    if (sc == '\n') newStu.sex = oldStu.sex;
    else { newStu.sex = sc; getchar(); }

    printf("请输入新班级（回车保持 %s）：", oldStu.className);
    fgets(buf, sizeof(buf), stdin);
    if (buf[0] == '\n') {
        strcpy(newStu.className, oldStu.className);
    } else {
        buf[strcspn(buf, "\n")] = '\0';
        strcpy(newStu.className, buf);
    }
    newStu.LS = oldStu.LS;

    if (updateStudent(L, oldStu, newStu)) {
        printf("修改成功！\n");
    }
}

//调用业务5：删除学生
void toDeleteStudent(StudentNode*& L) {
    printf("\n删除学生\n");
    int id;
    printf("请输入要删除的学号：");
    scanf("%d", &id);

    StudentNode* sNode = locateStudent(L, id);
    if (sNode == NULL) {
        printf("未找到学号 %d 的学生！\n", id);
        return;
    }
    printf("确认删除学生 [%s]？(y/n)：", sNode->stu.name);
    char confirm;
    scanf(" %c", &confirm);
    if (confirm == 'y' || confirm == 'Y') {
        Student stu;
        stu.id = id;
        if (deleteStudent(L, stu)) {
            printf("删除成功！\n");
        }
    } else {
        printf("取消删除。\n");
    }
}

//调用业务6：清空(销毁)所有学生
void toDestroyStudent(StudentNode*& L) {
    printf("\n清空所有学生数据\n");
    printf("确认清空所有学生？(y/n)：");
    char confirm;
    scanf(" %c", &confirm);
    if (confirm == 'y' || confirm == 'Y') {
        destroyStudent(L);
    } else {
        printf("取消操作。\n");
    }
}