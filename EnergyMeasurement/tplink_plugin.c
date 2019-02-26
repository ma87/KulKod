#include "tplink_plugin.h"

char * encrypt(const char * message, int * size_output)
{
    *size_output = 4 + strlen(message);
    char * crypted_message = malloc(*size_output);
    int size_crypted = __bswap_32 (strlen(message));

    memcpy(crypted_message, &size_crypted, sizeof(int));

    int key = 171;
    int i;
    for (i = 4 ; i < 4 + strlen(message) ; i++)
    {
        int c = (int)message[i - 4];
        int a = key ^ c;
        key = a;
        crypted_message[i] = (char)a;
    }
    return crypted_message;
}

void decrypt(char * buffer, int size_buffer)
{
    int key = 171;
    int i;
    for (i = 0 ; i <  size_buffer ; i++)
    {
        int c = (int)buffer[i];
        int a = key ^ c;
        key = (int)buffer[i];
        buffer[i] = (char)a;
    }
}

int run_measures(tplink_measurement_t * tplink_msrnt)
{
    return tplink_msrnt->is_running;
}

void stop_measure_thread(tplink_measurement_t * tplink_msrnt)
{
    tplink_msrnt->is_running = 0;
}

void compute_measure(tplink_measurement_t * tplink_msrnt, char * answer)
{
    // measure time elapsed since last measure
    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);
    double time_measure = current_time.tv_sec + current_time.tv_nsec / 1e9;
    double time_elapsed = time_measure - tplink_msrnt->last_measure_timestamp;

    // read in answer current power
    regmatch_t matches[1];
    double mw = 0.0;
    if(regexec(&tplink_msrnt->reg, answer, 1, matches, 0) == 0)
    {
	for (int i = matches[0].rm_so ; i < matches[0].rm_eo ; i++)
	{
		if (answer[i] == ':')
		{
			int number_digits = matches[0].rm_eo - i;
			char * str_mw = malloc((number_digits + 1) * sizeof(char));
			memcpy(str_mw, answer + (i + 1), number_digits);
			str_mw[number_digits] = '\0';
			mw = atoi(str_mw);
			free(str_mw);
			break;
		}
	}
    }

    // update energy_consumed
    tplink_msrnt->energy_consumed += (mw / 1e3) * time_elapsed;
    tplink_msrnt->last_measure_timestamp = time_measure;
}

void * measure(void * data)
{
    tplink_measurement_t * tplink_msrnt = (tplink_measurement_t *)data;

    // Socket communication initialization
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = 9999;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("measure: ERROR opening socket\n");
        stop_measure_thread(tplink_msrnt);
    }
    server = gethostbyname(tplink_msrnt->hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        stop_measure_thread(tplink_msrnt);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        printf("measure: ERROR connecting\n");
        stop_measure_thread(tplink_msrnt);
    }

    const char * command = "\{\"emeter\":\{\"get_realtime\":\{}}}";;
    int size_command;
    char * crypted_command = encrypt(command, &size_command);
    char buffer[1024];
    while (tplink_msrnt->is_running)
    {
        // Send command to tplink
        n = write(sockfd,crypted_command,size_command);
        if (n < 0)
        {
             printf("client: ERROR writing socket\n");
             stop_measure_thread(tplink_msrnt);
        }

        // Receive answer
        bzero(buffer, 1024);
        n = read(sockfd,buffer,1024);
        if (n < 0)
        {
             printf("client: ERROR reading socket\n");
             stop_measure_thread(tplink_msrnt);
        }

        // Decrypt answer
        // Size of answer is stored in the first 4 bytes
        int size_answer = n - 4;
        char * answer = (char *)malloc(size_answer);
        memcpy(answer, buffer + 4, size_answer);
        decrypt(answer, size_answer);

        // Process answer
        compute_measure(tplink_msrnt, answer);
        free(answer);

        // Wait for next measure
        sleep(tplink_msrnt->delta_time_measures);
    }
    
    close(sockfd);
}

int init_tplink_plugin(tplink_measurement_t * tplink_msrnt, const char * hostname, double delta_time_measures)
{
    tplink_msrnt->hostname = hostname;
    tplink_msrnt->energy_consumed = 0.0;
    
    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);
    tplink_msrnt->last_measure_timestamp = current_time.tv_sec + current_time.tv_nsec / 1e9;    // in seconds

    tplink_msrnt->delta_time_measures = delta_time_measures;
    tplink_msrnt->is_running = 1;
    const char *regex="\"power_mw\":[0-9]*";
    regcomp(&tplink_msrnt->reg, regex, REG_EXTENDED);
    int ret = pthread_create(&tplink_msrnt->measure_thread, NULL, measure, (void *)tplink_msrnt);
    return ret;
}

double get_current_energy_tplink(energy_data_t data)
{
    tplink_measurement_t * tplink_msrnt = (tplink_measurement_t *)data;

    return tplink_msrnt->energy_consumed;
}

/*
int main(int argc, char *argv[])
{
   tplink_measurement_t tplink;
   if (!init_tplink_plugin(&tplink, argv[1], 0.001))
   {
	sleep(1.0);
	stop_measure_thread(&tplink);
	pthread_join(tplink.measure_thread, NULL);
	printf("energy_consumed = %lf\n", get_current_energy_tplink((energy_data_t)&tplink));
   }
   return 0;
}
*/
