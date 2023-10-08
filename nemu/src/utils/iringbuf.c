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
    tracenode[cur].inst = inst;
    cur = (cur+1)%BUFFER_SIZE;
    full = full || cur == 0;
}
void showRb(){
    if(!full && !cur) return; 
    int end = cur;
    int i = full?cur:0;
    void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
    char buf[128];
    char * p;
    do{
        p = buf;
        p += sprintf(buf, "%s" FMT_WORD ": %08x ", (i+1)%BUFFER_SIZE==end?" --> ":"     ", tracenode[i].pc, tracenode[i].inst);
        disassemble(p, buf+sizeof(buf)-p, tracenode[i].pc, (uint8_t *)&tracenode[i].inst, 4);

        if ((i+1)%BUFFER_SIZE==end) printf(ANSI_FG_RED);
        puts(buf);
        break;
    }
    while (i = (i+1)%BUFFER_SIZE != end);
    puts(ANSI_NONE);
}
// \177表明是三个8进制的数字，防止16进制搅局
// 而且elf分别均为字符，就是ascii码那个，不是数字
// 如果不这样，e会被错误认为是16进制
// 是否是真的定位到了开头，是否是真的读取到了数据并寸到了eh中
// 是否魔数表明是否为elf文件
// void read_elf_header(Elf64_Ehdr *eh, int fd){
//     // 返回的是从文件开头开始计算的偏移量，即当前读写位置
//     assert(lseek(fd,0,SEEK_SET) == 0);
//     // 返回的是读取到的字节数，是读取到buf中的字节数
//     assert(read(fd,(void *)eh,sizeof(Elf64_Ehdr)) == sizeof(Elf64_Ehdr));
//     if(strncmp((char*)eh->e_ident,"\177ELF",4)){
//         panic("malformed ELF file");
//     }
// }

// void display_elf_header(Elf64_Ehdr eh){
//     switch(eh.e_ident[EI_CLASS]){
//         case ELFCLASS32:
//             printf("the elf is a elf32 file");
//         case ELFCLASS64:
//             printf("the elf is a elf64 file");
//         case ELFCLASSNONE:
//             printf("the elf file is not exist");
//     }
//     switch(eh.e_ident[EI_DATA]){
//         case ELFDATA2LSB:
//             printf("小端序");
//         case ELFDATA2MSB:
//             printf("大端序");
//         case ELFDATANONE:
//             printf("编码格式无效");
//     }
// }



// void read_section(int fd, Elf64_Shdr sh,void * dst){
//     assert(dst != NULL);
//     assert(lseek(fd,sh.sh_offset,SEEK_SET) == sh.sh_offset);
//     assert(read(fd,dst,sizeof(sh.sh_size)) == sh.sh_size);
// }
// void read_section_header(int fd, Elf64_Ehdr eh, Elf64_Shdr* shtbl){
//     assert(lseek(fd, eh.e_shoff,SEEK_SET) == eh.e_shoff);
//     for(int i=0;i<eh.e_shnum;i++){
//         assert(read(fd,&shtbl[i],eh.e_shentsize) == eh.e_shentsize);
//     }
// }
// typedef struct {
// 	char name[32]; // func name, 32 should be enough
// 	paddr_t addr;
// 	unsigned char info;
// 	Elf64_Xword size;
// } SymEntry;

// void read_symtbl_table(int fd,Elf64_Shdr shtbl[],int symidx){
//     Elf64_Sym symtbl[shtbl[symidx].sh_size];
//     read_section(fd, shtbl[symidx], symtbl);
// }
// void parse_elf(char * file){
//     int fd = fopen(file,"r");
// }



























// void parse_elf(char * elf_file){
//     Elf64_Ehdr eh;
//     int fd;
//     int idx;
//     fd = read_elf_file(elf_file);
//     read_elf_header(fd,&eh);
//     Elf64_Shdr sh[eh.e_shentsize * eh.e_shnum];
//     read_elf_shdr(fd,eh,sh);

// }
// int read_elf_file(char* elf_file){
//     int fd = open(elf_file,O_RDONLY);
//     return fd;
//     // 结构体本身也有足够大小的    
// }
// // 1.为什么使用了read函数之后读取不到数据？（光标的问题）
// // 因为：在我们write写操作之后，光标已经移动到了数据最末尾的位置
// // 就像你用word文档敲完一句话之后，那个光标总是停留在最后的位置，这里是同样的道理。
// // 所以当我们read读操作的时候，总是在末尾读数据，单数尾巴没有数据，所以啥也读不到。

// // eh是一个大的结构体
// void read_elf_header(int fd, Elf64_Ehdr* eh){
//     lseek(fd,0,SEEK_SET);
//     read(fd,eh,sizeof(Elf64_Ehdr));
// }

// // sh却是一个结构体的数组
// // 这不过是一个多次的指针调用罢了
// void read_elf_shdr(int fd, Elf64_Ehdr eh, Elf64_Shdr sh[]){
//     int off = eh.e_shoff;
//     lseek(fd,off,sizeof(Elf64_Shdr));
//     read(fd,sh,sizeof(Elf64_Shdr));
// }
// void read_symbol_table(int fd,Elf64_Shdr sh[],Elf64_Sym symtbl[],int idx){
//     Elf64_Shdr s1 = sh[idx];
//     int offset = s1.sh_offset;
//     int size = s1.sh_size;
//     lseek(fd,offset,SEEK_SET);
//     read(fd,symtbl,size);
//     read_elf_symentry(fd,sh,symtbl,size);
// }
// void read_elf_symtbl(int fd, Elf64_Ehdr eh,Elf64_Shdr sh[],Elf64_Sym symtbl[]){
//     int offset = 0;
//     int size = 0;
//     for(int i=0;i<eh.e_shnum;i++){
//         if(sh[i].sh_type == SHT_SYMTAB || sh[i].sh_type == SHT_DYNSYM){
//             read_symbol_table(fd, sh, symtbl, i);
//             break;
//         }
//     }
    
// }

// void read_elf_symentry(int fd,Elf64_Shdr sh[],Elf64_Sym symtbl[],int size){
//         for(int i=0;i<size/sizeof(Elf64_Sym);i++){
            
//         }
//     }
























// void parse_elf(char * elf_file)
// {
//     if(elf_file == NULL) return;
//     int fd = open(elf_file, O_RDONLY|O_SYNC);
//     Elf64_Ehdr eh;
//     read_elf_header(fd, &eh);
//     show_header(eh);

//     Elf64_Shdr sh[eh.e_shentsize * eh.e_shnum];
//     read_section_headers(fd,eh,sh);
//     show_section(fd,eh,sh);
// }
// void read_elf_header(int fd, Elf64_Ehdr* eh){
//     assert(lseek(fd,0,SEEK_SET) == 0);
//     if(strncmp((char*)eh->e_ident,"\177ELF", 4));
// }

// void read_section_headers(int fd,Elf64_Ehdr eh,void * dst)

// {
//     assert(lseek(fd, eh.e_shoff, SEEK_SET) == eh.e_shoff);
//         for(int i = 0; i < eh.e_shnum; i++) {
//             // assert(read(fd, (void *)&sh[i], eh.e_shentsize) == eh.e_shentsize);
//         }
    
// }
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
