# Host: localhost  (Version 5.7.3-m13-log)
# Date: 2024-08-08 14:25:45
# Generator: MySQL-Front 6.1  (Build 1.26)


#
# Structure for table "user_info"
#

DROP TABLE IF EXISTS `user_info`;
CREATE TABLE `user_info` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(32) NOT NULL,
  `pwd` varchar(32) NOT NULL,
  `online` tinyint(1) DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=utf8;

#
# Data for table "user_info"
#

INSERT INTO `user_info` VALUES (10,'张三','123',0),(11,'李四','123',0),(12,'111','111',0);

#
# Structure for table "friend"
#

DROP TABLE IF EXISTS `friend`;
CREATE TABLE `friend` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `user_id` int(11) NOT NULL,
  `friend_id` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`),
  KEY `friend_id` (`friend_id`),
  CONSTRAINT `friend_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `user_info` (`id`),
  CONSTRAINT `friend_ibfk_2` FOREIGN KEY (`friend_id`) REFERENCES `user_info` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;

#
# Data for table "friend"
#

INSERT INTO `friend` VALUES (7,10,11);

#
# Function "cal_age"
#

DROP FUNCTION IF EXISTS `cal_age`;
CREATE FUNCTION `cal_age`(birthdate date) RETURNS int(11)
begin
declare age int;
set age = year(curdate()) - year(birthdate);
if date_format(curdate(),'%m%d') < date_format(birthdate,'%m%d') then
set age = age - 1;
end if;
return age;
end;

#
# Function "find"
#

DROP FUNCTION IF EXISTS `find`;
CREATE FUNCTION `find`(ui_name varchar(100),ui_password varchar(100)) RETURNS varchar(100) CHARSET utf8
begin

declare res int default 0;
select count(*) into res from user_info
where name = ui_name;
if res = 0 then return "用户不存在";
end if;

select count(*) into res from user_info
where name = ui_name and password = ui_password;
if res = 0 then return "密码错误";
end if;

  return "登录成功";
end;

#
# Function "maxSal"
#

DROP FUNCTION IF EXISTS `maxSal`;
CREATE FUNCTION `maxSal`(emp_id int) RETURNS int(11)
begin
  declare max_sal int;
  select max(sal) into max_sal from emp
  where deptno = emp_id;
  return max_sal;
end;

#
# Procedure "isid"
#

DROP PROCEDURE IF EXISTS `isid`;
CREATE PROCEDURE `isid`(in dept_id int)
begin
    #声明变量
    declare empno_val int;
    declare sal_val int; 
    declare done int default 0;
    #声明游标
    declare mycursor cursor 
    for
    select empno,sal from emp where deptno = dept_id;
    #声明句柄
    declare continue handler for not found set done = 1;
    #打开游标
    open mycursor;
    #循环读取游标数据
    while done=0
      do
      fetch mycursor into empno_val,sal_val;
      if done=0 then 
         update emp set sal = sal_val + 100 where  empno= empno_val;
      end if;
    end while;
    #关闭游标
    close mycursor;
end;

#
# Procedure "sumn"
#

DROP PROCEDURE IF EXISTS `sumn`;
CREATE PROCEDURE `sumn`(in n int)
begin
declare res int default 0;
while n do
set res = res + n;
set n = n - 1;
end while;
select res;
end;
