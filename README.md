# Webserv
## A basic HTTP server in C++98

Webserv is a project that consists of creating a simple **HTTP/1.1 web server** written in C++98, without using any external web server. The goal is to understand how web servers work internally by **handling connections**, **parsing HTTP requests and send the responses**, handling **CGI scripts** and **methods**...

For this project, we successfully implemented a **fully non-blocking HTTP server using epoll()** with listening **sockets** to efficiently manage **multiple client connections**. We integrated **CGI** support by running a **Python script** that functions as an online calculator, demonstrating dynamic content generation.

Our server handles the HTTP methods **GET**, **POST**, and **DELETE**, and supports **auto-indexing** through a **directory listing** optional (if the index.html is not found). The **configuration file** format was inspired by **NGINX** and is parsed with a custom syntax parser that **builds an object tree** used later in the execution phase.

This project was an excellent opportunity to deepen my **understanding of C++** and apply all the concepts covered in previous modules, including low-level socket programming, event-driven I/O, and object-oriented design. I also **revisited pipes**, which I had previously coded in C to reproduce a shell, but this time used them for handling CGI processes.

GIT APPRENTISSAGE

The project emphasizes performance, correctness, and compliance with standard HTTP protocol 1.1 behavior.
