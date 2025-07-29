#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h>
#include <math.h>

const char *text_str = "Income before securities transactions was up 10.8 percent from $13.49 million in 1982 to $14.95 million in 1983.";

int
main(int argc, char *argv[])
{
	struct termios attr, new_attr;
	tcgetattr(0, &attr);
	new_attr = attr;
	/* & is bitwise AND, ~ is not */
	new_attr.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(0, 0, &new_attr);

	struct winsize termsize;
	ioctl(0, TIOCGWINSZ, &termsize);
	int xSize = termsize.ws_col;

	struct timespec startTime;
	struct timespec endTime;
	unsigned int startTimeInt;
	unsigned int endTimeInt;
	unsigned int startNanoSecond;
	unsigned int endNanoSecond;

	/* print the text */
	printf("press escape to exit\n");
	printf("\e[s");
	printf("\e[96m%s\e[0m\n", text_str);

	/* put cursor back */ 
	printf("\e[u");
	/* main loop :3 */
	unsigned int cursorPos = 0;

	char *typedString = malloc(strlen(text_str));

	while (1){
		int c = getc(stdin);
		if (c == '') break;
		if (c == '') {
			if(cursorPos > 0) {
				if (cursorPos % xSize == 0){
					/* moves the cursor to the previous line */
					printf("\e[A\e[%uG\e[96m", xSize);
					putc(text_str[cursorPos - 1], stdout);
					printf("\e[0m");
					// here bc printing moves it forwards, and moving it back the other way doesnt play nice
					printf("\e[%uG", xSize);
				} else {
					printf("\e[D");
					printf("\e[96m");
					putc(text_str[cursorPos - 1], stdout);
					printf("\e[D");
					printf("\e[0m");
				}
				cursorPos--;
			}
		}
		/* space in ascii is 32, all the non text chars are before 32 */
		else if (c >= ' ') {
			typedString[cursorPos] = c;
			if (cursorPos == 0){
				clock_gettime(CLOCK_MONOTONIC, &startTime);
				startTimeInt = startTime.tv_sec;
				startNanoSecond = startTime.tv_nsec;
			}
			if (c != text_str[cursorPos]){
				printf("\e[41m");
			}
			putc(c, stdout);
			printf("\e[0m");
			if ((cursorPos + 1) % xSize == 0){
				printf("\e[B\e[G");
			}
			cursorPos++;
			if (cursorPos == strlen(text_str)){
				clock_gettime(CLOCK_MONOTONIC, &endTime);
				endTimeInt = endTime.tv_sec;
				endNanoSecond = endTime.tv_nsec;
				int nanoSeconds = endNanoSecond - startNanoSecond;
				/* log10 of 1 is 0, log10 of 10 is 1 */
				int numLength = 1 + ((int) log10f((float) nanoSeconds));
				static char *zerosString = "000000000";
				printf("\nTime: %u.%s%u seconds", endTimeInt - startTimeInt, &zerosString[numLength], nanoSeconds);
				int correctCount = 0;
				for(int i = 0; i < strlen(text_str); i++){
					if (text_str[i] == typedString[i]){
						correctCount++;
					}
				}
				printf("\nAccuracy: %f%%", 100 * (float) correctCount / strlen(text_str));
				printf("\nTypos: %ld", strlen(text_str) - correctCount);
				break;
			}
		}
	}
	/* moves the cursor down after so the text isnt overlapping */
	printf("\e[%uB", (int) (strlen(text_str) / termsize.ws_col) - (int) (cursorPos / termsize.ws_col)); 
	tcsetattr(0, 0, &attr);
	return 0;
}
