#include <stdlib.h>
#include <iostream>
#include <string>

namespace MemoryManager
{

  struct memBlock
  {
    //struct memBlock* next;
    unsigned int size;
    bool isFree;
  };

  #define BLOCK_SIZE sizeof(memBlock)

  //TODO remove this in favor of depending on blocks to determine data
  memBlock* start;
  unsigned int size;

  // private namespace
  namespace
  {
    unsigned int continuousMemory(memBlock* block)
    {
      memBlock* currBlock = block;

      unsigned int currSize = 0;

      while (currBlock->isFree && currSize < size)
      {
        currSize += currBlock->size + BLOCK_SIZE; //TODO need to add blocksize to all but first
        currBlock = &currBlock[currBlock->size + BLOCK_SIZE];
      }


      if (currSize > 0)
      {
        // account for header of the first block
        currSize -= BLOCK_SIZE;

        // rewrite the curr block to account for continuous memory
        block->size = currSize;
      }


      return currSize;
    }

    void onOutOfMemory(void)
    {
      std::cerr << "Out of memory" << std::endl;
      exit(1);
    }

    memBlock* getNextBlock(memBlock* block)
    {
      memBlock* nextBlock = &block[block->size + BLOCK_SIZE];
      return nextBlock >= &start[size] ? NULL : nextBlock;
    }
  }

  void initialize(void* aStart, unsigned int aSize)
  {
    start = (memBlock*) aStart;
    start->isFree = true;
    start->size = aSize - BLOCK_SIZE;

    size = aSize;
  }

  void* allocate(unsigned int aSize)
  {
    memBlock* block = start;

    while (block != NULL)
    {
      unsigned int availableMemory = continuousMemory(block);
      if (availableMemory > aSize)
      {
        // suitable block has been found
        block->isFree = false;

        if (block->size > aSize + BLOCK_SIZE)
        {
          memBlock* newBlock = &block[aSize + BLOCK_SIZE];
          //memBlock* newBlock = getNextBlock(block);
          if (newBlock < &start[size])
          {
            newBlock->isFree = true;
            newBlock->size = availableMemory - aSize - BLOCK_SIZE;

            block->size = aSize;
          }
        }

        return &block[BLOCK_SIZE];
      } else
      {
        //block = (memBlock*) &block + block->size;
        block = getNextBlock(block);
      }
    }

    onOutOfMemory();
  }

  void deallocate(void* addr)
  {
    //TODO
    memBlock* block = &((memBlock*) addr)[-BLOCK_SIZE];
    block->isFree = true;
  }

  unsigned int freeRemaining(void)
  {
    unsigned int freeMemory = 0;

    memBlock* block = start;
    while (block != NULL)
    {
      //std::cerr<<"iteration"<<std::endl;
      if (block->isFree)
      {
        freeMemory += block->size;
      }
      block = getNextBlock(block);
    }

    return freeMemory;
  }

  unsigned int smallestFree(void)
  {
    unsigned int smallestBlock = size;

    memBlock* block = start;
    while (block != NULL)
    {
      if (block->isFree && continuousMemory(block) < smallestBlock) {
        smallestBlock = continuousMemory(block);
      }
      //TODO this needs to account for the fact i've already looked up continuous memory
      block = getNextBlock(block);
    }

    if (smallestBlock == size)
    {
      smallestBlock = 0;
    }

    return smallestBlock;
  }

  unsigned int largestFree(void)
  {
    unsigned int largestBlock = 0;

    memBlock* block = start;
    while (block != NULL)
    {
      if (block->isFree && continuousMemory(block) > largestBlock) {
        largestBlock = continuousMemory(block);
      }
      //TODO this needs to account for the fact i've already looked up continuous memory
      block = getNextBlock(block);
    }

    return largestBlock;
  }

};

const int MEMORY_MANAGER_SIZE = 65536;
void* managedMemory[MEMORY_MANAGER_SIZE];

void report(std::string str) {
  std::cout << str << ": " << std::endl;
  using namespace MemoryManager;
  std::cout << "Free: " << freeRemaining() << std::endl;
  std::cout << "Smallest: " << smallestFree() << std::endl;
  std::cout << "Largest: " << largestFree() << std::endl;

  std::cout << std::endl;
}

int main(void)
{
  using namespace MemoryManager;
  initialize(managedMemory, MEMORY_MANAGER_SIZE);
  report("Initialized");

  void* allocs[10];
  for (int i = 0; i < 10; i++) {
    allocs[i] = allocate(10);
  }
  report("Allocate 10 x 10");

  void* alloc100 = allocate(100);
  report("Allocate 100");

  void* alloc200 = allocate(200);
  report("Allocate 200");

  for (int i = 0; i < 5; i++) {
    deallocate(allocs[i]);
  }
  report("Deallocate 10x5");

  deallocate(alloc100);
  report("Deallocate 100");

  deallocate(allocs[8]);
  report("Deallocate 10x1");
}
