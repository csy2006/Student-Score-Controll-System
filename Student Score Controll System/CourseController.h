//CourseController.h (声明"课程"实体的流程控制及视图函数)
#pragma once
#ifndef COURSECONTROLLER_H
#define COURSECONTROLLER_H
#include "DataType.h"

//调用业务1：创建课程链表
void toCreateCourse(CourseNode*& LC);
//调用业务2：显示全部课程
void toDisplayCourse(CourseNode* LC);
//调用业务3：根据课程编号取得课程名称
void toGetCourseName(CourseNode* LC);
//调用业务4：添加一门课程
void toAddCourse(CourseNode*& LC);
//调用业务5：删除一门课程
void toDeleteCourse(CourseNode*& LC);

#endif