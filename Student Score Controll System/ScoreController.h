//ScoreController.h (视图控制层---声明"成绩"实体的流程控制及视图函数)
#pragma once
#ifndef SCORECONTROLLER_H
#define SCORECONTROLLER_H
#include "DataType.h"

//调用成绩业务1：显示所有学生的成绩
void toDisplayAllScore(StudentNode* L, CourseNode* LC);
//调用成绩业务1：显示班级学生的成绩
void toDisplayScoreByClassName(StudentNode* L, CourseNode* LC);
//调用成绩业务1：显示一个学生的成绩
void toDisplayScoreById(StudentNode* L, CourseNode* LC);
//调用成绩业务2：增加一个学生的全部成绩
void toAddScoreByStudent(StudentNode*& L, CourseNode* LC);
//调用成绩业务3：增加一门课程的成绩
void toAddScoreByCourse(StudentNode*& L, CourseNode* LC);
//调用成绩业务4：增加学生一个课程的成绩
void toAddAScoreByStudent(StudentNode*& L, CourseNode* LC);
//调用成绩业务5：清空(销毁)一个学生的全部成绩
void toDestroyScoreByStudent(StudentNode*& L);
//调用成绩业务6：清空(销毁)所有学生的全部成绩
void toDestroyALLScore(StudentNode*& L);
//调用成绩业务7：删除学生一个课程成绩
void toDeleteAScoreByStudent(StudentNode* L, CourseNode* LC);

#endif