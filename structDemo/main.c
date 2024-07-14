#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct PDU{
    int a;
    int b;
    int c;
    int d[];//柔性数组，结构体最后一个元素是数量未知的数组
}PDU;

int main()
{
    printf("%llu\n",sizeof (PDU));
    PDU *pdu = (PDU*)malloc(sizeof (PDU) + 20 * sizeof (char));
    pdu->a = 88;
    memcpy(pdu->d, "hello world", 12);
    printf("pdu->a = %d, pdu->d = %s\n", pdu->a, (char*)pdu->d);
    free(pdu);
    pdu = NULL;
    return 0;
}
