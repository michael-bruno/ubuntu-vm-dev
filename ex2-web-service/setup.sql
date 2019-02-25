create database registrations;
use registrations;

create table Contestant
(
    id int not null auto_increment,
    firstName varchar(20) not null,
    lastName varchar(20) not null,
    grade int not null,
    email varchar(50) not null,
    shirtSize char(1) not null,
    hrUsername varchar(50) not null,
    primary key (id)
);