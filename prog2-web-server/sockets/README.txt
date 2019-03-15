CpS 320 Program 2

Michael Bruno
3/15/2019

Checkpoint==========
    Time Spent: 5 hours
    Summary:
        - Rewrote Exercise 3 HTTP parsing logic
        - (CP) Reads in HTTP requests from client
        - (CP) Reads in remainder of HTTP request till an empty line with CRLF
        - (CP) Sends back HTTP response

    Problems:
        - Sometimes there is a memory leak after the
          server is closed in request header section.