# 

| Name | Link |
| :---- | :---- |
| Github | **[Programming\_C\_Summative](https://github.com/josep-prog/Programming_C_Summative_JosephNishimwe.git)**  |
| Smart Traffic Light Controller with Embedded Simulation and PCB Design | **[Project1](https://github.com/josep-prog/Programming_C_Summative_JosephNishimwe/blob/main/traffic_sim.c)**  |
| Linux Server Health Monitoring and Alert Automation Script | **[Project2](https://github.com/josep-prog/Programming_C_Summative_JosephNishimwe/blob/main/monitor.sh)**  |
| Course Performance and Academic Records Analyzer in C | **[Project3](https://github.com/josep-prog/Programming_C_Summative_JosephNishimwe/blob/main/records.c)**  |
| Data Analysis Toolkit Using Function Pointers and Callbacks | **[Project4](https://github.com/josep-prog/Programming_C_Summative_JosephNishimwe/blob/main/toolkit.c)**  |
| Multi-threaded Web Scraper | **[Project5](https://github.com/josep-prog/Programming_C_Summative_JosephNishimwe/blob/main/scraper.c)**  |
| Smart Traffic Light Controller with Embedded Simulation and PCB Design | **[view setup on tinkercard](https://www.tinkercad.com/things/kGNADKLfpu3/editel?returnTo=%2Fdashboard%2Fdesigns%2Fall&sharecode=NTuUN1g8f5Pgf3aA4MyZ2CTWlG_ybwyzzkQV0P55qFU)**  |

# 

# **Programming C Summative Challenges and Problem-Solving Reflection**

## **1\. Smart Traffic Light Controller**

![][image1]

When I started this project, I had a circuit with an Arduino UNO, a breadboard, six LEDs arranged as two traffic lights, resistors for each LED, and two push buttons. I built it in Tinkercad, and visually it looked correct. The wires were connected, the resistors were in place, and everything seemed fine. However, when I ran the simulation, both traffic lights remained on red and never changed. That was the first problem I had to figure out.

My first instinct was that something was wrong with the wiring. I carefully checked the breadboard connections, inspected the resistors, and traced each wire. Everything still looked correct. So I moved into the code, and that is where I found the real issue. The code had been written with a menu system, meaning the traffic light would only start cycling after I opened the Serial Monitor and typed the number **1**. The program had a variable called cycle\_running that started as false, and the entire state machine simply skipped itself while that remained false:

void run\_cycle\_state\_machine() {  
   if (\!cycle\_running) return;  
}

So the lights were not broken at all. The program was simply waiting for a command that I did not know I had to send. Once I understood that, the fix was simple I added one line at the end of setup() to auto-start the cycle the moment the board powered on:

handle\_menu(1);

That got the lights moving. But then I looked at the rest of the code and realized there was a deeper problem. The code was over 200 lines long and included logging, a serial menu, manual override commands, and a state machine with both a cycle\_running boolean and a cycle\_phase integer tracking things separately. For me, coming from a C background and being new to Arduino, it was difficult to follow what was actually doing the important work and what was just extra. The actual logic of a traffic light is really just four steps repeating forever  A goes green, A goes yellow, B goes green, B goes yellow, then back to the start. All the extra features were hiding that simple truth.

I also kept the adaptive green time feature because it connected the buttons to the actual behavior  when I press a button, it adds a vehicle to that intersection’s count, and that count changes how long the green light stays on:

unsigned long green\_time(struct Light \*x) {  
   if (x-\>vehicles \> 10\) return 10000;  
   if (x-\>vehicles \>  5\) return  7000;  
   return 5000;  
}

That connection between the physical button, the vehicle count, and the light timing was the part I was most satisfied with because it made the simulation feel realistic rather than just a fixed blinking pattern.

One smaller issue I noticed but did not fully fix was that the buttons added vehicles too fast. Since loop() runs thousands of times per second, holding a button down would cause the vehicle count to spike instantly. In a real physical build, this would need debounce handling, but in the Tinkercad simulation it was not critical enough to block the project.

## **2\. Linux Server Health Monitoring Script**

### **Adjusting to Bash Syntax**

Moving from C to Bash was surprisingly difficult. The biggest issue was **how Bash handles comparisons**.

In C:

if (cpu \> 80\)

In Bash:

if \[ "$cpu" \-gt 80 \]

Using \> by mistake caused unexpected behavior because Bash treats it as file redirection. This was one of the first bugs I encountered.

### **Reading CPU Usage**

The project suggested using top, but it is interactive and hard to extract values from. I discovered /proc/stat, which provides raw CPU data.

grep 'cpu ' /proc/stat | awk '{printf "%.0f", ($2+$4)\*100/($2+$4+$5)}'

This approach helped me understand how Linux exposes system-level information.

### **Handling User Input Safely**

To allow users to change thresholds at runtime, I needed input validation. I used regex:

\[\[ "$1" \=\~ ^\[0-9\]+$ \]\]

This ensured only valid numeric input was accepted.

### **Graceful Shutdown**

By default, Ctrl+C kills a script instantly. I learned to handle it using:

trap 'echo "Stopped"; exit 0' INT

This made the script exit cleanly instead of abruptly.

## **3\. Academic Records Analyzer (C)**

### **Dynamic Memory Management**

One major challenge was handling an unknown number of students. I implemented **dynamic array resizing using realloc**:

int new\_cap \= (capacity \== 0\) ? 4 : capacity \* 2;  
struct Student \*tmp \= realloc(students, new\_cap \* sizeof(struct Student));

I also learned an important safety rule:

Never assign realloc directly — always use a temporary pointer.

### **Sorting Without Built-in Functions**

Since I couldn’t use qsort, I implemented **bubble sort manually**:

for (i \= 0; i \< n \- 1; i++) {  
   for (j \= 0; j \< n \- 1 \- i; j++) {  
       if (students\[j\].gpa \< students\[j+1\].gpa) {  
           // swap  
       }  
   }  
}

This helped me understand how sorting actually works internally.

### **Fixing fgets() Newline Issues**

A very frustrating bug came from fgets() storing the newline character:

void fix\_newline(char \*s) {  
   char \*p \= strchr(s, '\\n');  
   if (p) \*p \= '\\0';  
}

Without this fix, string comparisons would silently fail.

### **Median Calculation Without Data Loss**

To compute the median without modifying the original dataset, I created a copy:

float temps\[n\];  
for (i \= 0; i \< n; i++) temps\[i\] \= students\[i\].gpa;

This preserved the original order while allowing sorting.

## **4\. Data Analysis Toolkit (Function Pointers)**

### **Understanding Function Pointers**

Initially, function pointers were confusing. The syntax looked complex: void (\*fn)(void);

But I learned that functions have memory addresses just like variables.

### 

### **Replacing Switch with Dispatch Table**

Instead of using a long switch statement, I used a **function pointer array**:

struct MenuItem {  
   char name\[50\];  
   void (\*fn)(void);  
};

menu\[choice \- 1\].fn();

This made the program cleaner and more scalable.

### **Using Callbacks for Flexibility**

For filtering, I used a callback function:

void apply\_filter(int (\*cb)(double)) {  
   if (cb(data\[i\])) {  
       result\[count++\] \= data\[i\];  
   }  
}

This allowed the filtering logic to change dynamically.

## **5\. Multi-threaded Web Scraper**

### **Using libcurl in C**

Unlike Python, C has no built-in HTTP tools. I used **libcurl**, which required understanding its structure:

curl\_easy\_setopt(curl, CURLOPT\_WRITEFUNCTION, save\_data);

The write callback allowed data to be written directly to a file.

### **Passing Data to Threads**

To pass multiple values to threads, I used a struct:

struct Job {  
   int num;  
   char url\[256\];  
};

This made thread communication structured and clean.

### **Ensuring Threads Finish Execution**

I discovered that without pthread\_join, the program could exit too early:

pthread\_join(threads\[i\], NULL);   
This ensured all downloads were completed before termination.

[image1]: summativeCircuit_complete_setup.png