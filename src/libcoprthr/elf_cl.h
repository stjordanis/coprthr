
//#ifndef __ANDROID__
#if(1)

#include "../libcoprthrcc/elfcl.h"

#else

/* DAR */

#ifndef _ELF_CL_H
#define _ELF_CL_H

#if defined(__ANDROID__)
//#include <libelf/libelf.h>
#include <elf.h>
#define Elf32_Xword Elf32_Word
#else
#include <elf.h>
#endif


#if defined(__i386__) || defined(__arm__)

struct cldev_entry {
	Elf32_Half e_type;
	Elf32_Word e_vendorid;
	Elf32_Word e_vendor;
};

struct clprgs_entry {
	Elf32_Word e_name;
	Elf32_Word e_info;
	Elf32_Half e_shndx;
	Elf32_Addr	e_offset;
	Elf32_Xword e_size;
};

struct clprgb_entry {
	Elf32_Word e_name;
	Elf32_Word e_info;
	Elf32_Half e_shndx;
	Elf32_Addr	e_offset;
	Elf32_Xword e_size;
};

struct clsymtab_entry {
	Elf32_Half e_name;
	Elf32_Half e_kind;
	Elf32_Addr e_sym;
	Elf32_Addr e_interp;
	Elf32_Half e_datatype;
	Elf32_Half e_vecn;
	Elf32_Half e_arrn;
	Elf32_Half e_addrspace;
	Elf32_Half e_ptrc;
	Elf32_Half e_narg;
	Elf32_Half e_arg0;
};

struct clargtab_entry {
	Elf32_Half e_kind;
	Elf32_Half e_datatype;
	Elf32_Half e_vecn;
	Elf32_Half e_arrn;
	Elf32_Half e_addrspace;
	Elf32_Half e_ptrc;
	Elf32_Half e_name;
	Elf32_Half e_nxt;
};

#define init_clsymtab_entry( \
	name,kind,sym,interp,datatype,vecn,arrn,addrspace,ptrc,narg,arg0) \
	(struct clsymtab_entry){ \
		(Elf32_Half)name, \
		(Elf32_Half)kind, \
		(Elf32_Addr)sym, \
		(Elf32_Addr)interp, \
		(Elf32_Half)datatype, \
		(Elf32_Half)vecn, \
		(Elf32_Half)arrn, \
		(Elf32_Half)addrspace, \
		(Elf32_Half)ptrc, \
		(Elf32_Half)narg, \
		(Elf32_Half)arg0 }

#define init_clargtab_entry( \
	kind,datatype,vecn,arrn,addrspace,ptrc,name,nxt) \
	(struct clargtab_entry){ \
		(Elf32_Half)kind, \
		(Elf32_Half)datatype, \
		(Elf32_Half)vecn, \
		(Elf32_Half)arrn, \
		(Elf32_Half)addrspace, \
		(Elf32_Half)ptrc, \
		(Elf32_Half)name, \
		(Elf32_Half)nxt \
	}


#elif defined(__x86_64__)


struct cldev_entry {
	Elf64_Word e_kind;
	Elf64_Half e_type;
	Elf64_Word e_vendorid;
	Elf64_Word e_vendor;
};

struct clprgs_entry {
	Elf64_Word e_name;
	Elf64_Word e_info;
	Elf64_Half e_shndx;
	Elf64_Addr	e_offset;
	Elf64_Xword e_size;
};

struct clprgb_entry {
	Elf64_Word e_name;
	Elf64_Word e_info;
	Elf64_Half e_shndx;
	Elf64_Addr	e_offset;
	Elf64_Xword e_size;
};

struct clsymtab_entry {
	Elf64_Half e_name;
	Elf64_Half e_kind;
	Elf64_Addr e_sym;
	Elf64_Addr e_interp;
	Elf64_Half e_datatype;
	Elf64_Half e_vecn;
	Elf64_Half e_arrn;
	Elf64_Half e_addrspace;
	Elf64_Half e_ptrc;
	Elf64_Half e_narg;
	Elf64_Half e_arg0;
};

struct clargtab_entry {
	Elf64_Half e_kind;
	Elf64_Half e_datatype;
	Elf64_Half e_vecn;
	Elf64_Half e_arrn;
	Elf64_Half e_addrspace;
	Elf64_Half e_ptrc;
	Elf64_Half e_name;
	Elf64_Half e_nxt;
};

#define init_clsymtab_entry( \
	name,kind,sym,interp,datatype,vecn,arrn,addrspace,ptrc,narg,arg0) \
	(struct clsymtab_entry){ \
		(Elf64_Half)name, \
		(Elf64_Half)kind, \
		(Elf64_Addr)sym, \
		(Elf64_Addr)interp, \
		(Elf64_Half)datatype, \
		(Elf64_Half)vecn, \
		(Elf64_Half)arrn, \
		(Elf64_Half)addrspace, \
		(Elf64_Half)ptrc, \
		(Elf64_Half)narg, \
		(Elf64_Half)arg0 \
	}

#define init_clargtab_entry( \
	kind,datatype,vecn,arrn,addrspace,ptrc,name,nxt) \
	(struct clargtab_entry){ \
		(Elf64_Half)kind, \
		(Elf64_Half)datatype, \
		(Elf64_Half)vecn, \
		(Elf64_Half)arrn, \
		(Elf64_Half)addrspace, \
		(Elf64_Half)ptrc, \
		(Elf64_Half)name, \
		(Elf64_Half)nxt \
	}


#else

#error unsupported ELF format

#endif


int 
elfcl_write(
   int fd,
   struct cldev_entry* cldev, unsigned int cldev_n,
   struct clprgs_entry* clprgs, unsigned int clprgs_n,
   char* cltexts_buf, size_t cltexts_sz,
   struct clprgb_entry* clprgb, unsigned int clprgb_n,
   char* cltextb_buf, size_t cltextb_sz,
   struct clsymtab_entry* clsymtab, size_t clsymtab_n,
   struct clargtab_entry* clargtab, size_t clargtab_n,
   char* clstrtab, size_t clstrtab_sz
);

unsigned int elfcl_get_clprgb_n( void* p );
void* elfcl_get_clprgb( void* p );
void* elfcl_get_cltextb( void* p );


#endif

#endif

