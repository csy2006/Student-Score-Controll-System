//HttpServer.h - HTTP服务器封装（基于cpp-httplib）
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "DataType.h"

//前置声明httplib
namespace httplib {
    class Server;
}

class HttpServer {
public:
    HttpServer();
    ~HttpServer();

    // 设置数据指针（由 main 传入）
    void setData(CourseNode*& lc, StudentNode*& l);
    // 启动服务器
    bool start(int port);

private:
    httplib::Server* svr;
    CourseNode** LC_ptr;
    StudentNode** L_ptr;

    //获取指针
    CourseNode* getLC() const { return LC_ptr ? *LC_ptr : NULL; }
    StudentNode* getL()  const { return L_ptr  ? *L_ptr  : NULL; }

    //从请求体读取JSON字符串值，返回新malloc的字符串（调用者需free）
    char* getJsonString(const char* json, const char* key);
    //从请求体读取JSON整数值
    int   getJsonInt(const char* json, const char* key);
    //构建JSON响应
    void buildCoursesJson(char* out, int outSize);
    void buildStudentsJson(char* out, int outSize);
    void buildScoresJson(char* out, int outSize);
    void buildStudentScoresJson(char* out, int outSize, int studentId);
    //计算学生平均分
    float calcAvgScore(ScoreNode* LS);
    //计算单科绩点
    float calcGradePoint(int score);
    //构建GPAJSON
    void buildGpaJson(char* out, int outSize);
    //构建反馈JSON
    void buildFeedbacksJson(char* out, int outSize);
    //反馈存储
    static const int MAX_FEEDBACKS = 200;
    struct Feedback { int cId; int rating; char comment[128]; } feedbacks[200];
    int feedbackCount;
};