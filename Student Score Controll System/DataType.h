//DataType.h
#pragma once
#ifndef DATATYPE_H
#define DATATYPE_H

//定义课程链表的结点类型
typedef struct CNode {
    int cId;                //课程编号
    char cName[20];         //课程名称
    struct CNode* cNext;    //下一个课程
} CourseNode;

//定义成绩链表的结点类型
typedef struct ScNode {
    int cId;                //课程编号
    int score;              //课程成绩
    struct ScNode* scNext;  //下一个成绩
} ScoreNode;

//定义学生类型
typedef struct {
    int id;                 // 学号
    char name[20];          //姓名
    char sex;               //性别 M/F
    char className[20];     //班级名称
    ScoreNode* LS;          //学生成绩头结点
} Student;

//定义学生链表的结点类型
typedef struct SNode {
    Student stu;            //学生信息
    struct SNode* next;     //下一个学生
} StudentNode;

#endif