#include "WriterThread.h"

WriterThread::WriterThread()
{
	cur_file = NULL;
	pthread_mutex_init(&input_requests_m, NULL);
	pthread_cond_init(&cond, NULL);
	pthread_create(&wrth, NULL, tramp, this);
}

WriterThread::~WriterThread()
{
	printf("WriterThread dies!\n");
	req_t t;
	t.req_type = REQUEST_QUIT;
	AppendWriteQueue(t);
	sleep(1);
	pthread_join(wrth, NULL);
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&input_requests_m);
}

void WriterThread::OpenNewFile(string fn)
{
	req_t t;
	t.req_type = REQUEST_NEW_FILE;
	t.sval = fn;
	AppendWriteQueue(t);
}

void WriterThread::AppendWrite(string val)
{
	req_t t;
	t.req_type = REQUEST_APPEND_WRITE;
	t.sval = val;
	AppendWriteQueue(t);
}

void WriterThread::AppendWrite(char *s, int l)
{       
	req_t t;
	t.req_type = REQUEST_APPEND_WRITE_BIN;
	t.p = (unsigned char *)malloc(l);
	if(!t.p) {
		fprintf(stderr, "running out of memory? l = %i\n", l);
		return;
	}
	memcpy(t.p, s, l);
	t.len = l;
	AppendWriteQueue(t);
}

void WriterThread::Flush(void)
{
	req_t t;
	t.req_type = REQUEST_FLUSH;
	AppendWriteQueue(t);
}

void WriterThread::AppendWriteQueue(req_t t)
{
	pthread_mutex_lock(&input_requests_m);
	input_requests.push(t);
	pthread_mutex_unlock(&input_requests_m);
	pthread_cond_broadcast(&cond);
}

void WriterThread::Thread()
{
	bool running = true;
	
	pthread_mutex_lock(&input_requests_m);
	do {
		while(!input_requests.empty()) {
			req_t r = input_requests.front();
			input_requests.pop();
			pthread_mutex_unlock(&input_requests_m);

			// dealing with inpr
			if(r.req_type != REQUEST_NEW_FILE && r.req_type != REQUEST_QUIT && cur_file == NULL) {
				fprintf(stderr, "Can't do anything without a file (t = %i)\n", r.req_type);
				goto eol;
			}
			
			switch(r.req_type) {
			case REQUEST_QUIT:
				running = false;
				break;
			case REQUEST_APPEND_WRITE:
				gzwrite(cur_file, r.sval.c_str(), r.sval.length());
				break;
			case REQUEST_APPEND_WRITE_BIN:
				gzwrite(cur_file, r.p, r.len);
				free(r.p); r.p = NULL;
				break;
			case REQUEST_NEW_FILE:
				fprintf(stderr, "Opening new file %s\n", r.sval.c_str());
				if(cur_file) {
					gzclose(cur_file);
				}
				cur_file = gzopen(r.sval.c_str(), "wb");
				if(cur_file == NULL) {
					fprintf(stderr, "Couldn't open file %s: ", r.sval.c_str());
					perror(NULL);
				}
				break;
			case REQUEST_FLUSH:
				printf("Flushing(wt)\n");
				gzflush(cur_file, Z_SYNC_FLUSH);
				break;
			default:
				fprintf(stderr, "unknown req? %i\n", r.req_type);
				break;
			}
			
eol:
			pthread_mutex_lock(&input_requests_m);
		}

		if(running) pthread_cond_wait(&cond, &input_requests_m);
	} while(running);

	pthread_mutex_unlock(&input_requests_m);
	
	if(cur_file) {
		gzclose(cur_file);
		cur_file = NULL;
	}
}
	
