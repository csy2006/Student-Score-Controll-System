//CourseWork.h (声明"课程"实体的业务操作函数)
#pragma once
#ifndef COURSEWORK_H
#define COURSEWORK_H
#include "DataType.h"

//业务1：创建课程链表（预置3门课程）
void createCourse(CourseNode*& LC);
//业务2：显示全部课程
void displayCourse(CourseNode* LC);
//业务3：根据课程编号取得课程名称
bool getCourseName(CourseNode* LC, int cId, char cName[]);
//辅助业务1：根据课程编号定位课程
CourseNode* locateCourse(CourseNode* LC, int cId);
//辅助业务2：统计课程数
int getCourseNumber(CourseNode* LC);
//辅助业务3：取得所有课程的课程号
bool getCourseIdsAndNames(CourseNode* LC, int cIds[]);
//业务4：添加一门课程（尾插法）
bool addCourse(CourseNode*& LC, int cId, const char cName[]);
//业务5：删除一门课程（按课程编号）
bool deleteCourse(CourseNode*& LC, int cId);

#endif