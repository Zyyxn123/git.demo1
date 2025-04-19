#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conio.h"
#include <time.h>

#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20
#define MAX_EMAIL_LENGTH 50
#define MAX_SECRET_QUESTION_LENGTH 50
#define MAX_SECRET_ANSWER_LENGTH 20
#define PAGE_SIZE 5
#define MAX_STUDENTS 100

// 用户结构体
typedef struct User {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    int type;  // 1:学生, 2:教师, 3:管理员
    char secretQuestion[MAX_SECRET_QUESTION_LENGTH];
    char secretAnswer[MAX_SECRET_ANSWER_LENGTH];
    struct User* prev;
    struct User* next;
} User;

User* userHead = NULL;
User* userTail = NULL;

// 学生结构体
typedef struct Student {
    int studentId;
    struct User user;
    int scores[3];  // 假设三门课程成绩：语、数、英
    struct Student* prev;
    struct Student* next;
} Student;

Student* studentHead = NULL;
Student* studentTail = NULL;
int studentCount = 0;

// 定义申诉状态枚举
typedef enum {
    PENDING,
    PROCESSED,
    REJECTED
} AppealStatus;

// 定义申诉结构体
typedef struct Appeal {
    char username[MAX_USERNAME_LENGTH];
    int subjectIndex;  // 0: 语文, 1: 数学, 2: 英语
    char reason[200];
    AppealStatus status;
    struct Appeal* next;
} Appeal;

Appeal* appealHead = NULL;

void gettime(){
    time_t t;
    struct tm* pt;
    // 获取当前时间
    time(&t);
    // 将时间转换为本地时间
    pt = localtime(&t);
    // 使用格式化输出简化代码
    printf("北京时间: %02d : %02d\n", pt->tm_hour, pt->tm_min);
    /*time_t t;
    struct tm* pt;
    char* pc;
    time(&t);
    pc = ctime(&t);

    pt = localtime(&t);
    if (pt->tm_hour > 9 && pt->tm_min > 9)
        printf("(北京时间):%d : %d", pt->tm_hour, pt->tm_min);
    else if (pt->tm_hour <= 9 && pt->tm_min > 9)
        printf("(北京时间):0%d : %d", pt->tm_hour, pt->tm_min);
    else if (pt->tm_hour <= 9 && pt->tm_min <= 9)
        printf("(北京时间):0%d : 0%d", pt->tm_hour, pt->tm_min);
    else if (pt->tm_hour > 9 && pt->tm_min <= 9)
        printf("(北京时间):%d : 0%d", pt->tm_hour, pt->tm_min);*/
}

// 保存学生信息到文件
void saveStudentsToFile() {
    FILE* file = fopen("students_grades.txt", "w");
    if (file == NULL) {
        perror("无法打开文件");
        return;
    }
    Student* current = studentHead;
    while (current != NULL) {
        fprintf(file, "%d %s %s %s %d %d %d %d \n",
            current->studentId, current->user.username,
            current->user.password, current->user.email,
            current->scores[0], current->scores[1], current->scores[2],
            current->user.type);
        current = current->next;
    }
    fclose(file);
    printf("学生成绩已下载到文件 students_grades.txt 中。\n");
}

// 保存申诉信息到文件
void saveAppealsToFile() {
    FILE* file = fopen("appeals.txt", "w");
    if (file == NULL) {
        perror("无法打开文件");
        return;
    }
    Appeal* current = appealHead;
    while (current != NULL) {
        fprintf(file, "%s %d %s %d\n", current->username, current->subjectIndex, current->reason, current->status);
        current = current->next;
    }
    fclose(file);
}

// 保存用户信息到文件
void saveUsersToFile() {
    FILE* file = fopen("users.txt", "w");
    if (file == NULL) {
        perror("无法打开文件");
        return;
    }
    User* current = userHead;
    while (current != NULL) {
        fprintf(file, "%s %s %s %d %s %s\n",
            current->username, current->password,
            current->email, current->type,
            current->secretQuestion, current->secretAnswer);
        current = current->next;
    }
    fclose(file);
}

// 从文件加载用户信息
void loadUsersFromFile() {
    FILE* file = fopen("users.txt", "r");
    if (file == NULL) {
        perror("无法打开文件");
        return;
    }
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    int type;
    char secretQuestion[MAX_SECRET_QUESTION_LENGTH];
    char secretAnswer[MAX_SECRET_ANSWER_LENGTH];

    while (fscanf(file, "%s %s %s %d %s %s", username, password, email, &type, secretQuestion, secretAnswer) == 6) {
        User* newUser = (User*)malloc(sizeof(User));
        strcpy(newUser->username, username);
        strcpy(newUser->password, password);
        strcpy(newUser->email, email);
        newUser->type = type;
        strcpy(newUser->secretQuestion, secretQuestion);
        strcpy(newUser->secretAnswer, secretAnswer);
        newUser->prev = userTail;
        newUser->next = NULL;
        if (userTail == NULL) {
            userHead = newUser;
        }
        else {
            userTail->next = newUser;
        }
        userTail = newUser;
    }
    fclose(file);
}

// 从文件加载学生信息
void loadStudentsFromFile() {
    FILE* file = fopen("students_grades.txt", "r");
    if (file == NULL) {
        perror("无法打开文件");
        return;
    }
    int studentId;
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    int type;
    int scores[3];
    while (fscanf(file, "%d %s %s %s %d %d %d %d", &studentId, username, password, email, &scores[0], &scores[1], &scores[2], &type) == 8) {
        Student* newStudent = (Student*)malloc(sizeof(Student));
        newStudent->studentId = studentId;
        strcpy(newStudent->user.username, username);
        strcpy(newStudent->user.password, password);
        strcpy(newStudent->user.email, email);
        newStudent->scores[0] = scores[0];
        newStudent->scores[1] = scores[1];
        newStudent->scores[2] = scores[2];
        newStudent->user.type = type;
        newStudent->prev = studentTail;
        newStudent->next = NULL;
        if (studentTail == NULL) {
            studentHead = newStudent;
        }
        else {
            studentTail->next = newStudent;
        }
        studentTail = newStudent;
        studentCount += 1;
    }
    fclose(file);
}

// 从文件加载申诉信息
void loadAppealsFromFile() {
    FILE* file = fopen("appeals.txt", "r");
    if (file == NULL) {
        return;
    }
    char username[MAX_USERNAME_LENGTH];
    int subjectIndex;
    char reason[200];
    AppealStatus status;

    while (fscanf(file, "%s %d %s %d", username, &subjectIndex, reason, &status) == 4) {
        Appeal* newAppeal = (Appeal*)malloc(sizeof(Appeal));
        strcpy(newAppeal->username, username);
        newAppeal->subjectIndex = subjectIndex;
        strcpy(newAppeal->reason, reason);
        newAppeal->status = status;
        newAppeal->next = appealHead;
        appealHead = newAppeal;
    }
    fclose(file);
}

// 注册功能
void registerUser() {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    int type;
    char secretQuestion[MAX_SECRET_QUESTION_LENGTH];
    char secretAnswer[MAX_SECRET_ANSWER_LENGTH];

    printf("请输入用户名: ");
    scanf("%s", username);
    User* current = userHead;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            printf("用户名已存在，请重新输入。\n");
            return;
        }
        current = current->next;
    }

    printf("请输入密码: ");
    char ch;
    int i = 0;
    while (1) {
        ch = _getch(); // 从控制台读取单个字符，不回显
        if (ch == '\r') {
            // 当按下回车键（\r 表示回车），结束输入
            password[i] = '\0'; // 为密码字符串添加结束符
            printf("\n");
            break;
        }
        else if (ch == '\b' && i > 0) {
            // 处理退格键，且保证不是第一个字符
            i--;
            printf("\b \b"); // 退格，输出空格覆盖星号，再退格
        }
        else {
            password[i++] = ch;
            printf("*"); // 输出*代替实际输入字符
        }
    }

    /*printf("请输入密码: ");
    scanf("%s", password);*/
    printf("请输入邮箱: ");
    scanf("%s", email);
    printf("请选择用户类型 (1:学生, 2:教师): ");
    scanf("%d", &type);

    printf("请设置密保问题: ");
    scanf("%s", secretQuestion);
    printf("请设置密保答案: ");
    scanf("%s", secretAnswer);

    User* newUser = (User*)malloc(sizeof(User));
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    strcpy(newUser->email, email);
    newUser->type = type;
    strcpy(newUser->secretQuestion, secretQuestion);
    strcpy(newUser->secretAnswer, secretAnswer);
    newUser->prev = userTail;
    newUser->next = NULL;
    if (userTail == NULL) {
        userHead = newUser;
    }
    else {
        userTail->next = newUser;
    }
    userTail = newUser;

    saveUsersToFile();
    printf("注册成功！\n");
}

// 基于密保修改密码
void modifyPasswordWithSecret() {
    char username[MAX_USERNAME_LENGTH];
    char newPassword[MAX_PASSWORD_LENGTH];
    char inputAnswer[MAX_SECRET_ANSWER_LENGTH];

    printf("请输入要修改密码的用户名: ");
    scanf("%s", username);
    User* current = userHead;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            printf("密保问题: %s\n", current->secretQuestion);
            printf("请输入密保答案: ");
            scanf("%s", inputAnswer);
            if (strcmp(current->secretAnswer, inputAnswer) == 0) {
                int i = 0;
                char ch;

                printf("请输入新密码: ");
                while (1) {
                    ch = _getch();  // 从控制台读取单个字符，不回显
                    if (ch == '\r') {  // 当按下回车键（\r 表示回车），结束输入
                        newPassword[i] = '\0';  // 为密码字符串添加结束符
                        printf("\n");
                        break;
                    }
                    else if (ch == '\b' && i > 0) {  // 处理退格键，且保证不是第一个字符
                        i--;
                        printf("\b \b");  // 退格，输出空格覆盖星号，再退格
                    }
                    else {
                        newPassword[i++] = ch;
                        printf("*");  // 输出*代替实际输入字符
                    }
                }

               /* printf("请输入新密码: ");
                scanf("%s", newPassword);*/
                strcpy(current->password, newPassword);
                saveUsersToFile();
                printf("密码修改成功！\n");
            }
            else {
                printf("密保答案错误，修改失败。\n");
            }
            return;
        }
        current = current->next;
    }
    printf("未找到该用户名，修改失败。\n");
}

// 基于密保找回密码
void retrievePasswordWithSecret() {
    char username[MAX_USERNAME_LENGTH];
    char inputAnswer[MAX_SECRET_ANSWER_LENGTH];

    printf("请输入要找回密码的用户名: ");
    scanf("%s", username);
    User* current = userHead;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            printf("密保问题: %s\n", current->secretQuestion);
            printf("请输入密保答案: ");
            scanf("%s", inputAnswer);
            if (strcmp(current->secretAnswer, inputAnswer) == 0) {
                printf("您的原密码是: %s\n", current->password);
            }
            else {
                printf("密保答案错误，找回失败。\n");
            }
            return;
        }
        current = current->next;
    }
    printf("未找到该用户名，找回失败。\n");
}

//创建系统初始化账号admin
void AccountSrvInitSys() {
    //User admin;
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    int type;
    char secretQuestion[MAX_SECRET_QUESTION_LENGTH];
    char secretAnswer[MAX_SECRET_ANSWER_LENGTH];

    printf("系统检测到您是第一次登录请设置系统管理员账号密码\n");
    printf("请输入用户名: ");
    scanf("%s", username);

    printf("请输入密码: ");
    char ch;
    int i = 0;
    while (1) {
        ch = _getch(); // 从控制台读取单个字符，不回显
        if (ch == '\r') {
            // 当按下回车键（\r 表示回车），结束输入
            password[i] = '\0'; // 为密码字符串添加结束符
            printf("\n");
            break;
        }
        else if (ch == '\b' && i > 0) {
            // 处理退格键，且保证不是第一个字符
            i--;
            printf("\b \b"); // 退格，输出空格覆盖星号，再退格
        }
        else {
            password[i++] = ch;
            printf("*"); // 输出*代替实际输入字符
        }
    }

    type = 3;
    printf("请输入邮箱: ");
    scanf("%s", email);
    printf("请设置密保问题: ");
    scanf("%s", secretQuestion);
    printf("请设置密保答案: ");
    scanf("%s", secretAnswer);

    User* newUser = (User*)malloc(sizeof(User));
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    strcpy(newUser->email, email);
    newUser->type = type;
    strcpy(newUser->secretQuestion, secretQuestion);
    strcpy(newUser->secretAnswer, secretAnswer);
    newUser->prev = userTail;
    newUser->next = NULL;
    if (userTail == NULL) {
        userHead = newUser;
    }
    else {
        userTail->next = newUser;
    }
    userTail = newUser;

    saveUsersToFile();
    printf("注册成功！\n");
}


// 登录功能
int login() {

    char choice;
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    while (1) {
        system("cls");
        printf("\n1. 正常登录\n2. 基于密保修改密码\n3. 基于密保找回密码\n4. 返回上一级\n请选择: ");
        //scanf("%d", &choice);
        choice = getchar();
        switch (choice) {
        case '1':
            printf("请输入用户名: ");
            scanf("%s", username);
            
            printf("请输入密码: ");
            char ch;
            int i = 0;
            while (1) {
                ch = _getch(); // 从控制台读取单个字符，不回显
                if (ch == '\r') {
                    // 当按下回车键（\r 表示回车），结束输入
                    password[i] = '\0'; // 为密码字符串添加结束符
                    printf("\n");
                    break;
                }
                else if (ch == '\b' && i > 0) {
                    // 处理退格键，且保证不是第一个字符
                    i--;
                    printf("\b \b"); // 退格，输出空格覆盖星号，再退格
                }
                else {
                    password[i++] = ch;
                    printf("*"); // 输出*代替实际输入字符
                }
            }
            
            /*printf("请输入密码: ");
            scanf("%s", password);*/
            User* current = userHead;
            while (current != NULL) {
                if (strcmp(current->username, username) == 0 && strcmp(current->password, password) == 0) {
                    return current->type;
                }
                current = current->next;
            }
            printf("用户名或密码错误，登录失败。\n");
            break;
        case '2':
            modifyPasswordWithSecret();
            break;
        case '3':
            retrievePasswordWithSecret();
            break;
        case '4':
            return 0;
        default:
            printf("无效的选择，请重新输入。\n");
        }
    }
}

// 添加学生信息到双向链表
//void addStudent(Student *newStudent) {
//if (studentHead == NULL) {
//    studentHead = newStudent;
//    studentTail = newStudent;
//    newStudent->prev = NULL;
//    newStudent->next = NULL;
//}
//else {
//    newStudent->prev = studentTail;
//    newStudent->next = NULL;
//   studentTail->next = newStudent;
//   studentTail = newStudent;
//}
//}

// 显示学生列表某一页
void displayStudentPage(int pageNumber) {
    Student* current = studentHead;
    int startIndex = (pageNumber - 1) * PAGE_SIZE;
    int endIndex = startIndex + PAGE_SIZE - 1;
    int index = 0;
    while (current != NULL && index < startIndex) {
        current = current->next;
        index++;
    }
    printf("学生列表 - 第 %d 页\n", pageNumber);
    while (current != NULL && index <= endIndex) {
        printf("学号: %d, 用户名: %s, 成绩: %d, %d, %d\n",
            current->studentId, current->user.username,
            current->scores[0], current->scores[1], current->scores[2]);
        current = current->next;
        index++;
    }
}

// 计算总成绩
int calculateTotalScore(Student* student) {
    int total = 0;
    for (int i = 0; i < 3; i++) {
        total += student->scores[i];
    }
    return total;
}

// 计算总成绩排名
int calculateTotalRank(Student* target) {
    int rank = 1;
    int targetTotal = calculateTotalScore(target);
    Student* current = studentHead;
    while (current != NULL) {
        if (calculateTotalScore(current) > targetTotal) {
            rank++;
        }
        current = current->next;
    }
    return rank;
}

// 计算某一科成绩排名
int calculateSubjectRank(Student* target, int subjectIndex) {
    int rank = 1;
    int targetScore = target->scores[subjectIndex];
    Student* current = studentHead;
    while (current != NULL) {
        if (current->scores[subjectIndex] > targetScore) {
            rank++;
        }
        current = current->next;
    }
    return rank;
}

// 成绩查询功能
void studentGradeQuery() {
    char username[MAX_USERNAME_LENGTH];
    printf("请输入要查询成绩的学生用户名: ");
    scanf("%s", username);
    Student* current = studentHead;
    while (current != NULL) {
        if (strcmp(current->user.username, username) == 0) {
            int totalScore = calculateTotalScore(current);
            int totalRank = calculateTotalRank(current);
            printf("学号: %d, 用户名: %s\n", current->studentId, current->user.username);
            printf("总成绩: %d, 总成绩排名: %d\n", totalScore, totalRank);
            const char* subjectNames[3] = { "语文", "数学", "英语" };
            for (int i = 0; i < 3; i++) {
                int subjectRank = calculateSubjectRank(current, i);
                printf("%s成绩: %d, %s成绩排名: %d\n", subjectNames[i], current->scores[i], subjectNames[i], subjectRank);
            }
            return;
        }
        current = current->next;
    }
    printf("未找到该学生信息。\n");
}

// 交换两个学生节点的数据
void swapStudents(Student* a, Student* b) {
    Student temp = *a;
    a->studentId = b->studentId;
    strcpy(a->user.username, b->user.username);
    strcpy(a->user.password, b->user.password);
    strcpy(a->user.email, b->user.email);
    a->user.type = b->user.type;
    strcpy(a->user.secretQuestion, b->user.secretQuestion);
    strcpy(a->user.secretAnswer, b->user.secretAnswer);
    for (int i = 0; i < 3; i++) {
        a->scores[i] = b->scores[i];
    }

    b->studentId = temp.studentId;
    strcpy(b->user.username, temp.user.username);
    strcpy(b->user.password, temp.user.password);
    strcpy(b->user.email, temp.user.email);
    b->user.type = temp.user.type;
    strcpy(b->user.secretQuestion, temp.user.secretQuestion);
    strcpy(b->user.secretAnswer, temp.user.secretAnswer);
    for (int i = 0; i < 3; i++) {
        b->scores[i] = temp.scores[i];
    }
}

// 查询本班成绩功能
void studentClassGradeQuery() {
    printf("本班学生成绩如下:\n");
    Student* current = studentHead;
    while (current != NULL) {
        printf("学号: %d, 用户名: %s, 成绩: %d, %d, %d\n",
            current->studentId, current->user.username,
            current->scores[0], current->scores[1], current->scores[2]);
        current = current->next;
    }

    // 总成绩排序
    for (int i = 0; i < studentCount - 1; i++) {
        current = studentHead;
        for (int j = 0; j < studentCount - i - 1; j++) {
            if (calculateTotalScore(current) < calculateTotalScore(current->next)) {
                swapStudents(current, current->next);
            }
            current = current->next;
        }
    }

    // 输出总成绩排名
    printf("总成绩排名：\n");
    current = studentHead;
    int rank = 1;
    while (current != NULL) {
        int totalScore = calculateTotalScore(current);
        printf("排名: %d, 学号: %d, 用户名: %s, 总成绩: %d\n", rank++, current->studentId, current->user.username, totalScore);
        current = current->next;
    }

    // 输出单科排名
    const char* subjectNames[3] = { "语文", "数学", "英语" };
    for (int subjectIndex = 0; subjectIndex < 3; subjectIndex++) {
        // 按单科成绩排序
        for (int i = 0; i < studentCount - 1; i++) {
            current = studentHead;
            for (int j = 0; j < studentCount - i - 1; j++) {
                if (current->scores[subjectIndex] < current->next->scores[subjectIndex]) {
                    swapStudents(current, current->next);
                }
                current = current->next;
            }
        }

        // 输出单科排名
        printf("\n%s成绩排名：\n", subjectNames[subjectIndex]);
        current = studentHead;
        rank = 1;
        while (current != NULL) {
            printf("排名: %d, 学号: %d, 用户名: %s, %s成绩: %d\n", rank++, current->studentId, current->user.username, subjectNames[subjectIndex], current->scores[subjectIndex]);
            current = current->next;
        }
    }
}

// 成绩分析功能
void studentGradeAnalysis() {
    //全班总成绩平均分
    int totalScore = 0;
    int count = 0;
    Student* current = studentHead;
    while (current != NULL) {
        int studentTotal = current->scores[0] + current->scores[1] + current->scores[2];
        totalScore += studentTotal;
        count++;
        current = current->next;
    }
    if (count == 0) {
        printf("暂无学生成绩数据。\n");
        return;
    }
    double averageScores = (double)totalScore / (count * 3);
    printf("全班总学生：%d人\n", studentCount);
    printf("全班平均成绩: %.2f分\n", averageScores);

    //各科目的平均分、最高分、最低分
    int totalScores[3] = { 0,0,0 };
    int maxScores[3] = { 0,0,0 };
    int minScores[3] = { 100,100,100 };

    current = studentHead;
    while (current != NULL) {
        for (int j = 0; j < 3; j++) {
            totalScores[j] += current->scores[j];
            if (current->scores[j] > maxScores[j]) {
                maxScores[j] = current->scores[j];
            }
            if (current->scores[j] < minScores[j]) {
                minScores[j] = current->scores[j];
            }
        }
        current = current->next;
    }

    const char* subjectNames[3] = { "语文", "数学", "英语" };

    for (int j = 0; j < 3; j++) {
        double averageScore = (double)totalScores[j] / studentCount;
        printf("%s平均分: %.2f分, 最高分: %d分, 最低分: %d分\n", subjectNames[j], averageScore, maxScores[j], minScores[j]);
    }

    // 成绩分布统计
    int scoreRanges[3][5] = { 0 };  // 3 门科目，5 个分数段

    current = studentHead;
    while (current != NULL) {
        for (int j = 0; j < 3; j++) {
            int score = current->scores[j];
            if (score < 60) {
                scoreRanges[j][0]++;
            }
            else if (score < 70) {
                scoreRanges[j][1]++;
            }
            else if (score < 80) {
                scoreRanges[j][2]++;
            }
            else if (score < 90) {
                scoreRanges[j][3]++;
            }
            else {
                scoreRanges[j][4]++;
            }
        }
        current = current->next;
    }

    const char* rangeNames[5] = { "0 - 59", "60 - 69", "70 - 79", "80 - 89", "90 - 100" };

    for (int j = 0; j < 3; j++) {
        printf("%s成绩分布：\n", subjectNames[j]);
        for (int k = 0; k < 5; k++) {
            printf("%s 分: %d 人\n", rangeNames[k], scoreRanges[j][k]);
        }
        printf("\n");
    }
}

// 成绩申诉功能
void studentGradeAppeal() {
    char username[MAX_USERNAME_LENGTH];
    int subjectIndex;
    char reason[200];

    printf("请输入要申诉成绩的学生用户名: ");
    scanf("%s", username);
    Student* current = studentHead;
    while (current != NULL) {
        if (strcmp(current->user.username, username) == 0) {
            printf("请选择申诉科目 (0: 语文, 1: 数学, 2: 英语): ");
            scanf("%d", &subjectIndex);
            if (subjectIndex < 0 || subjectIndex > 2) {
                printf("无效的科目选择，请输入 0 - 2 之间的数字。\n");
                return;
            }
            printf("请输入申诉理由: ");
            scanf(" %[^\n]", reason);

            Appeal* newAppeal = (Appeal*)malloc(sizeof(Appeal));
            strcpy(newAppeal->username, username);
            newAppeal->subjectIndex = subjectIndex;
            strcpy(newAppeal->reason, reason);
            newAppeal->status = PENDING;
            newAppeal->next = appealHead;
            appealHead = newAppeal;

            saveAppealsToFile();

            printf("成绩申诉已记录，请等待处理。\n");
            return;
        }
        current = current->next;
    }
    printf("未找到该学生信息。\n");
}

// 管理员处理申诉功能
void adminProcessAppeal() {
    if (appealHead == NULL) {
        printf("暂无待处理的成绩申诉。\n");
        return;
    }

    Appeal* current = appealHead;
    int count = 1;
    while (current != NULL) {
        if (current->status == PENDING) {
            const char* subjectNames[3] = { "语文", "数学", "英语" };
            printf("申诉 %d:\n", count);
            printf("学生用户名: %s\n", current->username);
            printf("申诉科目: %s\n", subjectNames[current->subjectIndex]);
            printf("申诉理由: %s\n", current->reason);

            int choice;
            printf("请选择处理方式 (1: 已处理, 2: 驳回): ");
            scanf("%d", &choice);
            if (choice == 1) {
                current->status = PROCESSED;
                printf("申诉已标记为已处理。\n");
            }
            else if (choice == 2) {
                current->status = REJECTED;
                printf("申诉已驳回。\n");
            }
            else {
                printf("无效的选择，请重新输入。\n");
            }
            count++;
        }
        current = current->next;
    }
}

// 反馈申诉结果给学生
void feedbackAppealResult() {
    Appeal* current = appealHead;
    while (current != NULL) {
        if (current->status != PENDING) {
            const char* subjectNames[3] = { "语文", "数学", "英语" };
            const char* statusNames[3] = { "待处理", "已处理", "驳回" };
            printf("学生 %s 关于 %s 的成绩申诉结果: %s\n", current->username, subjectNames[current->subjectIndex], statusNames[current->status]);
        }
        current = current->next;
    }
}

// 申诉统计功能
void appealStatistics() {
    int pendingCount = 0, processedCount = 0, rejectedCount = 0;
    Appeal* current = appealHead;
    while (current != NULL) {
        if (current->status == PENDING) {
            pendingCount++;
        }
        else if (current->status == PROCESSED) {
            processedCount++;
        }
        else if (current->status == REJECTED) {
            rejectedCount++;
        }
        current = current->next;
    }
    printf("待处理申诉数量: %d\n", pendingCount);
    printf("已处理申诉数量: %d\n", processedCount);
    printf("驳回申诉数量: %d\n", rejectedCount);
}

// 增加新的学生信息
void addNewStudent() {
    int studentId;
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    int scores[3];

    printf("请输入学生学号: ");
    scanf("%d", &studentId);
    printf("请输入学生用户名: ");
    scanf("%s", username);
    
    printf("请输入密码: ");
    char ch;
    int i = 0;
    while (1) {
        ch = _getch(); // 从控制台读取单个字符，不回显
        if (ch == '\r') {
            // 当按下回车键（\r 表示回车），结束输入
            password[i] = '\0'; // 为密码字符串添加结束符
            printf("\n");
            break;
        }
        else if (ch == '\b' && i > 0) {
            // 处理退格键，且保证不是第一个字符
            i--;
            printf("\b \b"); // 退格，输出空格覆盖星号，再退格
        }
        else {
            password[i++] = ch;
            printf("*"); // 输出*代替实际输入字符
        }
    }
    
    
    
    /*printf("请输入学生密码: ");
    scanf("%s", password);*/
    printf("请输入学生邮箱: ");
    scanf("%s", email);
    int type = 1;  // 学生类型
    studentCount += 1;
    /*printf("请输入学生三门课程成绩（语、数、英）: ");
    scanf("%d %d %d", &scores[0], &scores[1], &scores[2]);*/  
    while (1) {
        char input[100];
        printf("请输入学生三门课程成绩（语、数、英）: ");
        fgets(input, sizeof(input), stdin);
        if (sscanf(input, "%d %d %d", &scores[0], &scores[1], &scores[2]) == 3) {
            break;
        }
        else {
            printf("输入无效，请输入三个整数作为成绩。\n");
        }
    }
    //addStudent(newStudent);

    Student* newStudent = malloc(sizeof(Student));
    newStudent->studentId = studentId;
    strcpy(newStudent->user.username, username);
    strcpy(newStudent->user.password, password);
    strcpy(newStudent->user.email, email);
    newStudent->user.type = type;
    newStudent->scores[0] = scores[0];
    newStudent->scores[1] = scores[1];
    newStudent->scores[2] = scores[2];

    newStudent->prev = studentTail;
    newStudent->next = NULL;
    if (studentTail == NULL) {
        studentHead = newStudent;
    }
    else {
        studentTail->next = newStudent;
    }
    studentTail = newStudent;

    saveStudentsToFile();
    printf("新学生信息添加成功！\n");
}

// 删除学生信息
void deleteStudent() {
    char username[MAX_USERNAME_LENGTH];
    printf("请输入要删除的学生用户名: ");
    scanf("%s", username);
    Student* current = studentHead;
    while (current != NULL) {
        if (strcmp(current->user.username, username) == 0) {
            if (current->prev != NULL) {
                current->prev->next = current->next;
            }
            else {
                studentHead = current->next;
            }
            if (current->next != NULL) {
                current->next->prev = current->prev;
            }
            else {
                studentTail = current->prev;
            }
            free(current);
            saveStudentsToFile();
            printf("学生信息删除成功！\n");
            return;
        }
        current = current->next;
    }
    printf("未找到该学生信息，删除失败。\n");
}

// 修改学生信息
void modifyStudent() {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    printf("请输入要修改的学生用户名: ");
    scanf("%s", username);
    Student* current = studentHead;
    while (current != NULL) {
        if (strcmp(current->user.username, username) == 0) {
            printf("请输入新的学生学号: ");
            scanf("%d", &current->studentId);
            printf("请输入新的学生用户名: ");
            scanf("%s", current->user.username);
            printf("请输入新的学生密码: ");
            //scanf("%s", current->user.password);
            char ch;
            int i = 0;
            while (1) {
                ch = _getch(); // 从控制台读取单个字符，不回显
                if (ch == '\r') {
                    // 当按下回车键（\r 表示回车），结束输入
                    password[i] = '\0'; // 为密码字符串添加结束符
                    printf("\n");
                    break;
                }
                else if (ch == '\b' && i > 0) {
                    // 处理退格键，且保证不是第一个字符
                    i--;
                    printf("\b \b"); // 退格，输出空格覆盖星号，再退格
                }
                else {
                    password[i++] = ch;
                    printf("*"); // 输出*代替实际输入字符
                }
            }
            strcpy(current->user.password, password);


            printf("请输入新的学生邮箱: ");
            scanf("%s", current->user.email);
            printf("请输入新的学生三门课程成绩: ");
            scanf("%d %d %d", &current->scores[0], &current->scores[1], &current->scores[2]);
            saveStudentsToFile();
            printf("学生信息修改成功！\n");
            return;
        }
        current = current->next;
    }
    printf("未找到该学生信息，修改失败。\n");
}

// 查询学生信息
void queryStudent() {
    char username[MAX_USERNAME_LENGTH];
    printf("请输入要查询的学生用户名: ");
    scanf("%s", username);
    Student* current = studentHead;
    while (current != NULL) {
        if (strcmp(current->user.username, username) == 0) {
            printf("学号: %d, 用户名: %s, 密码: ******, 邮箱: %s, 成绩: %d, %d, %d\n",
                current->studentId, current->user.username, /*current->user.password*/ current->user.email,
                current->scores[0], current->scores[1], current->scores[2]);
            return;
        }
        current = current->next;
    }
    printf("未找到该学生信息。\n");
}

// 输出本班成绩
void outputClassGrades() {
    studentClassGradeQuery();
}


// 提醒管理员进行成绩的更新
//void remindAdminForUpdate() {
//    printf("已提醒管理员进行成绩更新，请等待处理。\n");
//}

// 管理员功能：管理员注册
void adminPasswordRegister() {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    int type;
    char secretQuestion[MAX_SECRET_QUESTION_LENGTH];
    char secretAnswer[MAX_SECRET_ANSWER_LENGTH];

    printf("请输入要注册的管理员用户名: ");
    //scanf("%s", username);
    gets(username);

    User* current = userHead;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            printf("用户名已存在，请重新输入。\n");
            return;
        }
        current = current->next;
    }

    printf("请输入密码: ");
    char ch;
    int i = 0;
    while (1) {
        ch = _getch(); // 从控制台读取单个字符，不回显
        if (ch == '\r') {
            // 当按下回车键（\r 表示回车），结束输入
            password[i] = '\0'; // 为密码字符串添加结束符
            printf("\n");
            break;
        }
        else if (ch == '\b' && i > 0) {
            // 处理退格键，且保证不是第一个字符
            i--;
            printf("\b \b"); // 退格，输出空格覆盖星号，再退格
        }
        else {
            password[i++] = ch;
            printf("*"); // 输出*代替实际输入字符
        }
    }

    /*printf("请输入密码: ");
    scanf("%19s", password);*/
    type = 3;
    printf("请输入邮箱: ");
    //scanf("%s", email);
    gets(email);
    printf("请设置密保问题: ");
    gets(secretQuestion);
    // scanf("%s", secretQuestion);
    printf("请设置密保答案: ");
    scanf("%s", secretAnswer);

    User* newUser = (User*)malloc(sizeof(User));
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    strcpy(newUser->email, email);
    newUser->type = type;
    strcpy(newUser->secretQuestion, secretQuestion);
    strcpy(newUser->secretAnswer, secretAnswer);
    newUser->prev = userTail;
    newUser->next = NULL;
    if (userTail == NULL) {
        userHead = newUser;
    }
    else {
        userTail->next = newUser;
    }
    userTail = newUser;

    saveUsersToFile();
    printf("注册成功！\n");
}

// 管理员功能：密码修改
void adminPasswordModify() {
    char username[MAX_USERNAME_LENGTH];
    char newPassword[MAX_PASSWORD_LENGTH];
    printf("请输入要修改密码的用户名: ");
    if (scanf("%19s", username) != 1) {
        printf("输入格式错误，请输入有效的用户名。\n");
        while (getchar() != '\n'); // 清空输入缓冲区
        return;
    }

    User* current = userHead;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            //printf("请输入新密码: ");
            //if (scanf("%19s", newPassword) != 1) {
            //    printf("输入格式错误，请输入有效的密码。\n");
            //    while (getchar() != '\n'); // 清空输入缓冲区
            //    return;
            //}

            int i = 0;
            char ch;

            printf("请输入新密码: ");
            while (1) {
                ch = _getch();  // 从控制台读取单个字符，不回显
                if (ch == '\r') {  // 当按下回车键（\r 表示回车），结束输入
                    newPassword[i] = '\0';  // 为密码字符串添加结束符
                    printf("\n");
                    break;
                }
                else if (ch == '\b' && i > 0) {  // 处理退格键，且保证不是第一个字符
                    i--;
                    printf("\b \b");  // 退格，输出空格覆盖星号，再退格
                }
                else {
                    newPassword[i++] = ch;
                    printf("*");  // 输出*代替实际输入字符
                }
            }

            strcpy(current->password, newPassword);
            saveUsersToFile();
            printf("密码修改成功！\n");
            return;
        }
        current = current->next;
    }
    printf("未找到该用户名，修改失败。\n");
}

// 管理员功能：密码找回
void adminPasswordRetrieve() {
    char username[MAX_USERNAME_LENGTH];
    printf("请输入要找回密码的用户名: ");
    scanf("%s", username);
    User* current = userHead;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            // 简单处理，这里直接输出默认密码，实际可通过邮箱等方式找回
            printf("%s\n",current->password);
            return;
        }
        current = current->next;
    }
    printf("未找到该用户名，找回失败。\n");
}

// 管理员功能：更新成绩库
void adminUpdateGradeDatabase() {
    FILE* file = fopen("students_grades.txt", "w");
    if (file == NULL) {
        perror("无法打开文件来更新成绩库");
        return;
    }
    Student* current = studentHead;
    while (current != NULL) {
        fprintf(file, "%d %s %d %d %d\n", current->studentId, current->user.username,
            current->scores[0], current->scores[1], current->scores[2]);
        current = current->next;
    }
    fclose(file);
    printf("成绩库更新成功！\n");
}

// 管理员功能：进行教师信息修改
void adminEnterTeacherModifyInfo() {
    char username[MAX_USERNAME_LENGTH];
    printf("请输入要修改信息的教师用户名: ");
    if (scanf("%19s", username) != 1) {
        printf("输入格式错误，请输入有效的用户名。\n");
        while (getchar() != '\n'); // 清空输入缓冲区
        return;
    }

    User* current = userHead;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0 && current->type == 2) {
            char newPassword[MAX_PASSWORD_LENGTH];
            char newEmail[MAX_EMAIL_LENGTH];
            //printf("请输入新密码: ");
            //if (scanf("%19s", newPassword) == 1) {
            //    strcpy(current->password, newPassword);
            //}
            //else {
            //    while (getchar() != '\n'); // 清空输入缓冲区
            //}

            int i = 0;
            char ch;

            printf("请输入新密码: ");
            while (1) {
                ch = _getch();  // 从控制台读取单个字符，不回显
                if (ch == '\r') {  // 当按下回车键（\r 表示回车），结束输入
                    newPassword[i] = '\0';  // 为密码字符串添加结束符
                    printf("\n");
                    break;
                }
                else if (ch == '\b' && i > 0) {  // 处理退格键，且保证不是第一个字符
                    i--;
                    printf("\b \b");  // 退格，输出空格覆盖星号，再退格
                }
                else {
                    newPassword[i++] = ch;
                    printf("*");  // 输出*代替实际输入字符
                }
            }

            printf("请输入新邮箱: ");
            if (scanf("%49s", newEmail) == 1) {
                strcpy(current->email, newEmail);
            }
            else {
                while (getchar() != '\n'); // 清空输入缓冲区
            }
            saveUsersToFile();
            printf("教师信息修改成功！\n");
            return;
        }
        current = current->next;
    }
    printf("未找到该教师用户名，修改失败。\n");
}

// 管理员功能：增加用户信息
void adminAddAccountAndPassword() {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    int type;
    char secretQuestion[MAX_SECRET_QUESTION_LENGTH];
    char secretAnswer[MAX_SECRET_ANSWER_LENGTH];
    printf("请输入要注册的用户名: ");
    if (scanf("%19s", username) != 1) {
        printf("输入格式错误，请输入有效的用户名。\n");
        while (getchar() != '\n'); // 清空输入缓冲区
        return;
    }
    User* current = userHead;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            printf("用户名已存在，请重新输入。\n");
            return;
        }
        current = current->next;
    }

    //printf("请输入密码: ");
    //if (scanf("%19s", password) != 1) {
    //    printf("输入格式错误，请输入有效的密码。\n");
    //    while (getchar() != '\n'); // 清空输入缓冲区
    //    return;
    //}

    printf("请输入密码: ");
    char ch;
    int i = 0;
    while (1) {
        ch = _getch(); // 从控制台读取单个字符，不回显
        if (ch == '\r') {
            // 当按下回车键（\r 表示回车），结束输入
            password[i] = '\0'; // 为密码字符串添加结束符
            printf("\n");
            break;
        }
        else if (ch == '\b' && i > 0) {
            // 处理退格键，且保证不是第一个字符
            i--;
            printf("\b \b"); // 退格，输出空格覆盖星号，再退格
        }
        else {
            password[i++] = ch;
            printf("*"); // 输出*代替实际输入字符
        }
    }

    printf("请输入邮箱: ");
    if (scanf("%49s", email) != 1) {
        printf("输入格式错误，请输入有效的邮箱。\n");
        while (getchar() != '\n'); // 清空输入缓冲区
        return;
    }
    printf("请选择用户类型 (1:学生, 2:教师, 3:管理员): ");
    if (scanf("%d", &type) != 1 || (type < 1 || type > 3)) {
        printf("输入格式错误，请输入有效的用户类型（1 - 3）。\n");
        while (getchar() != '\n'); // 清空输入缓冲区
        return;
    }
    printf("请设置密保问题: ");
    scanf("%s", secretQuestion);
    printf("请设置密保答案: ");
    scanf("%s", secretAnswer);
    /*User* newUser = (User*)malloc(sizeof(User));
    if (newUser == NULL) {
        printf("内存分配失败，无法完成注册。\n");
        return;
    }
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    strcpy(newUser->email, email);
    newUser->type = type;
    newUser->prev = userTail;
    newUser->next = NULL;
    if (userTail == NULL) {
        userHead = newUser;
    }
    else {
        userTail->next = newUser;
    }
    userTail = newUser;
    saveUsersToFile();
    printf("账号注册成功！\n");*/

    User* newUser = (User*)malloc(sizeof(User));
    if (newUser == NULL) {
        printf("内存分配失败，无法完成注册。\n");
        return;
    }
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    strcpy(newUser->email, email);
    newUser->type = type;
    strcpy(newUser->secretQuestion, secretQuestion);
    strcpy(newUser->secretAnswer, secretAnswer);
    newUser->prev = userTail;
    newUser->next = NULL;
    if (userTail == NULL) {
        userHead = newUser;
    }
    else {
        userTail->next = newUser;
    }
    userTail = newUser;

    saveUsersToFile();
    printf("注册成功！\n");
}

// 管理员功能：删除用户信息
void adminDeleteAccountAndPassword() {
    char username[MAX_USERNAME_LENGTH];
    printf("请输入要删除的用户名: ");
    if (scanf("%19s", username) != 1) {
        printf("输入格式错误，请输入有效的用户名。\n");
        while (getchar() != '\n'); // 清空输入缓冲区
        return;
    }
    User* current = userHead;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            if (current->prev != NULL) {
                current->prev->next = current->next;
            }
            else {
                userHead = current->next;
            }
            if (current->next != NULL) {
                current->next->prev = current->prev;
            }
            else {
                userTail = current->prev;
            }
            free(current);
            saveUsersToFile();
            printf("帐号及密码删除成功！\n");
            return;
        }
        current = current->next;
    }
    printf("未找到该用户名，删除失败。\n");
}

// 管理员功能：查找账号及密码
void adminFindAccountAndPassword() {
    char username[MAX_USERNAME_LENGTH];
    printf("请输入要查找的用户名: ");
    if (scanf("%19s", username) != 1) {
        printf("输入格式错误，请输入有效的用户名。\n");
        while (getchar() != '\n'); // 清空输入缓冲区
        return;
    }
    User* current = userHead;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            printf("用户名: %s, 密码: ******, 邮箱: %s, 用户类型: %d\n",
                current->username, current->email, current->type);
            return;
        }
        current = current->next;
    }
    printf("未找到该用户名。\n");
}

// 管理员功能：录入部分账号及密码
void adminInputAllAccountsAndPasswords() {
    int num;
    printf("请输入要录入的账号数量: ");
    if (scanf("%d", &num) != 1) {
        printf("输入格式错误，请输入有效的数字。\n");
        while (getchar() != '\n'); // 清空输入缓冲区
        return;
    }
    for (int i = 0; i < num; i++) {
        char username[MAX_USERNAME_LENGTH];
        char password[MAX_PASSWORD_LENGTH];
        char email[MAX_EMAIL_LENGTH];
        int type;
        char secretQuestion[MAX_SECRET_QUESTION_LENGTH];
        char secretAnswer[MAX_SECRET_ANSWER_LENGTH];

        printf("请输入第 %d 个账号的用户名: ", i + 1);
        if (scanf("%19s", username) != 1) {
            printf("输入格式错误，请输入有效的用户名。\n");
            while (getchar() != '\n'); // 清空输入缓冲区
            continue;
        }
        User* current = userHead;
        while (current != NULL) {
            if (strcmp(current->username, username) == 0) {
                printf("用户名 %s 已存在，跳过该账号录入。\n", username);
                break;
            }
            current = current->next;
        }
        if (current != NULL) continue;

        //printf("请输入第 %d 个账号的密码: ", i + 1);
        //if (scanf("%19s", password) != 1) {
        //    printf("输入格式错误，请输入有效的密码。\n");
        //    while (getchar() != '\n'); // 清空输入缓冲区
        //    continue;
        //}
        printf("请输入密码: ");
        char ch;
        int i = 0;
        while (1) {
            ch = _getch(); // 从控制台读取单个字符，不回显
            if (ch == '\r') {
                // 当按下回车键（\r 表示回车），结束输入
                password[i] = '\0'; // 为密码字符串添加结束符
                printf("\n");
                break;
            }
            else if (ch == '\b' && i > 0) {
                // 处理退格键，且保证不是第一个字符
                i--;
                printf("\b \b"); // 退格，输出空格覆盖星号，再退格
            }
            else {
                password[i++] = ch;
                printf("*"); // 输出*代替实际输入字符
            }
        }

        printf("请输入第 %d 个账号的邮箱: ", i + 1);
        if (scanf("%49s", email) != 1) {
            printf("输入格式错误，请输入有效的邮箱。\n");
            while (getchar() != '\n'); // 清空输入缓冲区
            continue;
        }
        printf("请输入第 %d 个账号的用户类型 (1:学生, 2:教师, 3:管理员): ", i + 1);
        if (scanf("%d", &type) != 1 || (type < 1 || type > 3)) {
            printf("输入格式错误，请输入有效的用户类型（1 - 3）。\n");
            while (getchar() != '\n'); // 清空输入缓冲区
            continue;
        }

        printf("请设置第 %d 个账号的密保问题: ", i + 1);
        scanf("%s", secretQuestion);
        printf("请设置第 %d 个账号的密保答案: ", i + 1);
        scanf("%s", secretAnswer);

        User* newUser = (User*)malloc(sizeof(User));
        if (newUser == NULL) {
            printf("内存分配失败，无法完成第 %d 个账号的录入。\n", i + 1);
            continue;
        }
        strcpy(newUser->username, username);
        strcpy(newUser->password, password);
        strcpy(newUser->email, email);
        newUser->type = type;
        strcpy(newUser->secretQuestion, secretQuestion);
        strcpy(newUser->secretAnswer, secretAnswer);
        newUser->prev = userTail;
        newUser->next = NULL;
        if (userTail == NULL) {
            userHead = newUser;
        }
        else {
            userTail->next = newUser;
        }
        userTail = newUser;
    }

    saveUsersToFile();
    printf("所有账号及密码录入完成。\n");
}

// 管理员功能：输出所有账号及密码
void adminOutputAllAccountsAndPasswords() {
    User* current = userHead;
    if (current == NULL) {
        printf("暂无账号信息。\n");
        return;
    }
    printf("所有账号及密码信息如下:\n");
    while (current != NULL) {
        printf("用户名: %s, 密码: ******, 邮箱: %s, 用户类型: %d\n",
            current->username, current->email, current->type);
        current = current->next;
    }
}

// 管理员功能：手动重新录入学生信息
void adminReinputStudentInfo() {

    // 先清空原有学生信息
    while (studentHead != NULL) {
        Student* temp = studentHead;
        studentHead = studentHead->next;
        free(temp);
    }
    studentTail = NULL;
    studentCount = 0;

    int num;
    printf("请输入要录入的学生数量: ");
    if (scanf("%d", &num) != 1) {
        printf("输入格式错误，请输入有效的数字。\n");
        while (getchar() != '\n'); // 清空输入缓冲区
        return;
    }

    for (int i = 0; i < num; i++) {
        char password[MAX_PASSWORD_LENGTH];
        Student* newStudent = (Student*)malloc(sizeof(Student));
        if (newStudent == NULL) {
            printf("内存分配失败，无法完成第 %d 个学生信息的录入。\n", i + 1);
            continue;
        }

        printf("请输入第 %d 个学生的学号: ", i + 1);
        if (scanf("%d", &newStudent->studentId) != 1) {
            printf("输入格式错误，请输入有效的学号。\n");
            while (getchar() != '\n'); // 清空输入缓冲区
            free(newStudent);
            continue;
        }

        printf("请输入第 %d 个学生的用户名: ", i + 1);
        if (scanf("%19s", newStudent->user.username) != 1) {
            printf("输入格式错误，请输入有效的用户名。\n");
            while (getchar() != '\n'); // 清空输入缓冲区
            free(newStudent);
            continue;
        }

        printf("请输入第 %d 个学生的密码: ", i + 1);
        //if (scanf("%19s", newStudent->user.password) != 1) {
        //    printf("输入格式错误，请输入有效的密码。\n");
        //    while (getchar() != '\n'); // 清空输入缓冲区
        //    free(newStudent);
        //    continue;
        //}
        char ch;
        int i = 0;
        while (1) {
            ch = _getch(); // 从控制台读取单个字符，不回显
            if (ch == '\r') {
                // 当按下回车键（\r 表示回车），结束输入
                password[i] = '\0'; // 为密码字符串添加结束符
                printf("\n");
                break;
            }
            else if (ch == '\b' && i > 0) {
                // 处理退格键，且保证不是第一个字符
                i--;
                printf("\b \b"); // 退格，输出空格覆盖星号，再退格
            }
            else {
                password[i++] = ch;
                printf("*"); // 输出*代替实际输入字符
            }
        }
        strcpy(newStudent->user.password, password);

        printf("请输入第 %d 个学生的邮箱: ", i + 1);
        if (scanf("%49s", newStudent->user.email) != 1) {
            printf("输入格式错误，请输入有效的邮箱。\n");
            while (getchar() != '\n'); // 清空输入缓冲区
            free(newStudent);
            continue;
        }

        newStudent->user.type = 1;  // 学生类型

        printf("请输入第 %d 个学生的三门课程成绩（用空格分隔）: ", i + 1);
        if (scanf("%d %d %d", &newStudent->scores[0], &newStudent->scores[1], &newStudent->scores[2]) != 3) {
            printf("输入格式错误，请输入有效的成绩。\n");
            while (getchar() != '\n'); // 清空输入缓冲区
            free(newStudent);
            continue;
        }

        if (studentHead == NULL) {
            studentHead = newStudent;
            studentTail = newStudent;
            newStudent->prev = NULL;
            newStudent->next = NULL;
        }
        else {
            newStudent->prev = studentTail;
            newStudent->next = NULL;
            studentTail->next = newStudent;
            studentTail = newStudent;
        }
        studentCount++;
    }

    saveStudentsToFile();
    printf("所有学生信息录入完成。\n");
}

// 学生主界面
void studentMainInterface() {
    int pageNumber = 1;
    char choice;
    do {
        system("cls");
        gettime();
        printf("****************************************\n");
        printf("*               学生主菜单：              *\n");
        printf("*             1. 成绩查询                *\n");
        printf("*             2. 查询本班成绩             *\n");
        printf("*             3. 成绩分析                *\n");
        printf("*             4. 成绩申诉                *\n");
        printf("*             5. 显示学生列表            *\n");
        printf("*             6. 返回上一层              *\n");
        printf("****************************************\n");
        printf("请输入你的选择: ");
        choice = getchar();
        //scanf("%d", &choice);
        switch (choice) {
        case '1':
            system("cls");
            studentGradeQuery();
            break;
        case '2':
            system("cls");
            studentClassGradeQuery();
            break;
        case '3':
            system("cls");
            studentGradeAnalysis();
            break;
        case '4':
            system("cls");
            studentGradeAppeal();
            break;
        case '5':
            system("cls");
            displayStudentPage(pageNumber);
            printf("****************************************\n");
            printf("*              1. 下一页                 *\n");
            printf("*              2. 上一页                 *\n");
            printf("****************************************\n");
            printf("请选择操作: ");
            int pageChoice;
            scanf("%d", &pageChoice);
            if (pageChoice == 1) {
                pageNumber++;
            }
            else if (pageChoice == 2 && pageNumber > 1) {
                pageNumber--;
            }
            break;
        case '6':
            return;
        //case 0:
        //    exit(0);
        //default:
        //    printf("无效的选择，请在 0 - 6 重新输入。\n");
        } 
    } while (choice != '6');
}

// 教师主界面
void teacherMainInterface() {
    char choice;
    do {
        system("cls");
        gettime();
        printf("*****************************************\n");
        printf("*              教师主菜单：               *\n");
        printf("*          1. 增加新的学生信息             *\n");
        printf("*          2. 删除学生信息                *\n");
        printf("*          3. 修改学生信息                *\n");
        printf("*          4. 查询学生信息                *\n");
        printf("*          5. 输出本班成绩                *\n");
        printf("*          6. 下载到文件中                *\n");
        printf("*          7. 成绩分析                   *\n");
        printf("*          8. 返回                      *\n");
        printf("****************************************\n");
        printf("请输入你的选择: ");
        choice = getchar();
        //scanf("%d", &choice);
        switch (choice) {
        case '1':
            system("cls");
            addNewStudent();
            break;
        case '2':
            system("cls");
            deleteStudent();
            break;
        case '3':
            system("cls");
            modifyStudent();
            break;
        case '4':
            system("cls");
            queryStudent();
            break;
        case '5':
            system("cls");
            outputClassGrades();
            break;
        case '6':
            system("cls");
            saveStudentsToFile();
            break;
        case '7':
            system("cls");
            studentGradeAnalysis();
            break;
        /*case '8':
            remindAdminForUpdate();
            break;*/
        case '8':
            return;
        /*default:
            printf("无效的选择，请在0-9中重新输入。\n");*/
        }
    } while (choice != '8');
}

//管理员功能：进行教师修改
void admireEnterTeacher() {
    char choice;
    do {
        system("cls");
        gettime();
        printf("****************************************\n");
        printf("*              1.进入教师端              *\n");
        printf("*              2.进行教师信息修改         *\n");
        printf("*              3.退出                   *\n");
        printf("****************************************\n");
        printf("请输入你的选择: ");
        //scanf("%d", &choice);
        choice = getchar();
        switch (choice) {
        case '1':
            system("cls");
            teacherMainInterface();
        case '2':
            system("cls");
            adminEnterTeacherModifyInfo();
        case '3':
            return;
        /*default:
            printf("无效的选择，请输入 1 - 3 之间的数字。\n");*/
        }
    } while (choice != '3');
}

// 管理员主菜单
void adminMainInterface() {
    char choice;
    do {
        system("cls");
        gettime();
        printf("****************************************\n");
        printf("*             管理员主菜单:               *\n");
        printf("*            1. 管理员注册                 *\n");
        printf("*            2. 密码修改                 *\n");
        printf("*            3. 密码找回                 *\n");
        printf("*            4. 更新成绩库                *\n");
        printf("*            5. 进行教师修改              *\n");
        printf("*            6. 增加帐号及密码             *\n");
        printf("*            7. 删除帐号及密码             *\n");
        printf("*            8. 查找账号及密码             *\n");
        printf("*            9. 录入所有账号及密码          *\n");
        printf("*            10. 输出所有账号及密码         *\n");
        printf("*            11. 手动重新录入学生信息       *\n");
        printf("*            12. 处理成绩申诉              *\n");
        printf("*            13. 反馈申诉结果              *\n");
        printf("*            14. 申诉统计                 *\n");
        printf("*            15. 退出管理员菜单            *\n");
        printf("****************************************\n");
        printf("请输入你的选择: ");
        choice = getchar();
        //if (scanf("%d", &choice) != 1) {
        //    printf("输入格式错误，请输入有效的数字选项。\n");
        //    while (getchar() != '\n'); // 清空输入缓冲区
        //    continue;
        //}
        switch (choice) {
        case '1':
            system("cls");
            adminPasswordRegister();
            break;
        case '2':
            system("cls");
            adminPasswordModify();
            break;
        case '3':
            system("cls");
            adminPasswordRetrieve();
            break;
        case '4':
            system("cls");
            adminUpdateGradeDatabase();
            break;
        case '5':
            system("cls");
            admireEnterTeacher();
            break;
        case '6':
            system("cls");
            adminAddAccountAndPassword();
            break;
        case '7':
            system("cls");
            adminDeleteAccountAndPassword();
            break;
        case '8':
            system("cls");
            adminFindAccountAndPassword();
            break;
        case '9':
            system("cls");
            adminInputAllAccountsAndPasswords();
            break;
        case '10':
            system("cls");
            adminOutputAllAccountsAndPasswords();
            break;
        case '11':
            system("cls");
            adminReinputStudentInfo();
            break;
        case '12':
            system("cls");
            adminProcessAppeal();
            break;
        case '13':
            system("cls");
            feedbackAppealResult();
            break;
        case '14':
            system("cls");
            appealStatistics();
            break;
        case '15':
            return;
        /*default:
            printf("无效的选择，请输入 1 - 15 之间的数字。\n");*/
        }
    } while (choice != '15');
}

// 判断用户文件是否为空的函数
int is_file_empty(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        // 文件打开失败
        return -1;
    }
    // 将文件指针移动到文件末尾
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return -1;
    }
    // 获取文件指针相对于文件开头的偏移量，即文件大小
    long size = ftell(file);
    fclose(file);
    // 如果文件大小为 0，则文件为空
    return size == 0;
}

int main() {
    const char* filename = "users.txt";
    int result = is_file_empty(filename);
    if (result == -1) {
        printf("文件打开失败或操作出错。\n");
    }
    else if (result == 1) {
        printf("文件为空。\n");
        AccountSrvInitSys();
    }
    else {
        printf("文件不为空。\n");
    }

    loadUsersFromFile();
    loadAppealsFromFile();
    loadStudentsFromFile();

    int userType;
    char choice;

    User* cur = userHead;
    if (cur == NULL) {
        printf("error\n");
    }
    else {
        printf("%s\n", cur->username);
    }
    Student* curr = studentHead;
    if (curr == NULL) {
        printf("error\n");
    }
    else {
        printf("%d\n", curr->studentId);
    }

    Sleep(2000);
    do{
        system("cls");
        gettime();
        printf("****************************************\n");
        printf("*               学生管理系统              *\n");
        printf("****************************************\n");
        printf("*                1. 注册                *\n");
        printf("*                2. 登录                *\n");
        printf("*                3. 退出                *\n");
        printf("****************************************\n");
        printf("请输入你的选择: ");
        //scanf("%c", &choice);
        choice = getchar();

        switch (choice) {
        case '1':
            system("cls");
            registerUser();
            break;
        case '2':
            system("cls");
            userType = login();
            if (userType) {
                printf("登录成功，用户类型: %d\n", userType);
            }
            if (userType == 1) {
                system("cls");
                studentMainInterface();
            }
            else if (userType == 2) {
                system("cls");
                teacherMainInterface();
            }
            else if (userType == 3) {
                system("cls");
                adminMainInterface();
            }
            break;
        case 3:
            return 0;
        /*default:
            printf("无效的选择，请重新输入。\n");*/
        }
    } while (choice != '3');
    return 0;
}
