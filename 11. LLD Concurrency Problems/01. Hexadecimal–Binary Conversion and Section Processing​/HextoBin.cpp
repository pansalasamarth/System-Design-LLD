#include <stdio.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <signal.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_THREAD 2

struct msg {
    long mtype;
    char data[33];
};

struct msg1 {
    long mtype;
    int data[4];
};

int validate(char hex[]) {
    int count=0;
    for(int i=0;hex[i]!='\0';i++) 
    {
        if(hex[i]=='\n') 
        {
            continue;
        }
        if(!isxdigit((unsigned char)hex[i]))
        {
            return 0;
        }
        count++;
    }
    if(count==0)
    {
        printf("Please enter at least one digit Hexadecimal number\n");
        return 0;
    }
    if(count>8)
    {
        printf("Please enter only 8 digit Hexadecimal number\n");
        return 0;
    }
    return 1;
}

void sendMessage(char str[], int type, int msgid) {
    struct msg message;
    message.mtype = type;
    strcpy(message.data, str);

    if(msgsnd(msgid, &message, sizeof(message.data), 0)<0)
    {
        printf("Error in message sending\n");
        exit(1);
    }
    
    printf("Message send successfully\n");
}

struct msg receiveMessage(int type, int msgid) {
    struct msg message;

    if(msgrcv(msgid, &message, sizeof(message.data), type, 0)<0)
    {
        printf("Error in message receiving\n");
        exit(1);
    }
    
    printf("Message receive successfully\n");

    return message;
}

void sendMessage1(int parts[], int type, int msgid) {
    struct msg1 message;
    message.mtype = type;
    
    for(int i=0;i<4;i++)
    {
        message.data[i]=parts[i];
    }

    if(msgsnd(msgid, &message, sizeof(message.data), 0)<0)
    {
        printf("Error in message sending\n");
        exit(1);
    }
    
    printf("Message send successfully\n");
}

struct msg1 receiveMessage1(int type, int msgid) {
    struct msg1 message;

    if(msgrcv(msgid, &message, sizeof(message.data), type, 0)<0)
    {
        printf("Error in message receiving\n");
        exit(1);
    }
    
    printf("Message receive successfully\n");

    return message;
}

char* convetHextoBin(const char* hex) {
    const char* table[] = {"0000","0001","0010","0011","0100","0101","0110","0111",
                           "1000","1001","1010","1011","1100","1101","1110","1111"};

    int length = strlen(hex);
    if(length<8)
    {
        length=8;
    }
    char* ans = (char *)malloc(length*4+1);
    if(ans==NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    ans[0]='\0';
    for(int i=0;hex[i]!='\0';i++)
    {
        if(hex[i]=='\n')
        {
            continue;
        }
        const char* temp;
        if(hex[i]>='0' && hex[i]<='9')
        {
            temp=table[hex[i]-'0'];
        }
        else if(hex[i]>='A' && hex[i]<='F')
        {
            temp=table[10 + (hex[i]-'A')];
        }
        else
        {
            temp=table[10 + (hex[i]-'a')];
        }
        strcat(ans, temp);
    }

    int n=strlen(ans);
    if(n<32)
    {
        int remains=32-n;
        char temp[33];
        memset(temp, '0', remains);
        temp[remains]='\0';
        strcat(temp, ans);
        strcpy(ans, temp);
    }
    return ans;
}

int convertBintoDec(const char* ans, int start, int end) {
    int num=0;
    int power=1;
    for(int i=end;i>=start;i--)
    {
        int temp=ans[i]-'0';
        num+=(temp*power);
        power*=2;
    }
    return num;
}

int* splitParts(const char *ans) {
    int *parts = (int *)malloc(4*sizeof(int));

    int num=convertBintoDec(ans, 0, 2);
    parts[0]=num;
    num=convertBintoDec(ans, 3, 11);
    parts[1]=num;
    num=convertBintoDec(ans, 12, 18);
    parts[2]=num;
    num=convertBintoDec(ans, 19, 31);
    parts[3]=num;

    return parts;
}

void reverseString(char *str) {
    char *start = str;
    char *end = str + strlen(str) - 1;
    char temp;
    while(start < end) 
    {
        temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

char* convertDectoHex(int n) {
    char* hexaDeciNum = (char *)malloc(5);
    if(n == 0)
    {
        hexaDeciNum[0] = '0';
        hexaDeciNum[1] = '\0';
        return hexaDeciNum;
    }
    int i = 0;
    while(n!=0) 
    {
        int temp = n%16;

        if(temp<10) 
        {
            hexaDeciNum[i++] = temp + '0';
        } 
        else 
        {
            hexaDeciNum[i++] = temp + 'A' - 10;
        }
        n = n/16;
    }
    hexaDeciNum[i]='\0';
    reverseString(hexaDeciNum);
    return hexaDeciNum;
}

char* inputHex() {
    char* hex = (char *)malloc(20);

    while(1)
    {
        printf("Enter hex value: ");
        fgets(hex, 20, stdin);

        if(validate(hex) == 1) 
        {
            return hex;
        }

        printf("Invalid hexadecimal number. Try again.\n");

        printf("--------------------------------------------------------------------\n");
        printf("--------------------------------------------------------------------\n");

    }
}

void *routine(void *arg)
{
    int* msgid = (int *)arg;

    char* hex = inputHex();

    printf("Sending hexadecimal value to P2...\n");
    sendMessage(hex, 1, *msgid);
    free(hex);

    struct msg message = receiveMessage(2, *msgid);
    const char* ans = message.data;

    printf("Binary value: %s\n", ans);
    printf("Binary number received successfully...\n");

    printf("--------------------------------------------------------------------\n");

    int* parts = splitParts(ans);

    printf("Sending four decimal sections to P2...\n");
    sendMessage1(parts, 3, *msgid);
    free(parts);

    for(int i=0;i<4;i++)
    {
        struct msg message1 = receiveMessage(4, *msgid);
        printf("Receiving section %d in hexadecimal form...\n", i+1);
        if(message1.data[0] == '\0')
        {
            printf("Hex section %d: 0\n", i+1);
        }
        else
        {
            printf("Hex section %d: %s\n", i+1, message1.data);
        }
    }

    printf("--------------------------------------------------------------------\n");
    printf("--------------------------------------------------------------------\n");

    return NULL;
}

void *routine1(void *arg)
{
    int* msgid = (int *)arg;

    struct msg message = receiveMessage(1, *msgid);

    const char* hex = message.data;
    printf("Hexadecimal value: %s", hex);
    printf("Hexadecimal number received successfully...\n");

    printf("--------------------------------------------------------------------\n");

    char* ans = convetHextoBin(hex);

    printf("Sending binary string to P1...\n");
    sendMessage(ans, 2, *msgid);
    free(ans);

    struct msg1 message1 = receiveMessage1(3, *msgid);
    int* nums = message1.data;

    printf("Four Decimal sections: \n");
    for(int i=0;i<4;i++)
    {
        printf("%d ", nums[i]);
    }
    printf("\n");

    printf("--------------------------------------------------------------------\n");

    for(int i=0;i<4;i++)
    {
        char* hexSection = convertDectoHex(nums[i]);
        printf("Sending %d section in hexadecimal form...\n", i+1);
        sendMessage(hexSection, 4, *msgid);
        free(hexSection);
    }

    return NULL;
}

int msgid;

void cleanup(int signum) {
    printf("\nExiting program. Removing message queue...\n");
    msgctl(msgid, IPC_RMID, NULL);
    exit(0);
}

int main()
{
    pthread_t t[MAX_THREAD];

    msgid = msgget(3, IPC_CREAT | 0666);
    if(msgid<0)
    {
        printf("Error in getting message queue\n");
        exit(1);
    }

    signal(SIGINT, cleanup);

    while(1)
    {
        for(int i=0;i<MAX_THREAD;i++)
        {
            if(i==0)
            {
                pthread_create(&t[i], NULL, routine, (void *)&msgid);
            }
            else
            {
                pthread_create(&t[i], NULL, routine1, (void *)&msgid);
            }
        }
        for(int i=0;i<MAX_THREAD;i++)
        {
            pthread_join(t[i], NULL);
        }
    }

    return 0;
}