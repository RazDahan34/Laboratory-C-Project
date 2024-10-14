
# Laboratory C Final Project

This repository contains the final project for the "20465 - Systems Programming Laboratory" course (2024B) at the Open University. The project is an assembler for a custom assembly language, implemented in C. The goal of this project is to emulate essential functionalities of system software, providing hands-on experience with large-scale software development.

## Project Structure

The project is organized into several C source and header files, each addressing a specific functionality within the assembler. It follows best practices for structured coding, data abstraction, and clear, maintainable code.

### Assembler Workflow

The assembler is divided into three key stages:

1. **Pre-Processing**
2. **First Pass**
3. **Second Pass**

These steps represent the core logic of the assembler's operation.

### Pre-Processing Stage
In the pre-processing stage, the assembler scans the source file line by line to identify and handle macro definitions. Macros are stored in a dedicated table and substituted throughout the file.

### First Pass
During the first pass, the assembler identifies symbols, such as labels, and assigns them memory addresses, which are stored for later use.

### Second Pass
The second pass translates the assembly instructions into binary machine code, substituting operation names with their corresponding binary representations and resolving symbol references with their memory addresses.

For detailed explanations of the algorithms used in each stage, refer to the project documentation.

## Prerequisites

Before running the project, ensure you have the following tools:

- **GCC Compiler**
- **Linux/Ubuntu** (other Linux distributions are also supported)

## Installation and Setup

To get started, follow these steps:

1. Clone the repository:
   ```bash
   git clone https://github.com/AlmogShKt/Labratory-C-Final-Project.git
   ```

2. Navigate to the project directory:
   ```bash
   cd Labratory-C-Final-Project
   ```

3. Switch to the Linux branch:
   ```bash
   git checkout linux
   ```

4. Compile the project using the provided Makefile:
   ```bash
   make
   ```
