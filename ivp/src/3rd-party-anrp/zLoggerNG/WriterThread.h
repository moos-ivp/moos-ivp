#ifndef __WriterThread_h__
#define __WriterThread_h__

#include <pthread.h>
#include <string>
#include <zlib.h>
#include <queue>

using namespace std;

#define REQUEST_NEW_FILE 1
#define REQUEST_APPEND_WRITE 2
#define REQUEST_APPEND_WRITE_BIN 3
#define REQUEST_QUIT 4
#define REQUEST_FLUSH 5

typedef struct {
	int req_type;
	string sval;
	int len;
	unsigned char *p;
} req_t;

class WriterThread {
public:
	WriterThread();
	~WriterThread();

	void OpenNewFile(string filename);
	void AppendWrite(string val);
	void AppendWrite(char *, int);

	void Flush(void);

private:
	pthread_t wrth;
	pthread_cond_t cond;
	gzFile cur_file;

	queue<req_t> input_requests;
	pthread_mutex_t input_requests_m;

	static void *tramp(void *p) { ((WriterThread *)p)->Thread(); return NULL; }
	void Thread();

	void AppendWriteQueue(req_t);
};

#endif
