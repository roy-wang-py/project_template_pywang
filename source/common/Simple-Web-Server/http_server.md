structure
---
```c++
template <class socket_type>
class ServerBase{
    class Session;

    class Response;  // response for client request, send
    // use Session->connection->set_timeout    asio::async_write(*session->connection->socket, streambuf

    class Content; //view of istream

    class Request; // request of client


    class Connection ; 
    /*
        socket
        timer ; construct every time
    */
   
   class Session ; 

   class Config; 







}
```