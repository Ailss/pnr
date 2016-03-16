# pnr


Step to start:
------------------------------------------
cd src  
make  
./pnr.elf [the blif file]  

The "stderr" output is the text format of the routing logic   
The "stdout" output is the binary foramt of the routing logic   

Hint: If you don't know how to save the stdout or stderr to a file, please google "linux I/O Redirection" or "linux Redirection"  

Samples
-----------------------------------------
two simple samples of blif file.  
./pnr.elf ../sample/ab.blif  
./pnr.elf ../sample/and.blif  



Bitstream:
----------------------------------------
  Becauase the minimum output of C++ is one byte(8 bits), I change the format of the bitstream a little bit. Please check your hardware design and change it a little bit.  
  
### The old rule is described as following
  
  
  Routing bitstream:                      // the ' is just used in the comment!  
      0: 6bits  6bits  6bits  6bits      //for example, if the input is (255,128,66,1), the real bitstream is 111111'000000'000010'000001  
      (input3)(input2)(input1)(input0)  
      1: 6bits 6bits 6bits 6bits  
      2: 6bits 6bits 6bits 6bits  
      .  
      .  
      .  
      .  
      239: 6bits 6bits 6bits 6bits  
  
  LUT bitstream:  
      0: 16bits(LUT_INIT) 1bit(DFF enable) 1bit(DFF reset)  
      1: 16bits 1bit 1bit  
      2: 16bits 1bit 1bit  
      ..  
      ..  
      ..  
      ..  
      239: 16bits 1bit 1bit  
      
  
  
  check the order of LUT_INIT.  
  If the bitstream is: 1110101011101010  
  
  
1  | 1  | 1  | 0  | 1  | 0  | 1  | 0  | 1  | 1  | 1  | 0  | 1  | 0  | 1  | 0    
1111  | 1110  | 1101  | 1100  | 1011  | 1010  | 1001  | 1000  | 0111  | 0110 | 0101  | 0100  | 0011  | 0010  | 0001  | 0000  
  
   
###  The format I am using. 
  Routing bitstream:                 //I add two extra 0 to the most significant bits  
      0: 8bits 8bits 8bits 8bits           //the new format corresponding to the above example is 00111111'00000000'00000010'00000001  
      1: 8bits 8bits 8bits 8bits  
      2: 8bits 8bits 8bits 8bits  
      .  
      .  
      .  
      .  
      239: 8bits 8bits 8bits 8bits  
  
  LUT bitstream:                    //I add 14 extra 0 to the most significant bits  
      0: 14bits(0) 16bits 1bit 1bit  
      1: 14bits(0) 16bits 1bit 1bit  
      2: 14bits(0) 16bits 1bit 1bit  
      ..  
      ..  
      ..  
      ..  
      239: 14bits(0) 16bits 1bit 1bit  

  
