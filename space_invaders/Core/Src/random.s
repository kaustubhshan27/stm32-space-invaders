/* Random number generator;
  Linear congruential generator
  from Numerical Recipes by Press et al.
  Jonathan Valvano */

/* How to use:
  1) call Random_Init once with a seed
      Random_Init(1);
      Random_Init(NVIC_CURRENT_R);
  2) call Random over and over to get a new random number
    n = Random32();    // 32 bit number
    m = (Random32()>>24)%60; // a number from 0 to 59 */

       .thumb							// tells the assembler to assemble the following code in Thumb mode
       .section .data,"aw",%progbits	// tells the assembler to assemble the following code into the .data section
       .align=2							// pads the location counter to a 2^n boundary
		M: .word 0						// places the variable M in the .data section
       .align=2							// pads the location counter to a 2^n boundary
       .section .text,"ax",%progbits	// tells the assembler to assemble the following code into the .text section
       .align=2							// pads the location counter to a 2^n boundary
       .global  Random_Init				// tells the assembler that the symbol that follows is a global symbol
       .global  Random32				// global symbols are visible to other modules that link to the current module

/* Input R0 is initial seed */
Random_Init:
       LDR R2,=M       // R2 = &M, R2 points to M
       STR R0,[R2]     // M=value from R0
       BX  LR

/* ------------Random32------------ */
/* Return R0= random number
  Linear congruential generator
  from Numerical Recipes by Press et al. */
Random32:
	   LDR R2,=M  			// R2 = &M, R4 points to M
       LDR R0,[R2]  		// R0=M
       LDR R1,=1664525
       MUL R0,R0,R1 		// R0 = 1664525*M
       LDR R1,=1013904223
       ADD R0,R1    		// 1664525*M+1013904223
       STR R0,[R2]  		// store M
       BX  LR
       .align
       .end
