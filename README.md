# ft_irc — Internet Relay Chat

Version: 9.1

## Summary
This project is about creating your own IRC server. Use an actual IRC client to connect to your server and test it. Understanding standard Internet protocols is beneficial.

---

## Table of contents
- [Introduction](#introduction)
- [General rules](#general-rules)
- [AI instructions](#ai-instructions)
- [Mandatory part](#mandatory-part)
	- [Requirements](#requirements)
	- [For macOS only](#for-macos-only)
	- [Test example](#test-example)
- [Bonus part](#bonus-part)
- [Submission and peer-evaluation](#submission-and-peer-evaluation)
- [Checksums](#checksums)

---

## Introduction
Internet Relay Chat (IRC) is a text-based communication protocol offering real-time messaging (public or private). Clients connect to servers to join channels; servers may be networked.

---

## General rules
- The program must not crash under any circumstance. If it crashes, the project is non-functional (grade 0).
- Provide a Makefile that compiles source files and avoids unnecessary relinking.
	- Required rules: $(NAME), all, clean, fclean, re.
- Compile with g++ using flags: `-Wall -Wextra -Werror`.
- Your code must comply with the C++11 standard. It should compile with `-std=c++11`.
- Prefer C++ standard headers and functions (e.g., `<cstring>` over `<string.h>`). C functions are allowed but prefer C++ equivalents.
- External libraries (including Boost) are forbidden.

---

## AI instructions

### Context
AI can assist many tasks. Explore AI capabilities but assess results critically. AI output may be inaccurate; review with peers.

### Main message
- Use AI to reduce repetitive tasks.
- Develop prompting skills.
- Learn how AI systems work and anticipate risks/biases.
- Build technical and collaboration skills.
- Only use AI-generated content you fully understand and can own.

### Learner rules
- Explore AI tools and understand how they work.
- Reflect before prompting to write clearer prompts.
- Systematically check and test AI outputs.
- Seek peer review.

### Outcomes, comments, and examples
- Develop prompting skills and boost productivity.
- Use peers for context-specific feedback and validation.
- Good/bad practice examples illustrate appropriate use of AI.

---

## Mandatory part

Program name: `ircserv`  
Files to submit: `Makefile`, `*.h`, `*.hpp`, `*.cpp`, `*.tpp`, `*.ipp`, and an optional configuration file.

Makefile must include: `NAME`, `all`, `clean`, `fclean`, `re`.

Arguments:
- port: listening port
- password: connection password

External functions allowed (TCP/IP / system calls): socket, close, setsockopt, getsockname, getprotobyname, gethostbyname, getaddrinfo, freeaddrinfo, bind, connect, listen, accept, htons, htonl, ntohs, ntohl, inet_addr, inet_ntoa, inet_ntop, send, recv, signal, sigaction, sigemptyset, sigfillset, sigaddset, sigdelset, sigismember, lseek, fstat, fcntl, poll (or equivalent)

Libft: n/a

Description: An IRC server implemented using the C++11 standard. You must not implement an IRC client or server-to-server communication.

Executable usage:
```
./ircserv <port> <password>
```

- `port`: port for incoming IRC connections
- `password`: connection password required by clients

Note: Although poll() is mentioned, you may use an equivalent (select(), kqueue(), epoll()).

---

### Requirements
- The server must handle multiple clients simultaneously without hanging.
- Forking is prohibited. All I/O must be non-blocking.
- Use a single poll() (or equivalent) instance to handle all I/O (read, write, listen, etc.).
	- If you perform read/recv or write/send on descriptors without poll() (or equivalent), the grade is 0.
- Choose one reference IRC client; it will be used for evaluation and must connect without errors.
- Communication must use TCP/IP (IPv4 or IPv6).
- Implement at least:
	- Authentication, nickname, username
	- Join a channel
	- Send/receive private messages
	- Messages sent to a channel must be forwarded to all other clients in that channel
	- Operators and regular users
	- Channel-operator commands: KICK, INVITE, TOPIC, MODE with modes:
		- i: invite-only channel
		- t: restrict TOPIC to channel operators
		- k: set/remove channel key (password)
		- o: give/take operator privilege
		- l: set/remove user limit
- Code should be clean and maintainable.

---

## For macOS only
macOS handles write() differently. You may use `fcntl()` to set non-blocking mode:
```
fcntl(fd, F_SETFL, O_NONBLOCK);
```
No other flags are permitted. File descriptors must be non-blocking.

---

## Test example
Verify handling of partial data and low-bandwidth scenarios. Example nc test:
```
$ nc -C 127.0.0.1 6667
com^Dman^Dd
$
```
Use Ctrl+D to send the command in parts: "com", then "man", then "d\n". You must aggregate received packets to rebuild commands.

---

## Bonus part
Optional features to make the server closer to real IRC:
- File transfer
- A bot

Bonus is only evaluated if the mandatory part is fully correct.

---

## Submission and peer-evaluation
Submit to your Git repository. Only repository content is evaluated. Double-check file names.

You are encouraged to create local tests (not submitted) to exercise the server during defense or peer evaluation. A brief modification may be requested during evaluation to verify understanding.

---

## Checksums
16D85ACC441674FBA2DF65190663F432222F81AA0248081A7C1C1823F7A96F0B74495  
15056E97427E5B22F07132659EC8D88B574BD62C94BB654D5835AAD889B014E078705  
709F6E02
