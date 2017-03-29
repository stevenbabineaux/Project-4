// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "list.h"




extern int * task4;
extern int * task5;
extern bool extraOutput;
bool done = FALSE;
extern bool twoLevel;



//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
////////////////////START PROJECT 4///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
AddrSpace::AddrSpace(OpenFile *executable){
	if(currentThread->getID() >= 2000)
	{
		printf("Thread Limit Exceeded. Terminating!\n");
		for(int i = 0; i < threadID; i++){
			char * files = new char[100];
			sprintf(files, "%d.swap", i);
			fileSystem->Remove(files);
			delete files;
		}
		Cleanup();
		currentThread->Finish();
		return;
		
	}
		
    if(!done){
    	printf("Number of physical pages: %d\n", NumPhysPages);
        printf("Page Size in bytes: %d\n\n", PageSize);
    	done = TRUE;
    }
    
    
    unsigned int i, count;
	space = false;

    executable->ReadAt((char *)&myNoff, sizeof(myNoff), 0);
    if ((myNoff.noffMagic != NOFFMAGIC) && 
		(WordToHost(myNoff.noffMagic) == NOFFMAGIC))
    	SwapHeader(&myNoff);
    ASSERT(myNoff.noffMagic == NOFFMAGIC);

// how big is address space?
    size = myNoff.code.size + myNoff.initData.size + myNoff.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;
    
    
    
    char* fileX = new char[64];
    fileN = new char[64];
    
    
    sprintf(fileX, "%d.swap", threadID);
    strcpy(fileN, fileX);
    
    
    fileSystem->Create(fileX, size);	// ASSERTION FAILED
    if(extraOutput){
    	printf("Swapfile %s created\n\n", fileX);
    }
    
    OpenFile * copyto = fileSystem->Open(fileX);
    
    char* Ecode = new char[myNoff.code.size];
    char* EinitData = new char[myNoff.initData.size];
    char* EuninitData = new char[myNoff.uninitData.size];
    
 	executable->ReadAt(Ecode, myNoff.code.size, myNoff.code.inFileAddr);
 	executable->ReadAt(EinitData, myNoff.initData.size, myNoff.initData.inFileAddr);
 	executable->ReadAt(EuninitData, myNoff.uninitData.size, myNoff.uninitData.inFileAddr);
 	
 	
 	copyto->WriteAt(Ecode, myNoff.code.size, myNoff.code.inFileAddr);
 	copyto->WriteAt(EinitData, myNoff.initData.size, myNoff.initData.inFileAddr);
 	copyto->WriteAt(EuninitData, myNoff.uninitData.size, myNoff.uninitData.inFileAddr);
 
 	delete Ecode;
 	delete EinitData;
 	delete EuninitData;
 	delete fileX;
 	delete copyto;
 	
    
    //fileSystem->Close(filename);
	

	//Change this to reference the bitmap for free pages
	//instead of total amount of pages
	//This requires a global bitmap instance
	
	count = 0;

	//If we get past the if statement, then there was sufficient space
	space = true;

	//This safeguards against the loop terminating due to reaching
	//the end of the bitmap but no contiguous space being available

    DEBUG('a', "Initializing address space, numPages=%d, size=%d\n", 
					numPages, size);
// first, set up the translation 
	if(twoLevel){
		
		inner = numPages % 4 == 0 ? numPages / 4 : numPages / 4 + 1;
		outerDivide = inner;
		HPT = new TranslationEntry*[4];
		for(int i = 0; i < 4; i++)
			HPT[i] = new TranslationEntry[inner];

		for(int i = 0; i < 4; i++){
			for(int j = 0; j < inner; j++){
				HPT[i][j].virtualPage = count;
				HPT[i][j].valid = FALSE;
				HPT[i][j].use = FALSE;
				HPT[i][j].dirty = FALSE;
				HPT[i][j].readOnly = FALSE;
				count++;
			}
		}	
	}

	else{	
	    pageTable = new TranslationEntry[numPages];
	    for (i = 0; i < numPages; i++) {
			pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
			//pageTable[i].physicalPage = i;	//Replace with pageTable[i].physicalPage = i + startPage;
			//pageTable[i].physicalPage = i + startPage;
			pageTable[i].valid = FALSE;
			pageTable[i].use = FALSE;
			pageTable[i].dirty = FALSE;
			pageTable[i].readOnly = FALSE;  // if the code segment was entirely on 
						// a separate page, we could set its 
						// pages to be read-only

		//Take the global bitmap and set the relevant chunks
		//to indicate that the memory is in use
		//memMap->Mark(i + startPage);
    }
}
    
}


void
AddrSpace::AssignPage(int vpn, int pAdr)
{
	startPage = pAdr;
	if(twoLevel){
		HPT[vpn / outerDivide][vpn % outerDivide].valid = TRUE;
		HPT[vpn / outerDivide][vpn % outerDivide].physicalPage = startPage;	
		
	}
	else{
		pageTable[vpn].valid = TRUE;
	
//	machine->pageTable[vpn].valid = TRUE;
		pageTable[vpn].physicalPage = startPage;
	}
	pAddr = startPage * PageSize;
	
	OpenFile * x = fileSystem->Open(fileN);
	
	memset(machine->mainMemory + pAddr, 0, PageSize);
	
	if (vpn * PageSize >= myNoff.code.virtualAddr && vpn * PageSize < myNoff.code.virtualAddr + myNoff.code.size) {
	
	/*
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			myNoff.code.virtualAddr + (startPage * PageSize), PageSize);
			*/		
        x->ReadAt(&(machine->mainMemory[myNoff.code.virtualAddr  + pAddr]),
			PageSize, myNoff.code.inFileAddr +  vpn * PageSize);    	
    }
  
   
    if (vpn *PageSize>= myNoff.initData.virtualAddr && vpn* PageSize < myNoff.initData.virtualAddr + myNoff.initData.size) {
    
    /*
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			myNoff.initData.virtualAddr + (startPage * PageSize),PageSize);
		*/
	
        x->ReadAt(&(machine->mainMemory[myNoff.initData.virtualAddr + pAddr]),
			PageSize, myNoff.initData.inFileAddr +  vpn* PageSize);
	}
	
	//ADD THINGS TO THE pageNumberList
	if(extraOutput){
    	printf("Page availability after adding the process: \n");
    	memMap->Print();
    }
	delete x;

}

void
AddrSpace::ReplacePage(int vAdr, int repAdr){
	printf("Virtual Address: %d   Replacement Physical Address: %d\n", vAdr, repAdr);
	if(twoLevel){
		HPT[vAdr / outerDivide][vAdr % outerDivide].valid = TRUE;
		HPT[vAdr / outerDivide][vAdr % outerDivide].physicalPage = repAdr;	
		
	}
	else{
		pageTable[vAdr].valid = TRUE;
		pageTable[vAdr].physicalPage = repAdr;
	}
	pAddr = repAdr * PageSize;
	OpenFile * x = fileSystem->Open(fileN);
	
	
	if (vAdr * PageSize >= myNoff.code.virtualAddr && vAdr * PageSize < myNoff.code.virtualAddr + myNoff.code.size) {
	
	/*
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			myNoff.code.virtualAddr + (startPage * PageSize), PageSize);
			*/		
        x->ReadAt(&(machine->mainMemory[myNoff.code.virtualAddr  + pAddr]),
			PageSize, myNoff.code.inFileAddr +  vAdr * PageSize);    	
    }
  
   
    if (vAdr *PageSize>= myNoff.initData.virtualAddr && vAdr* PageSize < myNoff.initData.virtualAddr + myNoff.initData.size) {
    
    /*
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			myNoff.initData.virtualAddr + (startPage * PageSize),PageSize);
		*/
	
        x->ReadAt(&(machine->mainMemory[myNoff.initData.virtualAddr + pAddr]),
			PageSize, myNoff.initData.inFileAddr +  vAdr* PageSize);
	}
	
	//ADD THINGS TO THE pageNumberList
	
	delete x;

	




}

void
AddrSpace::UpdateFile(int vAddr, int pAdr)
{
	if(twoLevel)
		HPT[vAddr / outerDivide][vAddr % outerDivide].physicalPage = NULL;
	else
		pageTable[vAddr].physicalPage = NULL;

	OpenFile * x = fileSystem->Open(fileN);
	
	
	x->WriteAt(&(machine->mainMemory[myNoff.code.virtualAddr + pAdr * PageSize]), PageSize, myNoff.code.inFileAddr + vAddr * PageSize);
	x->WriteAt(&(machine->mainMemory[myNoff.initData.virtualAddr, + pAdr * PageSize]), PageSize, myNoff.initData.inFileAddr + vAddr *  PageSize);
	delete x;
}




//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

//Because the initialization already zeroes out the memory to be used,
//is it even necessary to clear out any garbage data during deallocation?

AddrSpace::~AddrSpace()
{
	// Only clear the memory if it was set to begin with
	// which in turn only happens after space is set to true
//	int pagebegin = machine->pageTable[0].virtualPage;
	//printf("Begin Page: %d\n", pagebegin);
	if(space)
	{
		if(twoLevel){
			
			for(int i = 0; i < 4;i++){
				for(int j = 0; j < inner; j++){
					if(HPT[i][j].physicalPage < NumPhysPages && HPT[i][j].physicalPage >= 0){
						memMap->Clear(HPT[i][j].physicalPage);
					}
				}
			}
			for(int i = 0; i < 4; i++)
				delete [] HPT[i];
				
			delete [] HPT;
		
		}
		
		else{
			for(int i = 0; i < numPages; i++){	// We need an offset of startPage + numPages for clearing.
				if(pageTable[i].physicalPage < NumPhysPages && pageTable[i].physicalPage >= 0){
				//printf("Physical Page Delete: %d\n", pageTable[i].physicalPage);
				memMap->Clear(machine->pageTable[i].physicalPage);
				}

			}
			delete pageTable;
		}
		if(extraOutput){
			memMap->Print();
		}
		
	}
	
	//char* filename = new char[64];
	
	fileSystem->Remove(fileN);
	if(extraOutput){
		printf("Swapfile %s deleted\n\n", fileN);
	}	
	
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////END PROJECT 4///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
////////////////////START PROJECT 4///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void AddrSpace::SaveState() 
{
	if(twoLevel){
		HPT = machine->HPTable;
		
	}
	else{
 	   pageTable = machine->pageTable;
 	   
	}
	numPages = machine->pageTableSize;
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
	if(twoLevel){
		machine->HPTable = HPT;
		
	}
	else{
 	   machine->pageTable = pageTable;
 	   
	}
	machine->pageTableSize = numPages;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////END PROJECT 4///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
