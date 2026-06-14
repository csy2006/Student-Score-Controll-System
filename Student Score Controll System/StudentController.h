// StudentController.h
#pragma once
#include "DataType.h"   // 确保 StudentNode 类型在这里定义

// 调用业务1：初始化学生
void toInitStudent(StudentNode*& L);

// 调用业务2：输出全部学生
void toDisplayALLStudent(StudentNode* L);

// 调用业务2：输出班级的学生
void toDisplayStudentByClassName(StudentNode* L);

// 调用业务2：输出一个学生
void toDisplayStudentById(StudentNode* L);

// 调用业务3：增加学生
void toAddStudent(StudentNode*& L);

// 调用业务4：修改学生
void toUpdateStudent(StudentNode*& L);

// 调用业务5：删除学生
void toDeleteStudent(StudentNode*& L);

// 调用业务6：清空(销毁)所有学生
void toDestroyStudent(StudentNode*& L);