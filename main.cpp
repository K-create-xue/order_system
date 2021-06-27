  1 #include"db.hpp"
  2 
  3 void dish_test(){
  4     order_sys::TableDish dish;
  5 
  6     /*
  7     Json::Value val;
  8     val["id"]=3;
  9     val["name"]="麻婆豆腐2";
 10     val["price"]=1900;
 11 
 12     //dish.Insert(val);
 13     //dish.Update(val);
 14     dish.Delete(3);
 15 */
 16     Json::Value val;
 17     Json::StyledWriter writer;
 18     dish.SelectOne(1,&val);
 19     std::cout<<writer.write(val)<<std::endl;
 20 
 21 }
 22 void order_test(){

 23     order_sys::TableOrder order;
 24     //Json::Value val;
 25     //val["id"]=3;
 26     //val["dishes"].append(1);
 27     //val["dishes"].append(1);
 28     //val["status"]=1;
 29     //order.Insert(val);
 30     //order.Update(val);
 31     //order.Delete(3);
 32     Json::Value val;
 33     Json::StyledWriter writer;
 34     order.SelectOne(2,&val);
 35     std::cout<<writer.write(val)<<std::endl;
 36 }
 37 
 38 int main(){
 39     //dish_test();
 40     order_test();
 41     return 0;
 42 }
 
 
 
 
 
 Makefile
  1 LFLAG=-L/usr/lib64/mysql -lmysqlclient -ljsoncpp
  2 main:main.cpp db.hpp
  3     g++ -std=c++11 $^ -o $@ $(LFLAG)
  4 db:db.hpp
  5     g++ -std=c++11 $^ -o $@ -lmysql

