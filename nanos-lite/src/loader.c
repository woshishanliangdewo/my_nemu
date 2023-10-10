#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif
size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf64_Ehdr eh;
  ramdisk_read(&eh,0,sizeof(Elf64_Ehdr));
  Elf64_Phdr ph[eh.e_phnum];
  ramdisk_read(phdr, ehdr.e_phoff, sizeof(Elf_Phdr)*ehdr.e_phnum);
  for(int i=0;i<eh.e_phnum;i++){
      if(ph[i].p_type == PT_LOAD){
          ramdisk_read((void*)ph[i].p_vaddr,ph[i].p_offset,ph[i].p_memsz);
          memset((void*)(phdr[i].p_vaddr+phdr[i].p_filesz),0,ph[i].p_memsz-ph[i].p_filesz);
      }
  }
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

