#include "_public.h"

int main(int argc, char *argv[])
{
    char res[] = "wangziyi";
    char test[10];
    memset(test,0, sizeof(test));
    // STRNCPY(test, sizeof(test), res, sizeof(res));
    STRNCAT(test, sizeof(test), res, sizeof(res));


    int i = 0;
    while(test[i])
    {
        cout << test[i] << endl;
        i++;
    }
    return 0;
}