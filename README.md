# Pak Suzuki Assembly Line Parts Production. (Producer-Consumer Problem)

## Project Description & Mechanism:

This project is a multithreaded program that resembles the manufacturing and packaging of auto spare parts. To temporarily store parts created by a producer thread before they are consumed 
by a consumer thread and assembled into boxes, the program creates a buffer. The user can choose which kind of part to produce by entering the quantity and names of different car spare part types into the program. Additionally, the program offers the user multiple ways to see what's in the buffer, a list of the different kinds of parts, what's in the packed boxes, and how many parts are in the buffer. To keep the producer and consumer threads synchronized and avoid race situations, the program makes use of mutexes and semaphores. As the item(auto spare part) is consumed, it will be removed from the buffer/shared Memory region.
