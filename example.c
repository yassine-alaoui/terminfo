#include <termios.h>
#include <curses.h>
#include <term.h>
#include <termcap.h>
#include <unistd.h>
#include <stdlib.h>

#define KEY_ESC 27
#define KEY_EL 10
#define KEY_EF 4
#define KEY_DEL 127

/*	These are the structures of the old and new settings of th terminal */

static struct termios	g_old;
static struct termios	g_new;

/* Initialize new terminal i/o settings */

void	initTermios(void)
{
	/*	This saves the old terminfo (terminal informations)	*/
	tcgetattr(0, &g_old);
	/* We'll set the new terminfo equal to the old one so we can modify just want we want */
	g_new = g_old;
	/* ICANON make the terminfo Canonical */
	g_new.c_lflag &= ~ICANON;
	/* ECHO set the terminfo to not print the chars red */
	g_new.c_lflag &= ~ECHO;
	/* Sets the terminfo to the new one */
	tcsetattr(0, TCSANOW, &g_new);
}

char	my_getch(void)
{
	char	ch;
	int		ret;

	/*	We init the terminfo */
	initTermios();
	/*	Read with the new sittings */
	ret = read(0, &ch, 1);
	/* Resets the terminfo to the old one */
	tcsetattr(0, TCSANOW, &g_old);
	return (ch);
}

void	get_one_char(void)
{
	char	c;

	while (1)
	{
		/* Our own read with the new settings */
		c = my_getch();
		/* 
			UP Arrow is represented by the sequence ^[[A
			DOWN Arrow is represented by the sequence ^[[B
			So, we need to detect the the ESC_Sequence first ^[ and [
			So, c == KEY_ESC, skips ^[
			then, the next my_getchar() calls skips [
			finally, we are left wih Either 'A' or 'B' for UP and DOWN Arrows respectively, 
			D and C for LEFT and RIGHT Arrows respectively.
		 */
		if (c == KEY_ESC)
		{
			c = my_getch();
			c = my_getch();
			if (c == 'A')
				write(1, "Clicked on UP\n", 14);
			else if (c == 'B')
				write(1, "Clicked on DOWN\n", 16);
			else if (c == 'D')
				write(1, "Clicked on oem\n", 16);
		}
		/*
			If we have no ESCAPE Sequence, it means we Either have a 
			SIG Ctrl+D
			Or we have a normal Character a, b, c, d, Del, Enter...
		*/
		/*	This is for delete key */
		else if (c == KEY_DEL || c == 8)
			write(1, "Clicked on del\n", 15);
		/*	This is for Ctrl+D */
		else if (c == KEY_EF || c == 8)
			write(1, "Clicked on ctrl-d\n", 18);
		else
			write(1, &c, 1);
		/* the last one is for others */
	}
}

int	main(void)
{
	get_one_char();
}