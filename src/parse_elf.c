/*
** RopGadget - Release v3.4.2
** Jonathan Salwan - http://twitter.com/JonathanSalwan
** Allan Wirth - http://allanwirth.com/
** http://shell-storm.org
** 2012-11-11
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "ropgadget.h"

static const char * const flag_const[] = {
  "---", "--x",
  "-w-", "-wx",
  "r--", "r-x",
  "rw-", "rwx",
  "Err"
};

const char *get_flags(Elf64_Word flags)
{
  if (flags > 7) flags = 8;
  return flag_const[flags];
}

char *get_seg(Elf64_Word seg)
{
  if (seg == 0)
    return ("NULL");
  else if (seg == 1)
    return ("LOAD");
  else if (seg == 2)
    return ("DYNAMIC");
  else if (seg == 3)
    return ("INTERP");
  else if (seg == 4)
    return ("NOTE");
  else if (seg == 5)
    return ("SHLIB");
  else if (seg == 6)
    return ("PHDR");
  else if (seg == 7)
    return ("TLS");
  else if (seg == 8)
    return ("NUM");
  else if (seg == 0x60000000)
    return ("LOOS");
  else if (seg == 0x6fffffff)
    return ("HIOS");
  else if (seg == 0x70000000)
    return ("LOPROC");
  else if (seg == 0x7fffffff)
    return ("HIPROC");
  else if (seg == 0x6474e550)
    return ("EH_FRAME");
  else if (seg == 0x6474e551)
    return ("STACK");
  else if (seg == 0x6474e552)
    return ("RELRO");
  else if (seg == 0x65041580)
    return ("PAX_FLAGS");
  else
    return ("ERROR");
}

void process_filemode(char *file)
{
  int fd;
  unsigned char *data;
  struct stat filestat;

  fd = xopen(file, O_RDONLY, 0644);
  stat(file, &filestat);
  filemode.size = filestat.st_size;
  filemode.file = file;

  pMapElf = xmmap(0, filemode.size, PROT_READ, MAP_SHARED, fd, 0);
  data = (unsigned char*)pMapElf;
  filemode.data = data;
  close(fd);

  if (strncmp((char *)data, MAGIC_ELF, 4))
    {
      fprintf(stderr, "%sError%s: No elf format\n", RED, ENDC);
      exit(EXIT_FAILURE);
    }


  /* supported: - Linux/x86-32bits */
  /* supported: - FreeBSD/x86-32bits */
  if (ELF_F && (SYSV || LINUX || FREEBSD))
    {
      containerType = CONTAINER_ELF32;
      pElf32_Header = (Elf32_Ehdr *)data;
      pElf32_Phdr = (Elf32_Phdr *)(filemode.data + pElf32_Header->e_phoff);
      if (PROC8632)
        return;
    }
  else if (ELF_F64 && (SYSV || LINUX || FREEBSD))
    {
      containerType = CONTAINER_ELF64;
      pElf64_Header = (Elf64_Ehdr *)data;
      pElf64_Phdr = (Elf64_Phdr *)(filemode.data + pElf64_Header->e_phoff);
      if (PROC8664)
        return;
    }
  fprintf(stderr, "%sError%s: Architecture isn't supported\n", RED, ENDC);
  exit(EXIT_FAILURE);
}
