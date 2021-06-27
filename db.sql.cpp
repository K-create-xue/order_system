  1 create database if not exists order_sys;
  2 
  3 use order_sys;
  4 
  5 create table if not exists tb_dish(
  6     id int primary key auto_increment,
  7     name varchar(32) unique not null,
  8     price int not null,
  9     ctime datetime
 10 );
 11 
 12 create table if not exists tb_order(
 13     id int primary key auto_increment,
 14     dishes varchar(255) comment '[1,2]',
 15     status int comment '0-未完成,1-完成',
 16     mtime datetime
 17 );
 18 
 19 insert tb_dish values(null,"刀削面",900,now()),
 20                     (null,"油泼面",1200,now());
 21 insert tb_order values(null,"[1,2]",0,now()),
 22                     (null,"[1]",0,now());
