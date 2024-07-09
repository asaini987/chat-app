# Multi-threaded Chat Server and Client

This project is a multi-threaded, scalable chat server and client application implemented in C. The server uses `kqueue` for efficient event notification and `pthreads` for creating a thread pool to handle multiple client connections concurrently. The client connects to the server to send and receive messages.

## Features

- **Multi-threaded Server**: Uses a thread pool to handle multiple client connections.
- **Event-driven**: Utilizes non-blocking sockets and `kqueue` for efficient event notification.
- **Thread-safe**: Ensures thread safety using mutexes and read-write locks for shared resources.
- **Message Queues**: Each client has a message queue to handle incoming messages.
- **Broadcast Messages**: Server can broadcast messages to all connected clients.

## Components

- **Server**: Handles client connections, manages message queues, and broadcasts messages.
- **Client**: Connects to the server, sends messages, and displays received messages.

## Status

The project is currently in progress. The following features are being developed:

- Modifying the server to use a hybrid of multiplexing sockets and a thread pool archtiecture
- Private messaging between clients
- Adding user authentication
- Implementing chat rooms for users to join/create
- Enhancing the client interface for better user experience
- Adding SSL encryption to chat communciation

Coming soon: a GUI for the client using the GTK library

## Getting Started

### Prerequisites

- A BSD-based operating system (e.g, macOS)
- GCC compiler
- `make` utility

### Building the Project

1. Fork the repository:
    - Go to https://github.com/asaini987/chat-app and click the "Fork" button in the top-right corner.

2. Clone Your Forked Repository:
    - After forking, clone the repository using the URL for your fork:
    ```sh
    git clone https://github.com/YOUR_GITHUB_USERNAME/chat-app.git
    cd chat-app
    ```
    Replace `YOUR_GITHUB_USERNAME` with your actual GitHub username

3. Compile the server:
    ```sh
    make server_app
    ```

4. Compile the client:
    ```sh
    make client_app
    ```

### Running the Server

Start the server on a specified port:
```sh
./server_app <port>
```

### Running the Client

Start the client to connect to a specified IP address and port:
```sh
./client_app <server_ip_address> <server_port>
```

Now you can begin talking to the other clients in the chat room!
