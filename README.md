# Memory Manager

This is a memory manager written in C++. It is designed to be initialized with a large chunk of memory to carve into any number of variable sized blocks. Pointers to these blocks are then passed back.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Demo

A test program is included that calls allocate and deallocate functions from the memory manager, and then reports on the memory in the array. This information includes available memory, smallest available block, and largest available block;

```
./a.out
```

### Usage

This module can be modified to more closely fit your needs. This is designed as a general use memory manager (see Design below), and your software may benefit from being optimized for your use case. Making changes requires a C++ compiler (such as g++).

To compile:

```
g++ MemoryManager.cpp
```

### Notes

There are some compromises with the way headers are set up for each block of memory. Because data of the block is handled in the memory block provided to the manager, each block requires some bytes of overhead. This means you wont be able to use all the available free memory if it requires multiple allocations.

## Memory Management

### Why use one?

Using malloc and new in C++ requires the operating system to switch contexts between user-space code and kernel code. This context switching can cause a performance decrease in software that requires constant allocations to be made. By handling allocations primarily in user-space, this context switch is avoided.

Additionally, by handling all of a programs allocations through a custom manager, the entire block of memory can be returned before the program terminates. This ensures there are no memory leaks.

### Design

This Memory Manager is designed as a general purpose memory manager, designed to hold blocks of any size, and in any amount. It is to take in a large block of memory to be divided into smaller blocks as needed. The manager uses header information for each block to determine if a block is available, and how large the block is. As currently implemented, it is assumed that there will only be one memory manager in a program, based on a single block of memory. Neighboring blocks of memory that aren't in use are treated as a single large block. This allows for better fitting into the array and reducing fragmentation.

As is, each of these blocks has a header that contains size information, and a flag to note if it is free or in use.

This can be optimized for your use case if sizes of objects are known, which means size information is not needed in the header. This approach will be better memory optimized, but less flexible.

Another option is to treat memory blocks as a linked list, with a pointer to the next memory location. This will allow the logic to traverse the linked list, which does not require the memory to be handled as one large block. This can be more flexible, but requires more system calls or pre-allocated memory.
