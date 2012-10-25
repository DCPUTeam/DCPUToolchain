# Debugging symbols file specification 
*Version 1*

Goal: to provide a format where symbols with information useful to debugging can be stored. 

## Data types
* **uint8**: An 8-bit unsigned integer.
* **uint16**: A 16-bit unsigned integer.
* **uint32**: A 32-bit unsigned integer.
* **string**: An array of ASCII characters, terminated by a NULL byte.

## File structure

The header is as follows

    [uint32] Magic number (0xC0DEBEAF)
    [uint32] Number of symbols
    […]      Symbols

Each debugging symbol has the following fields

    [uint16] Length (in bytes)
    [uint8]  Type
    […]      Contents

## Types of symbols

Each type of symbol has its own type identifier and its own fields.

Note: the *Address* field refers to the position in memory relevant to each symbol.

### Line - 0x01

    [string] Path
    [uint16] Line number
    [uint16] Address
    
### String - 0x02
    
    [string] Data
    [uint16] Address
    
### Label - 0x03

    [string] Label
    [uint16] Address
    
