http_srv.cpp
  1 #include "httplib.h"
  2 
  3 void hello(const httplib::Request &req,httplib::Response &rsp){
  4     std::cout<<req.method<<std::endl;
  5     std::cout<<req.path<<std::endl;
  6     std::cout<<req.body<<std::endl;
  7     auto it=req.headers.begin();
  8     for(;it!=req.headers.end();it++){
  9         std::cout<<it->first<<"="<<it->second<<std::endl;
 10 
 11     }
 12     it=req.params.begin();
 13     for(;it!=req.params.end();it++){
 14         std::cout<<it->first<<"="<<it->second<<std::endl;
 15 
 16     }
 17     rsp.status=200;
 18     rsp.body="<html><body><h1>Hello K</h1></body></html>";
 19     rsp.set_header("Content-Type","text/html");
 20 
 21 }
 22 
 23 int main(){
 24 httplib::Server srv;
 25 
 26 srv.Get("/hello",hello);
 27 srv.listen("0.0.0.0",8080);
 28 return 0;
 29 }

