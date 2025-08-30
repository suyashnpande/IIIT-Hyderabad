# Assignment 1 – Data Structures and Algorithms for Problem Solving

This repository contains my solutions for Assignment 1.  
I have implemented **Question 1** (Seam Carving) and **Question 2** (Trie-based Spell Checker).

## Directory Structure

2025201063_A1/   
│-- 2025201063_A1_Q1.cpp   
│-- 2025201063_A1_Q2.cpp  
│-- README.md

## Question 1: Seam Carving
### Problem  
Seam carving is a content-aware image resizing technique. It reduces image width or height by iteratively removing low-energy seams.

### Approach  
- **Energy Calculation:** Used dual-gradient energy (x and y differences in RGB values).  
- **Dynamic Programming:** Built DP tables to compute the minimal energy path for vertical and horizontal seams.  
- **Seam Removal:** Removed one seam at a time until the image reached the target dimensions.  
- **OpenCV:** Used to read, display, and save images. RGB values were stored in a 3D dynamic array.

### How to Compile ( Linux )
Make sure OpenCV is installed. Then run:

 <code>
g++ 2025201063_A1_Q1.cpp -o q1 `pkg-config --cflags --libs opencv4`
</code> 

### How to Run
<code>
./q1
</code>

### Program will:
- Ask for the image path.
- Display original dimensions.
- Ask for new dimensions (must be smaller).
- Show intermediate seams and final resized image.
- Save result as resizedImage.jpg.


## Question 2: Trie-based Spell Checker (30 marks)
### Problem
Design an efficient spell checker using the Trie data structure which supports the functionalities mentioned
below.
1. Spell Check: Check if the input string is present in the dictionary.
2. Autocomplete: Find all the words in the dictionary which begin with the given input.
3. Autocorrect: Find all the words in the dictionary which are at an edit distance (Levenshtein distance)
of at most 3 from the given input.

### How to Compile
<code>
g++ 2025201063_A1_Q2.cpp -o q2
</code>

### How to run
<code>
./q2
</code>

### Input Format
-  First line will contain two space-separated integers n, q which represent the number of words in the dictionary and the number of queries to be processed, respectively.
-  Next n lines will contain a single string s which represents a word in the dictionary.
- Next q lines will contain two space-separated values: an integer ai and a string ti .
- ai = 1 means Spell Check operation needs to be done on ti .
- ai = 2 means Autocomplete operation needs to be done on ti .
- ai = 3 means Autocorrect operation needs to be done on ti .
- Both strings s and ti consist of lowercase English letters.

### Output Format
For each query,print the result in a new line.
- Spell Check: Print ‘1’ if the string is present in the dictionary, otherwise ‘0’.
- Autocomplete & Autocorrect: Print the number of words in the first line. The following lines will
be the set of words in lexicographical order.


