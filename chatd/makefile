SRC = src/main.cpp
TARGET = chatd
SETLIB = settings
SHALIB = sha256d
LOGLIB = Logger
CHLIB = MsgBased
UBLIB = UsrBased
SQLIB = sql_db
NLIB = netd
BINPATH = /usr/local/bin
LIBPATH = /usr/lib/chatd
DATAPATH = /var/lib/chatd
LOGPATH = /var/log/chatd

chatd:	$(SRC) libnet libsql libch libub liblog libsha libcf
		g++ -o $(TARGET) $(SRC) -I/usr/include/mysql -L. -Wl,-rpath=$(LIBPATH) -l$(NLIB) -l$(SQLIB) -l$(CHLIB) -l$(UBLIB) -l$(LOGLIB) -l$(SHALIB) -l$(SETLIB) /usr/lib/x86_64-linux-gnu/libmysqlclient.so

libnet:	src/$(NLIB).cpp src/$(NLIB).h
		g++ -o lib$(NLIB).so src/$(NLIB).cpp -shared -fPIC

libsql:	src/$(SQLIB).cpp src/$(SQLIB).h
		g++ -o lib$(SQLIB).so src/$(SQLIB).cpp -shared -fPIC

libch:  src/$(CHLIB).cpp src/$(CHLIB).h
		g++ -o lib$(CHLIB).so src/$(CHLIB).cpp -shared -fPIC

libub:	src/$(UBLIB).cpp src/$(UBLIB).h src/TmplBased.h
		g++ -o lib$(UBLIB).so src/$(UBLIB).cpp -shared -fPIC

liblog:	src/$(LOGLIB).cpp src/$(LOGLIB).h
		g++ -o lib$(LOGLIB).so src/$(LOGLIB).cpp -shared -fPIC

libsha: src/$(SHALIB).cpp src/$(SHALIB).h
		g++ -o lib$(SHALIB).so src/$(SHALIB).cpp -shared -fPIC

libcf:	src/$(SETLIB).cpp src/$(SETLIB).h
		g++ -o lib$(SETLIB).so src/$(SETLIB).cpp -shared -fPIC

clean:
		rm -rf $(TARGET)
		rm -rf lib$(CHLIB).so
		rm -rf lib$(UBLIB).so
		rm -rf lib$(SHALIB).so
		rm -rf lib$(NLIB).so
		rm -rf lib$(SETLIB).so
		rm -rf lib$(SQLIB).so
		rm -rf lib$(LOGLIB).so

install:
		install $(TARGET) $(BINPATH)
		mkdir $(LIBPATH)
		install lib$(CHLIB).so $(LIBPATH)
		install lib$(UBLIB).so $(LIBPATH)
		install lib$(SHALIB).so $(LIBPATH)
		install lib$(NLIB).so $(LIBPATH)
		install lib$(SQLIB).so $(LIBPATH)
		install lib$(SETLIB).so $(LIBPATH)
		install lib$(LOGLIB).so $(LIBPATH)
		mkdir $(DATAPATH) && chmod 777 $(DATAPATH)
		mkdir $(LOGPATH) && chmod 777 $(LOGPATH)

uninstall:
		rm -rf $(BINPATH)/$(TARGET)
		rm -rf $(LIBPATH)/lib$(CHLIB).so
		rm -rf $(LIBPATH)/lib$(UBLIB).so
		rm -rf $(LIBPATH)/lib$(SHALIB).so
		rm -rf $(LIBPATH)/lib$(NLIB).so
		rm -rf $(LIBPATH)/lib$(SQLIB).so
		rm -rf $(LIBPATH)/lib$(SETLIB).so
		rm -rf $(LIBPATH)/lib$(LOGLIB).so
		rm -rf $(LIBPATH)
		rm -rf $(DATAPATH)
		rm -rf $(LOGPATH)
