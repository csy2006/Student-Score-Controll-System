//StudentWork.cpp (实现"学生"实体的业务操作函数)
#define _CRT_SECURE_NO_WARNINGS
#pragma execution_character_set("utf-8")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "StudentWork.h"

//业务1：初始化学生（创建5条样本数据）
void initStudent(StudentNode*& L) {
    L = NULL;  //先清空
    L = NULL;  //先清空

    //样本数据
    int ids[]           = {20210101, 20210102, 20210103, 20210201, 20210202, 20210203};
    const char* names[] = {"张三", "李四", "王五", "赵六", "钱七", "无锡"};
    char sexes[]        = {'M', 'F', 'M', 'F', 'M', 'M'};  // 第6个学生"无锡"设为男性
    const char* classes[]= {"软件2101", "软件2101", "软件2101", "软件2102", "软件2102", "软件2102"};
    int n = 6;

    for (int i = 0; i < n; i++) {
        Student stu;
        stu.id = ids[i];
        strcpy(stu.name,  names[i]);
        stu.sex = sexes[i];
        strcpy(stu.className, classes[i]);
        stu.LS = NULL;  //成绩链表初始为空
        addStudent(L, stu);
    }
}

//业务2：显示学生信息
//mode：1=全部学生 2=按班级 3=按学号
void displayStudent(StudentNode* L, Student stu, int mode) {
    if (L == NULL) {
        printf("学生链表为空！\n");
        return;
    }

    printf("%-12s %-10s %-4s %-15s\n", "学号", "姓名", "性别", "班级");
    printf("--------------------------------------------------\n");

    StudentNode* p = L;
    while (p != NULL) {
        int shouldPrint = 0;
        if (mode == 1) {
            shouldPrint = 1;
        } else if (mode == 2) {
            shouldPrint = (strcmp(p->stu.className, stu.className) == 0);
        } else if (mode == 3) {
            shouldPrint = (p->stu.id == stu.id);
        }

        if (shouldPrint) {
            printf("%-12d %-10s %-4c %-15s\n",
                p->stu.id, p->stu.name,
                p->stu.sex, p->stu.className);
        }
        p = p->next;
    }
}

//业务3：增加学生（尾插法）
bool addStudent(StudentNode*& L, Student stu) {
    //检查学号是否已存在
    if (locateStudent(L, stu.id) != NULL) {
        printf("学号 %d 已存在，添加失败！\n", stu.id);
        return false;
    }

    StudentNode* newNode = (StudentNode*)malloc(sizeof(StudentNode));
    if (newNode == NULL) return false;

    newNode->stu  = stu;
    newNode->next = NULL;

    if (L == NULL) {
        L = newNode;
    } else {
        StudentNode* tail = L;
        while (tail->next != NULL) tail = tail->next;
        tail->next = newNode;
    }
    return true;
}

//业务4：修改学生
bool updateStudent(StudentNode*& L, Student oldStu, Student newStu) {
    StudentNode* p = locateStudent(L, oldStu.id);
    if (p == NULL) {
        printf("未找到学号 %d 的学生！\n", oldStu.id);
        return false;
    }
    //保留原成绩链表
    ScoreNode* tempLS = p->stu.LS;
    p->stu = newStu;
    p->stu.LS = tempLS;
    return true;
}

//业务5：删除学生
bool deleteStudent(StudentNode*& L, Student stu) {
    if (L == NULL) return false;

    //头结点就是目标
    if (L->stu.id == stu.id) {
        StudentNode* del = L;
        L = L->next;
        //释放成绩链表
        ScoreNode* sc = del->stu.LS;
        while (sc != NULL) {
            ScoreNode* tmp = sc->scNext;
            free(sc);
            sc = tmp;
        }
        free(del);
        return true;
    }

    StudentNode* prev = L;
    StudentNode* cur  = L->next;
    while (cur != NULL) {
        if (cur->stu.id == stu.id) {
            prev->next = cur->next;
            //释放成绩链表
            ScoreNode* sc = cur->stu.LS;
            while (sc != NULL) {
                ScoreNode* tmp = sc->scNext;
                free(sc);
                sc = tmp;
            }
            free(cur);
            return true;
        }
        prev = cur;
        cur  = cur->next;
    }

    printf("未找到学号 %d 的学生！\n", stu.id);
    return false;
}

//业务6：清空(销毁)所有学生
void destroyStudent(StudentNode*& L) {
    StudentNode* p = L;
    while (p != NULL) {
        StudentNode* tmp = p->next;
        //释放成绩链表
        ScoreNode* sc = p->stu.LS;
        while (sc != NULL) {
            ScoreNode* sctmp = sc->scNext;
            free(sc);
            sc = sctmp;
        }
        free(p);
        p = tmp;
    }
    L = NULL;
    printf("所有学生数据已清空。\n");
}

//辅助业务1：显示一个学生的详细信息
void showStudentDetails(Student stu) {
    printf("学号：%d\n", stu.id);
    printf("姓名：%s\n", stu.name);
    printf("性别：%c\n", stu.sex);
    printf("班级：%s\n", stu.className);
}

//辅助业务2：定位学生（按学号）
StudentNode* locateStudent(StudentNode* L, int id) {
    StudentNode* p = L;
    while (p != NULL) {
        if (p->stu.id == id) return p;
        p = p->next;
    }
    return NULL;
}