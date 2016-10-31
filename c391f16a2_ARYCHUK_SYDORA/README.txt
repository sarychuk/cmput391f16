for q7 first compile and run q7_first to create poi_index_temp. (Only need to run this
when new objects have been added.):

gcc -g q7_first.c sqlite3.c -lpthread -ldl -DSQLITE_ENABLE_RTREE=1
./a.out streets.db
