gcc server/server.c -o run/server -lmysqlclient;
gcc -rdynamic client/client.c -o run/client `pkg-config --cflags --libs gtk+-3.0`;