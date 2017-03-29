// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "noff.h"



#define UserStackSize		1024 	// increase this as necessary!

class AddrSpace {
  public:
    AddrSpace(OpenFile *executable);	// Create an address space,
					// initializing it with the program
					// stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space
    
    OpenFile * myFile;
    void AssignPage( int addr, int pAdr);
    void ReplacePage(int addr,  int repAdr); // addr  = Virtual Address  repAdr = Replacement Physical Address
    void UpdateFile(int vaddr, int paddr);
	noffHeader myNoff;
	
	int size;
	unsigned int startPage;
	bool space;
	char * fileN;
	int pAddr;
    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code



    void SaveState();			// Save/restore address space-specific
    
    void RestoreState();		// info on a context switch 
	TranslationEntry ** HPT;
	int outerDivide;
	int inner;
  private:
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
    unsigned int numPages;		// Number of pages in the virtual 
					// address space
			//Page number that the program starts at
								//in physical memory
			//Boolean to remember if there was enough space or not
};

#endif // ADDRSPACE_H
