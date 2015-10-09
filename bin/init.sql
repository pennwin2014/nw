drop database  if exists nw;
drop database  if exists nw;
create database nw;
grant all privileges on nw.*  to nc@localhost identified by 'nc';
grant file  on  *.* to nc@localhost identified by 'nc';
FLUSH  PRIVILEGES;
