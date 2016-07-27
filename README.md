# RasPi-Mastermind
Mastermind game using a Raspberry Pi

<h2> Introduction & Problem Specification </h2> 

Our task as hand was to implement our knowledge of C and assembly language with the use hardware wiring techniques using the Raspberry Pi 2. 
The challenge we are given lies with creating an application where a user need not type in numbers into a terminal, 
but instead should be able to use the provided breadboard to input values and initiate a response. 
We are to create a logical and physical system which interacts with the input and output devices, 
along with providing a user the ability to interact with the said system. The main application is based
on a board game named ‘Mastermind’, which consists of two players, a person who creates a secret combination, 
and a person who must guess a secret combination within a set number of guesses. 
Unlike our previous individual coursework, our goal is to use the raspberry Pi to accept user input values, 
process them using the created program logic, and display the output on an LCD display, rather than using the terminal.  

---

<h2>Hardware Specification</h2>

The following hardware specifications are based from the header file attached with this document. 
Please refer to that file if necessary. 
```
Red LED – GPIO 26 
Yellow LED – GPIO 12 
Button – GPIO 16 
LCD Register Select – GPIO 25 
LCD Enable – GPIO 24 
LCD Data 4 – 23 
LCD Data 5 – 17 
LCD Data 6 – 27 
LCD Data 7 – 22 
```

---

<h2>Code Structure</h2>

The code is contained within three files to provide an easier understanding of how each section of the code behaves,
and also to make it neat and editable when necessary. The header file contains all the definitions used in the code,
along with the ‘struct’ to hold information for the guesses and secret, along with the LCD information.
The header also declares the functions, which are implemented in the ‘cw3fun.c’ file.

---

<h2>Running</h2>

In order to run the code within a Raspberry Pi, the following code must be used:
```
sudo ./cw3
```

In order to run it in debug mode (check step by step process), the following code must be used:
```
sudo ./cw3 -d
```

In order to run it in verbose debug mode (clear outline of process), the following code must be used:
```
sudo ./cw3 -vd
```

---

<h2>Note</h2>

We are aware that some people may not have the Raspberry Pi and thus we have included a file named 'output.txt' which shows the output in debug mode.

---
