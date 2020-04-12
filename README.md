Welcome to the zsummerX wiki!  
  
### Introduction:  
[![Build Status](https://travis-ci.org/zsummer/zsummerX.svg?branch=master)](https://travis-ci.org/zsummer/zsummerX)  
zsummerX is an net library with cross-platform, high-performance and lightweight, it's depend by C++11 implementation.    
the underlying interface meet the standards proactor, and the implementation used by EPOLL LT / IOCP / SELECT, it's can work in windows / linux / mac  and support TCP, UDP, HTTP.  
the frame interface is advanced and esay, you can use the frame interface to quick development, it's provide OPTIONS module, don't need to worry problem with flexibility.  
    
  
** zsummerX is a high-performance lightweight network library that spans win / mac / linux / ios / android platforms and is implemented using the C ++ 11 standard. **
  
  
### Interface implementation   
#### Highly flexible bottom interface   
1.      Comply with the proactor network model, very close to the interface form of boost asio.
2.      Support TCP, UDP, support multi-threading.
3.      Support the post interface, you can transfer the data and methods of any external thread to the zsummer loop through this interface for execution.
4.      Support timer.
5.      The user needs to maintain the life cycle management of the cache, and the user needs to have an understanding of the underlying.
   
#### A network framework that is easier to use but still flexible enough 
1. The Session abstraction is again encapsulated on top of the underlying Socket abstraction, and is unifiedly managed by the SessionMansger singleton.
2. Encapsulate all implementations except for socket establishment, disconnection, and message processing, and then use the OPTION interface to provide simple and clean custom cuts for the needs of certain flexibility.
3. Due to the influence of single cases, the multi-threaded solution can only be processed after io processing, and socket io can only be single-threaded. (The game industry and most industries use the environment enough, and if it is not enough, it can be multi-process solution) 
  
   
#### The network framework provides the following feature support   
1.      Support [proto4z] (https://github.com/zsummer/proto4z) binary protocol (support C ++, lua, C #, SQL).
2.      Support http protocol, can build a web server or web client with a few lines of code.
3.      The above two protocols are the default customization solutions in Option. By customizing the default interface of option, any binary protocol can be supported.
4.      Support socket reconnection, disconnection reconnection parameters can be customized through Option.
5.      Support socket attribute modification, such as NoDelay and other attributes.
6.      The session provides timer messages. After registering the message, each session will receive a message after a certain interval, which is convenient for the implementation of heartbeat detection, for example.
7.      Support UserParam, by allowing the session to carry custom data, so that you can achieve O (1) level performance when acquiring some attributes, and avoid additional indexing of maps in the business layer. For example, the record of the socket establishment time for Session , The record detected by the last heartbeat, whether the session was authenticated for login, and related records, this feature can simplify the implementation of upper-layer logic and improve performance, but the implementation is only the cost of a few lines of code.  
8.      Support RC4 stream encryption, whether RC4 encryption is enabled or not, keys, etc. can be customized through options, which can effectively prevent replay attacks and bypass monitoring.
9.      tcp supports flash policy authentication.
10.      Support to limit the maximum number of accept connections. If the limit is exceeded, the socket of the new accept will be kicked immediately.
11.      Perfect exception protection mechanism, execute the message processing callback method provided by the user in try catch, which can catch all C ++ exceptions and limit exception errors to a single message processing flow.
12.      Provide a stack recording interface at runtime, which can print the stack information to the log when an exception is thrown.
13.      Proto4z exceptions will carry stack information, which is convenient for judging errors.
14.      Powerful [log4z log system] (https://github.com/zsummer/log4z), zsummerX's network library log uses a logger alone, and can independently control the logger's log level, display or other related controls.
15.      Provide a package optimization solution that can read and write network data at the maximum possible time. The performance improvement of this mechanism is very obvious in flood tests and actual projects.
16.      Maximum send queue control, can prevent memory out of control caused by network abnormalities or illegal attacks.
17.      You can manually control the creation and destruction of zsummerX's network buffer to improve the performance of memory usage.
18.      Complete statistical information, including low-level io times, throughput, packet sending and receiving times, send queue length, limit free memory number and usage statistics.
19.      Provide a friendly and concise network shutdown solution.
20.      One-click compilation, does not depend on any other dependent libraries that need to be pre-compiled. Down code can be compiled directly on any platform to compile static libraries and test files.
21.      LoopEvent supports immediate return mode and no event waiting mode, the former can be parasitic in U3D-like update, which is convenient for embedding in the client.
22.      Simple and pragmatic implementation scheme, the code size is less than 6000 lines.
23.      The effective use of C ++ 11 shared_ptr and functional, the ease of use of the interface and the stability of the upper layer code are close to the level of the scripting language.
24.      Provide [lua glue code summer.h summer.cpp] (https://github.com/zsummer/zsummerX/tree/master/example/luaTest), you can use zsummerX directly in lua.  
  
#### Network framework options code preview   
```C++

        enum ProtoType
        {
            PT_TCP,
            PT_HTTP,
        };
        
        struct SessionOptions 
        {

            ProtoType       _protoType = PT_TCP;
            std::string     _rc4TcpEncryption = ""; //empty is not encryption 
            bool            _openFlashPolicy = false; //check falsh client  
            bool            _setNoDelay = true; 
            bool            _joinSmallBlock = true; //merge small block  
            unsigned int    _sessionPulseInterval = 30000;  
            unsigned int    _connectPulseInterval = 5000;  
            unsigned int    _reconnects = 0; // can reconnect count 
            bool            _reconnectClean = true;//clean unsend block . 
            unsigned int    _maxSendListCount = 600;
            OnBlockCheck _onBlockCheck;
            OnBlockDispatch _onBlockDispatch;
            OnHTTPBlockCheck _onHTTPBlockCheck;
            OnHTTPBlockDispatch _onHTTPBlockDispatch;
            OnSessionEvent _onSessionClosed;
            OnSessionEvent _onSessionLinked;
            OnSessionEvent _onSessionPulse;

            CreateBlock _createBlock ;
            FreeBlock _freeBlock;
        };
        
        struct AccepterOptions
        {
            AccepterID _aID = InvalidAccepterID;
            TcpAcceptPtr _accepter;
            std::string        _listenIP;
            unsigned short    _listenPort = 0;
            unsigned int    _maxSessions = 5000;
            std::vector<std::string> _whitelistIP;
            unsigned long long _totalAcceptCount = 0;
            unsigned long long _currentLinked = 0;
            bool _closed = false;
            SessionOptions _sessionOptions;
        };
```   
### Compile conditions   
on windows need VS2013 or latter version  
on linux need g++ 4.7 or latter version  
  
### How to compile on linux, mac    
cd zsummerX/  
cmake .  
make  

### How to compile on windows   
open zsummerX.sln and compile  

### Runing frameStressTest  
**server**  
cd zsummerX/example/bin/  
./frameStressTest_d 0.0.0.0 81  

**client**  
cd zsummerX/example/bin/   
./frameStressTest_d 127.0.0.1 81 1  

  
### About The Author  
**Author**: YaweiZhang  
**mail**: yawei.zhang@foxmail.com  
**github**: https://github.com/zsummer/zsummerX  
