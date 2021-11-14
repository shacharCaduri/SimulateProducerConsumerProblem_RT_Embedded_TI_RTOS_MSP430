# SimulateProducerConsumerProblem Project

### **IMPORTANT BEFORE READING!!!**

This Project has been done using the lecturer guidance and all photos here are the copyrights of this lecturer only and thus must not be used or copied in any kind of way anywere. Those photos represented here are only to make the reader here in this README file understand part of what has been done in the project and not by any means not using it or copy it in any other way except seeing it here.
Description

### **Project Description**
 This project is a part one of a college project done as a part of a "Real Time Embedded Systems" course, in this course's final project we were asked to simulate the producer consumer problem while using TI RTOS kernel and the IDE: Code Composer Studio with MSP430 micro controller card while following the rules of multithreaded programming and Real Time systems with priority based scheduler and thus simulating the Prodcer Consumer Problem as described in the final project specifications. The Conceptual Abstraction / Algorithm for simulating Prodcer Consumer Problem that this project followed is as shown here:

![Conceptual Algorithm - Producer Consumer Problem](https://user-images.githubusercontent.com/73823590/141676323-18dfca25-209a-46cb-999a-5f91213b2bf8.PNG)

This has been done with using OS and RTOS system concepts like putting a clock to make the RTOS simulate time sharing system which are not assuming anything about the when threads will gain\lost CPU, or using locks to synchronizing objects that wants to get access to shared location using some kinds of semaphores: Resource Control Semaphores, Mutexes, and Scheduling Constraint Semaphores, defend from deadlocks and synchronization problems and Etc. see source code for more detailed understanding.

## **What the application does?**

Simulating The Producer Consumer Problem using multithreaded programming and RTOS (real time operating system) kernel and principles.

## **Why I used this technologies?**

I used TI-RTOS kernel with the IDE Code Composer because it was a college course project which was required in the course's syllabus.
Some challenges

One of the challenges I faced is one of the critical challenges in Real Time Embedded projects is memory management, cause all things need to be done in a manner of saving memory, timing for buisness or safety crictical systems and this is because:

a. those systems usually have small / limited memory capicity proportion to the system needs.

b. those systems usually have one main goal which it needs to accomplish and using the memory in a smart and efficient way.

I have managed to do it in this manner because without managing the memory correctly the project can't work because as mentioned before this project and even greater projects in RTOS embedded needs to meet conditions of memory saving so it can work because it have limited memory, but saving memory in a more efficient way is something that can always be improved and always should be, in those kind of systems.

## **How to install**
To install this project (INSTALLATION through Code Composer Studio) :

a. Download the project as an archived zip file (you can do it by clicking the green button 'Code' then 'Download ZIP').

b. Open Code Composer Studio (CCStudio) then click on the TAB project:

![click on project](https://user-images.githubusercontent.com/73823590/141676532-1d3c7469-b26c-40f5-8324-770c23d7cfb1.PNG)

c. Click on import CCS Project:

![import ccs project](https://user-images.githubusercontent.com/73823590/141676683-2d977974-84ca-4835-b7d7-08e02ca00bdc.png)

d. In the opened window that opened click on browse, and browse the archived zip version of this project you dowloaded earlier:

![click browse search your project winrar zip archive](https://user-images.githubusercontent.com/73823590/141676700-7649f648-c82d-499f-a264-607895f36e2c.png)

e. After that you will be able to click on the finish button, press finish and your project is installed on CCStudio.

![click finish - test project](https://user-images.githubusercontent.com/73823590/141676800-70c40ab7-33b2-4c59-b09c-9ad5f947eef9.PNG)

For any kind of issues or suggestions in the context of this installation you can contact me.

## **How to use?**

This project purpose for simulation only with the MSP430F5529 Micro controller card, and you use it by running it on this micro controller card that has been specified earlier here and in the description, I suggest anyone who want to know how to run will search for a guide how to run a project on CCStudio even tough it is done by simply search the button build\rebuild and then click debug and let it run, Further more it is important to mention that because this project run on a micro controller card it also has led blink service specification in simple words it is indicators on the card that it is running on and the indicators are the leds that are turning on according to this specification:

led blink specification

![image](https://user-images.githubusercontent.com/73823590/141676963-c445dc04-cb0c-444a-9f4f-5b745271099a.png)

The LEDs on the conroller should blink as described here in the photos, and the user can debug and see the behavior of the the threads in the system and thus see it is actually simulating the panama cannal as show in the conceptual abstraction in the description earlier in this document.
