//StudentWork.h (声明"学生"实体的业务操作函数)
#pragma once
#ifndef STUDENTWORK_H
#define STUDENTWORK_H
#include "DataType.h"

//业务1：初始化学生（创建5条样本数据）
void initStudent(StudentNode*& L);

//业务2：显示学生信息
//mode：1=全部学生 2=按班级 3=按学号
void displayStudent(StudentNode* L, Student stu, int mode);

//业务3：增加学生
bool addStudent(StudentNode*& L, Student stu);

//业务4：修改学生
bool updateStudent(StudentNode*& L, Student oldStu, Student newStu);

//业务5：删除学生
bool deleteStudent(StudentNode*& L, Student stu);

//业务6：清空(销毁)所有学生
void destroyStudent(StudentNode*& L);

//辅助业务1：显示一个学生的详细信息
void showStudentDetails(Student stu);

//辅助业务2：定位学生（按学号）
StudentNode* locateStudent(StudentNode* L, int id);

#endif