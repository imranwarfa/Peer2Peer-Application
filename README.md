# Peer2Peer-Application

# Overview

This project is a Peer-to-Peer (P2P) application designed for efficient content sharing among users in a Linux OS environment. It implements a hybrid architecture with a centralized Index Server and multiple Peers, enabling dynamic resource sharing. The application utilizes socket programming in C for robust communication, demonstrating key concepts of distributed systems and network programming.

# Features

  **1. Index Server**
  
    - Acts as a centralized directory for all registered files in the network.
    - Manages content registration and de-registration from peers.
    - Provides a menu-driven interface for users to browse available content.
    - Communicates with peers using UDP connections for streamlined and lightweight request handling.
    
  **2. Peer Communication**
  
    - Each peer can function as both a client (to request files) and a server (to share its files).
    - TCP connections ensure reliable data transfer during content exchanges.
    - Once a peer registers with the index server, it acts as a server for its content, enabling other peers to download files. 
    
    Example workflow:
    PeerA registers a file with the index server.
    PeerB queries the index server, retrieves PeerA's details, and establishes a direct connection to download the file.
    
# Architecture

Centralized Directory: The index server provides a single source of truth for all available files, enhancing search efficiency.

Dynamic Peer Interaction: Peers dynamically join and leave the network without disrupting overall functionality.

# Hybrid Communication Protocols:

UDP: Used for lightweight, stateless communication between the index server and peers.

TCP: Ensures reliable file transfers between peers.

# Key Advantages

Scalability: Easily supports the addition of new peers without modifying the core architecture.

Dynamic Resource Sharing: Peers can register or deregister files at runtime.

Efficient Content Retrieval: Centralized indexing ensures fast and accurate file discovery.

# Technical Details

- Programming Language: C
- Platform: Linux
- Networking:
  - Socket Programming for both TCP and UDP protocols.
  - Robust handling of connections and file transfers.
- Concurrency: Each peer manages multiple connections simultaneously, enabling parallel uploads and downloads.
  
# Usage

1. Run the Index Server: Start the server to enable peers to register and query for files.
2. Start Peer Applications:
   
   - Register files with the index server.
   - Query the server for available files.
   - Establish direct connections with other peers to exchange content.

3. File Exchange: Use the peer-to-peer communication module to download desired content.

# Project Outcomes

Demonstrated a functional P2P file-sharing architecture.

Showcased effective socket programming techniques for both TCP and UDP protocols.

Proved scalability by successfully handling multiple peers and concurrent connections.

# Future Enhancements

Implementing encryption for secure file transfers.

Adding a GUI interface for easier interaction.

Incorporating peer discovery protocols for fully decentralized architecture.
