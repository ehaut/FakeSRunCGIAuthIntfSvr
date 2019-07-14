#!/bin/bash

# for CentOs
# sudo yum -y install fcgi fcgi-dev spawn-fcgi 


# for Ubuntu
# sudo apt-get install spawn-fcgi libfcgi-dev

g++ /www/wwwroot/cgi.ehaut.cn/src/test.cpp -lfcgi++ -lfcgi -o /www/wwwroot/cgi.ehaut.cn/bin/test
