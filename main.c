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
	/* saves the cursor pos before printing the text so it can return after */
	printf("\e[s");
	printf("\e[96m%s\e[0m\n", text_str);

	/* put cursor back */ 
	printf("\e[u");
	unsigned int cursorPos = 0;

	char *typedString = malloc(strlen(text_str) + 1);

	/* main loop :3 */
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
				break;
			}
		}
	}
	/* moves the cursor down after so the text isnt overlapping */
	printf("\e[%uB", (int) (strlen(text_str) / termsize.ws_col) - (int) (cursorPos / termsize.ws_col)); 
	clock_gettime(CLOCK_MONOTONIC, &endTime);
	endTimeInt = endTime.tv_sec;
	endNanoSecond = endTime.tv_nsec;
	// 5.4 - 3.6 (= 1.8)
	// 5 - 3 = 2
	// 0.4 is smaller than 0.6
	// so we need to take the 1 from the int to make it go from 2 to 1
	// 1.4 - 0.6 = 0.8
	int seconds = endTimeInt - startTimeInt;
	if (endNanoSecond < startNanoSecond){
		seconds--;
		endNanoSecond = endNanoSecond + 1000000000;
	}
	int nanoSeconds = endNanoSecond - startNanoSecond;
	/* log10 of 1 is 0, log10 of 10 is 1 */
	int numLength = 1 + ((int) log10f((float) nanoSeconds));
	double nanoSecondsAsSeconds = nanoSeconds / pow(10, numLength);
	static char zerosString[10] = "000000000";
	//printf("\nTime: %u.%s%u seconds", seconds, &zerosString[numLength], nanoSeconds);
	printf("\nTime %lf", seconds + nanoSecondsAsSeconds);
	// Todo find a way to reduce the size of typed string, maybe just use the size of cursorPos as the test ends
	// as that should be the max the text goes
	// right now it counts the size of the entire text, even if the person isnt finished, or pressed escape
	// this leads to the program sayings theres more typos
	int correctCount = 0;
	//for(int i = 0; i < strlen(typedString); i++){
	for(int i = 0; i < cursorPos; i++){
		if (text_str[i] == typedString[i]){
			correctCount++;
		}
	}
	printf("\nAccuracy: %f%%", 100 * (float) correctCount / cursorPos);
	printf("\nTypos: %u", cursorPos - correctCount);
	/* simply words divided by minutes (seconds as a dobule divided by 60) */
	printf("\nWPM: %lf", ((double)correctCount / 5) / ((double)(seconds + nanoSecondsAsSeconds) / 60)); 
	//printf("\nnanoseconds: %lf", (nanoSecondsAsSeconds));
	//printf("\nnanosecondsInt: %d", nanoSeconds);
	//printf("\nCorrect Characters: %d", correctCount);
	tcsetattr(0, 0, &attr);
	return 0;
}
