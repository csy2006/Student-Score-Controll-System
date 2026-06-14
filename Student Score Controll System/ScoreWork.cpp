//ScoreWork.cpp (实现"成绩"实体的业务操作函数)
#define _CRT_SECURE_NO_WARNINGS
#pragma execution_character_set("utf-8")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>   //  for time() / srand()
#include "ScoreWork.h"
#include "StudentWork.h"
#include "CourseWork.h"

//业务1：显示学生成绩
//mode: 1=全部成绩 2=班级成绩 3=个人成绩
void displayScore(StudentNode* L, Student stu, int mode, CourseNode* LC) {
    if (L == NULL) {
        printf("学生链表为空！\n");
        return;
    }
    showScoreTableHeader(LC);

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
            showStudentScoreDetails(p->stu, LC);
        }
        p = p->next;
    }
}

//业务2：增加一个学生的全部成绩
bool addScoreByStudent(StudentNode*& L, int id, CourseNode* LC) {
    StudentNode* sNode = locateStudent(L, id);
    if (sNode == NULL) {
        printf("未找到学号 %d 的学生！\n", id);
        return false;
    }

    CourseNode* cp = LC;
    while (cp != NULL) {
        //若该课程成绩已存在则跳过
        if (locateStudentScore(sNode->stu, cp->cId) != NULL) {
            cp = cp->cNext;
            continue;
        }
        int score;
        printf("请输入学生[%s]课程[%s]的成绩：", sNode->stu.name, cp->cName);
        scanf("%d", &score);

        ScoreNode* sc = (ScoreNode*)malloc(sizeof(ScoreNode));
        sc->cId = cp->cId;
        sc->score = score;
        sc->scNext = NULL;

        //头插到成绩链表
        sc->scNext = sNode->stu.LS;
        sNode->stu.LS = sc;

        cp = cp->cNext;
    }
    sortStudentScoreByCid(sNode->stu);
    return true;
}

//业务3：增加一门课程的全部成绩
bool addScoreByCourse(StudentNode*& L, int cId, CourseNode* LC) {
    //验证课程存在
    char cName[20];
    if (!getCourseName(LC, cId, cName)) {
        printf("课程编号 %d 不存在！\n", cId);
        return false;
    }

    StudentNode* p = L;
    while (p != NULL) {
        //若已有成绩则跳过
        if (locateStudentScore(p->stu, cId) != NULL) {
            p = p->next;
            continue;
        }
        int score;
        printf("请输入学生[%s]课程[%s]的成绩：", p->stu.name, cName);
        scanf("%d", &score);

        ScoreNode* sc = (ScoreNode*)malloc(sizeof(ScoreNode));
        sc->cId   = cId;
        sc->score = score;
        sc->scNext = NULL;

        sc->scNext = p->stu.LS;
        p->stu.LS  = sc;
        sortStudentScoreByCid(p->stu);

        p = p->next;
    }
    return true;
}

//业务4：增加学生一个课程成绩
//返回值：0=成功 1=学生不存在 2=课程不存在 3=成绩已存在
int addAScoreByStudent(StudentNode*& L, int id, int cId, CourseNode* LC) {
    StudentNode* sNode = locateStudent(L, id);
    if (sNode == NULL) return 1;

    char cName[20];
    if (!getCourseName(LC, cId, cName)) return 2;

    if (locateStudentScore(sNode->stu, cId) != NULL) return 3;

    int score;
    printf("请输入学生[%s]课程[%s]的成绩：", sNode->stu.name, cName);
    scanf("%d", &score);

    ScoreNode* sc = (ScoreNode*)malloc(sizeof(ScoreNode));
    sc->cId   = cId;
    sc->score = score;
    sc->scNext = NULL;

    sc->scNext    = sNode->stu.LS;
    sNode->stu.LS = sc;
    sortStudentScoreByCid(sNode->stu);
    return 0;
}

//业务5：清空(销毁)一个学生的全部成绩
bool destroyScoreByStudent(StudentNode*& L, int id) {
    StudentNode* sNode = locateStudent(L, id);
    if (sNode == NULL) {
        printf("未找到学号 %d 的学生！\n", id);
        return false;
    }
    ScoreNode* sc = sNode->stu.LS;
    while (sc != NULL) {
        ScoreNode* tmp = sc->scNext;
        free(sc);
        sc = tmp;
    }
    sNode->stu.LS = NULL;
    printf("学生[%s]的成绩已清空。\n", sNode->stu.name);
    return true;
}

//业务6：清空(销毁)所有学生的全部成绩
void destroyALLScore(StudentNode*& L) {
    StudentNode* p = L;
    while (p != NULL) {
        ScoreNode* sc = p->stu.LS;
        while (sc != NULL) {
            ScoreNode* tmp = sc->scNext;
            free(sc);
            sc = tmp;
        }
        p->stu.LS = NULL;
        p = p->next;
    }
    printf("所有学生成绩已清空。\n");
}

//业务7：删除学生一个课程成绩
//返回值：0=成功 1=学生不存在 2=课程不存在 3=成绩不存在
int deleteAScoreByStudent(StudentNode* L, int id, int cId, CourseNode* LC) {
    StudentNode* sNode = locateStudent(L, id);
    if (sNode == NULL) return 1;

    char cName[20];
    if (!getCourseName(LC, cId, cName)) return 2;

    ScoreNode** pp = &(sNode->stu.LS);
    while (*pp != NULL) {
        if ((*pp)->cId == cId) {
            ScoreNode* del = *pp;
            *pp = del->scNext;
            free(del);
            return 0;
        }
        pp = &((*pp)->scNext);
    }
    return 3;
}

//辅助业务1：定位学生成绩（按课程号）
ScoreNode* locateStudentScore(Student stu, int cId) {
    ScoreNode* p = stu.LS;
    while (p != NULL) {
        if (p->cId == cId) return p;
        p = p->scNext;
    }
    return NULL;
}

//辅助业务2：学生成绩链表排序（按课程号升序，冒泡排序）
void sortStudentScoreByCid(Student& stu) {
    if (stu.LS == NULL) return;
    bool swapped;
    do {
        swapped = false;
        ScoreNode* p = stu.LS;
        while (p->scNext != NULL) {
            if (p->cId > p->scNext->cId) {
                //交换数据
                int tmpCid   = p->cId;
                int tmpScore = p->score;
                p->cId   = p->scNext->cId;
                p->score = p->scNext->score;
                p->scNext->cId   = tmpCid;
                p->scNext->score = tmpScore;
                swapped = true;
            }
            p = p->scNext;
        }
    } while (swapped);
}

//辅助业务4：初始化所有学生所有课程的成绩（90% ≥60，10% <60，100分制）
void initScores(StudentNode*& L, CourseNode* LC) {
    srand((unsigned)time(NULL));
    StudentNode* sp = L;
    while (sp != NULL) {
        CourseNode* cp = LC;
        while (cp != NULL) {
            int score;
            // 90% 概率生成 ≥60 的成绩，10% 概率生成 <60 的成绩
            if (rand() % 10 < 9) {
                score = 60 + rand() % 41;  // 60~100
            } else {
                score = rand() % 60;        // 0~59
            }
            // 直接写入，若已存在则跳过（先清空的情况）
            if (locateStudentScore(sp->stu, cp->cId) == NULL) {
                ScoreNode* sc = (ScoreNode*)malloc(sizeof(ScoreNode));
                sc->cId   = cp->cId;
                sc->score  = score;
                sc->scNext = sp->stu.LS;
                sp->stu.LS = sc;
            }
            cp = cp->cNext;
        }
        sp = sp->next;
    }
    // 排序
    sp = L;
    while (sp != NULL) {
        sortStudentScoreByCid(sp->stu);
        sp = sp->next;
    }
}

//辅助业务3：显示成绩表的表头
void showScoreTableHeader(CourseNode* LC) {
    printf("%-12s %-10s", "学号", "姓名");
    CourseNode* p = LC;
    while (p != NULL) {
        printf(" %-10s", p->cName);
        p = p->cNext;
    }
    printf(" %-8s\n", "平均分");
    printf("------------------------------------------------------------\n");
}

//辅助业务4：显示一个学生的详细成绩
void showStudentScoreDetails(Student stu, CourseNode* LC) {
    printf("%-12d %-10s", stu.id, stu.name);
    int total = 0, count = 0;
    CourseNode* cp = LC;
    while (cp != NULL) {
        ScoreNode* sc = locateStudentScore(stu, cp->cId);
        if (sc != NULL) {
            printf(" %-10d", sc->score);
            total += sc->score;
            count++;
        } else {
            printf(" %-10s", "--");
        }
        cp = cp->cNext;
    }
    if (count > 0)
        printf(" %-8.1f\n", (float)total / count);
    else
        printf(" %-8s\n", "--");
}

// 直接操作函数实现

//直接增加一个学生的全部成绩（scores数组按顺序对应课程链表）
bool addScoreDirectByStudent(StudentNode*& L, int id, int scores[], int n, CourseNode* LC) {
    StudentNode* sNode = locateStudent(L, id);
    if (sNode == NULL) return false;

    CourseNode* cp = LC;
    int i = 0;
    while (cp != NULL && i < n) {
        if (locateStudentScore(sNode->stu, cp->cId) == NULL && scores[i] >= 0) {
            ScoreNode* sc = (ScoreNode*)malloc(sizeof(ScoreNode));
            sc->cId   = cp->cId;
            sc->score = scores[i];
            sc->scNext = sNode->stu.LS;
            sNode->stu.LS = sc;
        }
        cp = cp->cNext;
        i++;
    }
    sortStudentScoreByCid(sNode->stu);
    return true;
}

//直接增加一门课程的全部成绩
bool addScoreDirectByCourse(StudentNode*& L, int cId, int scores[], int n, CourseNode* LC) {
    char cName[20];
    if (!getCourseName(LC, cId, cName)) return false;

    StudentNode* p = L;
    int i = 0;
    while (p != NULL && i < n) {
        if (locateStudentScore(p->stu, cId) == NULL && scores[i] >= 0) {
            ScoreNode* sc = (ScoreNode*)malloc(sizeof(ScoreNode));
            sc->cId   = cId;
            sc->score = scores[i];
            sc->scNext = p->stu.LS;
            p->stu.LS  = sc;
            sortStudentScoreByCid(p->stu);
        }
        p = p->next;
        i++;
    }
    return true;
}

//直接增加学生一个课程成绩
int addAScoreDirect(StudentNode*& L, int id, int cId, int score, CourseNode* LC) {
    StudentNode* sNode = locateStudent(L, id);
    if (sNode == NULL) return 1;

    char cName[20];
    if (!getCourseName(LC, cId, cName)) return 2;

    if (locateStudentScore(sNode->stu, cId) != NULL) return 3;

    ScoreNode* sc = (ScoreNode*)malloc(sizeof(ScoreNode));
    sc->cId   = cId;
    sc->score = score;
    sc->scNext = sNode->stu.LS;
    sNode->stu.LS = sc;
    sortStudentScoreByCid(sNode->stu);
    return 0;
}

//直接修改学生一个课程成绩
int updateAScoreDirect(StudentNode*& L, int id, int cId, int newScore) {
    StudentNode* sNode = locateStudent(L, id);
    if (sNode == NULL) return 1;

    ScoreNode* sc = locateStudentScore(sNode->stu, cId);
    if (sc == NULL) return 3;

    sc->score = newScore;
    return 0;
}