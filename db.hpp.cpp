  1 #include<iostream> 
  2 #include<string> 
  3 #include<mysql/mysql.h> 
  4 #include<jsoncpp/json/json.h> 
  5 #include<mutex> 
  6  
  7 namespace order_sys{ 
  8 #define MYSQL_SERVER "127.0.0.1" 
  9 #define MYSQL_USER "root" 
 10 #define MYSQL_PASSWD "1" 
 11 #define MYSQL_DBNAME "order_sys" 
 12     static MYSQL* MysqlInit(){ 
 13         //初始化句柄 
 14         //mysql_init(句柄地址) 
 15         MYSQL *mysql=NULL; 
 16         mysql=mysql_init(NULL); 
 17             if(mysql==NULL){ 
 18                 std::cout<<"mysql init failed\n"; 
 19                 return NULL; 
 20             } 
 21          
 22         //连接服务器 
 23         //mysql_real_connect(句柄，服务器IP，用户名，密码,数据库名称，端口，套接字文件，客户端标志) 
 24         if(mysql_real_connect(mysql,MYSQL_SERVER,MYSQL_USER,MYSQL_PASSWD,MYSQL_DBNAME, 
 25             3306,NULL,0)==NULL){ 
 26                 std::cout<<mysql_error(mysql)<<std::endl; 
 27                 return NULL; 
 28             } 
 29         //设置字符集 
 30         //mysql_set_character_set(句柄，字符集名称) 
 31         if(mysql_set_character_set(mysql,"utf8")!=0){ 
 32             std::cout<<mysql_error(mysql)<<std::endl; 
 33             return NULL; 
 34         } 
 35         //选择数据库 
 36         //mysql_select_db(句柄，数据库名称) 
 37         if(mysql_select_db(mysql,MYSQL_DBNAME)!=0){ 
 38             std::cout<<mysql_error(mysql)<<std::endl; 
 39             return NULL; 
 40         } 
 41         return mysql; 
 42     }  
 43  
 44     static void MysqlRelease(MYSQL *mysql){ 
 45         if(mysql!=NULL){ 
 46             mysql_close(mysql); 
 47         } 
 48         return ; 
 49     }    
 50     static bool MysqlQuery(MYSQL *mysql,const std::string &sql){ 
 51         //mysql_query(句柄，sql语句) 
 52         if(mysql_query(mysql,sql.c_str())!=0){ 
 53             std::cout<<sql<<std::endl; 
 54             std::cout<<mysql_error(mysql)<<std::endl; 
 55             return false; 
 56         } 
 57         return true; 
 58     } 
 59  
 60     class TableDish{ 
 61         private: 
 62             MYSQL *_mysql; 
 63             std::mutex _mutex; 
 64         public: 
 65             TableDish(){/*mysql 初始化*/ 
 66                 _mysql=MysqlInit(); 
 67                 if(_mysql==NULL){ 
 68                     exit(-1); 
 69                 } 
 70             } 
 71             ~TableDish(){ 
 72                 MysqlRelease(_mysql); 
 73                 _mysql=NULL; 
 74             } 
 75             bool Insert(const Json::Value &dish){ 
 76                 //组织sql语句 
 77 #define DISH_INSERT "insert tb_dish values(null,'%s',%d,now());" 
 78                 char str_sql[4096]={0}; 
 79                 sprintf(str_sql,DISH_INSERT, 
 80                     dish["name"].asCString(), 
 81                     dish["price"].asInt()); 
 82                 //insert tbname value() 
 83                 //执行sql语句 
 84                 return MysqlQuery(_mysql,str_sql); 
 85             } 
 86             bool Delete(int dish_id){ 
 87 #define DISH_DELETE "delete from tb_dish where id=%d;" 
 88             char str_sql[4096]={0}; 
 89             sprintf(str_sql,DISH_DELETE,dish_id); 
 90             return MysqlQuery(_mysql,str_sql); 
 91             } 
 92             bool Update(const Json::Value &dish){ 
 93 #define DISH_UPDATE "update tb_dish set name='%s',price=%d where id=%d;" 
 94             char str_sql[4096]={0}; 
 95             sprintf(str_sql,DISH_UPDATE, 
 96             dish["name"].asCString(), 
 97             dish["price"].asInt(), 
 98             dish["id"].asInt()); 
 99             return MysqlQuery(_mysql,str_sql); 
100             } 
101             bool SelectAll(Json::Value *dishes){ 
102 #define DISH_SELECT "select * from tb_dish;" 
103             _mutex.lock(); 
104             bool ret=MysqlQuery(_mysql,DISH_SELECT); 
105             if(ret==false){ 
106                 _mutex.unlock(); 
107                 return false; 
108             } 
109             MYSQL_RES *res=mysql_store_result(_mysql); 
110             _mutex.unlock(); 
111             if(res==NULL){ 
112                 std::cout<<"store result failed!\n"; 
113                 return false; 
114             } 
115             int num=mysql_num_rows(res); 
116             for(int i=0;i<num;i++){ 
117                 MYSQL_ROW row=mysql_fetch_row(res); 
118                 Json::Value dish; 
119                 dish["id"]=std::stoi(row[0]); 
120                 dish["name"]=row[1]; 
121                 dish["price"]=std::stoi(row[2]); 
122                 dish["ctime"]=row[3]; 
123                 dishes->append(dish); 
124             } 
125             mysql_free_result(res); 
126             return true; 
127             } 
128             bool SelectOne(int dish_id,Json::Value *dish){ 
129 #define DISH_SELECTONE "select * from tb_dish where id=%d;" 
130             char str_sql[4096]={0}; 
131             sprintf(str_sql,DISH_SELECTONE,dish_id); 
132             _mutex.lock(); 
133             bool ret=MysqlQuery(_mysql,str_sql); 
134             if(ret==false){ 
135                 _mutex.unlock(); 
136                 return false; 
137             } 
138             MYSQL_RES *res=mysql_store_result(_mysql); 
139             _mutex.unlock(); 
140             if(res==NULL){ 
141                 std::cout<<"store result failed!\n"; 
142                 return false; 
143             } 
144             int num=mysql_num_rows(res); 
145             if(num!=1){ 
146                 std::cout<<"result error\n"; 
147                 mysql_free_result(res); 
148                 return false; 
149             } 
150             MYSQL_ROW row=mysql_fetch_row(res); 
151             (*dish)["id"]=dish_id; 
152             (*dish)["name"]=row[1]; 
153             (*dish)["price"]=std::stoi(row[2]); 
154             (*dish)["ctime"]=row[3]; 
155             mysql_free_result(res); 
156             return true; 
157             } 
158     }; 
159     class TableOrder{ 
160         private: 
161             MYSQL *_mysql; 
162             std::mutex _mutex; 
163         public: 
164             TableOrder(){/*mysql 初始化*/ 
165                 _mysql=MysqlInit(); 
166                 if(_mysql==NULL){ 
167                     exit(-1); 
168                 } 
169             } 
170             ~TableOrder(){ 
171                 if(_mysql!=NULL){ 
172                     MysqlRelease(_mysql); 
173                 } 
174             } 
175             bool Insert(const Json::Value &order){ 
176 #define ORDER_INSERT "insert tb_order values(null,'%s',0,now());" 
177             char str_sql[4096]={0}; 
178             Json::FastWriter writer; 
179             std::string dishes=writer.write(order["dishes"]); 
180             sprintf(str_sql,ORDER_INSERT,dishes.c_str()); 
181             return MysqlQuery(_mysql,str_sql); 
182             } 
183             bool Delete(int order_id){ 
184 #define ORDER_DELETE "delete from tb_order where id=%d;" 
185                 char str_sql[4096]={0}; 
186                 sprintf(str_sql,ORDER_DELETE,order_id); 
187                 return MysqlQuery(_mysql,str_sql); 
188             } 
189             bool Update(const Json::Value &order){ 
190 #define ORDER_UPDATE "update tb_order set dishes='%s',status=%d where id=%d;" 
191                 Json::FastWriter writer; 
192                 int order_id=order["id"].asInt(); 
193                 int status=order["status"].asInt(); 
194                 std::string dishes=writer.write(order["dishes"]); 
195                 char str_sql[4096]={0}; 
196                 sprintf(str_sql,ORDER_UPDATE,dishes.c_str(),status,order_id); 
197                 return MysqlQuery(_mysql,str_sql); 
198             } 
199             bool SelectAll(Json::Value *orders){ 
200 #define ORDER_SELECTALL "select * from tb_order;" 
201                 _mutex.lock(); 
202                 bool ret=MysqlQuery(_mysql,ORDER_SELECTALL); 
203                 if(ret==false){ 
204                     _mutex.unlock(); 
205                     return false; 
206                 } 
207                 MYSQL_RES *res=mysql_store_result(_mysql); 
208                 _mutex.unlock(); 
209                 if(res==NULL){ 
210                     std::cout<<mysql_error(_mysql)<<std::endl; 
211                     return false; 
212                 } 
213                 int num=mysql_num_rows(res); 
214                 for(int i=0;i<num;i++){ 
215                     MYSQL_ROW row=mysql_fetch_row(res); 
216                     Json::Value order; 
217                     order["id"]=std::stoi(row[0]); 
218                     order["dishes"]=row[1]; 
219                     order["status"]=std::stoi(row[2]); 
220                     order["mtime"]=row[3]; 
221                     orders->append(order); 
222                 } 
223                 mysql_free_result(res); 
224                 return true; 
225             } 
226             bool SelectOne(int order_id,Json::Value *order){ 
227 #define ORDER_SELECTONE "select * from tb_order where id=%d;" 
228                 char str_sql[4096]={0}; 
229                 sprintf(str_sql,ORDER_SELECTONE,order_id); 
230                 _mutex.lock(); 
231                 bool ret=MysqlQuery(_mysql,str_sql); 
232                 if(ret==false){ 
233                     _mutex.unlock(); 
234                     return false; 
235                 } 
236                 MYSQL_RES *res=mysql_store_result(_mysql); 
237                 _mutex.unlock(); 
238                 if(res==NULL){ 
239                     std::cout<<mysql_error(_mysql)<<std::endl; 
240                     return false; 
241                 } 
242                 int rows=mysql_num_rows(res);
243                 if(rows!=1){
244                     std::cout<<"one result error\n";
245                     mysql_free_result(res);
246                     return false;
247                 }
248             MYSQL_ROW row=mysql_fetch_row(res);
249             (*order)["id"]=order_id;
250             (*order)["dishes"]=row[1];
251             (*order)["status"]=std::stoi(row[2]);
252             (*order)["mtime"]=row[3];
253 
254             mysql_free_result(res);
255             return true;
256             }
257 
258     };
259 };
260 
