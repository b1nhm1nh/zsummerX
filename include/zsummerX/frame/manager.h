/*
 * zsummerX License
 * -----------
 *
 * zsummerX is licensed under the terms of the MIT license reproduced below.
 * This means that zsummerX is free software and can be used for both academic
 * and commercial purposes at absolutely no cost.
 *
 *
 * ===============================================================================
 *
 * Copyright (C) 2010-2015 YaweiZhang <yawei.zhang@foxmail.com>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ===============================================================================
 *
 * (end of COPYRIGHT)
 */


#ifndef ZSUMMER_TCPSESSION_MANAGER_H_
#define ZSUMMER_TCPSESSION_MANAGER_H_

#include "config.h"
#include "session.h"
namespace zsummer
{
    namespace network
    {


        inline SessionBlock * DefaultCreateBlock();
        inline void DefaultFreeBlock(SessionBlock *sb);

        inline OnBlockCheckResult DefaulBlockCheck(const char * begin, unsigned int len, unsigned int bound, unsigned int blockLimit)
        {
            auto ret = zsummer::proto4z::checkBuffIntegrity(begin, len, bound, blockLimit);
            return std::make_pair((BLOCK_CHECK_TYPE)ret.first, (unsigned int)ret.second);
        }
        inline void DefaultBlockDispatch(TcpSessionPtr session, const char * begin, unsigned int len)
        {
            LCW("DefaultBlockDispatch empty. ");
        }

        inline OnBlockCheckResult DefaultHTTPBlockCheck(const char * begin, unsigned int len, unsigned int bound,
                bool & isChunked, std::string& method, std::string & line, std::map<std::string,std::string> & head, std::string & body)
        {
            auto ret = zsummer::proto4z::checkHTTPBuffIntegrity(begin, len, bound, isChunked, method, line, head, body);
            return std::make_pair((BLOCK_CHECK_TYPE)ret.first, ret.second);
        }

        inline void DefaultHTTPBlockDispatch(TcpSessionPtr session, const std::string & mthoed, const std::string &methodLine, 
            const std::map<std::string, std::string> &head, const std::string & body)
        {
            LCW("DefaultHTTPBlockDispatch empty.");
        }


        /*
			SessionManager is a singleton singleton, which is an advanced encapsulation of the underlying interface of zsummerX. 
			If you need to encapsulate yourself, you can refer to the frame method or the example in the example to encapsulate or use it.
            This singleton provides high-level operable interfaces for all networks, such as starting the network module singleton, 
			opening the network loop, and sequentially closing some network functions, and finally exiting the network loop, 
			adding multiple listening interfaces, adding multiple outgoing connections, and sending data
             	A cross-thread notification mechanism post, create a cancellation timer, get connection information, set timing detection, 
			set the number and interval of disconnected replays, set whether to support flash policy, set the maximum number of connections 
			that can be connected, set the protocol to be binary TCP Protocol (proto4z protocol stream),
                The setting protocol is HTTP (can be used as a web server and client, and it is convenient to do some SDK authentication and platform access).
        */
        class SessionManager
        {
        private:
            SessionManager();

        public:// !get the single and global object pointer   
            static SessionManager & getRef();
            inline static SessionManager * getPtr(){ return &getRef(); };
        public:
            // To use SessionManager, you must first call start to start the service.
            bool start();

            // Exit the message loop.
            void stop();
            

            // Block the current thread and start the message loop. It is better to use this by default. 
			// It is recommended to use runOnce when you want to control the run with more fineness.
            bool run();
            inline bool isRunning(){ return _running; }

            // Perform message processing once, if isImmediately is true, you need to return immediately regardless of whether there is data in the current processing,
			// can be embedded in any thread for flexible use
            // The default is false, if there is no network message and event message, it will block for a short period of time, and a message notification will be immediately awakened.
            bool runOnce(bool isImmediately = false);

            // handle: std::function<void()>
            // switch initiative, in the multi-thread it's switch call thread simultaneously.
            // Post a handler to the SessionManager thread for processing, thread safety.
            template<class H>
            void post(H &&h){ _summer->post(std::move(h)); }
			 
            //it's blocking call. support ipv6 & ipv4 .
            inline std::string getHostByName(const std::string & name) { return zsummer::network::getHostByName(name); }
            // The timer is created in milliseconds, not thread-safe.
            template <class H>
            zsummer::network::TimerID createTimer(unsigned int delayms, H &&h, bool useSystemTime = true)
            { return _summer->createTimer(delayms, std::move(h), useSystemTime); }
            template <class H>
            zsummer::network::TimerID createTimer(unsigned int delayms, const H &h, bool useSystemTime = true)
            {
                return _summer->createTimer(delayms, h, useSystemTime);
            }
            // Cancel the timer. Note that if you cancel the current timer in the callback handler of the timer, it will fail.
            bool cancelTimer(unsigned long long timerID){ return _summer->cancelTimer(timerID); }



            AccepterID addAccepter(const std::string& listenIP, unsigned short listenPort);
            AccepterOptions & getAccepterOptions(AccepterID aID);
            bool openAccepter(AccepterID aID);
            AccepterID getAccepterID(SessionID sID);


            SessionID addConnecter(const std::string& remoteHost, unsigned short remotePort);
            SessionOptions & getConnecterOptions(SessionID cID);
            bool openConnecter(SessionID cID);
            TcpSessionPtr getTcpSession(SessionID sID);

            std::string getRemoteIP(SessionID sID);
            unsigned short getRemotePort(SessionID sID);

            //send data.
            void sendSessionData(SessionID sID, const char * orgData, unsigned int orgDataLen);

            void fakeSessionData(SessionID sID, const char * orgData, unsigned int orgDataLen);

            //close session socket.
            void kickSession(SessionID sID);
            void kickClientSession(AccepterID aID = InvalidAccepterID);
            void kickConnect(SessionID cID = InvalidSessionID);
            void stopAccept(AccepterID aID = InvalidAccepterID);
        public:
            //statistical information
            inline unsigned long long getStatInfo(int stat){ return _statInfo[stat]; }
            alignas(unsigned long long) unsigned long long _statInfo[STAT_SIZE];

        public:
            SessionBlock * CreateBlock();
            void FreeBlock(SessionBlock * sb);
        private:
            std::deque<SessionBlock*> _freeBlock;
        private:
            friend class TcpSession;
            // An established session has been closed.
            void removeSession(TcpSessionPtr session);

            // Accept to the new connection.
            void onAcceptNewClient(zsummer::network::NetErrorCode ec, const TcpSocketPtr & s, const TcpAcceptPtr & accepter, AccepterID aID);
        private:

            // Message loop
            EventLoopPtr _summer;

            //! The following set of parameters are used to control the opening and closing of the message loop
            bool  _running = true;  // The default is on, otherwise it will exit the message loop when appropriate.

            //! The following set of IDs is used to generate the corresponding unique ID.
            AccepterID _lastAcceptID = InvalidAccepterID; //accept ID sequence. range  (0 - power(2,32))
            SessionID _lastSessionID = InvalidSessionID;//session ID sequence. range  (0 - __MIDDLE_SEGMENT_VALUE)
            SessionID _lastConnectID = InvalidSessionID;//connect ID sequence. range  (__MIDDLE_SEGMENT_VALUE - power(2,32))

            //! Store current session information and accept listener information.
            std::unordered_map<SessionID, TcpSessionPtr> _mapTcpSessionPtr;
            std::unordered_map<AccepterID, AccepterOptions > _mapAccepterOptions;
        };

        inline SessionBlock * DefaultCreateBlock()
        {
            return SessionManager::getRef().CreateBlock();
        }

        inline void DefaultFreeBlock(SessionBlock *sb)
        {
            SessionManager::getRef().FreeBlock(sb);
        }

    }
}


#endif
