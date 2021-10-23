  /*************************************************************************                                                                                                                                                                                                    
      > File Name: main.cpp
      > Author: Black_K
      > Mail: xzp01@foxmail.com 
      > Created Time: Thu 21 Oct 2021 12:38:16 PM CST
   ************************************************************************/
  //#include<regex>
  #include"db.hpp"
  #include"httplib.h"
  #include <regex>
  
  
  #define WWWROOT "./wwwroot"
  using namespace httplib;
  
  order_sys::TableDish *tb_dish=NULL;
  order_sys::TableOrder *tb_order=NULL;
  
  void DishInsert(const Request &req,Response &rsp){
      //1.业务处理
      //1.解析正文 （json反序列化），得到菜品信息
      Json::Value dish;
      Json::Reader reader;
      bool ret=reader.parse(req.body,dish);
      if(ret==false){
          rsp.status=400;
          Json::Value reason;
          Json::FastWriter writer;
          reason["result"]=false;
          reason["reason"]="dish info parse failed!";
          rsp.body=writer.write(reason);
          rsp.set_header("Content-Type","application/json");
          return ;
      }
      //2.将菜品信息插入数据库
      ret=tb_dish->Insert(dish);
      if(ret==false){
          Json::Value reason;
          Json::FastWriter writer;
          reason["result"]=false;
          reason["reason"]="mysql insert failed!";
          rsp.status=500;
          rsp.body=writer.write(reason);
          rsp.set_header("Content-Type","application/json");
          std::cout<<"mysql insert dish error!"<<std::endl;
          return ;
      }
      //3.设置响应信息
      //rsp.status=200;  
      return ;
  }
  
  void DishDelete(const Request &req,Response &rsp){
      //1.获取要删除的菜品id  /dish/id
      int dish_id=std::stoi(req.matches[1]);
      //2.数据库删除操作
      bool ret=tb_dish->Delete(dish_id);
      if(ret==false){
          std::cout<<"mysql delete dish error!"<<std::endl;
          rsp.status=500;
          return ;
      }
      return ;
  }
  
  void DishUpdate(const Request &req,Response &rsp){
      int dish_id=std::stoi(req.matches[1]);
      Json::Value dish;
      Json::Reader reader;
      bool ret=reader.parse(req.body,dish);
      if(ret==false){
          rsp.status=400;
          std::cout<<"update -parse dish info error!\n";
          return ;
      }
      dish["id"]=dish_id;
      ret=tb_dish->Update(dish);
      if(ret==false){
          std::cout<<"mysql update dish error!"<<std::endl;
          rsp.status=500;
          return ;
      }
      return ;
  }
  
W>void DishGetAll(const Request &req,Response &rsp){
      Json::Value dishes;
      bool ret=tb_dish->SelectAll(&dishes);
     if(ret==false){
          std::cout<<"mysql getALl dish error!"<<std::endl;
          rsp.status=500;
          return ;
     }
     Json::FastWriter writer;
     rsp.body=writer.write(dishes);
     return ;
  }
  
  
  void DishGetOne(const Request &req,Response &rsp){
      int dish_id=std::stoi(req.matches[1]);
      Json::Value dish;
      bool ret=tb_dish->SelectOne(dish_id,&dish);
     if(ret==false){
          std::cout<<"mysql getOne dish error!"<<std::endl;
          rsp.status=500;
          return ;
     }
     Json::FastWriter writer;
     rsp.body=writer.write(dish);
      return ;
  }
  
  
  void OrderInsert(const Request &req,Response &rsp){
      Json::Value order;
      Json::Reader reader;
      bool ret=reader.parse(req.body,order);                                                                                                                                                                                                                                    
      if(ret==false){
          std::cout<<"insert parse order info  error!"<<std::endl;
          rsp.status=400;
          return ;
      }
      ret=tb_order->Insert(order);
      if(ret==false){
          std::cout<<"insert mysql order  error!"<<std::endl;
          rsp.status=500;
          return ;
     }
      return ;
  }
  
  void OrderDelete(const Request &req,Response &rsp){
      int order_id=std::stoi(req.matches[1]);
      bool ret=tb_order->Delete(order_id);
      if(ret==false){
          std::cout<<"mysql delete order  error!"<<std::endl;
          rsp.status=500;
          return ;
      }
      return ;
  }
  
  void OrderUpdate(const Request &req,Response &rsp){
      int order_id=std::stoi(req.matches[1]);
      Json::Value order;
      Json::Reader reader;
      bool ret=reader.parse(req.body,order);
      if(ret==false){
          std::cout<<"update parse order info error!"<<std::endl;
          rsp.status=400;
          return ;
      }
      
      order["id"]=order_id;
      ret=tb_order->Update(order);
      if(ret==false){
          std::cout<<"mysql update order  error!"<<std::endl;
          rsp.status=500;
          return ;
      }
      return ;
  }
  
W>void OrderGetAll(const Request &req,Response &rsp){
      Json::Value orders;
      bool ret=tb_order->SelectAll(&orders);
      if(ret==false){
          std::cout<<"mysql getall order  error!"<<std::endl;
          rsp.status=500;
          return ;
      }
      Json::FastWriter writer;
      rsp.body=writer.write(orders);
      return ;
  }
  
  void OrderGetOne(const Request &req,Response &rsp){
      int order_id=std::stoi(req.matches[1]);
      Json::Value order;                                                                                                                                                                                                                                                        
      bool ret=tb_order->SelectOne(order_id,&order);
      if(ret==false){
          std::cout<<"mysql getone order  error!"<<std::endl;
          rsp.status=500;
          return ;
  }
      Json::FastWriter writer;
      rsp.body=writer.write(order);
      return ;
  }
  
  int main(){
      tb_dish=new order_sys::TableDish();
      tb_order=new order_sys::TableOrder();
      
      //dish_test();
      //order_test();
      Server server;
      server.set_base_dir(WWWROOT);
      //正则表达式：(\d+)标识匹配一个数字字符一次或多次
      //正则表达式：()表示捕捉匹配括号中规则的文本
      //R"()" 表示括号中的字符串去除特殊字符的特殊含义
      server.Post("/dish",DishInsert);
      server.Delete(R"(/dish/(\d+))",DishDelete);
      server.Put(R"(/dish/(\d+))",DishUpdate);
      server.Get(R"(/dish)",DishGetAll);
      server.Get(R"(/dish/(\d+))",DishGetOne);
      
      server.Post("/order",OrderInsert);
      server.Delete(R"(/order/(\d+))",OrderDelete);
      server.Put(R"(/order/(\d+))",OrderUpdate);
      server.Get(R"(/order)",OrderGetAll);
      server.Get(R"(/order/(\d+))",OrderGetOne);
      
      server.listen("0.0.0.0",9000);
      
      return 0;
  }
  
  
  void dish_test(){
      order_sys::TableDish dish;
  
      //增删改
      /*
      Json::Value val;
      val["id"]=1;
      val["name"]="麻婆豆";
      val["price"]=2300;
  
      //dish.Insert(val);
      dish.Update(val);
      dish.Delete(1);
      */
  
      //查
      Json::Value val;
      Json::StyledWriter writer;
      //dish.SelectAll(&val);
      dish.SelectOne(2,&val);
      std::cout<<writer.write(val)<<std::endl;                                                                                                                                                                                                                                                                               
      }

      void order_test(){
      order_sys::TableOrder order;
      
      //增删改
      /*
      Json::Value val;
      val["id"]=3;
      val["dishes"].append(2);
      val["dishes"].append(3);
      val["status"]=1;
      //order.Insert(val);
      //order.Update(val);
      order.Delete(3);
      */
  
      //查
      Json::Value val;
      Json::StyledWriter writer;
      //order.SelectAll(&val);
      order.SelectOne(1,&val);
      std::cout<<writer.write(val)<<std::endl;
  }                                      
     
