#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <time.h>
#include <grp.h>
#include <sys/wait.h>
#include <signal.h>
#define HISTORY_MAX_SIZE 20
#define MAX_BUF_SIZE 10000
int childid = -1;
pid_t shellID;
int pp = 0;
int flag = 0;
int initial_hist_size = 0;
char history_path[PATH_MAX];
char temp_dir[PATH_MAX];
char prev_dir[PATH_MAX];
char home_dir[PATH_MAX];
char curr_dir[PATH_MAX];
char *history[HISTORY_MAX_SIZE];
char* PIDNAME[100];
int PIDS[100];
char *delimeter = " \f\n\v\r\t";
void pinfoaddress(char *dir);
void discoveraddress(char *dir);
static int comp(const void *a, const void *b);
void sort(char **sortarray, int n);
char *addressmaker(char *path, char *name);
void addresscompressor(char *path);
void discoverdirall(char *directsfileflag);
void discoveryrecursive(char *directsfileflag[], int n);
void discoverdirdf(char *directsfileflag, char *flag_type);
void discoverdirfile(char *directsfileflag, char *file_folder_name);
void discoverdirdffile(char *directsfileflag, char *flag_type, char *file_folder_name);
void discoverymain(char *str);
void procinfo(char *str);
void printerl(char *directories[], int n);
void printer(char *arr[], int n);
void address_print();
void gatherhistory();
void storehistory();
void historyfn(char *com);
void printhistory();
void ls(char *str);
void cd(char *str);
void echo(char *str);
void pwd();
void fg(char *com);
void bg(char *com[]);
void bgmakeup(char *com[], int n);
void signalfn();
void signalchandler(int s_ID);
void signalzhandler(int s_ID);
void jobs();
void jobs()
{
    char status;
    pid_t id;
    char id_no[20];
    long long count;
    int i,j;
    struct naming
    {
        char* comname;
        int idno;
    };
    typedef struct naming naming;
    naming arr[100];
    // char* comname[100];
    // int idno[100];
    for(i=0;i<100;i++)
    {
        arr[i].comname =  (char*)malloc(sizeof(char)*1000);
    }
    j = 0;
    for(i=0;i<100;i++)
    {
        if(PIDS[i] !=0)
        {
            arr[j].idno  =PIDS[i];
            arr[j++].comname = PIDNAME[i];
        }

    }
    for(i=0;i<j;i++)
    {
        printf("[%d] PROCESS NAME: %s PROCESS ID: %d  ",i,arr[i].comname,arr[i].idno);
        char path[200];
        sprintf(id_no,"%d",arr[i].idno);
        strcpy(path,"/proc/");
        strcat(path,id_no);
        strcat(path,"/stat");
        FILE * fd;
        fd = fopen(path,"r");
        fscanf(fd, "%d %*s %c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %lld", &id, &status, &count);
        if(status == 'T')
        {
            printf("STATUS: STOPPED\n");
            PIDS[i] = 0;
        }
        else
        {
            printf("STATUS: RUNNING\n");
        }
    }
    return;
}
void signalchandler(int s_ID)
{
    // int fg_kill_proc=0;
    // pid_t id = getpid();
    // if(id != shellID)
    // {
    //     return;
    // }
    // else if(childid != -1)
    // {
    //     kill(childid,SIGINT);
    //     fg_kill_proc ++;
    // }
    // signal(SIGINT,signalchandler);
}
void signalzhandler(int s_ID)
{
    int bg_proc = 0;
    pid_t id = getpid();
    if(id != shellID)
    {
        return;
    }
    else if(childid != -1)
    {
        kill(childid,SIGTTIN);
        kill(childid,SIGTSTP);
        bg_proc++;
        PIDS[pp++] = childid;
    }
    signal(SIGTSTP,signalzhandler);
}
void pinfoaddress(char *dir)
{
    int root = 0, i, j, tempo = 0, count = 0;
    char cwd[PATH_MAX], root_handle[PATH_MAX], temp[PATH_MAX];
    root_handle[0] = '\0';
    getcwd(cwd, sizeof(cwd));
    if (strcmp(dir, cwd) == 0)
    {
        printf(".");
        return;
    }
    for (i = 0; i <= strlen(cwd); i++)
    {
        if (cwd[i] == '/')
        {
            root++;
        }
    }
    for (i = 0; i <= strlen(dir); i++)
    {
        if (dir[i] == '/')
        {
            tempo++;
        }
        if (tempo > root)
        {
            // printf("hello2\n");
            temp[i] = '\0';
            if (strcmp(temp, cwd) == 0)
            {
                // printf("hello3\n");
                for (j = i; j <= strlen(dir); j++)
                {
                    root_handle[j - i] = dir[j];
                }
                printf(".%s", root_handle);
                return;
            }
        }
        temp[i] = dir[i];
    }
    strcpy(root_handle, cwd);
    printf("%s", root_handle);
    return;
}
void discoveraddress(char *dir)
{
    // char hostname[10000], username[10000];
    int root = 0, i, j, tempo = 0, count = 0;
    char cwd[PATH_MAX], root_handle[PATH_MAX], temp[PATH_MAX];
    root_handle[0] = '\0';
    getcwd(cwd, sizeof(cwd));
    // gethostname(hostname, HOST_NAME_MAX);
    // getlogin_r(username, LOGIN_NAME_MAX);
    if (strcmp(dir, cwd) == 0)
    {
        printf(".");
        return;
    }
    for (i = 0; i < strlen(dir); i++)
    {
        if (dir[i] == '/')
        {
            root++;
        }
    }
    for (i = 0; i < strlen(cwd); i++)
    {
        if (cwd[i] == '/')
        {
            tempo++;
        }
        if (tempo > root)
        {
            temp[i] = '\0';
            if (strcmp(temp, dir) == 0)
            {
                for (j = i; j <= strlen(cwd); j++)
                {
                    root_handle[j - i] = cwd[j];
                }
                printf(".%s", root_handle);
                return;
            }
        }
        temp[i] = cwd[i];
    }
    strcpy(root_handle, cwd);
    printf("%s", root_handle);
    return;
}
static int comp(const void *a, const void *b)
{
    return strcasecmp(*(const char **)a, *(const char **)b);
}
void sort(char **sortarray, int n)
{
    qsort(sortarray, n, sizeof(const char *), comp);
    return;
}
char *addressmaker(char *path, char *name)
{
    char *make = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
    strcat(make, path);
    strcat(make, "/");
    strcat(make, name);
    return make;
}
void addresscompressor(char *path)
{
    int root = 0, i, j, tempo = 0, count = 0;
    char cwd[PATH_MAX], root_handle[PATH_MAX], temp[PATH_MAX];
    root_handle[0] = '\0';
    if (strcmp(curr_dir, path) == 0)
    {
        printf(".\n");
        return;
    }
    for (i = 0; i < strlen(curr_dir); i++)
    {
        if (curr_dir[i] == '/')
        {
            root++;
        }
    }
    for (i = 0; i < strlen(path); i++)
    {
        if (path[i] == '/')
        {
            tempo++;
        }
        if (tempo > root)
        {
            temp[i] = '\0';
            if (strcmp(temp, curr_dir) == 0)
            {
                for (j = i; j <= strlen(path); j++)
                {
                    root_handle[j - i] = path[j];
                }
                printf(".%s\n", root_handle);
                return;
            }
        }
        temp[i] = path[i];
    }
    strcpy(root_handle, path);
    printf("./%s\n", root_handle);
    return;
}
void discoverdirall(char *directsfileflag)
{
    DIR *dir;
    dir = opendir(directsfileflag);
    if (!dir)
    {
        return;
    }
    struct dirent *entity;
    while ((entity = readdir(dir)) != NULL)
    {
        char *way = addressmaker(directsfileflag, entity->d_name);
        if ((strcmp(entity->d_name, "..") == 0) || (strcmp(entity->d_name, ".") == 0))
        {
            continue;
        }
        addresscompressor(way);
        discoverdirall(way);
    }
    return;
}
void discoverdirdf(char *directsfileflag, char *flag_type)
{
    DIR *dir;
    dir = opendir(directsfileflag);
    if (!dir)
    {
        return;
    }
    struct dirent *entity;
    while ((entity = readdir(dir)) != NULL)
    {
        struct stat var;
        char *way = addressmaker(directsfileflag, entity->d_name);
        if ((strcmp(entity->d_name, "..") == 0) || (strcmp(entity->d_name, ".") == 0))
        {
            continue;
        }
        stat(way, &var);
        if ((strcmp(flag_type, "-f") == 0) && S_ISREG(var.st_mode))
        {
            addresscompressor(way);
        }
        else if ((strcmp(flag_type, "-d") == 0) && S_ISDIR(var.st_mode))
        {
            addresscompressor(way);
        }
        discoverdirdf(way, flag_type);
    }
    return;
}
void discoverdirfile(char *directsfileflag, char *file_folder_name)
{
    DIR *dir;
    dir = opendir(directsfileflag);
    if (!dir)
    {
        return;
    }
    struct dirent *entity;
    while ((entity = readdir(dir)) != NULL)
    {
        struct stat var;
        char *way = addressmaker(directsfileflag, entity->d_name);
        if ((strcmp(entity->d_name, "..") == 0) || (strcmp(entity->d_name, ".") == 0))
        {
            continue;
        }
        stat(way, &var);
        if ((strcmp(file_folder_name, entity->d_name) == 0))
        {
            addresscompressor(way);
        }
        discoverdirfile(way, file_folder_name);
    }
    return;
}
void discoverdirdffile(char *directsfileflag, char *flag_type, char *file_folder_name)
{
    DIR *dir;
    dir = opendir(directsfileflag);
    if (!dir)
    {
        return;
    }
    struct dirent *entity;
    while ((entity = readdir(dir)) != NULL)
    {
        struct stat var;
        char *way = addressmaker(directsfileflag, entity->d_name);
        if ((strcmp(entity->d_name, "..") == 0) || (strcmp(entity->d_name, ".") == 0))
        {
            continue;
        }
        stat(way, &var);
        if ((strcmp(flag_type, "-f") == 0) && S_ISREG(var.st_mode)  && (strcmp(file_folder_name, entity->d_name) == 0))
        {
            addresscompressor(way);
        }
        else if (((strcmp(flag_type, "-d") == 0) && S_ISDIR(var.st_mode)) && (strcmp(file_folder_name, entity->d_name) == 0))
        {
            addresscompressor(way);
        }
        discoverdirdffile(way, flag_type,file_folder_name);
    }
    return;
}
void discoverymain(char *str)
{
    struct stat var;
    int i, j, k, l;
    char *directs[MAX_BUF_SIZE];
    char *files[MAX_BUF_SIZE];
    char *directsfileflag[MAX_BUF_SIZE];
    char *option;

    for (i = 0; i < MAX_BUF_SIZE; i++)
    {
        files[i] = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
        directs[i] = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
        directsfileflag[i] = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
    }
    option = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
    char *command = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
    command = strtok(str, delimeter);
    int c1 = 0, c2 = 0, c3 = 0, count = 0;
    int q = 0, w = 0, e = 0; // file =q,directory=w
    while (1)
    {
        option = strtok(NULL, delimeter);
        stat(option, &var);
        if (option == NULL)
        {
            if (c1 == 0)
            {
                count = 1;
                strcpy(directsfileflag[e], curr_dir);
                strcpy(directs[w], curr_dir);
            }
            break;
        }
        if (strcmp(option, "~") == 0)
        {
            strcpy(directsfileflag[e++], home_dir);
            strcpy(directs[w++], home_dir);
            c1++;
        }
        else if (strcmp(option, "-f") == 0 || strcmp(option, "-d") == 0)
        {
            strcpy(directsfileflag[e++], option);
            c1++;
        }
        else if (S_ISDIR(var.st_mode))
        {
            strcpy(directs[w++], option);
            strcpy(directsfileflag[e++], option);
            c1++;
        }
        else
        {
            strcpy(directsfileflag[e++], option);
            c1++;
        }
    }
    // int iter = e - 1;
    char *temp = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
    if (c1 > 3)
    {
        printf("EXCESS COMMANDS\n");
        return;
    }
    if (c1 == 0) // flag 0
    {
        discoveryrecursive(directsfileflag, 0);
    }
    if (c1 == 1) // flag 11->filename,12->dir_name,13->-d,14->-f
    {
        stat(directsfileflag[0], &var);
        if (strcmp(directsfileflag[0], "-f") == 0)
        {
            strcpy(temp, directsfileflag[0]);
            strcpy(directsfileflag[0], curr_dir);
            strcpy(directsfileflag[1], temp);
            discoveryrecursive(directsfileflag, 14);
        }
        else if (strcmp(directsfileflag[0], "-d") == 0)
        {
            strcpy(temp, directsfileflag[0]);
            strcpy(directsfileflag[0], curr_dir);
            strcpy(directsfileflag[1], temp);
            discoveryrecursive(directsfileflag, 13);
        }
        else if (S_ISDIR(var.st_mode))
        {
            discoveryrecursive(directsfileflag, 12);
        }
        else
        {
            strcpy(temp, directsfileflag[0]);
            strcpy(directsfileflag[0], curr_dir);
            strcpy(directsfileflag[1], temp);
            discoveryrecursive(directsfileflag, 11);
        }
    }
    if (c1 == 2) // flag 211->dir,file;212->dir,-f;213->dir,-d;221->-f,file;222-d,file;231 -f-d/-d-f
    {
        stat(directsfileflag[0], &var);
        if (S_ISDIR(var.st_mode))
        {
            if (strcmp(directsfileflag[1], "-f") == 0)
            {
                discoveryrecursive(directsfileflag, 212);
            }
            else if (strcmp(directsfileflag[1], "-d") == 0)
            {
                discoveryrecursive(directsfileflag, 213);
            }
            else
            {
                discoveryrecursive(directsfileflag, 211);
            }
        }
        else if (strcmp(directsfileflag[0], "-f") == 0)
        {
            if (strcmp(directsfileflag[1], "-d") == 0)
            {
                strcpy(directsfileflag[0], curr_dir);
                discoveryrecursive(directsfileflag, 231);
            }
            else
            {
                strcpy(directsfileflag[2], directsfileflag[1]);
                strcpy(directsfileflag[1], directsfileflag[0]);
                strcpy(directsfileflag[0], curr_dir);
                discoveryrecursive(directsfileflag, 221);
            }
        }
        else if (strcmp(directsfileflag[0], "-d") == 0)
        {
            if (strcmp(directsfileflag[1], "-f") == 0)
            {
                strcpy(directsfileflag[0], curr_dir);
                discoveryrecursive(directsfileflag, 231);
            }
            else
            {
                strcpy(directsfileflag[2], directsfileflag[1]);
                strcpy(directsfileflag[1], directsfileflag[0]);
                strcpy(directsfileflag[0], curr_dir);
                discoveryrecursive(directsfileflag, 222);
            }
        }
        else
        {
            printf("INVALID\n");
        }
    }
    if (c1 == 3) // flag 31->dir,-d,file;32->dir,-f,file
    {
        stat(directsfileflag[0], &var);
        if (S_ISDIR(var.st_mode) && (strcmp(directsfileflag[1], "-d") == 0))
        {
            discoveryrecursive(directsfileflag, 31);
        }
        if (S_ISDIR(var.st_mode) && (strcmp(directsfileflag[1], "-f") == 0))
        {
            discoveryrecursive(directsfileflag, 32);
        }
        else
        {
            printf("INVALID\n");
        }
    }
    return;
}
void discoveryrecursive(char *directsfileflag[], int n)
{
    char *file_name = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
    int i;
    char *curr_stor = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
    strcpy(curr_stor, curr_dir);
    if (n == 0 || n == 12 || n == 231)
    {
        discoverdirall(directsfileflag[0]); // i.e. given just some directory we have to print all
    }
    else if (n == 212 || n == 213 || n == 13 || n == 14) // i.e. given directory and the flags
    {
        discoverdirdf(directsfileflag[0], directsfileflag[1]);
    }
    else if (n == 211 || n == 11) // directory and file given
    {
        char* yo = (char*)malloc(sizeof(char)*MAX_BUF_SIZE);
        if((directsfileflag[1][0] == '\"')&&(directsfileflag[1][strlen(directsfileflag[1])-1] == '\"'))
        {
            for(i=1;i<strlen(directsfileflag[1])-1;i++)
            {
                yo[i-1] = directsfileflag[1][i]; 
            }
            yo[i-1] = '\0';
            discoverdirfile(directsfileflag[0], yo);
        }
        else
        {
            discoverdirfile(directsfileflag[0],directsfileflag[1]);
        }
    }
    else if (n == 221 || n == 222 || n == 31 || n == 32)
    {
         char* yo = (char*)malloc(sizeof(char)*MAX_BUF_SIZE);
        if((directsfileflag[2][0] == '\"')&&(directsfileflag[2][strlen(directsfileflag[2])-1] == '\"'))
        {
            printf("%ld\n",strlen(directsfileflag[2]));
            for(i=1;i<strlen(directsfileflag[2])-1;i++)
            {
                printf("ola!\n");
                yo[i-1] = directsfileflag[2][i]; 
            }
            yo[i-1] = '\0';
            discoverdirdffile(directsfileflag[0], directsfileflag[1], yo); // flags and file given
        }
        else
        {
            discoverdirdffile(directsfileflag[0], directsfileflag[1],directsfileflag[2]); // flags and file given
        }
    }
    strcpy(curr_dir, curr_stor);
    chdir(curr_dir);
    return;
}
void procinfo(char *str)
{
    int i, j, n, k, p = 0;
    pid_t curr_proc_id;
    char *command2, *temp1, *temp2, *path, *buffer;
    char *ids[30];
    char state;
    char status;
    for (int i = 0; i < 30; i++)
    {
        ids[i] = (char *)malloc(sizeof(char *) * 10000);
    }
    command2 = (char *)malloc(10000 * sizeof(char));
    temp1 = (char *)malloc(10000 * sizeof(char));
    temp2 = (char *)malloc(10000 * sizeof(char));
    path = (char *)malloc(MAX_BUF_SIZE * sizeof(char));
    buffer = (char *)malloc(MAX_BUF_SIZE * sizeof(char));
    strcpy(temp1, str);
    strcpy(temp2, str);
    command2 = strtok(temp1, delimeter);
    n = 0;
    i = 0;
    while (1)
    {
        ids[n] = strtok(NULL, delimeter);
        if (ids[n] == NULL)
        {
            break;
        }
        n++;
    }
    if (n == 0)
    {
        curr_proc_id = getpid();
        printf("PID :  %d\n", curr_proc_id);
        sprintf(path, "/proc/%d/status", curr_proc_id);
        FILE *fp = fopen(path, "r");
        if (!fp)
        {
            perror("UNABLE TO GET INFO\n");
        }
        else
        {
            fgets(buffer, 300, fp);
            fgets(buffer, 300, fp);
            fgets(buffer, 300, fp);

            sscanf(buffer, "%c", &status);
            // printf("%c\n", status);
            printf("PROCESS STATUS : %c\n", status);
            fclose(fp);
        }
        sprintf(path, "/proc/%d/statm", curr_proc_id);
        fp = fopen(path, "r");
        if (!fp)
        {
            perror("UNABLE TO GET INFO\n");
        }
        else
        {
            while ((k = fgetc(fp)) != ' ')
                buffer[p++] = k;
            buffer[p] = '\0';
            printf("MEMORY : %s\n", buffer);
            fclose(fp);
        }
        sprintf(path, "/proc/%d/exe", curr_proc_id);
        p = -1;
        p = readlink(path, buffer, MAX_BUF_SIZE - 1);
        // printf("%s\n", buffer);
        if (p == -1)
        {
            perror("UNABLE TO GET INFO\n");
        }
        else
        {
            buffer[p] = '\0';
            printf("EXECUTABLE PATH: ");
            pinfoaddress(buffer);
            printf("\n");
        }
    }
    else
    {
        for (i = 0; i < n; i++)
        {
            curr_proc_id = atoi(ids[i]);
            printf("PID :  %d\n", curr_proc_id);
            sprintf(path, "/proc/%d/status", curr_proc_id);
            FILE *fp = fopen(path, "r");
            if (!fp)
            {
                perror("UNABLE TO GET INFO\n");
            }
            else
            {
                fgets(buffer, 512, fp);
                sscanf(buffer, "%c", &state);
                printf("PROCESS STATUS : %c\n", state);
                fclose(fp);
            }
            sprintf(path, "/proc/%d/statm", curr_proc_id);
            fp = fopen(path, "r");
            if (!fp)
            {
                perror("UNABLE TO GET INFO\n");
            }
            else
            {
                while ((k = fgetc(fp)) != ' ')
                    buffer[p++] = k;
                buffer[p] = '\0';
                printf("MEMORY: %s\n", buffer);
                fclose(fp);
            }
            sprintf(path, "/proc/%d/exe", curr_proc_id);
            p = -1;
            p = readlink(path, buffer, MAX_BUF_SIZE - 1);
            if (p == -1)
            {
                perror("UNABLE TO GET INFO\n");
            }
            else
            {
                buffer[p] = '\0';
                // mod_cwd_rel(buffer);
                printf("EXECUTABLE PATH: ");
                pinfoaddress(buffer);
                printf("\n");
            }
        }
    }
}
void printerl(char *directories[], int n)
{
    int i, j = 0, k = 0;
    struct stat var;
    char *files[n];
    char *folders[n];
    for (i = 0; i < n; i++)
    {
        files[i] = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
        folders[i] = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
    }
    for (i = 0; i < n; i++)
    {
        stat(directories[i], &var);
        if (S_ISDIR(var.st_mode))
        {
            folders[j++] = directories[i];
        }
        else if (S_ISREG(var.st_mode))
        {
            files[k++] = directories[i];
        }
    }
    char *arr1[j];
    char *arr2[k];
    for (i = 0; i < j; i++)
    {
        arr1[i] = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
    }
    for (i = 0; i < k; i++)
    {
        arr2[i] = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
    }
    for (i = 0; i < k; i++)
    {
        stat(files[i], &var);
        struct passwd *pwd = getpwuid(var.st_uid);
        arr2[i][0] = (S_ISREG(var.st_mode) == 0) ? 'd' : '-';
        arr2[i][1] = (var.st_mode & S_IRUSR) ? 'r' : '-';
        arr2[i][2] = (var.st_mode & S_IWUSR) ? 'w' : '-';
        arr2[i][3] = (var.st_mode & S_IXUSR) ? 'x' : '-';
        arr2[i][4] = (var.st_mode & S_IRGRP) ? 'r' : '-';
        arr2[i][5] = (var.st_mode & S_IWGRP) ? 'w' : '-';
        arr2[i][6] = (var.st_mode & S_IXGRP) ? 'x' : '-';
        arr2[i][7] = (var.st_mode & S_IROTH) ? 'r' : '-';
        arr2[i][8] = (var.st_mode & S_IWOTH) ? 'w' : '-';
        arr2[i][9] = (var.st_mode & S_IXOTH) ? 'x' : '-';
        arr2[i][10] = '\0';
        printf("%s\t", arr2[i]);
        printf("%3ld\t%s\t%s\t%6ld\t", var.st_nlink, getpwuid(var.st_uid)->pw_name, getgrgid(var.st_gid)->gr_name, var.st_size);
        struct tm tm, tmnow;
        time_t now = time(NULL);
        localtime_r(&now, &tmnow);
        char time_str[100];
        localtime_r(&var.st_mtime, &tm);

        if (tm.tm_year == tmnow.tm_year)
        {
            strftime(time_str, sizeof(time_str), "%b %e %H:%M", &tm);
            printf("%s  \t", time_str);
        }
        else
        {
            strftime(time_str, sizeof(time_str), "%b %e  %Y", &tm);
            printf("%s  \t", time_str);
        }
        if (var.st_mode & S_IXUSR)
        {
            printf("\033[0;32m%s\033[0m\n", files[i]);
        }
        else
        {
            printf("%s\n", files[i]);
        }
    }
    for (i = 0; i < j; i++)
    {
        stat(folders[i], &var);
        struct passwd *pwd = getpwuid(var.st_uid);
        arr1[i][0] = (S_ISREG(var.st_mode) == 0) ? 'd' : '-';
        arr1[i][1] = (var.st_mode & S_IRUSR) ? 'r' : '-';
        arr1[i][2] = (var.st_mode & S_IWUSR) ? 'w' : '-';
        arr1[i][3] = (var.st_mode & S_IXUSR) ? 'x' : '-';
        arr1[i][4] = (var.st_mode & S_IRGRP) ? 'r' : '-';
        arr1[i][5] = (var.st_mode & S_IWGRP) ? 'w' : '-';
        arr1[i][6] = (var.st_mode & S_IXGRP) ? 'x' : '-';
        arr1[i][7] = (var.st_mode & S_IROTH) ? 'r' : '-';
        arr1[i][8] = (var.st_mode & S_IWOTH) ? 'w' : '-';
        arr1[i][9] = (var.st_mode & S_IXOTH) ? 'x' : '-';
        arr1[i][10] = '\0';
        printf("%s\t", arr1[i]);
        printf("%3ld\t%s\t%s\t%6ld\t", var.st_nlink, getpwuid(var.st_uid)->pw_name, getgrgid(var.st_gid)->gr_name, var.st_size);
        struct tm tm, tmnow;
        time_t now = time(NULL);
        localtime_r(&now, &tmnow);
        char time_str[100];
        localtime_r(&var.st_mtime, &tm);

        if (tm.tm_year == tmnow.tm_year)
        {
            strftime(time_str, sizeof(time_str), "%b %e %H:%M", &tm);
            printf("%s  \t", time_str);
        }
        else
        {
            strftime(time_str, sizeof(time_str), "%b %e  %Y", &tm);
            printf("%s  \t", time_str);
        }
        printf("\033[0;34m%s\033[0m\n", folders[i]);
    }
}
void printer(char *arr[], int n)
{
    struct stat var;
    for (int i = 0; i < n; i++)
    {
        stat(arr[i], &var);
        if (S_ISDIR(var.st_mode))
        {
            printf("\033[0;34m%s\033[0m\n", arr[i]);
        }
        else if (var.st_mode & S_IXUSR)
        {
            printf("\033[0;32m%s\033[0m\n", arr[i]);
        }
        else
        {
            printf("%s\n", arr[i]);
        }
    }
    return;
}
void address_print()
{
    if (flag == 1)
    {
        flag = 0;
        return;
    }
    else
    {
        char hostname[10000], username[10000];
        int root = 0, i, j, tempo = 0, count = 0;
        char cwd[PATH_MAX], root_handle[PATH_MAX], temp[PATH_MAX];
        root_handle[0] = '\0';
        getcwd(cwd, sizeof(cwd));
        gethostname(hostname, HOST_NAME_MAX);
        getlogin_r(username, LOGIN_NAME_MAX);
        if (strcmp(home_dir, cwd) == 0)
        {
            printf("\033[0;32m%s@%s:\033[0;34m~>\033[0m ", username, hostname);
            return;
        }
        for (i = 0; i < strlen(home_dir); i++)
        {
            if (home_dir[i] == '/')
            {
                root++;
            }
        }
        for (i = 0; i < strlen(cwd); i++)
        {
            if (cwd[i] == '/')
            {
                tempo++;
            }
            if (tempo > root)
            {
                temp[i] = '\0';
                if (strcmp(temp, home_dir) == 0)
                {
                    for (j = i; j <= strlen(cwd); j++)
                    {
                        root_handle[j - i] = cwd[j];
                    }
                    printf("\033[0;32m%s@%s:\033[0;34m~%s >\033[0m ", username, hostname, root_handle);
                    return;
                }
            }
            temp[i] = cwd[i];
        }
        strcpy(root_handle, cwd);
        printf("\033[0;32m%s@%s:\033[0;34m%s >\033[0m ", username, hostname, root_handle);
    }
    return;
}
void gatherhistory()
{
    // printf("reachedgather\n");
    char *temp;
    temp = (char *)malloc(10000 * sizeof(char));
    char *array[HISTORY_MAX_SIZE];
    int i, m, n, k = 0, j = 0;
    for (i = 0; i < HISTORY_MAX_SIZE; i++)
    {
        array[i] = (char *)malloc(10000 * sizeof(char));
    }
    int fd = open(history_path, O_RDWR | O_CREAT, 0777);
    if (fd == -1)
    {
        perror("ERROR!");
        return;
    }
    m = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    n = read(fd, temp, m);
    char *temp2 = (char *)malloc(10000 * sizeof(char));
    j = 0;
    for (i = 0; i < m; i++)
    {
        temp2[j] = temp[i];
        if (temp[i] == '\n')
        {
            temp2[j] = '\0';
            if (j != 0)
            {
                strcpy(history[k++], temp2);
            }
            j = -1;
        }
        if (i == m - 1)
        {
            temp2[j + 1] = '\0';
            strcpy(history[k++], temp2);
            j = -1;
        }
        j++;
    }
    // printf("%d\n", k);
    initial_hist_size = k;
    close(fd);
    return;
}
void storehistory()
{
    int i, j;
    char *his;
    int fd = open(history_path, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (fd == -1)
    {
        perror("ERROR!");
        return;
    }
    for (i = 0; i < initial_hist_size - 1; i++)
    {
        if (*history[i] != '\0')
        {
            his = (char *)malloc(sizeof(char) * 10000);
            strcpy(his, history[i]);
            int k = strlen(his);
            his[k] = '\n';
            his[k + 1] = '\0';
            write(fd, his, strlen(his));
        }
        // his = (char*)malloc(sizeof(char)*10000);
        // strcpy(his,history[i]);
        // write(fd,his,strlen(his));
    }
    return;
}
void historyfn(char *com)
{
    // printf("reached fn\n");
    // printf("%s\n", com);
    int k = initial_hist_size;
    int i, j;
    // printf("reached history\n");
    if (k == 0)
    {
        strcpy(history[k], com);
        // printf("whats stored: %s\n",history[k]);
        k++;
        // printf("whats stored: %s\n",history[k-1]);
        initial_hist_size = k;
        return;
    }
    // printf("prev: %s curr: %s\n", history[k - 1],com);
    if (strcmp(history[k - 1], com) == 0)
    {
        return;
    }
    if (k < HISTORY_MAX_SIZE)
    {
        strcpy(history[k], com);
        // printf("whats stored11: %s\n",history[k]);
        k++;
        initial_hist_size = k;
        // printf("innitial_hist_size:  %d\n", initial_hist_size);
    }
    else
    {
        // printf("initial_hist_size: %d", initial_hist_size);
        // free(history[0]);
        i = 1;
        for (j = 1; j < HISTORY_MAX_SIZE; j++)
        {
            if (*history[i] != '\0')
            {
                history[i - 1] = history[i];
                i++;
            }
        }
        history[HISTORY_MAX_SIZE - 1] = strdup(com);
    }
    return;
}
void printhistory()
{
    int i;
    printf("HISTORY: \n");
    if (initial_hist_size < 10)
    {
        for (i = 0; i < initial_hist_size; i++)
        {
            if (*history[i] != '\0')
                printf("%s\n", history[i]);
        }
    }
    else
    {
        for (i = (initial_hist_size - 10); i < initial_hist_size; i++)
        {
            if (*history[i] != '\0')
                printf("%s\n", history[i]);
        }
    }
    return;
}
void ls(char *str)
{
    int i, j, n;
    char *command2, *temp1, *temp2;
    char *options[30];
    char *directories[30];
    struct stat var;
    for (int i = 0; i < 30; i++)
    {
        directories[i] = (char *)malloc(sizeof(char *) * 10000);
        options[i] = (char *)malloc(sizeof(char *) * 10000);
    }

    int c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0; // c1=l;c2=a;c3=la;c4=al
    command2 = (char *)malloc(10000 * sizeof(char));
    temp1 = (char *)malloc(10000 * sizeof(char));
    temp2 = (char *)malloc(10000 * sizeof(char));
    strcpy(temp1, str);
    strcpy(temp2, str);
    command2 = strtok(temp1, delimeter);
    n = 0;
    i = 0;
    while (1)
    {
        options[n] = strtok(NULL, delimeter);
        if (options[n] == NULL)
        {
            break;
        }
        n++;
    }
    j = 0;
    for (i = 0; i < n; i++)
    {
        if (strcmp(options[i], "-l") == 0)
        {
            c1++;
        }
        else if (strcmp(options[i], "-a") == 0)
        {
            c2++;
        }
        else if (strcmp(options[i], "-la") == 0)
        {
            c3++;
        }
        else if (strcmp(options[i], "-al") == 0)
        {
            c4++;
        }
        else
        {
            strcpy(directories[j], options[i]);
            j++;
        }
    }
    i = 0;
    strcpy(temp_dir, curr_dir);
    // printf("j: %d\n",j);
    if (j == 0) // i.e no 2nd option
    {
        DIR *dir = opendir(".");
        if (dir == NULL)
        {
            printf("UNABLE TO LIST THE CURRENT FILE/DIRECTORY\n");
            return;
        }
        struct dirent *entity; // since it can be both file or directory
        entity = readdir(dir);
        char *sortarray[30];
        int k = 0, cond = 0;
        while (entity != NULL)
        {
            if (c1 == 0 && c2 == 0 && c3 == 0 && c4 == 0) // base cases
            {
                cond = 1;
                if (strncmp(entity->d_name, ".", 1) != 0)
                {
                    sortarray[k] = entity->d_name;
                    k++;
                }
                entity = readdir(dir);
            }
            else if (c1 > 0 && c2 == 0 && c3 == 0 && c4 == 0) // only -l
            {
                cond = 2;
                if (strncmp(entity->d_name, ".", 1) != 0)
                {
                    sortarray[k] = entity->d_name;
                    k++;
                }
                entity = readdir(dir);
            }
            else if (c1 == 0 && c2 > 0 && c3 == 0 && c4 == 0) // only -a
            {
                cond = 3;
                sortarray[k] = entity->d_name;
                k++;
                entity = readdir(dir);
            }
            else if ((c1 > 0 && c2 > 0) || c3 > 0 || c4 > 0) // both -l -a
            {
                cond = 4;
                sortarray[k] = entity->d_name;
                k++;
                entity = readdir(dir);
            }
        }
        closedir(dir);
        sort(sortarray, k);
        if (cond == 1)
        {
            printer(sortarray, k);
        }
        else if (cond == 2)
        {
            printerl(sortarray, k);
        }
        else if (cond == 3)
        {
            printer(sortarray, k);
        }
        else if (cond == 4)
        {
            printerl(sortarray, k);
        }
    }
    while (i < j)
    {
        DIR *dir;
        printf("%s : \n", directories[i]);
        if (directories[i] == NULL)
        {
            printf("%d\n", i);
            printf("BUBBYE!!");
            return;
        }
        else if (strcmp(directories[i], ".") == 0)
        {
            dir = opendir(".");
        }
        else if (strcmp(directories[i], "..") == 0)
        {
            dir = opendir("..");
        }
        else if (strcmp(directories[i], "~") == 0)
        {
            dir = opendir(home_dir);
        }
        else
        {
            if (!opendir(directories[i]))
            {
                stat(directories[i], &var);
                if (S_ISREG(var.st_mode))
                {

                    if (c1 == 0 && c2 == 0 && c3 == 0 && c4 == 0) // base cases
                    {
                        printer(directories, 1);
                    }
                    else if (c1 > 0 && c2 == 0 && c3 == 0 && c4 == 0) // only -l
                    {
                        printerl(directories, 1);
                    }
                    else if (c1 == 0 && c2 > 0 && c3 == 0 && c4 == 0) // only -a
                    {
                        printer(directories, 1);
                    }
                    else if ((c1 > 0 && c2 > 0) || c3 > 0 || c4 > 0) // both -l -a
                    {
                        printerl(directories, 1);
                    }
                    i++;
                    continue;
                }
                else
                {
                    printf("\033[1;31mINVALID FILE/DIRECTORY\033[0m\n");
                    return;
                }
            }
            else
            {
                chdir(directories[i]);
                dir = opendir(".");
            }
        }
        if (dir == NULL)
        {
            printf("UNABLE TO LIST THE CURRENT FILE/DIRECTORY\n");
            return;
        }
        struct dirent *entity; // since it can be both file or directory
        entity = readdir(dir);
        char *sortarray[30];
        int k = 0, cond = 0;
        while (entity != NULL)
        {
            if (c1 == 0 && c2 == 0 && c3 == 0 && c4 == 0) // base cases
            {
                cond = 1;
                if (strncmp(entity->d_name, ".", 1) != 0)
                {
                    sortarray[k] = entity->d_name;
                    k++;
                }
                entity = readdir(dir);
            }
            else if (c1 > 0 && c2 == 0 && c3 == 0 && c4 == 0) // only -l
            {
                cond = 2;
                if (strncmp(entity->d_name, ".", 1) != 0)
                {
                    sortarray[k] = entity->d_name;
                    k++;
                }
                entity = readdir(dir);
            }
            else if (c1 == 0 && c2 > 0 && c3 == 0 && c4 == 0) // only -a
            {
                cond = 3;
                sortarray[k] = entity->d_name;
                k++;
                entity = readdir(dir);
            }
            else if ((c1 > 0 && c2 > 0) || c3 > 0 || c4 > 0) // both -l -a
            {
                cond = 4;
                sortarray[k] = entity->d_name;
                k++;
                entity = readdir(dir);
            }
        }
        closedir(dir);
        sort(sortarray, k);
        if (cond == 1)
        {
            printer(sortarray, k);
        }
        else if (cond == 2)
        {
            printerl(sortarray, k);
        }
        else if (cond == 3)
        {
            printer(sortarray, k);
        }
        else if (cond == 4)
        {
            printerl(sortarray, k);
        }
        chdir(temp_dir);
        i++;
    }
}
void cd(char *str)
{
    int i, j;
    char *command2, *temp;
    command2 = (char *)malloc(10000 * sizeof(char));
    temp = (char *)malloc(10000 * sizeof(char));
    strcpy(temp, str);
    command2 = strtok(temp, delimeter);
    char *option = strtok(NULL, delimeter);
    if (option == NULL)
    {
        chdir(home_dir);
        getcwd(curr_dir, sizeof(curr_dir));
        return;
    }
    else if (strcmp(option, ".") == 0)
    {
        strcpy(prev_dir, curr_dir);
        return;
    }
    else if (strcmp(option, "..") == 0)
    {
        strcpy(prev_dir, curr_dir);
        chdir("..");
        getcwd(curr_dir, sizeof(curr_dir));
    }
    else if (strcmp(option, "-") == 0)
    {
        chdir(prev_dir);
        strcpy(prev_dir, curr_dir);
        getcwd(curr_dir, sizeof(curr_dir));
    }
    else if (strcmp(option, "~") == 0)
    {
        strcpy(prev_dir, curr_dir);
        chdir(home_dir);
        getcwd(curr_dir, sizeof(curr_dir));
        return;
    }
    else
    {
        if (!opendir(option))
        {
            printf("\033[1;31mINVALID FILE/DIRECTORY\033[0m\n");
        }
        else
        {
            strcpy(prev_dir, curr_dir);
            chdir(option);
            getcwd(curr_dir, sizeof(curr_dir));
        }
    }
}
void echo(char *str)
{
    int i;
    char *temp, *command1;
    command1 = (char *)malloc(10000 * sizeof(char));
    temp = (char *)malloc(10000 * sizeof(char));
    if (temp == NULL || command1 == NULL)
    {
        printf("\033[1;31mUNABLE TO MALLOC FOR ECHO COMMANDS\033[0m");
    }
    assert(command1 != NULL);
    for (i = 0; i <= strlen(str); i++)
    {
        temp[i] = str[i];
    }
    command1 = strtok(temp, delimeter);
    command1 = strtok(NULL, delimeter);
    while (command1 != NULL)
    {
        printf("%s ", command1);
        command1 = strtok(NULL, delimeter);
    }
    printf("\n");
    // free(command1);
    // free(temp);
    return;
}
void pwd()
{
    char cwd[PATH_MAX];
    if (cwd == NULL)
    {
        printf("\033[1;31mERROR FOR MALLOC FOR PATH\033[0m");
        return;
    }
    getcwd(cwd, sizeof(cwd));
    if (cwd == NULL)
    {
        if (errno == ERANGE)
        {
            printf("\033[1;31mPATH SIZE IS MORE THAN THE ALLOCATED MEMORY FOR IT\033[0m");
        }
        else
        {
            printf("\033[1;31mUNABLE TO RETRIEVE PATH\033[0m");
        }
        return;
    }
    printf("%s\n", cwd);
    return;
}
void fg(char *com)
{
    // printf("ola\n");
    // printf("%s\n",com);
    int i;
    char *temp, *str, *counter;
    temp = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
    str = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
    strcpy(temp, com);
    int q = 0;
    counter = strtok(temp, delimeter);
    while (counter != NULL)
    {
        q++;
        counter = strtok(NULL, delimeter);
    }
    char *commands[q + 1];
    for (i = 0; i < q + 1; i++)
    {
        commands[i] = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
    }
    q = 0;
    commands[q] = strtok(com, delimeter);
    while (commands[q] != NULL)
    {
        q++;
        commands[q] = strtok(NULL, delimeter);
    }
    commands[q] = NULL;
    pid_t PID;
    int PIDSTAT;
    PID = fork();
    if(PID<0)
    {
        printf("Forking failed!\n");
        return;
    }
    int timerinit = time(NULL);
    if (PID == 0)
    {
        // printf("%s\n",commands[0]);
        execvp(commands[0], commands);
        printf("INVALID COMMAND\n");
        exit(0);
    }
    else
    {
        childid = PID;
        waitpid(PID, &PIDSTAT, WCONTINUED | WUNTRACED);
        // printf("PROCESS ID: %d\n",PID);
    }
    int timerend = time(NULL);
    if ((timerend - timerinit) >= 0)
    {
        address_print();
        flag = 1;
        if ((timerend - timerinit) > 0)
            printf("\033[0;34m%d seconds >\033[0m  ", timerend - timerinit);
    }
    return;
}
void bg(char *com[])
{

    pid_t PID;
    // pid_t p1;
    int PIDSTAT;
    PID = fork();
    // int timerinit = time(NULL);
    // printf("done fork\n");
    if (PID == 0)
    {
        // p1 = getpid();
        // printf("%s\n",commands[0]);
        int retval = execvp(com[0], com);
        if (retval == -1)
            printf("INVALID COMMAND\n");
        exit(0);
    }
    else
    {
        // printf("%d\n",PID);
        PIDNAME[pp] = com[0];
        PIDS[pp++] = PID;
        // waitpid(PID,&PIDSTAT,WCONTINUED|WUNTRACED);
        printf("\nPROCESS %d %s OF ID: %d\n", pp,com[0], PID);
    }
    // int timereNUnd = time(NULL);
    // if((timerend-timerinit)>=0)
    // {
    //     address_print()
    //     flag =1;
    //     printf("\033[0;34m]%d seconds >\033[0m  ",timerend-timerinit);
    // }
    // pp = i;
    // printf("pp till bgmakeup: %d\n",pp);
    // address_print();
    return;
}
void bgmakeup(char *com[], int n)
{
    int i, j;
    // pp = 0;
    for (i = 0; i < n; i++)
    {
        char *temp, *str, *counter;
        temp = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
        str = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
        strcpy(temp, com[i]);
        int q = 0;
        counter = strtok(temp, delimeter);
        while (counter != NULL)
        {
            q++;
            counter = strtok(NULL, delimeter);
        }
        // printf("q :=%d",q);
        char *commands[q + 1];
        for (j = 0; j < q + 1; j++)
        {
            commands[j] = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
        }
        q = 0;
        commands[q] = strtok(com[i], delimeter);
        // printf("commands : %s\n",commands[q]);
        while (commands[q] != NULL)
        {
            q++;
            commands[q] = strtok(NULL, delimeter);
        }
        // printf("q later: %d\n",q);
        // printf("commands: \n");
        // for(int j =0;j<q;j++)
        // {
        //     printf("%s\n",commands[j]);
        // }
        commands[q] = NULL;

        bg(commands);
    }
    return;
}
void signalfn()
{
    // printf("ola!!!\n");
    int i;
    // pp++;
    for (i = 0; i < 100; i++)
    {
        if (PIDS[i] == 0)
        {
            continue;
        }
        int ID, stat;
        ID = PIDS[i];
        char* name = PIDNAME[i];
        // printf(" ID inside: %d\n",ID);
        if (waitpid(-1, &stat, WNOHANG) == ID)
        {
            // printf("hello!!\n");
            if (WEXITSTATUS(stat) == 0)
            {
                printf("\033[0;33m PROCESS %s OF ID %d HAS BEEN EXITED NORMALLY \033[0m \n",name,ID);
                // flag = 1;
                // printf("%d\n",pp);
                // address_print();
                // flag = 1;
                // address_print();
            }
            else
            {
                printf("\033[0;33m PROCESS OF ID %d HAS BEEN EXITED ABNORMALLY \033[0m", ID);
                // flag =0;
                // address_print();
            }
            PIDS[i] = 0;
            // pp --;
            // pp--;
            // printf("pp inside: %d",pp);
            // flag = flag*-1;
        }
    }
    return;
}
int main()
{
    shellID = getpid();
    signal(SIGINT,signalchandler);
    signal(SIGTSTP,signalzhandler);
    int i;
    pp = 0;
    for (i = 0; i < 100; i++)
    {
        PIDNAME[i] = (char*)malloc(sizeof(char)*1000);
    }
    for (i = 0; i < 100; i++)
    {
        PIDS[i] = 0;
    }
    for (i = 0; i < HISTORY_MAX_SIZE; i++)
    {
        history[i] = (char *)malloc(sizeof(char) * 10000);
    }
    getcwd(home_dir, sizeof(home_dir));
    strcpy(prev_dir, home_dir);
    strcpy(curr_dir, home_dir);
    strcpy(history_path, home_dir);
    char *hist = "/history.txt";
    strcat(history_path, hist);
    char *str, *command, *options, *temp1, *temp2, *temp3[30], *mainstr;
    char *commandarr[30];
    for (i = 0; i < 30; i++)
    {
        commandarr[i] = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
    }
    int l, j, fd;
    fd = open(history_path, O_RDWR | O_CREAT, 0777);
    if (fd == -1)
    {
        perror("ERROR!");
        return 0;
    }
    gatherhistory();
    while (1) // HEREEEEEEEEEEEEEEEEE
    {
        // printf("pp :   %d\n",pp);
        signalfn();
        // pp = 0;
        // for (i = 0; i < 10000; i++)
        // {
        //     PIDS[i] = 0;
        // }
        temp1 = (char *)malloc(10000 * sizeof(char));
        temp2 = (char *)malloc(10000 * sizeof(char));
        for (i = 0; i < 30; i++)
        {
            temp3[i] = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
        }
        str = (char *)malloc(10000 * sizeof(char));
        command = (char *)malloc(10000 * sizeof(char));
        mainstr = (char *)malloc(10000 * sizeof(char));
        if (str == NULL || command == NULL || mainstr == NULL || temp1 == NULL || temp2 == NULL || temp3 == NULL)
        {
            printf("\033[1;31mOOPSIE!\033[0m\n");
        }
        address_print();
        size_t inputsize = 10000;
        int l = getline(&str, &inputsize, stdin); // input
        if(l == -1)
        {
            printf("\n");
            return 0;
        }
        str[strlen(str) - 1] = '\0';
        // strcpy(options,str);
        // char* lullu = strtok(options,delimeter);
        if (str != NULL /*&& (strcmp(lullu, ";") != 0)*/)
        {
            historyfn(str);
        }
        strcpy(temp1, str);
        command = strtok(str, delimeter);
        int k = 0;
        commandarr[k] = strtok(temp1, ";");
        while (commandarr[k] != NULL)
        {
            k++;
            commandarr[k] = strtok(NULL, ";");
        }
        if (k == 0)
        {
            continue;
        }
        for (i = 0; i < k; i++)
        {
            char *new = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
            strcpy(new, commandarr[i]);
            int q = 0, index, w = 0;
            for (j = 0; j < strlen(commandarr[i]); j++)
            {
                if (commandarr[i][j] == '&')
                    w++;
            }
            strcpy(mainstr, commandarr[i]);
            strcpy(temp2, mainstr);
            temp3[q] = strtok(temp2, "&");
            while (temp3[q] != NULL)
            {
                q++;
                temp3[q] = strtok(NULL, "&");
            }
            if (q > w && q > 1)
            {
                fg(temp3[q - 1]);
                flag = 0;
                bgmakeup(temp3, q - 1);
            }
            else if (q == w)
            {
                bgmakeup(temp3, q);
            }
            else
            {

                command = strtok(commandarr[i], delimeter);
                if (command == NULL)
                {
                    continue;
                }
                else if (strcmp(command, "echo") == 0)
                {
                    echo(mainstr);
                }
                else if (strcmp(command, "pwd") == 0)
                {
                    pwd();
                }
                else if (strcmp(command, "cd") == 0)
                {
                    cd(mainstr);
                }
                else if (strcmp(command, "ls") == 0)
                {
                    ls(mainstr);
                }
                else if (strcmp(command, "history") == 0)
                {
                    printhistory();
                }
                else if (strcmp(command, "pinfo") == 0)
                {
                    procinfo(mainstr);
                }
                else if (strcmp(command, "discover") == 0)
                {
                    discoverymain(mainstr);
                }
                else if (strcmp(command, ";") == 0)
                {
                    printf("Unexpected token ';");
                }
                else if(strcmp(command,"jobs") == 0)
                {
                    jobs();
                }
                else if (strcmp(command, "-1") == 0)
                {
                    storehistory();
                    printf("\033[0;36mEXITING THE TERMINAL BYE!\033[0m\n");
                    return 0;
                }
                else
                {
                    fg(new);
                }
            }
        }
    }
    return 0;
}
