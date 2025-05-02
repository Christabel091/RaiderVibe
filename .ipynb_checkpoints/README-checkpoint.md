# RaiderVibe

**A campus‑community discussion platform for MTSU students**

---

## Overview

RaiderVibe is a Reddit‑style client–server application written in C that lets Middle Tennessee State University students register, post updates or reviews on professors, events, scholarships, free resources, and more. It supports threaded comments, real‑time broadcasts, keyword search, and concurrent multi‑user interaction—all via a command‑line interface (CLI).

---

## Features

- **User Authentication**: Register unique usernames; log in/out; session tracking  
- **Posting & Commenting**: Create short updates or reviews; reply in threads  
- **Real‑Time Broadcasts**: Push new posts/comments to all connected clients  
- **Keyword Search**: Retrieve posts by entering search terms  
- **Concurrency**: Handle multiple clients using POSIX threads (pthreads)  
- **Persistence**: Store users, posts, and comments in plain‑text files  
- **CLI‑Based**: Fully terminal‑driven, with future GUI planned  

---

## Prerequisites

- GCC (or compatible C compiler)  
- GNU Make  
- POSIX‑compliant OS (Linux/macOS)  
- Internet if cloning from GitHub

---

## Installation & Build

1. **Clone the repo**  
   ```bash
   git clone https://github.com/Christabel091/RaiderVibe.git
   cd RaiderVibe
   ```

2. **Build everything**  
   ```bash
   make all
   ```

3. **Clean build artifacts**  
   ```bash
   make clean
   ```

---

## Makefile Targets

Run `make help` to see a list of available commands. Common targets include:

| Target      | Description                               |
|-------------|-------------------------------------------|
| `all`       | Compile server and client executables     |
| `server`    | Build only the server                     |
| `client`    | Build only the client                     |
| `clean`     | Remove binaries and temporary files       |
| `help`      | Show all Makefile targets and descriptions |

---

## Usage

1. **Start the server**  
   ```bash
   ./server
   ```

2. **Launch one or more clients** (in separate terminals)  
   ```bash
   ./client
   ```

3. **Follow CLI prompts** to register, log in, post, comment, search, and view real‑time updates.

---

## Future Enhancements

- Migrate from CLI to a graphical UI  
- Add moderation tools and analytics dashboard  
- Integrate a lightweight database for persistence  
- Offer mobile‑friendly interface  

---

