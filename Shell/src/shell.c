#include "../include/shell.h"

char *function_names[7] = {
    "pwd", "cd", "date", "echo", "help", "exit", "ls"};

int (*functions[])(char **) = {
    &pwd_function, &cd_function, &date_function, &echo_function, &help, &exit_function, &ls_function};

// 读取字符串
char *read_line()
{
    size_t size = 512;
    char *buffer = (char *)malloc(sizeof(char) * size);
    int i = 0;
    char ch = 0;
    if (buffer == NULL)
    {
        perror("In file shell.c, buffer can be allocated normally\n");
        free(buffer);
        return NULL;
    }
    
    else
    {
        memset(buffer, 0, sizeof(char) * size);
        while (1)
        {
            ch = getchar();
            if (ch == EOF || ch == '\n')
            {
                // critical
                buffer[i] = '\0';
                return buffer;
            }
            else
            {
                buffer[i++] = ch;
            }

            if (i >= size)
            {
                size *= 2;
                buffer = (char *)realloc(buffer, sizeof(char) * size);
                if (buffer == NULL)
                { // 检查分配是否成功
                    perror("Allocation fault\n");
                    free(buffer);
                    return NULL;
                }
            }
        }
    }
}

// 进入 shell 循环处理
int loop()
{
    while (1)
    {
        char hostname[256];

        char *username = get_user_name();
        char *path = getcwd(NULL, 0);
        char *prefix = (char *)malloc(sizeof(char) * 512);
        memset(prefix, 0, sizeof(char) * 512);
        gethostname(hostname, sizeof(hostname));

        strcat(prefix, username);
        strcat(prefix, "@");
        strcat(prefix, hostname);

        strcat(prefix, ":");

        strcat(prefix, path);

        printf("\033[44;37;5m %s$\033[0m", prefix);

        char *buffer = read_line();
       

        if (buffer == NULL)
        {
            perror("Wrong in readline\n");
            return -1;
        }else if (buffer[0] == NULL){
            free(buffer);
            continue;
        }
        int status = pipe_function(buffer);

        if(status == -1){
            printf("Invalid pipe input\n");
            free(buffer);
            continue;
        }else if(status == 1){
            free(buffer);
            continue;
        }

        status = redirect_function(buffer);
        
        if(status == -1){
            printf("Invalid redirect input\n");
            free(buffer);
            continue;
        }else if(status == 1){
            free(buffer);
            continue;
        }
        char **tokens = split(buffer);
        int size = sizeof(tokens) / sizeof(char *);
        if (tokens == NULL)
        {
            perror("Wrong in split method\n");
            free(buffer);
            return -1;
        }
        int run = execute(tokens);
        free(buffer);
        free(tokens);
    }
    return 1;
}

// 提取指令中的所有参数
char **split(char *instruction)
{
    size_t size = 1024;
    char **tokens = (char **)malloc(sizeof(char *) * size);
    memset(tokens, 0, sizeof(char *) * size);
    int i = 0;
    if (tokens == NULL)
    {
        perror("Allocation fault in split\n");
        free(tokens);
        return NULL;
    }
    else
    {
        char *token = strtok(instruction, " ");

        // 循环读入指令参数
        while (token != NULL)
        {
            // printf("%s\n", token);
            tokens[i++] = token;
            if (i > size)
            {
                size *= 2;
                tokens = (char **)realloc(tokens, sizeof(char *) * size);
            }
            if (tokens == NULL)
            {
                perror("Realloc Fault in tokens\n");
                return NULL;
            }
            token = strtok(NULL, " ");
        }
        // printf("i = %d\n", i);
        // for(int z = 0; z < i; z++){
        //     printf("%s\n", tokens[z]);
        // }
        return tokens;
    }
}

// pwd 获取当前工作路径
int pwd_function(char **tokens)
{
    char *path = getcwd(NULL, 0);
    if (path == NULL)
    {
        perror("Fault happens in getcwd()\n");
        return 0;
    }
    printf("%s\n", path);
    free(path);
    return 1;
}

// cd 切换工作路径
int cd_function(char **tokens)
{

    if (tokens == NULL)
    {
        perror("Invalid input\n");
        return 1;
    }
    if (chdir(tokens[1]) == 0)
    { // 成功
        return 0;
    }
    else
    {// 失败
        printf("-bash: cd: %s: No such file or directory\n", tokens[1]);
        return 1;
    }
}

// date 获取当前系统时间
int date_function(char **tokens)
{
    time_t time_ptr;
    struct tm *tmp_ptr = NULL;

    time(&time_ptr);

    tmp_ptr = localtime(&time_ptr);

    char year[5], month[2], day[3], hour[3], minute[3], second[3];
    sprintf(year, "%d", (tmp_ptr->tm_year + 1900));
    sprintf(month, "%d", (tmp_ptr->tm_mon + 1));

    sprintf(day, "%d", (tmp_ptr->tm_mday));

    sprintf(hour, "%d", (tmp_ptr->tm_hour));

    sprintf(minute, "%d", (tmp_ptr->tm_min));
    sprintf(second, "%d", (tmp_ptr->tm_sec));

    char *timeline = (char *)malloc(sizeof(char) * 30);
    memset(timeline, 0, sizeof(char) * 30);
    strcat(timeline, year);
    strcat(timeline, "-");
    strcat(timeline, month);
    strcat(timeline, "-");
    strcat(timeline, day);
    strcat(timeline, " ");
    strcat(timeline, hour);
    strcat(timeline, ":");
    strcat(timeline, minute);
    strcat(timeline, ":");
    strcat(timeline, day);
    printf("%s\n", timeline);
    free(timeline);
    return 1;
}

// echo 回显
int echo_function(char **tokens)
{
    if (tokens == NULL)
    {
        perror("Input tokens is NULL\n");
        return 0;
    }

    else
    {
        char *res = (char *)malloc(sizeof(char) * 100);

        if (res == NULL)
        {
            perror("Allocation Fault\n");
            free(res);
            return 0;
        }
        memset(res, 0, sizeof(char) * 100);

        if (tokens[1] == NULL)
        {
            printf("\n");
            free(res);
            return 1;
        }

        int i = 1;

        while (tokens[i] != NULL)
        {
            strcat(res, tokens[i]);
            strcat(res, " ");
            i++;
        }

        printf("%s\n", res);
        free(res);
        return 1;
    }
}

// help 展示内部命令
int help(char **tokens)
{
    printf("GNU bash, version 5.0.17(1)-release (x86_64-pc-linux-gnu)\nThese shell commands are defined internally.  Type `help' to see this list.\n");
    printf("----------------------------------------------------------------------------------\n");
    printf("[pwd] show the current working path\n");
    printf("[echo] print the string again\n");
    printf("[cd] convert to a specified path\n");
    printf("[date] print the current time and date\n");
    printf("[ls] check all the files in currnet directory\n");
    printf("----------------------------------------------------------------------------------\n");
    printf("The above commands are implemented, and you can also try other commands which system can support\n");
    printf("Here are some example commands\n");
    printf("[ps] have a look at all the running process\n");
    printf("[ping] ping to some websites to test the network\n");
    printf("Use the man command for information on other programs.\n");
    printf("Also, my shell supports for simple pipes and redirects\n");
    printf("------------------------------------------------------------------------------------\n");
    printf("Have a good time ~\n");
    
    return 1;
}

// 调用外部进程
int outer_process(char **tokens)
{
    int cnt = 0; // tokens 长度
    char *args[3];
    for (int i = 0; i < 3; i++)
    {
        args[i] = NULL;
    }
    while (tokens[cnt] != NULL)
    {
        args[cnt] = tokens[cnt];
        cnt++;
    }

    cnt--;
    int pid = fork();
    if (pid < 0)
    {
        perror("Fork Failure\n");
        return 0;
    }
    else if (pid == 0)
    {

        int flag = execvp(args[0], args);

        if (!flag)
        {
            perror("Wrong command input\n");
            return 0;
        }
    }
    else
    {
        int wc = wait(NULL);
    }
    return 1;
}

// exit 退出 shell 程序
int exit_function(char **tokens)
{
    printf("logout\n");
    free(tokens);
    exit(0);
    return 1;
}

// ls 查看当前目录下所有文件
int ls_function(char **tokens)
{
 
    if (tokens == NULL)
    {
        perror("Invalid NULL input\n");
        return 1;
    }
    int cnt = 0;
 
    int read_hidden_flag = 0;
    char *path = (char*)malloc(sizeof(char) * 512);
    
    if(!path){
        perror("Failue in allocating for path\n");
        
        return 1;
    }
    
    memset(path, 0, sizeof(char) * 512);
  
    while (tokens[cnt] != NULL)
    {
        cnt++;
    }
    if (cnt > 1 && tokens[1][0] != '-')
    {
        strcpy(path, tokens[1]);
    }
    else if (cnt > 1 && strcmp(tokens[1], "-a") == 0)
    {
        path = "./";
        read_hidden_flag = 1;
    }
    else
    {
        path = "./";
    }
    DIR *dir = opendir(path);

    if (dir == NULL)
    {
        perror("Failure in openning dir\n");
        return 1;
    }
    else
    {
        struct dirent *files = NULL;
        while (files = readdir(dir)) // 遍历目录下所有文件
        {
            if (!read_hidden_flag && files->d_name[0] == '.') // 如果不需要显式隐藏文件则忽略
                continue;
            if (files->d_type == DT_DIR) // 目录进行特别显示
            {
                printf("\033[33;46;5m %s \033[0m", files->d_name);
                // printf("%s", files -> d_name);
            }
            else
            {
                char fullpath[NAME_MAX + 1] = {0};
                sprintf(fullpath, "%s %s", path, files->d_name); //将目录下文件连接到目录路径下
                struct stat st = {0};
                stat(fullpath, &st);
                if (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
                { //如果目录下文件有执行权限
                    printf("%s ", files->d_name);
                }
                else
                {
                    printf("%s ", files->d_name);
                }
            }
        }
        printf("\n");
        closedir(dir); //关闭目录
        return 0;
    }
}

// 获取用户名
char *get_user_name()
{
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw)
    {
        return pw->pw_name;
    }
    return "";
}
// 执行命令的接口
int execute(char **tokens)
{
    if (tokens == NULL)
    {
        perror("In execute function, the input instruction is NULL\n");
        return 0;
    }
    else
    {
        char *func_name = tokens[0];
        int total_func = sizeof(function_names) / sizeof(char *);

        for (int i = 0; i < total_func; i++)
        {
            
            if (strcmp(func_name, function_names[i]) == 0)
            {
                int run = (*functions[i])(tokens);
                return run;
            }
        }
        int run = outer_process(tokens);
        return run;
    }
}

// pipe 管道, 返回 0代表没有管道，-1代表进程失败，1代表管道执行成功
int pipe_function(char *line){
    int size = strlen(line);
    int pipe_idx = -1;
    for(int i = 0; i < size; i++){
        if (line[i] == '|'){
            pipe_idx = i;
        }
    }
    if(pipe_idx == -1){ // 表示原指令当中没有管道标志
        return 0;
    }else{
        if(line[pipe_idx - 1] != ' ' || (pipe_idx == size - 1 || line[pipe_idx + 1] != ' ')){ // 表示管道不合法，直接退出程序
            perror("Invalid pipe, missing parameter\n");
            return -1;
        }
    }
    int point[2];
    int success = pipe(point);
    if(success == -1){ // 创建失败
        return -1;
    }
    int pid = fork();

    if(pid < 0){
        perror("Failure fork\n");
        return -1;
    }else if(pid == 0){ // 子进程
       
        close(point[0]); // 关闭读
     
        int s_out = dup(STDOUT_FILENO);
        dup2(point[1], STDOUT_FILENO); // 直接使得标准输出流也指向 point[1]指向的句柄，也就是管道的另一头
        
        close(point[1]);
        // 将管道左边的指令切割下来
        char *left_com = (char *)malloc(sizeof(char) * 512);
        if(left_com == NULL){
            perror("Failure in allocating\n");
            return -1;
        }
        memset(left_com, 0, sizeof(char) * 512);
        for(int i = 0; i < pipe_idx - 1; i++){
            left_com[i] = line[i];
        }
        // printf("The left command is %s\n", left_com);
        char **tokens = split(left_com);
        if(execute(tokens) != 1){
            free(tokens);
            free(left_com);
            return -1;
        }
        free(left_com);
        free(tokens);
        dup2(s_out, STDOUT_FILENO); // 恢复标准输出缓冲区
        exit(0);
    }else{// 父进程
        wait(NULL);
  
        close(point[1]);
        int s_in = dup(STDIN_FILENO);
        dup2(point[0], STDIN_FILENO); // 将管道0作为命令的输入
        close(point[0]);
        int cnt = 0;
        char* right_com = (char*)malloc(sizeof(char) * 512);
        if(right_com == NULL){
            perror("Failure in allocating\n");
            return -1;
        }
        memset(right_com, 0, sizeof(char) * 512);
        
        for(int i = pipe_idx + 2; i < size; i++){
            right_com[cnt++] = line[i];
        }


        char **tokens = split(right_com);
        if(execute(tokens) != 1){
            free(tokens);
            free(right_com);
            return -1;
        }

        free(right_com);
        free(tokens);
        dup2(s_in, STDIN_FILENO); // 恢复标准输入缓冲区
    }
    return 1;
    
}

// redirect 重定向, -1 表示不合法，0 表示没有重定向，1 表示合法调用重定向
int redirect_function(char * line){
    
    int size = strlen(line);

    int output_flag = 0;

    int output_idx = size;

    char *output_file = (char*) malloc(sizeof(char) * 256);
    if(output_file == NULL){
        perror("Allocation failure\n");
    
        return -1;
    }
    char *command = (char*) malloc(sizeof(char) * 32);
    
   
    // 初始化
    // memset(input_file, 0, sizeof(char) * 32);
    memset(output_file, 0, sizeof(char) * 256);
    memset(command, 0, sizeof(char) * 32);

    for(int i = 0; i < size; i++){


        if(line[i] == '>'){
            output_flag = 1;
            output_idx = i;
            if(i == size - 1 || i == size - 2){
                perror("Missing parameter\n");
           
                free(output_file);
                return -1;
            }
        }

    }
    if(!output_flag){
        return 0;
    }
    for(int i = 0; i < output_idx - 1; i++){
        command[i] = line[i];
    }
    int cnt = 0;
    for(int i = output_idx + 2; i < size; i++){
        output_file[cnt++] = line[i];
    }

    int rc = fork();
 
    if(rc < 0){
        perror("Fork Failure\n");

        free(output_file);
        return -1;
    }else if(rc == 0){ // 子进程
        
        if(output_flag){
            freopen(output_file, "w", stdout); 
        }
       
        
        char ** tokens = split(command);
        int status = execute(tokens);
        if(status != 1){
            // free(input_file);
            free(output_file);
            free(tokens);
            exit(1);
            return -1;
        }
        // free(input_file);
        free(tokens);
        free(output_file);
        // free(command);
        
        exit(0);

    }else{ // 父进程
        wait(NULL);

    }

    free(output_file);
    return 1;

    
}

