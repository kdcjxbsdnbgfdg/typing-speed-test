#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>

const char *text_str = "Income before securities transactions was up 10.8 percent from $13.49 million in 1982 to $14.95 million in 1983. Earnings per share (adjusted for a 10.5 percent stock dividend distributed on August 26) advanced 10 percent to $2.39 in 1983 from $2.17 in 1982. Earnings may rise for 7 years. Hopefully, earnings per share will grow another 10 percent. Kosy, Klemin, and Bille began selling on May 23, 1964. Their second store was founded in Renton on August 3, 1965. From 1964 to 1984, they opened more than 50 stores through-out the country. As they expanded, 12 regional offices had to be organized. Each of these 12 regional offices had to be organized. Each of these 12 regions employs from 108 to 578 people. National headquarters employs 1,077 people. Carole owns 118 stores located in 75 cities ranging as far west as Seattle and as far east as Boston. She owns 46 stores south of the Mason-Dixon line and 24 stores north of Denver. Carole buys goods from 89 companies located in 123 countries and all 50 states. Carole started in business on March 3, 1975. She had less than $6,000 in capital assets.";

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

	/* print the text */
	printf("press escape to exit\n");
	printf("\e[s");
	printf("\e[96m%s\e[0m\n", text_str);

	/* put cursor back */ 
	printf("\e[u");
	/* main loop :3 */
	unsigned int cursorPos = 0;
	while (1){
		int c = getc(stdin);
		if (c == '') break;
		if (c == '') {
			if(cursorPos > 0) {
				cursorPos--;
				if (cursorPos % xSize == 0 && cursorPos != 0){
					/* moves the cursor up, then to the very right, and makes the text coloured */
					printf("\e[F\e[%uG\e[96m", xSize);
					putc(text_str[cursorPos], stdout);
					printf("\e[0m");
				} else {
				printf("\e[D");
				printf("\e[96m");
				putc(text_str[cursorPos], stdout);
				printf("\e[D");
				printf("\e[0m");
				}
			}
		}
		else {
			if (c != text_str[cursorPos]){
				printf("\e[31m");
			}
			putc(c, stdout);
			printf("\e[0m");
			cursorPos++;
		}

	}

	tcsetattr(0, 0, &attr);
	return 0;
}
