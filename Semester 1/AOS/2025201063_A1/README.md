_____________________________________________________________________________________

Advanced Operating Systems – Assignment 1
_____________________________________________________________________________________

Overview:

This assignment consists of two main programs written in C++ using only system calls:

Q1: File Reversal with Multiple Modes – Reverse a given file in different ways based on the flag provided.
Q2: File & Permission Verification – Verify directory/file permissions and check whether the reversal result is correct based on the mode used.

The focus is on low-level file operations (read, write, lseek, stat, mkdir, open, close),
handling large files (>1GB) efficiently, and ensuring correct file permissions.
_____________________________________________________________________________________

Directory Structure:

    2025201063_A1/
       |-- 2025201063_A1_Q1.cpp
       |-- 2025201063_A1_Q2.cpp
       |-- README.md

_____________________________________________________________________________________

Q1. File Reversal with Multiple Modes
    Given a file, you need to reverse its contents using different reversal strategies and 
    store the result in a new file in the directory named Assignment1.

Instruction:
For 2025201063_A1_Q1.cpp File:

Compilation: 
    g++ 2025201063_A1_Q1.cpp

Flag 0:
    • Reverses the file in fixed-size blocks (block size must be specified as the 3rdargument).
    • Each block is reversed individually, but blocks remain in original order.

    Execution:   
        ./a.out <input file> 0 <block size>
        ex: ./a.out test.txt 0 5186

    Output file:
        Assignment1/0 <input file name>
        ex: Assignment1/0_test.txt

Flag 1: 
    • Reverses the entire file contents globally.
    • First byte becomes last, last byte becomes first, etc

    Execution:
        ./a.out <input file> 1
        ex: ./a.out test.txt 1
    
    Output file:
        Assignment1/1 <input file name>
        ex: Assignment1/1_test.txt

Flag2: 
    • Requires start and end indices as command-line arguments.
    • Reverses two parts: from beginning to start index, and from end index to end offile.
    • The portion between start and end indices remains unchanged.
    • Indices are zero-based; assume the first character is at index 0.

    Execution:
        ./a.out <input file> 2 <start index> <end index>
        ex: ./a.out test.txt 2 12 18
    
    Output file: 
        Assignment1/2 <input file name>
        ex: Assignment1/2_test.txt

Summary of Usage:
• ./a.out <input file> 0 <block size>
• ./a.out <input file> 1
• ./a.out <input file> 2 <start index> <end index>

_____________________________________________________________________________________

Q2. File & Permission Verification
    1. Check the permissions for the two files and the directory.
    2. Check whether the content in the new file follows the expected reversal pattern based on the flag used in Q1.

Verification Logic by Flag
    • Flag 0 (Block-wise Reversal):
        – Read both files in blocks of the specified size.
        – For each block, check that the new file’s block is the reverse of the old file’s block; blocks must appear in the same order.
    • Flag 1 (Full File Reversal):
        – Read from the start of the original and from the end of the new file in chunks.
        – Compare each chunk from the original with the corresponding reversed chunk from the new file.
    • Flag 2 (Partial Range Reversal):
        – Check that bytes [0, start index-1] in the new file are the reverse of [0, start index1] in the old file.
        – Check that bytes [start index, end index] are identical in both files.
        – Check that bytes [end index+1, EOF] in the new file are the reverse of the corresponding bytes in the old file.

Instruction:
For 2025201063_A1_Q2.cpp File:

Compilation:
    g++ 2025201063_A1_Q2.cpp 

    Flag 0:
        ./a.out <new_file_path> <old_file_path> <directory> 0 <block_size>

    Flag 1: 
        ./a.out <new_file_path> <old_file_path> <directory> 1

    Flag 2
        ./a.out <new_file_path> <old_file_path> <directory> 2 <start_index> <end_index>

Usage Examples
    • For flag 0: ./a.out Assignment1/0_input.txt input.txt Assignment1 0 1024
    • For flag 1: ./a.out Assignment1/1_input.txt input.txt Assignment1 1
    • For flag 2: ./a.out Assignment1/2_input.txt input.txt Assignment1 2 5 10

Output Format: 
    Directory is created: Yes  
    Whether file contents are correctly processed: Yes  
    Both Files Sizes are Same: Yes  
    User has read permissions on newfile: Yes  
    ...  
    Others has execute permission on newfile: No  
    (30 lines total: for new file, old file, and directory)

_____________________________________________________________________________________

Expected Permissions:

    Directory (Assignment1/): 700 → User: rwx, Group: none, Others: none.

    New file (Assignment1/Xinput.txt): 600 → User: rw-, Group: none, Others: none.

    Original file: 644 → User: rw-, Group: r--, Others: r--.

_____________________________________________________________________________________

Notes & Tips:

    -Run the program from the directory where your input file is located. This way, you only need to provide the file name, not the full path.
    -Make sure you have write permissions in the directory so that the Assignment1 folder can be created.
_____________________________________________________________________________________













