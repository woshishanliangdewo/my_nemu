#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<common.h>
#include<elf.h>
#include<unistd.h>
#include<fcntl.h>
#define BUFFER_SIZE 32
// #define VOS_ERR -1
// #define VOS_OK 0
// static char* phead = NULL;
// static char* ptail = NULL;
// static char* pValidRead = NULL;
// static char* pValidWrite = NULL;

// 这是每一次安装的一个轨迹的节点
// 因为这是我们的循环缓冲
// 本来应该是有一个度指针，一个写指针，一个头指针，一个尾指针
// 当然因为是数组，所以可以用索引代替指针，因为我们的头指针和尾指针
// 数组中可以选择有或者没有，所以这里选择了没有
// 然后我们写的时候根据指针就行了，这个是写指针
// 因为我们是一次性都读出来，所以度指针就是写指针
typedef struct{
    word_t pc;
    u_int32_t inst;
} TraceNode;
int cur = 0;
// int pread;
// int pwrite;
bool full = false;
TraceNode tracenode[BUFFER_SIZE];

void insertRb(word_t pc, u_int32_t inst){
    if(cur % BUFFER_SIZE == 0){
        cur = cur%BUFFER_SIZE;
    }    
    tracenode[cur].pc = pc;
    tracenode[cur++].inst = inst;
    if(cur % BUFFER_SIZE == 0)
    full = full || cur == 0;
}
void showRb(){
    if(!full) return; 
    int end = cur;
    int i = full?cur:0;
    void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
    char buf[128];
    char * p;
    do{
        p = buf;
        p += sprintf(buf, "%s" FMT_WORD ": %08x ", (i+1)%BUFFER_SIZE==end?" --> ":"     ", tracenode[i].pc, tracenode[i].inst);
    }
    while (i = (i+1)%BUFFER_SIZE != end);
}

void parse_elf(char * elf_file)
{
    if(elf_file == NULL) return;
    int fd = open(elf_file, O_RDONLY|O_SYNC);
    Elf64_Ehdr eh;
    read_elf_header(fd, &eh);
    show_header(eh);

    Elf64_Shdr sh[eh.e_shentsize * eh.e_shnum];
    read_section_headers(fd,eh,sh);
    show_section(fd,eh,sh);
}
void read_elf_header(int fd, Elf64_Ehdr* eh){
    assert(lseek(fd,0,SEEK_SET) == 0);
    if(strncmp((char*)eh->e_ident,"\177ELF", 4));
}

void read_section_headers(int fd,Elf64_Ehdr eh,void * dst)
{
    assert(lseek(fd, eh.e_shoff, SEEK_SET) == eh.e_shoff);
        for(int i = 0; i < eh.e_shnum; i++) {
            // assert(read(fd, (void *)&sh[i], eh.e_shentsize) == eh.e_shentsize);
        }
    
}
// typedef struct tail_rec_node{
//     paddr_t pc;
//     paddr_t depend;
//     struct tail_rec_node* next;
// } TailRecNode;
// TailRecNode * tail_rec_head = NULL;
// static void read_elf_header(int fd,)
// 以下写法错误的原因，我们尝试了使用malloc分配堆内存
// 但是我们可以使用数组进行分配，就是malloc使得操作不方便了，
// 不方便的地方是我们存储的内容。

// void initRingBuffer(){
//     if(phead == NULL){
//         phead = (char*) malloc(BUFFER_SIZE * sizeof(char)); 
//     }
//     memset(phead,0,sizeof(BUFFER_SIZE));
//     pValidRead = phead;
//     pValidWrite = phead;
//     ptail = phead + BUFFER_SIZE;
// }

// TraceNode iringbuf;

// void trace_inst(word_t pc, u_int32_t inst){
//     initRingBuffer();
//     iringbuf.pc = pc;
//     iringbuf.inst = inst;
//     WriteRingBuffer(iringbuf,sizeof(iringbuf));
// }


// void FreeRingBuffer(){
//     if(phead != NULL){
//         free(phead);
//     }
// }

// int WriteRingBuffer(TraceNode pBuff, int AddLen){
//     if(phead == NULL){
//         printf("没有初始化呢哦");
//         return VOS_ERR;
//     }
//     if(AddLen > ptail - phead){
//         printf("WriteRingBuff:New add buff is too long\n");	
// 		return VOS_ERR;
//     }
//     if(pValidWrite + AddLen > ptail){
//         int preLen = ptail - pValidRead;
//         int lastLen = AddLen - preLen;
//         memcpy(pValidWrite, pBuff, preLen);	
// 		memcpy(phead, pBuff + preLen, lastLen);

//         pValidWrite = phead + lastLen;
//     }else{
//         memcpy(pValidWrite, pBuff, AddLen);	//将新数据内容添加到缓冲区
// 		pValidWrite += AddLen;	//新的有效数据尾地址
//     }
//     return VOS_OK;
// }

// int ReadRingBuffer(char * pBuff, int len){
//     if(phead == NULL){
//         printf("没有初始化呢哦");
//         return VOS_ERR;
//     }
//     if(len>ptail - phead){
//         printf("你太长了");
//         return VOS_ERR;
//     }
//     if(pValidRead + len > ptail){
//         int prelen = ptail - pValidRead;
//         int lastlen = len- prelen;
//         memcpy(pBuff, pValidRead, prelen);
// 		memcpy(pBuff + prelen, phead, lastlen);

//         pValidRead = phead + lastlen;
//     }
//     else
// 	{
// 		memcpy(pBuff, pValidRead, len);
// 		pValidRead += len;
// 	}
	
// 	return len;

// }
