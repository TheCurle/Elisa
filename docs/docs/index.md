Specification v3.5

# BRIEF

ELISA, or Proto, is a VM language to be run on top of a VM alike the JVM and/or CSR.  
It is to serve as a well constructed Intermediary Language (IL) for Erythro and Minerva, alongside any other language that may want to share the Proto backend.

The VM uses a (as of yet) minimal instruction set:

* ADD
* SUBTRACT
* DIVIDE
* MULTIPLY
* AND
* OR
* XOR
* NOT
* DATA
* LABEL
* COMPARE-
  * EQUALITY
  * LESS-THAN
  * GREATER-THAN
* JUMP-IF
  * NOT-EQUAL
  * EQUAL
  * ERROR
* FUNCTION
* RETURN
* DEREF
* POINTERTO
* CALL
* STORE
* WRITE
* DEBUG

There is also a special instruction called META, that can only be used before any VM code is written.  

It defines metadata of the file, which are used to generate constraints for the back-end interpreter and assembler. 
Through this, you can define properties like 64bit-only code.  

The VM is type-inferred, but the presence of the META property "typing" allows you to use strongly typed instructions.
This serves mostly as a debugging utility, but has other uses, such as JVM level output.  

The VM is register based, as opposed to a stack machine design. That meaning, it has a set of registers of an undetermined length;
r0 to r\<infinity\>.

This allows the VM language to serve as a SSA (Static Single Assignment) IL for the languages described above.
This design allows for more thorough optimization passes, as well as making the backend interpreter/assembler a heckuvvalot simpler to implement.  

The VM also has a few usage-specific registers:

 * comparison: Stores the result of a COMPARE-TO operation. Equivalent to three of the bits in an x86 FLAGS register.  
 * data[x]: Stores mutable and immutable data defined by the DATA instruction.  
 * param[x]: Stores the parameters of a function, the range x defined by the FUNCTION instruction and the registers filled by the CALL instruction.  


<hr />

VM code syntax is as follows:  
 > INSTRUCTION (DATA[:type])+ -> OUTPUT[:type]

The regex syntax (a)+ means: the contents of the brackets may be repeated any amount between 1 to infinity.  

The INSTRUCTION can be any of the above instruction set, as long as it is legal and recognised.    
Any amount of DATA input is valid, for eg. function calls with varargs input. As long as the instruction supports it, it'll work.  
DATA is space separated. This meaning, you cannot directly input a string or a char. You must store the string using DATA and use the data<x> index registers.  

The OUTPUT is where the result of the operation will be stored. Due to the SSA form, operations like ADD will not apply in-place to the source or value register, but instead must save into a third register.  

# Types

The Elisa spec recognizes six datatypes:  
 - char (aliased to i8)  
 - short (aliased to i16)  
 - int (aliased to i32)  
 - long (aliased to i64) [only accessible on META BITNESS 64 programs]  
 - DATASTRUCT (implied by the usage of a data register)  
 - DATASTATIC (implied by the usage of a data register)  
  
The differentiation between DATASTATIC and DATASTRUCT is an implementation detail.  
A sample implementation would keep track of the metadata of the data registers - whether it was created by a static or mutable DATA instruction.  

Type information may be appended to any data input and output.  

Floating point types may be handled in software, using whatever data type the standard library ends up using.  

# Instructions  
A thorough description of the VM instructions is as follows:  

## ADD IN IN -> OUT  
The two INputs are added together and the result is saved into the OUTput register, if the register is not already used.  

## SUBTRACT IN IN -> OUT  
The last INput is subtracted from the first, and the result is saved in the OUTput register, if the register is not already used.  

## MULTIPLY IN IN -> OUT  
The two INputs are multiplied together and the result is saved in the OUTput register, if the register is not already used.  

## DIVIDE IN IN -> OUT  
The first INput is divided by the last, and the result is saved in the OUTput register, if the register is not already used.  

## AND IN IN -> OUT
For every bit in the INputs, they are compared to each other and the result of the boolean AND operator is stored, per bit, in the OUTput.

## OR IN IN -> OUT
For every bit in the INputs, they are compared to each other and the result of the boolean OR operator is stored, per bit, in the OUTput.

## XOR IN IN -> OUT
For every bit in the INputs, they are compared to each other and the result of the boolean XOR operator is stored, per bit, in the OUTput.

## NOT IN -> OUT
For every bit in the INput, the result of the boolean NOT operator is stored in the OUTput.


## DATA [STATIC] <UNTIL LINE BREAK>  

Depending on the type of data being input, and whether the STATIC keyword is present, this instruction does many different things.  
In the simplest case, you may use the STATIC keyword to save a string to be placed into the RODATA of the application:  
 \> DATA STATIC "hello Forgecord"  

The STATIC keyword enforces using operating system level protection, that the data is never written to. If an attempt is made, the program will generate a General Protection Fault, sometimes called a Segfault in *NIX operating systems.  
The STATIC keyword may be omitted to allow the data to be written to. This allows for creation of structs, and similar data structures.  

You may then lock this structure:  
 \> DATA int int int char int  
 \> ADD $3 data0 -> r0           // r0 represents the char in the struct  
 \> WRITE $15 -> r0                  // Writing $15 into r0, which is now an alias of the char, saves into the mutable data0 struct.    

 \> DATA STATIC data0            // data1 now contains an immutable copy of data0. Reading data1 will return { 0, 0, 0, 15, 0 }  
                                // Writing to data0 will not write to data1. It can therefore be called a "snapshot copy".  

Upon creation of a DATA register, it is initialised to zeroes, unless an explicit initialiser is given:  
 
 \> DATA x0000000000000000 x0000000000000000 x00000000 x0000000000000000  
This is the equivalent of "DATA int int short int".  

 \> DATA x0000000000400000 x1237A71F17125BDA x51516112 xBABDABDBADBFABBE  
This is equivalent to "DATA int int short int" and saving these values individually.  

Both of these examples may have "DATA" replaced with "DATA STATIC" to write-protect them. Note that in this case, the DATA initiailiser form is the only way to assign non-zero values to a DATA STATIC register. It is, in effect, write-once.  

As touched on above, you may use string constants in lieu of manually writing out the char values:  
 \> DATA STATIC "sup"  
 \> DATA STATIC x73 x75 x70  
 // Demonstrate that these DATA operations are equivalent:  
 \> DATA "%d\r\n"   
 \> COMPARE-TO data0 data1  
 \> CALL printf data2 comparison  
 $ 1  

Every invocation of the DATA instruction creates a new register data<x> where the number appended represents the location of the data in the relevant executable section.  
This serves as a much cleaner way of indexing the data, as opposed to manually abusing pointer and type punning.  

Performing arithmetic on a data register acts very similar to pointer arithmetic - Every static data object defined in order may be traversed with addition and subtraction:  
 \> DATA x05  
 \> DATA x04   
 \> DATA xFFFFFFF2  
 \> STORE $2 -> r0    
 \> ADD r0 data0 -> r1  
// data0 is moved ahead 2 bytes - r1 now points to data2, or 0xF2 if it is read as a byte (on Little Endian systems, 0xFF on Big Endian)  


## LABEL name  
Simply creates a label for later use with JUMP and its variants.  
Essentially equivalent to the assembler line   
\> name:  

## COMPARE-TO

The COMPARE-TO instruction has its own dedicated output register: comparison.  

If the comparison returns true, the digit 1 is placed into the output register.  
If the comparison returns false, the digit 0 is placed into the output register.  
If the comparison generates an error (due to type mismatch, or other errors), the register is filled with 1s.  

The COMPARE-TO instruction has three intersections: 

### COMPARE-EQUALITY-TO x y  
The COMPARE-EQUALITY-TO instruction checks whether x and y are equal;  
 This can be a deep search when using data registers,  
 Or a simple numerical check when x and y are registers,  
 Or a simple check when x and y are value literals,  
 Or if one of the registers (for example, x) is the output of a DEREF, the value in the virtual address r[x] is compared against the value in y.  
 If both of the registers were initialised by DEREF, then the values at both virtual addresses are compared.  
  If one or both of the registers were initialised by the offset of a data register, then a deep scan is performed.  

If x and y are determined to be equal, then the comparison register is set to 1.  
If x and y are determined to be not equal, then the comparison register is set to 0.  
If the comparison generated an error, then the comparison register is filled with 1s.  

### COMPARE-LESS-THAN x y  
If either x or y are the output of a DEREF instruction, then they are parsed as virtual addresses and the underlying values are compared.  
If either x or y are value literals, that is used to compare.  

If x is determined to be less than y, the comparison register is set to 1.  
If x is determined to be greater than or equal to y, the comparison register is set to 0.  
If the comparison generated an error, then the comparison register is filled with 1s.  

### COMPARE-GREATER-THAN x y  
If either x or y are the output of a DEREF instruction, then they are parsed as virtual addresses and the underlying values are compared.  
If either x or y are value literals, that is used to compare.  

If x is determined to be greater than y, the comparison register is set to 1.  
If x is determined to be less than or equal to y, the comparison register is set to 0.  
If the comparison generated an error, then the comparison register is filled with 1s.  

## JUMP label  
JUMP simply moves the instruction pointer around; you can change the currently executed section of code to a label - no matter if the label has already been defined.  

### JUMP-IF-EQUAL label  
JUMP-IF-EQUAL executes JUMP if the comparison register holds a 1. Otherwise, it moves to the next line.  

### JUMP-IF-NOT-EQUAL label  
JUMP-IF-NOT-EQUAL executes JUMP if the comparison register holds a 0. Otherwise, it moves to the next line.  

### JUMP-IF-ERROR label  
JUMP-IF-ERROR executes JUMP if the comparison register holds all 1s. Otherwise, it moves to the next line.  

## FUNCTION name [x] [-1] | [y |]* 
 
FUNCTION defines a function label, which are handled much like labels; however, JUMP will not work for function labels - you must use CALL.  

The x input defines a number of inputs. It may be 0 to define varargs. 0 may be combined with a number to combine required vars and varargs, for a printf-like function:  
 \> FUNCTION kernprintf 1 -1 | // 1 required argument, combined with 0 to infinity arguments following.  
This code is equivalent to the C line:  
 \> void kernprintf(char* pattern, ...);  

The pipe symbol after the function params is required to end the parsing of parameters.  

Function returns are handled by appending a "r<y> |" block after the pipe symbol.  
The r number is used to define the register that the output is returned into.  

These return blocks may be defined multiple times, for languages that are able to handle them, like Erythro.  
This is not required, though; multiple returns are purely optional.   

For example, a function with three returns and the corresponding Erythro code:  
 \> FUNCTION getPCIDevice 1 | r0 | r1 | r3 |  
 | bus : line : device :: getPCIDevice(str deviceName);  

It is implied that any code written in Elisa has correct syntax, as it is designed to be the output of a compiler, which has higher level abilties to typecheck function inputs and outputs.    
Therefore, type checking in Elisa is not required, but this places a greater emphasis on the user writing direct Elisa code to ensure that the program is proper.  

## RETURN (reg)*  
The RETURN instruction is a utility operation.  
It should be treated as a no-op on most systems.  

On systems where it is needed, it takes the input registers and places them into whatever place they are required to suit the needs of the calling convention.  
It can therefore be treated as a stack wrapper for these systems.  
Registers are placed into core registers and on the stack in order of being passed here.  

## DEREF register -> OUTPUT  

To allow for more thorough pointer usage, and to conform with the C standard, the DEREF instruction operates a lot like the * operator in C.  
If you input a data register, the data indicated by that index is stored in OUTPUT. This will not generate an ISR unless there is external system modification occurring.  

If you input a normal register, the DEREF parses the number stored within as an address, and return *(size_t*)registerValue.   
This may generate an ISR, depending on the circumstances of the system running the code.  

## POINTERTO register -> OUTPUT 

This instruction is almost redundant when combined with the DATA registers, but it exists for compatibility with the C standard.  
The register input is parsed for history, and a pointer is generated that indicates the location in memory where the register is stored.  
If the register is stored in a core register, then this function will be a noop. If the register is stored on stack (for eg. function locals) then a stack pointer will be generated.  

The data registers supercede the need for this operation in most circumstances, and this exists purely for compatibility.  

## CALL function [param]*  

This is the equivalent of JUMP, but for FUNCTIONs.   
It passes each of the input parameters in order into the parameter registers param0-param<infinity>.    
It then JUMPs to the function code.  

It is intended that the assembler/interpreter handle the param register set, and convert it to the appropriate calling convention for the target system.  
This allows Elisa to be completely system agnostic.  

## STORE value -> register  

The input is parsed for a numerical value, and it is then moved into the register defined.  
The register may be defined in the form rx, or simply x, where x represents the index of the register.  

 \> STORE $50 -> r0  
 \> DATA "%d\r\n"  
 \> CALL printf data0 r0  
 $ 50  
 
## WRITE value -> register 

Write is an alias of DEREF register -> temp; STORE value -> temp;  
This means that the register is parsed for a location - it must hold a valid address or be a data register.  

This allows you to assign new values to mutable data structs:  

 \> DATA "hello Forgecord"  
 \> DATA STATIC "%s\r\n"  
 \> CALL printf data1 data0  
 \> WRITE "hello readers" -> data0  
 \> CALL printf data1 data0  
 $ hello Forgecord  
   hello readers  
 
## DEBUG
This instruction is akin to the 0xCA bytecode op - upon reading this, the interpreter shall seize execution but retain the global state.  
An assembler shall substitute this with an INT3 or similar operation.  

It serves as a breakpoint.  


## META property value

The META keyword is reserved for describing VM features.  
It allows you to define constraints that are specific to the program being described.

Initial properties, defaults, possible values:


|  PROPERTY NAME | DEFAULT VALUE |                 ACCEPTED VALUES                    |                DESCRIPTION               |  
|----------------|---------------|----------------------------------------------------|------------------------------------------|   
|     BITNESS    |      32       |                   16, 32, 64                       |    How wide is the target system word?   |  
|    REGISTERS   |       0       |            0 to 9223372036854775807                | How many registers does the program use? |  
|      TYPING    |  NOT PRESENT  |              PRESENT, NOT PRESENT                  |   Can the backend expect to see types?   |  
|     TRACKING   |  NOT PRESENT  |    [0 to infinity, 0 to infinity], NOT PRESENT     |        Source to IL line mappings        |  