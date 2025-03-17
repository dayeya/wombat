# Wombat Programming Language
HUGE DISCLAIMER - THE LANGUAGE IS STILL IN DEVELOPMENT STAGE

# Why?
Why not?, It is a fun project to work with a complex learning curve making the time I've invested in it worth it. <br>
For a long time I wanted something new to work with, something *FUN*, so why not create my own?

# Development Goals
- [X] Compiled
- [X] Statically typed
- [ ] Rich and easy-to-understand syntax 
- [ ] Fast to ship, fun to develop.
- [ ] Cross-platform. DISCLAIMER - ATM THE COMPILER SUPPORTS ONLY UNIX-OS. 
- [ ] **Readable** and professional, by encforcing developers who are usually dismissive of clean code to start sticking to it.
- [ ] Turing-complete

# Language vision
Here, you can find wombats visions.

Hello, World! - using wombat
```
import std

fn main() -> free
    write! "Hello, World!";
end
```

Socket boilerplate - using wombat
```
import std
from std.err import MaybeErr
from std.net import { Socket, SockError }

fn connectTo(ip: String, port: int) -> MaybeErr<SockError>
    mark! "NOT IMPLEMENTED";
end

fn startClient(sock: ref<Socket>) -> free
    mark! "NOT IMPLEMENTED!;
end

fn main() -> free
    match connectTo("127.0.0.1", 50000) with
    | Some(_) -> startClient(mem! sock);
    | SockError.NotAlive -> terminate! "ERROR: 127.0.0.1:50000 is not alive";
    end
end
```

# Ideas and contribution
Wombat is in its baby steps, so be gentle. <br>
Any criticism will be welcomed and any request to join the project will be received with love.