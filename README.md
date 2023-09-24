# Project: Secure Message Transmission

## Description:

This project aims to create a secure message transmission system using a 
multi-processing approach. It's designed to allow legitimate receivers to decode encoded messages successfully while thwarting master spy processes from intercepting and decoding the messages.

## Key Components and Processes:

- Process Creation:
A parent process initializes the system, creating several child processes, including a sender, a receiver, a master spy, a set of helper processes, and a group of spy processes.

- Message Encoding and Splitting:
The sender process retrieves a message from an input file and splits it into columns based on word separations. If a column has an empty string, it's replaced with "alright."

- Child Process Encoding:
Each child process is responsible for encoding a specific column of the message. Encoding involves shifting characters within words based on column and position, with special character transformations and number encoding.
The encoded columns are placed in shared memory.

- Helper Processes:
Helper processes continuously shuffle the messages in shared memory to hinder spy processes' attempts to acquire all columns. They swap encoded messages between different locations in the shared memory.

- Spy Processes:
Spy processes randomly access shared memory locations to retrieve encoded messages. They transmit these messages to the master spy process.

- Master Spy Process:
The master spy process attempts to order the columns correctly. It drops columns it already received. Once it believes it has all the columns, it decodes the messages and stores them in a file called "spy.txt."

- Receiver Process:
Similar to the master spy process, the receiver process retrieves encoded messages from shared memory, orders them correctly, and drops duplicates. When confident, it decodes the messages and saves them in a file called "receiver.txt."

- Parent Process Evaluation:
The parent process evaluates whether the receiver successfully deciphers the message before the master spy process. If successful, the operation is marked as a success; otherwise, it's considered a failure.

### Simulation Termination:

The simulation may end under the following conditions:

The receiver fails to decode messages beyond a user-defined threshold.
The receiver successfully decodes messages beyond a user-defined threshold.
This project showcases a unique approach to secure message transmission through encoding, shuffling, and decoding, with the main goal of ensuring that legitimate recipients can decipher messages before any master spy. The success or failure of the operation is determined by these conditions.
