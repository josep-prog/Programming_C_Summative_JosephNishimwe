# 

| Name | Link |
| :---- | :---- |
| Github | **[Programming\_C\_Summative](https://github.com/josep-prog/Programming_C_Summative_JosephNishimwe.git)**  |
| Smart Traffic Light Controller with Embedded Simulation and PCB Design | **[Project1](https://github.com/josep-prog/Programming_C_Summative_JosephNishimwe/blob/main/traffic_sim.c)**  |
| Linux Server Health Monitoring and Alert Automation Script | **[Project2](https://github.com/josep-prog/Programming_C_Summative_JosephNishimwe/blob/main/monitor.sh)**  |
| Course Performance and Academic Records Analyzer in C | **[Project3](https://github.com/josep-prog/Programming_C_Summative_JosephNishimwe/blob/main/records.c)**  |
| Data Analysis Toolkit Using Function Pointers and Callbacks | **[Project4](https://github.com/josep-prog/Programming_C_Summative_JosephNishimwe/blob/main/toolkit.c)**  |
| Multi-threaded Web Scraper | **[Project5](https://github.com/josep-prog/Programming_C_Summative_JosephNishimwe/blob/main/scraper.c)**  |

# 

# **Programming C Summative Challenges and Problem-Solving Reflection**

## **1\. Smart Traffic Light Controller**

### **Understanding Non-Blocking Execution**

One of the first challenges I faced was understanding how to simulate **non-blocking behavior**. The project referenced millis() from Arduino, but I was working in standard C, where that function does not exist.

Initially, I thought sleep() could replace it, but I quickly realized that sleep() blocks the entire program. That meant nothing else could run while waiting which defeated the purpose.

The solution came when I learned about **threads using pthreads**. I created a separate thread to simulate vehicle arrival:

// runs in background  
while (1) {  
   sleep(3);  
   inter\_a-\>vehicles \+= rand() % 4;  
   inter\_b-\>vehicles \+= rand() % 4;  
}

This allowed the system to:

* Keep updating traffic in the background  
* Still accept user input in the main thread

That was my first real understanding of **concurrency in C**.

### **Sharing Data Between Threads**

Once I introduced threads, I ran into another issue:  
 How do I share data between them?

Since pthread\_create() only accepts one argument (void \*), passing multiple structures felt confusing. I solved this by using **global pointers**:

struct Intersection \*inter\_a;  
struct Intersection \*inter\_b;

This allowed both the main thread and the sensor thread to access the same data. While globals are not always ideal, for this project they were a simple and effective solution.

### 

### **Adaptive Timing Logic**

Another challenge was defining what “adaptive traffic timing” actually means. I translated it into a simple rule-based system:

* Heavy traffic → longer green light  
* Medium traffic → moderate time  
* Low traffic → default time

if (vehicles \> 10\) green\_time \= 10;  
else if (vehicles \> 5\) green\_time \= 7;  
else green\_time \= 5;

This made the system dynamic and responsive.

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