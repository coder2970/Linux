#ifndef __COMMON_HPP__
#define __COMMON_HPP

#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

const std::string fifoname = "fifo";
mode_t mode = 0666;
#define SIZE 1024
#endif